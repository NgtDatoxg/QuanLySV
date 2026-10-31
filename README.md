# QuanLySV
# Hệ Thống Quản Lý Sinh Viên (C++)

Ứng dụng dòng lệnh (CLI) quản lý hồ sơ sinh viên viết bằng C++, áp dụng mô hình Lập trình Hướng đối tượng (OOP) và thư viện chuẩn C++ STL.

## Tính năng chính
- **Quản lý hồ sơ:** Thêm, tìm kiếm và xóa sinh viên theo mã số (ID).
- **Sắp xếp:** Sắp xếp danh sách sinh viên theo điểm GPA giảm dần bằng bộ so sánh tùy biến (custom comparator).
- **Ràng buộc dữ liệu:** Chống trùng lặp ID sinh viên và kiểm tra tính hợp lệ của điểm GPA (thang điểm 0.0 - 4.0).
- **Lưu trữ dữ liệu:** Tự động đọc và lưu dữ liệu vào tệp tin `students.csv`.

## Công nghệ sử dụng
- **Ngôn ngữ:** C++ (C++11 trở lên)
- **Kiến thức cốt lõi:** Lập trình Hướng đối tượng (OOP), Cấu trúc dữ liệu STL (`std::vector`, `std::sort`), Luồng thao tác tệp tin (`<fstream>`)

