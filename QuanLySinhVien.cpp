#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <regex>
#include <stdexcept>

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
    Student() = default;
    Student(std::string id, std::string name, std::string major, double gpa) {
        setId(id);
        setName(name);
        setMajor(major);
        setGpa(gpa);
    }

    // Getters
    [[nodiscard]] std::string getId() const { return id; }
    [[nodiscard]] std::string getName() const { return name; }
    [[nodiscard]] std::string getMajor() const { return major; }
    [[nodiscard]] double getGpa() const { return gpa; }


    void setId(const std::string& newId) {
        if (newId.empty()) throw std::invalid_argument("ID không được để trống.");
        id = newId;
    }

    void setName(const std::string& newName) {
        if (newName.empty()) throw std::invalid_argument("Tên không được để trống.");
        name = newName;
    }

    void setMajor(const std::string& newMajor) {
        major = newMajor.empty() ? "Chưa rõ" : newMajor;
    }

    void setGpa(double newGpa) {
        if (newGpa < 0.0 || newGpa > 4.0) {
            throw std::invalid_argument("GPA phải nằm trong khoảng 0.0 đến 4.0.");
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
        std::cout << "| " << std::left << std::setw(10) << id 
                  << "| " << std::setw(22) << name 
                  << "| " << std::setw(15) << major
                  << "| " << std::fixed << std::setprecision(2) << std::setw(6) << gpa 
                  << "| " << std::setw(12) << getStandingString() << "|\n";
    }
};

class StudentManager {
private:

    std::unordered_map<std::string, Student> studentMap;
    std::string filename;

public:
    explicit StudentManager(std::string dbFile) : filename(std::move(dbFile)) {
        loadFromFile();
    }

    ~StudentManager() {
        saveToFile();
    }

    void addStudent(const std::string& id, const std::string& name, const std::string& major, double gpa) {
        if (studentMap.find(id) != studentMap.end()) {
            std::cout << "[Lỗi] Sinh viên với ID " << id << " đã tồn tại.\n";
            return;
        }
        try {
            studentMap.emplace(id, Student(id, name, major, gpa));
            std::cout << "[Thành công] Đã thêm sinh viên " << name << ".\n";
        } catch (const std::exception& e) {
            std::cout << "[Lỗi] " << e.what() << "\n";
        }
    }

    void updateStudent(const std::string& id) {
        auto it = studentMap.find(id);
        if (it == studentMap.end()) {
            std::cout << "[Lỗi] Không tìm thấy sinh viên.\n";
            return;
        }
        
        std::string newName, newMajor;
        double newGpa;
        std::cout << "Nhập Tên mới (bỏ qua nếu giữ nguyên): ";
        std::getline(std::cin, newName);
        std::cout << "Nhập Ngành mới (bỏ qua nếu giữ nguyên): ";
        std::getline(std::cin, newMajor);
        std::cout << "Nhập GPA mới (-1 để giữ nguyên): ";
        std::cin >> newGpa;

        try {
            if (!newName.empty()) it->second.setName(newName);
            if (!newMajor.empty()) it->second.setMajor(newMajor);
            if (newGpa != -1.0) it->second.setGpa(newGpa);
            std::cout << "[Thành công] Đã cập nhật thông tin.\n";
        } catch (const std::exception& e) {
            std::cout << "[Lỗi] " << e.what() << "\n";
        }
    }

    void searchStudentById(const std::string& id) const {
        auto it = studentMap.find(id);
        if (it != studentMap.end()) {
            printTableHeader();
            it->second.display();
            printTableFooter();
        } else {
            std::cout << "[Thông báo] Không tìm thấy sinh viên.\n";
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
            std::cout << "[Thông báo] Không tìm thấy sinh viên nào khớp tên.\n";
            return;
        }
        displayList(results);
    }

    void deleteStudent(const std::string& id) {
        if (studentMap.erase(id)) {
            std::cout << "[Thành công] Đã xóa sinh viên ID: " << id << "\n";
        } else {
            std::cout << "[Lỗi] Không tìm thấy sinh viên.\n";
        }
    }

    void displaySortedByGpa() const {
        if (studentMap.empty()) {
            std::cout << "[Thông báo] Danh sách trống.\n";
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

        std::cout << "\n=== THỐNG KÊ HỌC LỰC ===\n";
        std::cout << "Tổng số sinh viên: " << studentMap.size() << "\n";
        std::cout << "Điểm trung bình toàn khóa: " << std::fixed << std::setprecision(2) << (sumGpa / studentMap.size()) << "\n";
        std::cout << "GPA Cao nhất: " << maxGpa << " | Thấp nhất: " << minGpa << "\n";
        std::cout << "Phân loại: Xuất sắc (" << xuatSac << ") | Giỏi (" << gioi << ") | Khá (" << kha 
                  << ") | Trung bình (" << tb << ") | Yếu (" << yeu << ")\n";
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
        std::cout << "+-----------+-----------------------+----------------+--------+-------------+\n";
        std::cout << "| ID        | Họ và Tên             | Chuyên Ngành   | GPA    | Học Lực     |\n";
        std::cout << "+-----------+-----------------------+----------------+--------+-------------+\n";
    }

    static void printTableFooter() {
        std::cout << "+-----------+-----------------------+----------------+--------+-------------+\n";
    }

    static std::string toLowerCase(std::string str) {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
};

int main() {
    StudentManager manager("students_pro.csv");
    int choice;

    while (true) {
        std::cout << "\n===== HỆ THỐNG QUẢN LÝ SINH VIÊN  =====\n";
        std::cout << "1. Hiển thị danh sách (Sắp xếp theo GPA)\n";
        std::cout << "2. Thêm mới sinh viên\n";
        std::cout << "3. Cập nhật thông tin sinh viên\n";
        std::cout << "4. Tìm kiếm theo ID\n";
        std::cout << "5. Tìm kiếm theo Tên\n";
        std::cout << "6. Xóa sinh viên\n";
        std::cout << "7. Xem thống kê tổng quan\n";
        std::cout << "8. Lưu & Thoát\n";
        std::cout << "Lựa chọn: ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            continue;
        }
        std::cin.ignore(); 

        try {
            if (choice == 1) manager.displaySortedByGpa();
            else if (choice == 2) {
                std::string id, name, major; double gpa;
                std::cout << "ID: "; std::getline(std::cin, id);
                std::cout << "Tên: "; std::getline(std::cin, name);
                std::cout << "Chuyên ngành: "; std::getline(std::cin, major);
                std::cout << "GPA (0.0-4.0): "; std::cin >> gpa;
                manager.addStudent(id, name, major, gpa);
            } 
            else if (choice == 3) {
                std::string id; std::cout << "Nhập ID cần sửa: "; std::getline(std::cin, id);
                manager.updateStudent(id);
            }
            else if (choice == 4) {
                std::string id; std::cout << "Nhập ID: "; std::getline(std::cin, id);
                manager.searchStudentById(id);
            }
            else if (choice == 5) {
                std::string name; std::cout << "Nhập Tên cần tìm: "; std::getline(std::cin, name);
                manager.searchStudentByName(name);
            }
            else if (choice == 6) {
                std::string id; std::cout << "Nhập ID cần xóa: "; std::getline(std::cin, id);
                manager.deleteStudent(id);
            }
            else if (choice == 7) manager.displayStatistics();
            else if (choice == 8) {
                std::cout << "Đã lưu dữ liệu. Thoát chương trình!\n"; break;
            }
        } catch (const std::exception& e) {
            std::cout << "Đã xảy ra lỗi: " << e.what() << "\n";
        }
    }
    return 0;
}