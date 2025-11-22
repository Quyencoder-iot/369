# Qt Models Learning Resources / Tài Liệu Học Qt Models

Tài liệu học toàn diện về Qt Model/View Architecture bằng tiếng Việt.

## 📚 Các Tài Liệu Có Sẵn

### 1. **Qt_Models_Learning_Guide.md** - Hướng Dẫn Chi Tiết
Tài liệu chính với:
- Giải thích kiến trúc Model/View
- Phân tích từng loại model (List, Table, Tree, Proxy, SQL)
- Sơ đồ mối quan hệ giữa các model
- Phương pháp đọc và hiểu code Qt model hiệu quả
- Ví dụ thực tế cho từng loại model
- Checklist và tips khi đọc code

**Nên đọc đầu tiên để hiểu toàn bộ kiến trúc!**

### 2. **Qt_Models_Examples.cpp** - Code Mẫu Thực Tế
File C++ chứa 7 ví dụ hoàn chỉnh:
1. Simple List Model
2. Table Model with Custom Data
3. Tree Model (hierarchical data)
4. Sort/Filter Proxy Model
5. SQL Models (database integration)
6. Advanced Custom Roles
7. Index Mapping (proxy ↔ source)

Plus: Debugging tips và Performance optimization

**Dùng để thực hành và tham khảo code thực tế!**

### 3. **Qt_Models_Quick_Reference.md** - Cheat Sheet
Tài liệu tham khảo nhanh với:
- Decision tree: chọn model nào?
- Essential methods cho từng loại model
- Common roles và signals
- Code patterns phổ biến
- Debugging checklist
- Learning path từng tuần

**In ra để tra cứu nhanh khi code!**

## 🚀 Cách Sử Dụng

### Học Lần Đầu (Beginner):
1. Đọc **Qt_Models_Learning_Guide.md** để hiểu concepts
2. Thử compile và chạy từng example trong **Qt_Models_Examples.cpp**
3. Giữ **Qt_Models_Quick_Reference.md** bên cạnh để tra cứu

### Đọc Code Có Sẵn (Reading existing code):
1. Mở **Qt_Models_Quick_Reference.md** phần "Debugging Checklist"
2. Xác định loại model (List/Table/Tree/Proxy/SQL)
3. Đọc theo thứ tự: data structure → rowCount → data() → setData()
4. Tham khảo **Qt_Models_Learning_Guide.md** phần "Phương Pháp Đọc Code"

### Viết Code Mới (Writing new code):
1. Dùng Decision Tree trong **Qt_Models_Quick_Reference.md** để chọn model type
2. Copy template từ **Qt_Models_Examples.cpp**
3. Tham khảo Essential Methods và Common Patterns trong Quick Reference

## 📖 Thứ Tự Học Đề Xuất

```
Tuần 1: QAbstractListModel
├── Đọc: Learning Guide - Section "QAbstractListModel"
├── Code: Examples.cpp - Example 1
└── Practice: Tạo list model với dữ liệu riêng

Tuần 2: QAbstractTableModel
├── Đọc: Learning Guide - Section "QAbstractTableModel"
├── Code: Examples.cpp - Example 2
└── Practice: Tạo table model hiển thị data 2D

Tuần 3: Proxy Models
├── Đọc: Learning Guide - Section "Proxy Models"
├── Code: Examples.cpp - Example 4
└── Practice: Thêm sort/filter cho model có sẵn

Tuần 4: SQL Models
├── Đọc: Learning Guide - Section "SQL Models"
├── Code: Examples.cpp - Example 5
└── Practice: Connect với SQLite database

Tuần 5: Tree Models (Advanced)
├── Đọc: Learning Guide - Section "Tree Model"
├── Code: Examples.cpp - Example 3
└── Practice: Tạo hierarchical data viewer
```

## 🎯 Key Concepts Cần Nắm

- **Model/View separation**: Model quản lý data, View hiển thị
- **QModelIndex**: Represents một cell trong model (row, column, parent)
- **Roles**: Cùng một cell có thể return khác nhau tùy role (display, edit, decoration, etc.)
- **Signals**: dataChanged, rowsInserted, rowsRemoved để notify view
- **Proxy Models**: Wrapper để add features (sort/filter) không modify source

## 🔧 Compile Examples

```bash
# Cần Qt5 hoặc Qt6
qmake -project
qmake
make

# Hoặc với cmake
mkdir build && cd build
cmake ..
make
```

## 📞 Troubleshooting

**Q: Model không hiển thị trong view?**
- Check `rowCount()` return > 0
- Check `data()` return valid QVariant cho Qt::DisplayRole
- Verify `view->setModel(model)` đã được gọi

**Q: View không update khi data thay đổi?**
- Emit `dataChanged()` sau khi modify data
- Dùng `beginInsertRows()`/`endInsertRows()` khi insert
- Dùng `beginRemoveRows()`/`endRemoveRows()` khi remove

**Q: Proxy model không filter?**
- Check `setSourceModel()` đã được gọi
- Call `invalidateFilter()` sau khi thay đổi filter criteria
- View phải dùng proxy model, không phải source model

## 🌟 Tài Nguyên Bổ Sung

- Qt Official Docs: https://doc.qt.io/qt-5/model-view-programming.html
- Qt Examples: https://doc.qt.io/qt-5/examples-itemviews.html
- Qt Forum: https://forum.qt.io/

---

**Happy Learning! / Chúc học tốt! 🚀**
