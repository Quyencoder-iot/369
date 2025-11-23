// Ví dụ: Custom Delegate với con trỏ
// Compile: g++ -fPIC -std=c++17 delegate_example.cpp -o delegate_example $(pkg-config --cflags --libs Qt6Core Qt6Widgets)

#include <QApplication>
#include <QMainWindow>
#include <QTableView>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QSpinBox>
#include <QDateEdit>
#include <QModelIndex>
#include <QDebug>

// ✅ Custom Delegate cho số nguyên
class SpinBoxDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    SpinBoxDelegate(QObject* parent = nullptr) 
        : QStyledItemDelegate(parent) {
        qDebug() << "✅ SpinBoxDelegate created with parent:" << parent;
    }
    
    // ✅ Tạo editor widget - editor được parent sở hữu
    QWidget* createEditor(QWidget* parent, 
                         const QStyleOptionViewItem& option,
                         const QModelIndex& index) const override {
        Q_UNUSED(option);
        Q_UNUSED(index);
        
        // ✅ Editor được parent widget sở hữu
        // Qt sẽ tự động delete editor khi không cần nữa
        QSpinBox* editor = new QSpinBox(parent);
        editor->setMinimum(0);
        editor->setMaximum(150);
        editor->setFrame(false);
        
        qDebug() << "✅ SpinBox editor created with parent:" << parent;
        return editor;
    }
    
    // ✅ Set dữ liệu từ model vào editor
    void setEditorData(QWidget* editor, 
                      const QModelIndex& index) const override {
        QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
        
        // ✅ Lấy dữ liệu từ model thông qua index
        int value = index.model()->data(index, Qt::EditRole).toInt();
        spinBox->setValue(value);
        
        qDebug() << "✅ Set editor data:" << value;
    }
    
    // ✅ Lấy dữ liệu từ editor về model
    void setModelData(QWidget* editor, 
                     QAbstractItemModel* model,
                     const QModelIndex& index) const override {
        QSpinBox* spinBox = static_cast<QSpinBox*>(editor);
        spinBox->interpretText();
        int value = spinBox->value();
        
        // ✅ Set dữ liệu vào model
        model->setData(index, value, Qt::EditRole);
        
        qDebug() << "✅ Set model data:" << value;
    }
    
    // ✅ Cập nhật vị trí và kích thước editor
    void updateEditorGeometry(QWidget* editor,
                             const QStyleOptionViewItem& option,
                             const QModelIndex& index) const override {
        Q_UNUSED(index);
        editor->setGeometry(option.rect);
    }
};

// ✅ Custom Delegate cho ngày tháng
class DateEditDelegate : public QStyledItemDelegate {
    Q_OBJECT
    
public:
    DateEditDelegate(QObject* parent = nullptr) 
        : QStyledItemDelegate(parent) {}
    
    QWidget* createEditor(QWidget* parent, 
                         const QStyleOptionViewItem& option,
                         const QModelIndex& index) const override {
        Q_UNUSED(option);
        Q_UNUSED(index);
        
        QDateEdit* editor = new QDateEdit(parent);
        editor->setCalendarPopup(true);
        editor->setDate(QDate::currentDate());
        
        return editor;
    }
    
    void setEditorData(QWidget* editor, 
                      const QModelIndex& index) const override {
        QDateEdit* dateEdit = static_cast<QDateEdit*>(editor);
        QDate date = index.model()->data(index, Qt::EditRole).toDate();
        dateEdit->setDate(date);
    }
    
    void setModelData(QWidget* editor, 
                     QAbstractItemModel* model,
                     const QModelIndex& index) const override {
        QDateEdit* dateEdit = static_cast<QDateEdit*>(editor);
        QDate date = dateEdit->date();
        model->setData(index, date, Qt::EditRole);
    }
    
    void updateEditorGeometry(QWidget* editor,
                             const QStyleOptionViewItem& option,
                             const QModelIndex& index) const override {
        Q_UNUSED(index);
        editor->setGeometry(option.rect);
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT
    
public:
    MainWindow(QWidget* parent = nullptr) : QMainWindow(parent) {
        setWindowTitle("Ví Dụ: Custom Delegates");
        resize(700, 400);
        
        // ✅ Tạo Model
        m_model = new QStandardItemModel(this);
        m_model->setHorizontalHeaderLabels({"Tên", "Tuổi", "Ngày Sinh"});
        
        // Thêm dữ liệu
        addRow("Nguyễn Văn A", 25, QDate(1998, 5, 15));
        addRow("Trần Thị B", 30, QDate(1993, 8, 20));
        addRow("Lê Văn C", 28, QDate(1995, 12, 10));
        
        // ✅ Tạo View
        m_view = new QTableView(this);
        m_view->setModel(m_model);
        
        // ✅ Tạo Delegates với parent là view
        // View sẽ sở hữu và quản lý delegates
        m_spinDelegate = new SpinBoxDelegate(m_view);
        m_dateDelegate = new DateEditDelegate(m_view);
        
        // ✅ Set delegate cho từng cột
        m_view->setItemDelegateForColumn(1, m_spinDelegate);  // Cột Tuổi
        m_view->setItemDelegateForColumn(2, m_dateDelegate);  // Cột Ngày Sinh
        
        // ✅ Default delegate cho các cột khác
        m_view->setItemDelegate(new QStyledItemDelegate(m_view));
        
        setCentralWidget(m_view);
        
        qDebug() << "✅ Delegates được tạo với parent là view";
        qDebug() << "✅ View sẽ tự động delete delegates khi bị destroy";
    }
    
private:
    void addRow(const QString& name, int age, const QDate& birthDate) {
        QList<QStandardItem*> items;
        items << new QStandardItem(name);
        items << new QStandardItem(QString::number(age));
        items << new QStandardItem(birthDate.toString("yyyy-MM-dd"));
        m_model->appendRow(items);
    }
    
    QStandardItemModel* m_model;
    QTableView* m_view;
    SpinBoxDelegate* m_spinDelegate;  // ✅ Delegate được view sở hữu
    DateEditDelegate* m_dateDelegate; // ✅ Delegate được view sở hữu
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}

#include "delegate_example.moc"
