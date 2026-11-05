#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>

namespace {
std::string padRight(const std::string& value, std::size_t width) {
    if (value.length() >= width) return value;
    return value + std::string(width - value.length(), ' ');
}
}

enum class AcademicStanding {
    XuatSac, Gioi, Kha, TrungBinh, Yeu
};

class Student {
private:
    std::string id;
    std::string name;
    std::string major;
    double gpa;

public:
    Student() : gpa(0.0) {}
    Student(std::string id, std::string name, std::string major, double gpa) {
        setId(id);
        setName(name);
        setMajor(major);
        setGpa(gpa);
    }

    [[nodiscard]] std::string getId() const { return id; }
    [[nodiscard]] std::string getName() const { return name; }
    [[nodiscard]] std::string getMajor() const { return major; }
    [[nodiscard]] double getGpa() const { return gpa; }

    void setId(const std::string& newId) {
        if (newId.empty()) throw std::invalid_argument("ID khong duoc de trong.");
        id = newId;
    }

    void setName(const std::string& newName) {
        if (newName.empty()) throw std::invalid_argument("Ten khong duoc de trong.");
        name = newName;
    }

    void setMajor(const std::string& newMajor) {
        major = newMajor.empty() ? "Chua ro" : newMajor;
    }

    void setGpa(double newGpa) {
        if (newGpa < 0.0 || newGpa > 4.0) {
            throw std::invalid_argument("GPA phai nam trong khoang 0.0 den 4.0.");
        }
        gpa = newGpa;
    }

    [[nodiscard]] AcademicStanding getStanding() const {
        if (gpa >= 3.6) return AcademicStanding::XuatSac;
        if (gpa >= 3.2) return AcademicStanding::Gioi;
        if (gpa >= 2.5) return AcademicStanding::Kha;
        if (gpa >= 2.0) return AcademicStanding::TrungBinh;
        return AcademicStanding::Yeu;
    }

    [[nodiscard]] std::string getStandingString() const {
        switch (getStanding()) {
            case AcademicStanding::XuatSac: return "Xuat sac";
            case AcademicStanding::Gioi: return "Gioi";
            case AcademicStanding::Kha: return "Kha";
            case AcademicStanding::TrungBinh: return "Trung binh";
            default: return "Yeu";
        }
    }

    [[nodiscard]] std::string toCSV() const {
        return id + "," + name + "," + major + "," + std::to_string(gpa);
    }

    void display() const {
        std::cout << "| " << padRight(id, 10)
                  << " | " << padRight(name, 22)
                  << " | " << padRight(major, 15)
                  << " | " << std::right << std::fixed << std::setprecision(2)
                  << std::setw(6) << gpa
                  << " | " << padRight(getStandingString(), 12) << " |\n";
    }
};

class StudentManager {
private:
    std::unordered_map<std::string, Student> studentMap;
    std::string filename;
    void createDefaultFileIfMissing() {
        std::ifstream checkFile(filename);
        if (!checkFile.is_open()) {
            std::ofstream newFile(filename);
            if (newFile.is_open()) {
                newFile << "ID,Name,Major,GPA\n";
                newFile.close();
                std::cout << "[Khoi tao] Da tao file du lieu: " << filename << "\n";
            }
        }
    }
public:
    explicit StudentManager(std::string dbFile) : filename(std::move(dbFile)) {
        createDefaultFileIfMissing();
        loadFromFile();
    }

    ~StudentManager() {
        saveToFile();
    }

    void addStudent(const std::string& id, const std::string& name, const std::string& major, double gpa) {
        if (studentMap.find(id) != studentMap.end()) {
            std::cout << "[Loi] Sinh vien voi ID " << id << " da ton tai.\n";
            return;
        }
        try {
            studentMap.emplace(id, Student(id, name, major, gpa));
            std::cout << "[Thanh cong] Da them sinh vien " << name << ".\n";
        } catch (const std::exception& e) {
            std::cout << "[Loi] " << e.what() << "\n";
        }
    }

    void updateStudent(const std::string& id) {
        auto it = studentMap.find(id);
        if (it == studentMap.end()) {
            std::cout << "[Loi] Khong tim thay sinh vien.\n";
            return;
        }
        
        std::string newName, newMajor, gpaInput;
        std::cout << "Nhap Ten moi (bo qua neu giu nguyen): ";
        std::getline(std::cin, newName);
        std::cout << "Nhap Nganh moi (bo qua neu giu nguyen): ";
        std::getline(std::cin, newMajor);
        std::cout << "Nhap GPA moi (bo qua hoac nhap -1 de giu nguyen): ";
        std::getline(std::cin, gpaInput);

        try {
            if (!newName.empty()) it->second.setName(newName);
            if (!newMajor.empty()) it->second.setMajor(newMajor);
            if (!gpaInput.empty()) {
                double gpaVal = std::stod(gpaInput);
                if (gpaVal != -1.0) it->second.setGpa(gpaVal);
            }
            std::cout << "[Thanh cong] Da cap nhat thong tin.\n";
        } catch (const std::exception& e) {
            std::cout << "[Loi] " << e.what() << "\n";
        }
    }

    void searchStudentById(const std::string& id) const {
        auto it = studentMap.find(id);
        if (it != studentMap.end()) {
            printTableHeader();
            it->second.display();
            printTableFooter();
        } else {
            std::cout << "[Thong bao] Khong tim thay sinh vien.\n";
        }
    }

    void searchStudentByName(const std::string& namePart) const {
        std::vector<Student> results;
        std::string lowerQuery = toLowerCase(namePart);
        
        for (const auto& pair : studentMap) {
            if (toLowerCase(pair.second.getName()).find(lowerQuery) != std::string::npos) {
                results.push_back(pair.second);
            }
        }

        if (results.empty()) {
            std::cout << "[Thong bao] Khong tim thay sinh vien nao khop ten.\n";
            return;
        }
        displayList(results);
    }

    void deleteStudent(const std::string& id) {
        if (studentMap.erase(id)) {
            std::cout << "[Thanh cong] Da xoa sinh vien ID: " << id << "\n";
        } else {
            std::cout << "[Loi] Khong tim thay sinh vien.\n";
        }
    }

    void displaySortedByGpa() const {
        if (studentMap.empty()) {
            std::cout << "[Thong bao] Danh sach trong.\n";
            return;
        }
        std::vector<Student> sortedStudents;
        sortedStudents.reserve(studentMap.size());
        for (const auto& pair : studentMap) sortedStudents.push_back(pair.second);

        std::sort(sortedStudents.begin(), sortedStudents.end(), 
            [](const Student& a, const Student& b) { return a.getGpa() > b.getGpa(); });

        displayList(sortedStudents);
    }

    void displayStatistics() const {
        if (studentMap.empty()) return;
        
        int xuatSac = 0, gioi = 0, kha = 0, tb = 0, yeu = 0;
        double sumGpa = 0.0, maxGpa = -1.0, minGpa = 5.0;

        for (const auto& pair : studentMap) {
            double gpa = pair.second.getGpa();
            sumGpa += gpa;
            maxGpa = std::max(maxGpa, gpa);
            minGpa = std::min(minGpa, gpa);

            switch (pair.second.getStanding()) {
                case AcademicStanding::XuatSac: xuatSac++; break;
                case AcademicStanding::Gioi: gioi++; break;
                case AcademicStanding::Kha: kha++; break;
                case AcademicStanding::TrungBinh: tb++; break;
                case AcademicStanding::Yeu: yeu++; break;
            }
        }

        std::cout << "\n=== THONG KE HOC LUC ===\n";
        std::cout << "Tong so sinh vien: " << studentMap.size() << "\n";
        std::cout << "Diem trung binh toan khoa: " << std::fixed << std::setprecision(2) << (sumGpa / studentMap.size()) << "\n";
        std::cout << "GPA Cao nhat: " << maxGpa << " | Thap nhat: " << minGpa << "\n";
        std::cout << "Phan loai: Xuat sac (" << xuatSac << ") | Gioi (" << gioi << ") | Kha (" << kha 
                  << ") | Trung binh (" << tb << ") | Yeu (" << yeu << ")\n";
    }

private:
    void displayList(const std::vector<Student>& list) const {
        printTableHeader();
        for (const auto& s : list) s.display();
        printTableFooter();
    }

    void loadFromFile() {
        std::ifstream file(filename);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string id, name, major, gpaStr;

            if (std::getline(ss, id, ',') && std::getline(ss, name, ',') &&
                std::getline(ss, major, ',') && std::getline(ss, gpaStr, ',')) {
                try {
                    studentMap.emplace(id, Student(id, name, major, std::stod(gpaStr)));
                } catch (...) { continue; }
            }
        }
    }

    void saveToFile() const {
        std::ofstream file(filename);
        if (!file.is_open()) return;
        for (const auto& pair : studentMap) {
            file << pair.second.toCSV() << "\n";
        }
    }

    static void printTableHeader() {
        printTableBorder();
        std::cout << "| " << padRight("ID", 10)
                  << " | " << padRight("Ho va Ten", 22)
                  << " | " << padRight("Chuyen Nganh", 15)
                  << " | " << padRight("GPA", 6)
                  << " | " << padRight("Hoc Luc", 12) << " |\n";
        printTableBorder();
    }

    static void printTableFooter() {
        printTableBorder();
    }

    static void printTableBorder() {
        std::cout << "+------------+------------------------+-----------------+--------+--------------+\n";
    }

    static std::string toLowerCase(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
};

int main() {
    StudentManager manager("students_pro.csv");
    std::string choiceStr;

    while (true) {
        std::cout << "\n===== HE THONG QUAN LY SINH VIEN =====\n";
        std::cout << "1. Hien thi danh sach (Sap xep theo GPA)\n";
        std::cout << "2. Them moi sinh vien\n";
        std::cout << "3. Cap nhat thong tin sinh vien\n";
        std::cout << "4. Tim kiem theo ID\n";
        std::cout << "5. Tim kiem theo Ten\n";
        std::cout << "6. Xoa sinh vien\n";
        std::cout << "7. Xem thong ke tong quan\n";
        std::cout << "8. Luu & Thoat\n";
        std::cout << "Lua chon: ";
        
        if (!std::getline(std::cin, choiceStr) || choiceStr.empty()) {
            continue;
        }

        int choice = 0;
        try {
            choice = std::stoi(choiceStr);
        } catch (...) {
            continue;
        }

        try {
            if (choice == 1) manager.displaySortedByGpa();
            else if (choice == 2) {
                std::string id, name, major, gpaStr;
                std::cout << "ID: "; std::getline(std::cin, id);
                std::cout << "Ten: "; std::getline(std::cin, name);
                std::cout << "Chuyen nganh: "; std::getline(std::cin, major);
                std::cout << "GPA (0.0-4.0): "; std::getline(std::cin, gpaStr);
                manager.addStudent(id, name, major, std::stod(gpaStr));
            } 
            else if (choice == 3) {
                std::string id; 
                std::cout << "Nhap ID can sua: "; std::getline(std::cin, id);
                manager.updateStudent(id);
            }
            else if (choice == 4) {
                std::string id; 
                std::cout << "Nhap ID: "; std::getline(std::cin, id);
                manager.searchStudentById(id);
            }
            else if (choice == 5) {
                std::string name; 
                std::cout << "Nhap Ten can tim: "; std::getline(std::cin, name);
                manager.searchStudentByName(name);
            }
            else if (choice == 6) {
                std::string id; 
                std::cout << "Nhap ID can xoa: "; std::getline(std::cin, id);
                manager.deleteStudent(id);
            }
            else if (choice == 7) manager.displayStatistics();
            else if (choice == 8) {
                std::cout << "Da luu du lieu. Thoat chuong trinh!\n"; 
                break;
            }
        } catch (const std::exception& e) {
            std::cout << "Da xay ra loi: " << e.what() << "\n";
        }
    }
    return 0;
}