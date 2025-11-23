# 🎨 Visual Guide - Hiểu Con Trỏ Qua Hình Ảnh

> Học bằng hình ảnh giúp hiểu nhanh hơn 3 lần!

---

## 🏗️ Kiến Trúc Model-View-Delegate

```
┌─────────────────────────────────────────────────────────┐
│                    Qt Application                        │
│                                                          │
│  ┌──────────────┐      ┌──────────────┐                │
│  │  QTableView  │      │  QListView   │                │
│  │   (View 1)   │      │   (View 2)   │                │
│  │              │      │              │                │
│  │  m_model: ●──┼──────┼───● m_model  │                │
│  └──────────────┘      └──────────────┘                │
│         │                     │                         │
│         │      Con trỏ        │                         │
│         └──────────┬──────────┘                         │
│                    ↓                                     │
│          ┌─────────────────────┐                        │
│          │ QStandardItemModel  │                        │
│          │      (Model)        │                        │
│          │                     │                        │
│          │  ┌────┬────┬────┐  │                        │
│          │  │ An │ 20 │ 8.5│  │                        │
│          │  ├────┼────┼────┤  │                        │
│          │  │Bình│ 22 │ 7.0│  │                        │
│          │  ├────┼────┼────┤  │                        │
│          │  │Chi │ 19 │ 9.0│  │                        │
│          │  └────┴────┴────┘  │                        │
│          └─────────────────────┘                        │
│                    △                                     │
│                    │                                     │
│              ┌─────┴──────┐                             │
│              │ Delegate   │                             │
│              │ (Tùy chỉnh)│                             │
│              └────────────┘                             │
└─────────────────────────────────────────────────────────┘

📝 GIẢI THÍCH:
● = Con trỏ (pointer)
→ = Trỏ đến (points to)
2 Views cùng trỏ đến 1 Model = Chia sẻ dữ liệu!
```

---

## 🔗 Con Trỏ Là Gì?

### Concept: Con Trỏ = Địa Chỉ

```
MEMORY (RAM)
┌─────────────────────────────────────┐
│ Address  │  Content                 │
├──────────┼──────────────────────────┤
│ 0x1000   │  [int] 42                │
│ 0x1004   │  [int] 100               │
│ 0x1008   │  [Object] Model {...}  ◄─┼─── Con trỏ trỏ đến đây!
│ 0x100C   │  [int] 25                │
│ 0x1010   │  [Object] View {...}     │
└──────────┴──────────────────────────┘

QStandardItemModel* ptr = 0x1008;
                    ^       ^
                    |       └─ Địa chỉ bộ nhớ
                    └───────── Con trỏ lưu địa chỉ
```

### Visual: Raw Pointer

```
┌─────────────────────┐
│   QTableView* view  │  ← Biến con trỏ
│                     │
│   value: 0x5000  ───┼───┐
└─────────────────────┘   │
                          │
                          ↓
                    ┌─────────────────┐
                    │  QTableView     │  ← Object thực
                    │  [at 0x5000]    │
                    │                 │
                    │  m_model: ●─────┼───→ Model object
                    │  m_delegate: ●──┼───→ Delegate object
                    └─────────────────┘
```

---

## 🔄 Chia Sẻ Model Qua Con Trỏ

### Scenario: Một Model, Nhiều Views

```
CODE:
QStandardItemModel* model = new QStandardItemModel();
view1->setModel(model);
view2->setModel(model);
view3->setModel(model);

MEMORY:
┌──────────────┐
│ view1        │
│ m_model: ●───┼─┐
└──────────────┘ │
                 │
┌──────────────┐ │      ┌─────────────────┐
│ view2        │ │      │  MODEL          │
│ m_model: ●───┼─┼──────▶  [at 0x2000]    │
└──────────────┘ │      │                 │
                 │      │  Data:          │
┌──────────────┐ │      │  ├─ Row 0      │
│ view3        │ │      │  ├─ Row 1      │
│ m_model: ●───┼─┘      │  └─ Row 2      │
└──────────────┘        └─────────────────┘

✅ TẤT CẢ đều trỏ đến 0x2000
✅ Thay đổi 1 chỗ → cả 3 views thấy!
```

---

## 👨‍👩‍👧‍👦 Parent-Child Ownership

### Visual: Parent Sở Hữu Children

```
CODE:
QWidget* parent = new QWidget();
QTableView* child1 = new QTableView(parent);  // ← parent parameter
QTableView* child2 = new QTableView(parent);  // ← parent parameter

delete parent;  // Children tự động bị xóa!

MEMORY STRUCTURE:
┌─────────────────────────────────────────┐
│         QWidget (Parent)                │
│         [at 0x1000]                     │
│                                         │
│  m_children: [                          │
│    ┌──────────────────────┐             │
│    │  QTableView (Child1) │             │
│    │  [at 0x2000]         │             │
│    │  m_parent: 0x1000 ───┼───┐         │
│    └──────────────────────┘   │         │
│                               │         │
│    ┌──────────────────────┐   │         │
│    │  QTableView (Child2) │   │         │
│    │  [at 0x3000]         │   │         │
│    │  m_parent: 0x1000 ───┼───┤         │
│    └──────────────────────┘   │         │
│  ]                            │         │
└───────────────────────────────┼─────────┘
                                │
         Parent trỏ ngược về ◄──┘

🔥 delete parent:
   1. Tự động delete child1
   2. Tự động delete child2
   3. Không cần delete thủ công!
```

---

## 🆚 Types of Pointers

### 1. Raw Pointer (T*)

```
QTableView* ptr = new QTableView();

┌─────────────┐
│  ptr        │──────▶ [QTableView object]
└─────────────┘

delete ptr;  // Phải xóa thủ công!

┌─────────────┐
│  ptr        │──────▶ [❌ Đã xóa - DANGLING!]
└─────────────┘
                Nguy hiểm: ptr vẫn giữ địa chỉ cũ
```

### 2. QPointer (Safe Pointer)

```
QPointer<QTableView> ptr = new QTableView();

┌─────────────┐
│ QPointer    │──────▶ [QTableView object]
│  ptr        │        [có guard]
└─────────────┘

delete ptr;  // Xóa object

┌─────────────┐
│ QPointer    │──────▶ nullptr
│  ptr        │        ✅ Tự động set về null!
└─────────────┘
                An toàn: ptr == nullptr
```

### 3. QSharedPointer (Reference Counting)

```
QSharedPointer<Model> ptr1(new Model);
QSharedPointer<Model> ptr2 = ptr1;
QSharedPointer<Model> ptr3 = ptr1;

┌──────────┐
│  ptr1    │─┐
└──────────┘ │
             │
┌──────────┐ │      ┌─────────────────┐
│  ptr2    │─┼──────▶  Model Object   │
└──────────┘ │      │  ref_count: 3   │
             │      └─────────────────┘
┌──────────┐ │
│  ptr3    │─┘
└──────────┘

ptr1 ra khỏi scope → ref_count: 2
ptr2 ra khỏi scope → ref_count: 1
ptr3 ra khỏi scope → ref_count: 0 → TỰ ĐỘNG DELETE!
```

---

## 🔍 Null Pointer

### Nguy Hiểm: Dereference Null

```
QAbstractItemModel* model = view->model();  // Có thể = nullptr!

❌ KHÔNG NULL CHECK:
model->setData(...);  // CRASH nếu model = nullptr!

     ┌─────────┐
     │ model   │──────▶ nullptr (0x0)
     └─────────┘
            │
            └────────▶ Crash! ☠️


✅ CÓ NULL CHECK:
if (model != nullptr) {
    model->setData(...);  // An toàn!
}

     ┌─────────┐
     │ model   │──────▶ nullptr ?
     └─────────┘
            │
            ├─▶ Yes: Skip
            └─▶ No: Execute safely ✅
```

---

## 🎨 Delegate Paint Function

### Pointer Flow in paint()

```
CODE:
void paint(QPainter* painter, ...) {
    painter->fillRect(...);
    painter->drawText(...);
}

VISUAL:
┌──────────────────────┐
│    Qt Framework      │
│                      │
│  1. Tạo QPainter     │
│     ┌────────────┐   │
│     │  QPainter  │   │
│     │ [0x7000]   │   │
│     └────────────┘   │
│          │           │
│          ↓           │
│  2. Gọi paint()      │
│     với pointer      │
└──────────┼───────────┘
           │
           ↓
┌──────────┼────────────────────────┐
│  Your Code                        │
│                                   │
│  void paint(QPainter* painter) {  │
│      ^                            │
│      │ Nhận con trỏ từ Qt        │
│      │                           │
│    painter->fillRect(...);       │
│           ^                      │
│           └─ Dùng -> để gọi      │
│  }                               │
└───────────────────────────────────┘
           │
           ↓
┌──────────┼───────────┐
│    Qt Framework      │
│                      │
│  3. Qt tự động xóa   │
│     QPainter         │
│                      │
│  ❌ delete painter   │
│     KHÔNG CẦN!       │
└──────────────────────┘
```

---

## 🔗 Proxy Model Chain

### Chain of Pointers

```
┌──────────────┐
│  QTableView  │
│              │
│  m_model: ●──┼────┐
└──────────────┘    │
                    ↓
              ┌─────────────────────┐
              │ QSortFilterProxy    │
              │ Model               │
              │                     │
              │ m_sourceModel: ●────┼────┐
              └─────────────────────┘    │
                                         ↓
                                   ┌─────────────────┐
                                   │ QStandardItem   │
                                   │ Model           │
                                   │                 │
                                   │ (Actual data)   │
                                   └─────────────────┘

VIEW → PROXY → SOURCE
  ●  →   ●   →   ●    (Chain of pointers!)

Khi get data:
1. View hỏi Proxy
2. Proxy transform & hỏi Source
3. Source trả data
4. Proxy transform & trả về View
```

---

## 💥 Common Pointer Errors

### Error 1: Memory Leak

```
❌ SAI:
QStandardItemModel* model = new QStandardItemModel();
// ... forget to delete

MEMORY:
┌─────────────┐
│   model     │──────▶ [Model object]
└─────────────┘        [LEAKED! 🔥]
     │
     Function ends
     │
     ↓
┌─────────────┐
│   (gone)    │   ╳╳▶ [Model object]
└─────────────┘        [Still in memory! LEAK!]


✅ ĐÚNG - Dùng parent:
QStandardItemModel* model = new QStandardItemModel(parent);

MEMORY:
┌─────────────┐        ┌─────────────┐
│   parent    │───┐    │   model     │
└─────────────┘   │    └─────────────┘
                  │           △
                  └───────────┘
                   Owns child

delete parent → model tự động xóa! ✅
```

### Error 2: Dangling Pointer

```
❌ SAI:
QTableView* view = new QTableView();
delete view;
view->show();  // CRASH! ☠️

MEMORY TIMELINE:

T1: Tạo object
┌─────────┐
│  view   │──────▶ [QTableView at 0x5000]
└─────────┘        ✅ Valid

T2: Delete object
┌─────────┐
│  view   │──────▶ [❌ Freed memory]
└─────────┘        (Dangling!)

T3: Dereference
view->show()
  ^
  │
  Crash! Địa chỉ không hợp lệ ☠️


✅ ĐÚNG - Dùng QPointer:
QPointer<QTableView> view = new QTableView();
delete view;
if (view) {  // false
    view->show();  // Không chạy
}

┌─────────┐
│  view   │──────▶ nullptr ✅
└─────────┘        Safe!
```

---

## 🧪 Quiz: Pointer Puzzle

### Puzzle 1
```
QStandardItemModel* m1 = new QStandardItemModel();
QStandardItemModel* m2 = m1;

m1 == m2 ?
```

<details>
<summary>Đáp án</summary>

**TRUE!**

```
┌────┐       ┌─────────────────┐
│ m1 │──────▶│  Model object   │
└────┘   ┌──▶│  [at 0x1000]    │
         │   └─────────────────┘
┌────┐   │
│ m2 │───┘
└────┘

Cùng địa chỉ → m1 == m2
```
</details>

### Puzzle 2
```
view1->setModel(model);
view2->setModel(model);

// Sửa data qua view1
view1->model()->setData(...);

// view2 có thấy thay đổi không?
```

<details>
<summary>Đáp án</summary>

**CÓ!**

```
view1 ──┐
        ├──▶ [Same Model] ◀── Data changed here!
view2 ──┘                    All views see it! ✅
```
</details>

---

## 📊 Memory Layout Summary

```
STACK (Local variables)
┌─────────────────────────────┐
│ QStandardItemModel* model;  │──┐
│ QTableView* view;           │──┼─┐
│ int x = 42;                 │  │ │
└─────────────────────────────┘  │ │
                                 │ │
HEAP (Dynamic allocation)        │ │
┌─────────────────────────────┐  │ │
│ [QStandardItemModel object] │◀─┘ │
│ [at 0x1000]                 │    │
│                             │    │
│ [QTableView object]         │◀───┘
│ [at 0x2000]                 │
│   m_model: 0x1000 ──────────┼────→ Points back
└─────────────────────────────┘

CON TRỎ LƯU Ở STACK
OBJECT THỰC Ở HEAP
```

---

## 🎯 Mental Model: Khi Nào Dùng Con Trỏ?

```
                    CẦN CON TRỎ?
                         │
         ┌───────────────┼───────────────┐
         │                               │
    Có cần chia sẻ?              Chỉ 1 owner?
         │                               │
         YES                             NO
         │                               │
    Raw Pointer*                   Stack object
    (với parent)                   hoặc unique_ptr
         │
    ┌────┴────┐
    │         │
Lo ngại    Nhiều owners
dangling?  chia sẻ?
    │         │
QPointer  QSharedPointer
```

---

**Học bằng hình ảnh xong rồi! Giờ đến code thực tế! 💪**

*Xem file `simple_model_example.cpp` để thực hành!*
