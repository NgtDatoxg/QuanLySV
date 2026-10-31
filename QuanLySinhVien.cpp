#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>


class Student {
private:
    std::string id;
    std::string name;
    double gpa;

public:
    Student() : id(""), name(""), gpa(0.0) {}
    Student(std::string id, std::string name, double gpa) 
        : id(id), name(name), gpa(gpa) {}

  
    std::string getId() const { return id; }
    std::string getName() const { return name; }
    double getGpa() const { return gpa; }


    void setName(const std::string& newName) { name = newName; }
    void setGpa(double newGpa) {
        if (newGpa >= 0.0 && newGpa <= 4.0) {
            gpa = newGpa;
        }
    }


    std::string toCSV() const {
        return id + "," + name + "," + std::to_string(gpa);
    }

    // Hiển thị dạng bảng
    void display() const {
        std::cout << "| " << std::left << std::setw(12) << id 
                  << "| " << std::setw(25) << name 
                  << "| " << std::fixed << std::setprecision(2) << std::setw(8) << gpa 
                  << "|\n";
    }
};


class StudentManager {
private:
    std::vector<Student> students;
    std::string filename;

public:
    StudentManager(const std::string& dbFile) : filename(dbFile) {
        loadFromFile();
    }

    ~StudentManager() {
        saveToFile();
    }


    bool addStudent(const std::string& id, const std::string& name, double gpa) {
        if (findStudentIndexById(id) != -1) {
            std::cout << "[Lỗi] Sinh viên với ID " << id << " đã tồn tại trong hệ thống.\n";
            return false;
        }
        if (gpa < 0.0 || gpa > 4.0) {
            std::cout << "[Lỗi] GPA không hợp lệ (phải từ 0.0 đến 4.0).\n";
            return false;
        }

        students.emplace_back(id, name, gpa);
        std::cout << "[Thành công] Đã thêm sinh viên " << name << ".\n";
        return true;
    }


    int findStudentIndexById(const std::string& id) const {
        for (size_t i = 0; i < students.size(); ++i) {
            if (students[i].getId() == id) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void searchStudent(const std::string& id) const {
        int index = findStudentIndexById(id);
        if (index != -1) {
            printTableHeader();
            students[index].display();
            printTableFooter();
        } else {
            std::cout << "[Thông báo] Không tìm thấy sinh viên có ID: " << id << "\n";
        }
    }


    bool deleteStudent(const std::string& id) {
        int index = findStudentIndexById(id);
        if (index != -1) {
            students.erase(students.begin() + index);
            std::cout << "[Thành công] Đã xóa sinh viên ID: " << id << "\n";
            return true;
        }
        std::cout << "[Lỗi] Không tìm thấy sinh viên cần xóa.\n";
        return false;
    }

  
    void sortByGpaDescending() {
        std::sort(students.begin(), students.end(), [](const Student& a, const Student& b) {
            return a.getGpa() > b.getGpa();
        });
        std::cout << "[Thành công] Đã sắp xếp danh sách theo GPA giảm dần.\n";
    }


    void displayAll() const {
        if (students.empty()) {
            std::cout << "[Thông báo] Danh sách sinh viên trống.\n";
            return;
        }
        printTableHeader();
        for (const auto& s : students) {
            s.display();
        }
        printTableFooter();
    }

    void loadFromFile() {
        students.clear();
        std::ifstream file(filename);
        if (!file.is_open()) return;

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string id, name, gpaStr;

            if (std::getline(ss, id, ',') &&
                std::getline(ss, name, ',') &&
                std::getline(ss, gpaStr, ',')) {
                try {
                    double gpa = std::stod(gpaStr);
                    students.emplace_back(id, name, gpa);
                } catch (...) {
                    continue; 
                }
            }
        }
        file.close();
    }

   
    void saveToFile() const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[Lỗi] Không thể mở file để lưu dữ liệu.\n";
            return;
        }
        for (const auto& s : students) {
            file << s.toCSV() << "\n";
        }
        file.close();
    }

private:
    static void printTableHeader() {
        std::cout << "+------------+--------------------------+---------+\n";
        std::cout << "| ID         | Họ và Tên                | GPA     |\n";
        std::cout << "+------------+--------------------------+---------+\n";
    }

    static void printTableFooter() {
        std::cout << "+------------+--------------------------+---------+\n";
    }
};

int main() {
    StudentManager manager("students.csv");
    int choice;

    while (true) {
        std::cout << "\n===== HỆ THỐNG QUẢN LÝ SINH VIÊN (C++ OOP) =====\n";
        std::cout << "1. Hiển thị danh sách sinh viên\n";
        std::cout << "2. Thêm mới sinh viên\n";
        std::cout << "3. Tìm kiếm sinh viên theo ID\n";
        std::cout << "4. Xóa sinh viên theo ID\n";
        std::cout << "5. Sắp xếp danh sách theo điểm GPA giảm dần\n";
        std::cout << "6. Lưu & Thoát\n";
        std::cout << "Lựa chọn của bạn (1-6): ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "Vui lòng nhập số hợp lệ!\n";
            continue;
        }

        std::cin.ignore(); 

        if (choice == 1) {
            manager.displayAll();
        } else if (choice == 2) {
            std::string id, name;
            double gpa;
            std::cout << "Nhập ID sinh viên: ";
            std::getline(std::cin, id);
            std::cout << "Nhập Họ và Tên: ";
            std::getline(std::cin, name);
            std::cout << "Nhập GPA (0.0 - 4.0): ";
            std::cin >> gpa;
            manager.addStudent(id, name, gpa);
        } else if (choice == 3) {
            std::string id;
            std::cout << "Nhập ID sinh viên cần tìm: ";
            std::getline(std::cin, id);
            manager.searchStudent(id);
        } else if (choice == 4) {
            std::string id;
            std::cout << "Nhập ID sinh viên cần xóa: ";
            std::getline(std::cin, id);
            manager.deleteStudent(id);
        } else if (choice == 5) {
            manager.sortByGpaDescending();
            manager.displayAll();
        } else if (choice == 6) {
            std::cout << "Dữ liệu đã được tự động lưu. Tạm biệt!\n";
            break;
        } else {
            std::cout << "Lựa chọn không hợp lệ, vui lòng chọn lại.\n";
        }
    }

    return 0;
}