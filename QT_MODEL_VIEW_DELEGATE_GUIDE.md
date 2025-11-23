# Hướng Dẫn Nhanh: Con Trỏ trong Qt Model-View-Delegate

## 📚 Mục Lục
1. [Tổng Quan Kiến Trúc](#tổng-quan)
2. [Con Trỏ Quan Trọng](#con-trỏ-quan-trọng)
3. [Ví Dụ Thực Tế](#ví-dụ-thực-tế)
4. [Best Practices](#best-practices)
5. [Tóm Tắt Nhanh](#tóm-tắt-nhanh)

---

## 🎯 Tổng Quan Kiến Trúc {#tổng-quan}

### Model-View-Delegate là gì?

```
┌─────────┐      ┌─────────┐      ┌──────────┐
│  Model  │◄────►│  View   │◄────►│ Delegate │
│ (Dữ liệu)│      │(Hiển thị)│      │(Chỉnh sửa)│
└─────────┘      └─────────┘      └──────────┘
```

**Model**: Quản lý dữ liệu
**View**: Hiển thị dữ liệu
**Delegate**: Xử lý chỉnh sửa và render từng item

### Tại sao cần hiểu con trỏ?

Qt sử dụng **parent-child ownership** và **smart pointers** để quản lý bộ nhớ tự động. Hiểu con trỏ giúp:
- Tránh memory leaks
- Quản lý lifecycle đúng cách
- Tối ưu hiệu năng

---

## 🔑 Con Trỏ Quan Trọng {#con-trỏ-quan-trọng}

### 1. Model Pointers

#### QAbstractItemModel*
```cpp
// Model được tạo và quản lý bởi parent widget
QStandardItemModel* model = new QStandardItemModel(this); // 'this' là parent
// Khi parent bị destroy, model tự động bị delete
```

**Quy tắc vàng**: 
- ✅ Luôn truyền `parent` khi tạo model trong widget
- ✅ Không delete model thủ công nếu có parent
- ✅ Sử dụng `QPointer<QAbstractItemModel>` nếu cần kiểm tra null

#### Ví dụ:
```cpp
class MyWidget : public QWidget {
    Q_OBJECT
private:
    QStandardItemModel* m_model; // Con trỏ thành viên
    
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // ✅ ĐÚNG: Truyền 'this' làm parent
        m_model = new QStandardItemModel(this);
        
        // Model sẽ tự động bị delete khi MyWidget bị destroy
    }
    
    // ❌ SAI: Không cần delete trong destructor
    // ~MyWidget() { delete m_model; } // KHÔNG CẦN!
};
```

### 2. View Pointers

#### QAbstractItemView*
```cpp
QTableView* view = new QTableView(this);
view->setModel(model); // View KHÔNG sở hữu model, chỉ tham chiếu
```

**Quan trọng**:
- View **không sở hữu** model (không delete model)
- View có thể share model với nhiều view khác
- Model phải sống lâu hơn view

#### Ví dụ:
```cpp
void setupViews() {
    QStandardItemModel* sharedModel = new QStandardItemModel(this);
    
    // Nhiều view có thể dùng chung 1 model
    QTableView* view1 = new QTableView(this);
    QTreeView* view2 = new QTreeView(this);
    QListView* view3 = new QListView(this);
    
    view1->setModel(sharedModel); // ✅ OK
    view2->setModel(sharedModel); // ✅ OK - cùng model
    view3->setModel(sharedModel); // ✅ OK - cùng model
    
    // Khi widget bị destroy, tất cả đều được cleanup tự động
}
```

### 3. Delegate Pointers

#### QAbstractItemDelegate*
```cpp
QStyledItemDelegate* delegate = new QStyledItemDelegate(this);
view->setItemDelegate(delegate);
```

**Quan trọng**:
- Delegate được sở hữu bởi view (view sẽ delete delegate)
- Có thể set delegate cho từng cột riêng biệt
- Delegate có thể được share giữa các view

#### Ví dụ:
```cpp
void setupDelegates() {
    QTableView* view = new QTableView(this);
    
    // Delegate chung cho toàn bộ view
    QStyledItemDelegate* defaultDelegate = new QStyledItemDelegate(view);
    view->setItemDelegate(defaultDelegate);
    
    // Delegate riêng cho cột số (column 0)
    QSpinBoxDelegate* spinDelegate = new QSpinBoxDelegate(view);
    view->setItemDelegateForColumn(0, spinDelegate);
    
    // Delegate riêng cho cột ngày (column 1)
    QDateEditDelegate* dateDelegate = new QDateEditDelegate(view);
    view->setItemDelegateForColumn(1, dateDelegate);
    
    // ✅ Tất cả delegate sẽ được view quản lý và delete tự động
}
```

### 4. Index Pointers (QModelIndex)

**QUAN TRỌNG**: `QModelIndex` **KHÔNG phải con trỏ**, nhưng chứa con trỏ nội bộ!

```cpp
QModelIndex index = model->index(row, column, parent);
// Index chỉ hợp lệ khi model còn tồn tại
// Index có thể trở thành invalid nếu model thay đổi cấu trúc
```

**Quy tắc**:
- ✅ Luôn kiểm tra `index.isValid()` trước khi dùng
- ✅ Không lưu index lâu dài, chỉ dùng ngay sau khi lấy
- ✅ Nếu cần lưu, dùng `QPersistentModelIndex`

#### Ví dụ:
```cpp
void processIndex(const QModelIndex& index) {
    if (!index.isValid()) {
        return; // ✅ Kiểm tra hợp lệ
    }
    
    QVariant data = index.data();
    // ...
}

// ❌ SAI: Lưu index lâu dài
QModelIndex savedIndex; // Có thể trở thành invalid!

// ✅ ĐÚNG: Dùng QPersistentModelIndex nếu cần lưu
QPersistentModelIndex persistentIndex(model->index(0, 0));
```

---

## 💻 Ví Dụ Thực Tế {#ví-dụ-thực-tế}

### Ví Dụ 1: Setup Cơ Bản

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        // ✅ Tạo model với parent
        m_model = new QStandardItemModel(this);
        
        // Setup dữ liệu
        m_model->setHorizontalHeaderLabels({"Tên", "Tuổi", "Địa chỉ"});
        m_model->appendRow({
            new QStandardItem("Nguyễn Văn A"),
            new QStandardItem("25"),
            new QStandardItem("Hà Nội")
        });
        
        // ✅ Tạo view với parent
        m_view = new QTableView(this);
        m_view->setModel(m_model); // View tham chiếu model
        
        // ✅ Set central widget
        setCentralWidget(m_view);
        
        // Tất cả sẽ tự động cleanup khi MainWindow bị destroy
    }
    
private:
    QStandardItemModel* m_model;  // Con trỏ thành viên
    QTableView* m_view;           // Con trỏ thành viên
};
```

### Ví Dụ 2: Custom Delegate với Con Trỏ

```cpp
#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QModelIndex>

class SpinBoxDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    SpinBoxDelegate(QObject* parent = nullptr) 
        : QStyledItemDelegate(parent) {}
    
    // Tạo editor widget
    QWidget* createEditor(QWidget* parent, 
                         const QStyleOptionViewItem& option,
                         const QModelIndex& index) const override {
        // ✅ Editor được parent widget sở hữu
        QSpinBox* editor = new QSpinBox(parent);
        editor->setMinimum(0);
        editor->setMaximum(100);
        return editor;
    }
    
    // Set dữ liệu vào editor
    void setEditorData(QWidget* editor, 
                      const QModelIndex& index) const override {
        QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
        int value = index.model()->data(index, Qt::EditRole).toInt();
        spinBox->setValue(value);
    }
    
    // Lấy dữ liệu từ editor về model
    void setModelData(QWidget* editor, 
                     QAbstractItemModel* model,
                     const QModelIndex& index) const override {
        QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
    }
    
    // Cập nhật geometry của editor
    void updateEditorGeometry(QWidget* editor,
                             const QStyleOptionViewItem& option,
                             const QModelIndex& index) const override {
        editor->setGeometry(option.rect);
    }
};
```

### Ví Dụ 3: Custom Model với Con Trỏ

```cpp
#include <QAbstractTableModel>
#include <QVector>

class CustomTableModel : public QAbstractTableModel {
    Q_OBJECT
    
public:
    CustomTableModel(QObject* parent = nullptr) 
        : QAbstractTableModel(parent) {}
    
    int rowCount(const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_data.size();
    }
    
    int columnCount(const QModelIndex& parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return 3; // 3 cột
    }
    
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_data.size()) {
            return QVariant();
        }
        
        if (role == Qt::DisplayRole || role == Qt::EditRole) {
            return m_data[index.row()][index.column()];
        }
        
        return QVariant();
    }
    
    bool setData(const QModelIndex& index, const QVariant& value, int role) override {
        if (!index.isValid() || role != Qt::EditRole) {
            return false;
        }
        
        m_data[index.row()][index.column()] = value.toString();
        emit dataChanged(index, index, {role});
        return true;
    }
    
    Qt::ItemFlags flags(const QModelIndex& index) const override {
        if (!index.isValid()) {
            return Qt::NoItemFlags;
        }
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
    }
    
    void addRow(const QString& col1, const QString& col2, const QString& col3) {
        beginInsertRows(QModelIndex(), m_data.size(), m_data.size());
        m_data.append({col1, col2, col3});
        endInsertRows();
    }
    
private:
    QVector<QVector<QString>> m_data; // Dữ liệu nội bộ
};
```

### Ví Dụ 4: Quản Lý Nhiều View với 1 Model

```cpp
class MultiViewWidget : public QWidget {
    Q_OBJECT
    
public:
    MultiViewWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // ✅ 1 model được share
        m_sharedModel = new QStandardItemModel(this);
        populateModel();
        
        // ✅ Nhiều view dùng chung model
        m_tableView = new QTableView(this);
        m_treeView = new QTreeView(this);
        m_listView = new QListView(this);
        
        m_tableView->setModel(m_sharedModel);
        m_treeView->setModel(m_sharedModel);
        m_listView->setModel(m_sharedModel);
        
        // Layout
        QHBoxLayout* layout = new QHBoxLayout(this);
        layout->addWidget(m_tableView);
        layout->addWidget(m_treeView);
        layout->addWidget(m_listView);
        
        // ✅ Khi model thay đổi, tất cả view tự động cập nhật
        connect(m_sharedModel, &QStandardItemModel::dataChanged,
                this, &MultiViewWidget::onDataChanged);
    }
    
private slots:
    void onDataChanged(const QModelIndex& topLeft, 
                      const QModelIndex& bottomRight) {
        qDebug() << "Data changed in model, all views updated automatically!";
    }
    
private:
    QStandardItemModel* m_sharedModel; // ✅ 1 model
    QTableView* m_tableView;           // ✅ View 1
    QTreeView* m_treeView;             // ✅ View 2
    QListView* m_listView;             // ✅ View 3
    
    void populateModel() {
        // Populate data...
    }
};
```

---

## ✅ Best Practices {#best-practices}

### 1. Ownership Rules (Quy Tắc Sở Hữu)

```
Widget (Parent)
  ├── Model (child của Widget) ✅
  ├── View (child của Widget) ✅
  │   └── Delegate (child của View) ✅
  └── Layout (child của Widget) ✅
```

**Quy tắc**:
- ✅ Luôn truyền parent khi tạo object
- ✅ Không delete object có parent thủ công
- ✅ Qt tự động cleanup khi parent bị destroy

### 2. Model Lifetime

```cpp
// ✅ ĐÚNG: Model sống lâu hơn view
QStandardItemModel* model = new QStandardItemModel(this);
QTableView* view = new QTableView(this);
view->setModel(model);

// ❌ SAI: Model bị delete trước view
QStandardItemModel* model = new QStandardItemModel();
QTableView* view = new QTableView(this);
view->setModel(model);
delete model; // View sẽ crash!
```

### 3. Index Safety

```cpp
// ✅ ĐÚNG: Kiểm tra index
QModelIndex index = model->index(row, col);
if (index.isValid()) {
    QVariant data = index.data();
}

// ✅ ĐÚNG: Dùng persistent index nếu cần lưu
QPersistentModelIndex persistent = model->index(0, 0);

// ❌ SAI: Lưu QModelIndex lâu dài
QModelIndex saved = model->index(0, 0);
// ... sau khi model thay đổi cấu trúc ...
// saved có thể đã invalid!
```

### 4. Smart Pointers (Optional)

```cpp
#include <QPointer>

// QPointer tự động set về nullptr khi object bị delete
QPointer<QStandardItemModel> model = new QStandardItemModel(this);

// Kiểm tra an toàn
if (model) {
    // Model vẫn còn sống
    model->setData(index, value);
}
```

### 5. Signal/Slot với Con Trỏ

```cpp
// ✅ ĐÚNG: Connect với con trỏ thành viên
connect(m_model, &QStandardItemModel::dataChanged,
        this, &MyWidget::onDataChanged);

// ✅ ĐÚNG: Lambda với capture con trỏ
connect(m_view, &QTableView::clicked, this, [this](const QModelIndex& index) {
    if (index.isValid() && m_model) {
        // Sử dụng m_model an toàn
    }
});
```

---

## 📝 Tóm Tắt Nhanh {#tóm-tắt-nhanh}

### Checklist Nhanh

| Thành Phần | Parent | Ownership | Lưu Ý |
|------------|--------|-----------|-------|
| **Model** | Widget | Widget sở hữu | Phải sống lâu hơn View |
| **View** | Widget | Widget sở hữu | Không sở hữu Model |
| **Delegate** | View | View sở hữu | Có thể share giữa Views |
| **Index** | - | Không sở hữu | Kiểm tra isValid() |
| **Editor** | View | View sở hữu | Tự động cleanup |

### Code Pattern Chuẩn

```cpp
class MyWidget : public QWidget {
    Q_OBJECT
    
public:
    MyWidget(QWidget* parent = nullptr) : QWidget(parent) {
        // 1. Tạo Model với parent
        m_model = new QStandardItemModel(this);
        
        // 2. Tạo View với parent
        m_view = new QTableView(this);
        
        // 3. Set model cho view
        m_view->setModel(m_model);
        
        // 4. Tạo Delegate với parent là view
        m_delegate = new MyCustomDelegate(m_view);
        m_view->setItemDelegate(m_delegate);
        
        // 5. Setup layout
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(m_view);
    }
    
    // ✅ Không cần destructor - Qt tự động cleanup
    
private:
    QStandardItemModel* m_model;
    QTableView* m_view;
    MyCustomDelegate* m_delegate;
};
```

### Các Lỗi Thường Gặp

1. ❌ **Delete object có parent**
   ```cpp
   delete m_model; // SAI nếu m_model có parent
   ```

2. ❌ **Model bị delete trước View**
   ```cpp
   QStandardItemModel* model = new QStandardItemModel();
   view->setModel(model);
   delete model; // View sẽ crash!
   ```

3. ❌ **Không kiểm tra index**
   ```cpp
   QVariant data = index.data(); // SAI nếu index invalid
   ```

4. ❌ **Lưu QModelIndex lâu dài**
   ```cpp
   QModelIndex saved = model->index(0, 0);
   // Sau khi model thay đổi, saved có thể invalid
   ```

### Tips Học Nhanh

1. **Vẽ sơ đồ**: Vẽ parent-child tree để hiểu ownership
2. **Dùng debugger**: Xem object nào được delete khi nào
3. **Đọc Qt docs**: Tìm "parent" trong constructor
4. **Practice**: Viết code nhỏ, test từng phần
5. **Memory check**: Dùng Valgrind hoặc AddressSanitizer

---

## 🎓 Bài Tập Thực Hành

### Bài 1: Tạo Simple Table
- Tạo model với 3 cột
- Tạo view hiển thị model
- Thêm 5 dòng dữ liệu

### Bài 2: Custom Delegate
- Tạo delegate cho cột số (QSpinBox)
- Tạo delegate cho cột ngày (QDateEdit)
- Áp dụng cho các cột khác nhau

### Bài 3: Multi-View
- Tạo 1 model
- Tạo 3 view khác nhau (Table, Tree, List)
- Khi edit ở view này, view khác tự động cập nhật

---

## 📚 Tài Liệu Tham Khảo

- [Qt Model/View Programming](https://doc.qt.io/qt-6/model-view-programming.html)
- [QAbstractItemModel](https://doc.qt.io/qt-6/qabstractitemmodel.html)
- [QAbstractItemView](https://doc.qt.io/qt-6/qabstractitemview.html)
- [QAbstractItemDelegate](https://doc.qt.io/qt-6/qabstractitemdelegate.html)

---

**Chúc bạn học tốt! 🚀**
