# Hướng Dẫn Con Trỏ (Pointer) trong C++

Tài liệu này giải thích về con trỏ trong C++, đặc biệt trong ngữ cảnh Qt.

## 📚 Tài Liệu

- **[con-tro-pointer-cpp.md](con-tro-pointer-cpp.md)** - Hướng dẫn toàn diện về con trỏ và tham chiếu
  - Mục đích sử dụng con trỏ
  - So sánh con trỏ vs tham chiếu
  - Con trỏ trong Qt
  - Best practices và lỗi thường gặp

## 💻 Ví Dụ Thực Tế

- **[vi-du-con-tro.cpp](vi-du-con-tro.cpp)** - File ví dụ code minh họa:
  - Con trỏ cơ bản
  - Con trỏ vs tham chiếu
  - Quản lý bộ nhớ động
  - Polymorphism
  - Qt-style parent-child pattern
  - Model-View pattern (giống Qt Model)
  - Smart pointers

## 🚀 Chạy Ví Dụ

```bash
# Biên dịch
make

# Hoặc
g++ -std=c++11 vi-du-con-tro.cpp -o vi-du-con-tro

# Chạy
./vi-du-con-tro
```

## 🔑 Điểm Quan Trọng

1. **Con trỏ** lưu địa chỉ bộ nhớ, có thể NULL
2. **Tham chiếu** là alias của biến, không thể NULL
3. **Qt dùng con trỏ** cho parent-child relationship và model-view
4. **Luôn kiểm tra nullptr** trước khi dùng con trỏ
5. **Ưu tiên smart pointer** để tránh memory leak

## 📖 Nội Dung Chính

### Khi Nào Dùng Con Trỏ?
- Cần khả năng NULL (optional parameter)
- Quản lý bộ nhớ động
- Polymorphism với base class
- Qt objects (parent-child)
- Mảng động

### Khi Nào Dùng Tham Chiếu?
- Đảm bảo luôn có giá trị hợp lệ
- Muốn cú pháp đơn giản
- Truyền vào hàm mà không muốn copy
- Không cần thay đổi địa chỉ

Xem chi tiết trong [con-tro-pointer-cpp.md](con-tro-pointer-cpp.md)
