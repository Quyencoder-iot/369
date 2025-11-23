// Ví dụ thực tế về Con Trỏ và Tham Chiếu trong C++ và Qt
// Compile: g++ -std=c++11 vi-du-con-tro.cpp -o vi-du

#include <iostream>
#include <memory>
#include <vector>
#include <string>

// ============================================
// PHẦN 1: CON TRỎ CƠ BẢN
// ============================================

void viDuConTroCoBan() {
    std::cout << "\n=== PHẦN 1: CON TRỎ CƠ BẢN ===\n";
    
    int value = 42;
    int* ptr = &value;  // Con trỏ lưu địa chỉ của value
    
    std::cout << "Giá trị của value: " << value << std::endl;
    std::cout << "Địa chỉ của value: " << &value << std::endl;
    std::cout << "Giá trị của ptr (địa chỉ): " << ptr << std::endl;
    std::cout << "Giá trị tại địa chỉ ptr trỏ đến: " << *ptr << std::endl;
    
    // Thay đổi giá trị qua con trỏ
    *ptr = 100;
    std::cout << "Sau khi *ptr = 100, value = " << value << std::endl;
}

// ============================================
// PHẦN 2: CON TRỎ vs THAM CHIẾU
// ============================================

// Hàm nhận tham chiếu
void tangGiaTriThamChieu(int& ref) {
    ref += 10;  // Thay đổi trực tiếp, không cần *
    std::cout << "Trong hàm (tham chiếu): ref = " << ref << std::endl;
}

// Hàm nhận con trỏ
void tangGiaTriConTro(int* ptr) {
    if (ptr != nullptr) {  // Phải kiểm tra NULL
        *ptr += 10;  // Cần dereference với *
        std::cout << "Trong hàm (con trỏ): *ptr = " << *ptr << std::endl;
    }
}

void soSanhConTroVaThamChieu() {
    std::cout << "\n=== PHẦN 2: CON TRỎ vs THAM CHIẾU ===\n";
    
    int value1 = 10;
    int value2 = 10;
    
    std::cout << "Giá trị ban đầu: value1 = " << value1 << ", value2 = " << value2 << std::endl;
    
    // Dùng tham chiếu
    tangGiaTriThamChieu(value1);
    std::cout << "Sau hàm tham chiếu: value1 = " << value1 << std::endl;
    
    // Dùng con trỏ
    tangGiaTriConTro(&value2);  // Truyền địa chỉ
    std::cout << "Sau hàm con trỏ: value2 = " << value2 << std::endl;
    
    // Tham chiếu không thể NULL
    int& ref = value1;  // Phải khởi tạo ngay
    std::cout << "Tham chiếu ref trỏ đến giá trị: " << ref << std::endl;
    // int& ref2;       // LỖI! Không thể khai báo mà không khởi tạo
    
    // Con trỏ có thể NULL
    int* ptr = nullptr;  // Hợp lệ
    std::cout << "Con trỏ ptr có thể là nullptr: " << (ptr == nullptr ? "true" : "false") << std::endl;
    // tangGiaTriConTro(ptr);  // An toàn vì có kiểm tra NULL
}

// ============================================
// PHẦN 3: CON TRỎ VỚI NULL/nullptr
// ============================================

void viDuNullptr() {
    std::cout << "\n=== PHẦN 3: CON TRỎ VỚI nullptr ===\n";
    
    int* ptr = nullptr;  // Khởi tạo với nullptr
    
    // Luôn kiểm tra trước khi dùng
    if (ptr != nullptr) {
        *ptr = 10;
    } else {
        std::cout << "Con trỏ là nullptr, không thể sử dụng" << std::endl;
    }
    
    // Bây giờ gán giá trị hợp lệ
    int value = 42;
    ptr = &value;
    
    if (ptr != nullptr) {
        std::cout << "Con trỏ hợp lệ, giá trị: " << *ptr << std::endl;
    }
}

// ============================================
// PHẦN 4: QUẢN LÝ BỘ NHỚ ĐỘNG
// ============================================

void quanLyBoNhoDong() {
    std::cout << "\n=== PHẦN 4: QUẢN LÝ BỘ NHỚ ĐỘNG ===\n";
    
    // Cách cũ - nguy hiểm nếu quên delete
    int* arr = new int[5];
    for (int i = 0; i < 5; i++) {
        arr[i] = i * 10;
    }
    
    std::cout << "Mảng động: ";
    for (int i = 0; i < 5; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;
    
    delete[] arr;  // QUAN TRỌNG: Phải giải phóng!
    arr = nullptr;  // Tốt: Gán nullptr sau khi delete
    
    // Cách mới - an toàn với smart pointer (C++11)
    std::unique_ptr<int[]> smartArr(new int[5]);
    for (int i = 0; i < 5; i++) {
        smartArr[i] = i * 20;
    }
    
    std::cout << "Mảng với smart pointer: ";
    for (int i = 0; i < 5; i++) {
        std::cout << smartArr[i] << " ";
    }
    std::cout << std::endl;
    // Tự động giải phóng khi ra khỏi scope
}

// ============================================
// PHẦN 5: POLYMORPHISM VỚI CON TRỎ
// ============================================

class Animal {
public:
    virtual void makeSound() = 0;
    virtual ~Animal() {}  // Virtual destructor quan trọng!
};

class Dog : public Animal {
public:
    void makeSound() override {
        std::cout << "Woof! Woof!" << std::endl;
    }
};

class Cat : public Animal {
public:
    void makeSound() override {
        std::cout << "Meow! Meow!" << std::endl;
    }
};

void viDuPolymorphism() {
    std::cout << "\n=== PHẦN 5: POLYMORPHISM VỚI CON TRỎ ===\n";
    
    // Con trỏ base class có thể trỏ đến derived class
    Animal* pet1 = new Dog();
    Animal* pet2 = new Cat();
    
    pet1->makeSound();  // "Woof! Woof!"
    pet2->makeSound();  // "Meow! Meow!"
    
    // Mảng con trỏ - rất hữu ích
    Animal* pets[] = {new Dog(), new Cat(), new Dog()};
    std::cout << "\nMảng động vật:\n";
    for (int i = 0; i < 3; i++) {
        pets[i]->makeSound();
    }
    
    // Giải phóng bộ nhớ
    delete pet1;
    delete pet2;
    for (int i = 0; i < 3; i++) {
        delete pets[i];
    }
}

// ============================================
// PHẦN 6: VÍ DỤ QT-STYLE (Không cần Qt để hiểu)
// ============================================

// Giả lập cách Qt quản lý đối tượng
class QObject {
public:
    QObject(QObject* parent = nullptr) : m_parent(parent) {
        if (parent) {
            parent->addChild(this);
        }
    }
    
    virtual ~QObject() {
        // Qt tự động xóa tất cả children
        for (QObject* child : m_children) {
            delete child;
        }
    }
    
    void addChild(QObject* child) {
        m_children.push_back(child);
    }
    
protected:
    QObject* m_parent;
    std::vector<QObject*> m_children;  // Danh sách con trỏ
};

class QWidget : public QObject {
public:
    QWidget(QObject* parent = nullptr) : QObject(parent) {
        std::cout << "QWidget được tạo" << std::endl;
    }
    
    ~QWidget() {
        std::cout << "QWidget bị hủy" << std::endl;
    }
};

class QPushButton : public QWidget {
public:
    QPushButton(const std::string& text, QWidget* parent = nullptr) 
        : QWidget(parent), m_text(text) {
        std::cout << "QPushButton '" << m_text << "' được tạo" << std::endl;
    }
    
    ~QPushButton() {
        std::cout << "QPushButton '" << m_text << "' bị hủy" << std::endl;
    }
    
private:
    std::string m_text;
};

void viDuQtStyle() {
    std::cout << "\n=== PHẦN 6: VÍ DỤ QT-STYLE ===\n";
    
    // Tạo parent widget
    QWidget* parent = new QWidget();
    
    // Tạo children với parent
    new QPushButton("OK", parent);
    new QPushButton("Cancel", parent);
    
    std::cout << "\nChỉ cần delete parent, children tự động bị xóa:\n";
    delete parent;  // Tự động xóa button1 và button2!
    
    // Không cần delete button1 và button2 riêng
}

// ============================================
// PHẦN 7: MODEL-VIEW PATTERN (Giống Qt Model)
// ============================================

class Item {
public:
    Item(const std::string& data) : m_data(data) {}
    std::string data() const { return m_data; }
    
private:
    std::string m_data;
};

class Model {
public:
    Model() {}
    
    ~Model() {
        // Model sở hữu tất cả items, phải giải phóng
        for (Item* item : m_items) {
            delete item;
        }
    }
    
    void addItem(const std::string& data) {
        // Tạo item mới trên heap
        Item* item = new Item(data);
        m_items.push_back(item);  // Lưu con trỏ
    }
    
    Item* getItem(int index) {
        if (index >= 0 && index < m_items.size()) {
            return m_items[index];  // Trả về con trỏ
        }
        return nullptr;  // Trả về nullptr nếu không hợp lệ
    }
    
    int itemCount() const {
        return m_items.size();
    }
    
private:
    std::vector<Item*> m_items;  // Danh sách con trỏ
};

class View {
public:
    void setModel(Model* model) {
        m_model = model;  // Lưu con trỏ đến model
    }
    
    void display() {
        if (m_model == nullptr) {
            std::cout << "Không có model!" << std::endl;
            return;
        }
        
        std::cout << "\nHiển thị dữ liệu từ model:\n";
        for (int i = 0; i < m_model->itemCount(); i++) {
            Item* item = m_model->getItem(i);
            if (item != nullptr) {
                std::cout << "  [" << i << "] " << item->data() << std::endl;
            }
        }
    }
    
private:
    Model* m_model = nullptr;  // Con trỏ đến model
};

void viDuModelView() {
    std::cout << "\n=== PHẦN 7: MODEL-VIEW PATTERN ===\n";
    
    // Tạo model
    Model* model = new Model();
    model->addItem("Item 1");
    model->addItem("Item 2");
    model->addItem("Item 3");
    
    // Tạo view và gán model
    View view;
    view.setModel(model);  // Truyền con trỏ
    view.display();
    
    // Có thể có nhiều view cùng xem một model
    View view2;
    view2.setModel(model);  // Cùng một con trỏ model
    view2.display();
    
    delete model;  // Model tự động xóa tất cả items
}

// ============================================
// PHẦN 8: SMART POINTERS (C++11+)
// ============================================

void viDuSmartPointer() {
    std::cout << "\n=== PHẦN 8: SMART POINTERS ===\n";
    
    // unique_ptr - sở hữu duy nhất
    {
        std::unique_ptr<int> ptr(new int(42));
        std::cout << "unique_ptr giá trị: " << *ptr << std::endl;
        // Tự động giải phóng khi ra khỏi scope
    }
    
    // shared_ptr - chia sẻ quyền sở hữu
    {
        std::shared_ptr<int> ptr1 = std::make_shared<int>(100);
        std::shared_ptr<int> ptr2 = ptr1;  // Chia sẻ
        
        std::cout << "shared_ptr count: " << ptr1.use_count() << std::endl;
        std::cout << "Giá trị: " << *ptr1 << ", " << *ptr2 << std::endl;
        // Tự động giải phóng khi không còn ai dùng
    }
}

// ============================================
// MAIN FUNCTION
// ============================================

int main() {
    std::cout << "===========================================" << std::endl;
    std::cout << "VÍ DỤ VỀ CON TRỎ VÀ THAM CHIẾU TRONG C++" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    viDuConTroCoBan();
    soSanhConTroVaThamChieu();
    viDuNullptr();
    quanLyBoNhoDong();
    viDuPolymorphism();
    viDuQtStyle();
    viDuModelView();
    viDuSmartPointer();
    
    std::cout << "\n===========================================" << std::endl;
    std::cout << "KẾT THÚC CHƯƠNG TRÌNH" << std::endl;
    std::cout << "===========================================" << std::endl;
    
    return 0;
}
