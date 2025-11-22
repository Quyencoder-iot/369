# 🚀 Quick Start - Học Qt Models trong 5 phút

## 1️⃣ Build và chạy ngay

```bash
./build.sh
cd build
./qt_models_demo
```

## 2️⃣ Concepts cần nhớ

### 📋 List Model (1D data)
```cpp
class MyListModel : public QAbstractListModel {
    // Bắt buộc:
    int rowCount() { return m_items.size(); }
    QVariant data(index, role) { return m_items[index.row()]; }
}
```

### 📊 Table Model (2D data)
```cpp
class MyTableModel : public QAbstractTableModel {
    // Thêm:
    int columnCount() { return 3; }
    QVariant headerData() { return column_names[section]; }
}
```

### 🌳 Tree Model (Hierarchical)
```cpp
class MyTreeModel : public QAbstractItemModel {
    // Phức tạp hơn:
    QModelIndex index(row, col, parent) { /* create index */ }
    QModelIndex parent(index) { /* get parent */ }
}
```

## 3️⃣ Signals quan trọng

```cpp
// Trước khi thêm rows
beginInsertRows(parent, first, last);
// ... thêm data ...
endInsertRows();

// Khi data thay đổi
emit dataChanged(topLeft, bottomRight);
```

## 4️⃣ Quick Examples

### Filter data:
```cpp
QSortFilterProxyModel *proxy = new QSortFilterProxyModel;
proxy->setSourceModel(sourceModel);
proxy->setFilterWildcard("*search*");
view->setModel(proxy);
```

### Database:
```cpp
QSqlTableModel *model = new QSqlTableModel;
model->setTable("employees");
model->select();
view->setModel(model);
```

## 5️⃣ Debug Tips

```cpp
// Always check
if (!index.isValid()) return QVariant();

// Debug output
qDebug() << "Row:" << index.row() << "Data:" << m_data;

// Validate
Q_ASSERT(row >= 0 && row < rowCount());
```

## 📚 Next Steps

1. Run từng example: `./list_model_example`
2. Đọc source code trong `examples/`
3. Modify và experiment
4. Check docs khi cần

**Happy Coding! 🎉**