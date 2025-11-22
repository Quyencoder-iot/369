# 🎨 Qt Models - Visual Guide

## 📊 Model Hierarchy

```
                    QAbstractItemModel
                           │
        ┌──────────────────┼──────────────────┬─────────────┐
        │                  │                  │             │
 QAbstractListModel  QAbstractTableModel  QSqlQueryModel  QIdentityProxyModel
        │                  │                  │             │
        │                  │                  │             │
   QStringListModel   (Custom Table)    QSqlTableModel   QSortFilterProxyModel
                                              │
                                              │
                                    QSqlRelationalTableModel
```

---

## 🔄 Model-View Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                                                              │
│  ┌─────────────┐         ┌─────────────┐                   │
│  │    MODEL    │◄───────►│    VIEW     │                   │
│  │             │         │             │                   │
│  │  - Data     │         │  - Display  │                   │
│  │  - Logic    │         │  - Input    │                   │
│  └─────────────┘         └─────────────┘                   │
│         ▲                       │                            │
│         │                       │                            │
│         │                       ▼                            │
│         │              ┌─────────────┐                      │
│         └──────────────│  DELEGATE   │                      │
│                        │             │                      │
│                        │  - Editing  │                      │
│                        │  - Painting │                      │
│                        └─────────────┘                      │
│                                                              │
└─────────────────────────────────────────────────────────────┘
```

### Signals & Slots Flow

```
MODEL emits                         VIEW receives
─────────────────────────────────────────────────────
dataChanged(topLeft, bottomRight) → Updates cells
rowsInserted(parent, first, last) → Adds rows to display
rowsRemoved(parent, first, last)  → Removes rows from display
layoutChanged()                   → Re-layouts entire view
modelReset()                      → Reloads everything
```

---

## 📝 List Model Structure

```
QAbstractListModel
    │
    ├─ m_data: QVector<Item>
    │
    ├─ rowCount() ─────┐
    │                  │
    └─ data(index)     │
           │           │
           └───────────┼─────► View displays items
                       │
                   0: Item 1
                   1: Item 2
                   2: Item 3
                   3: Item 4
```

### Data Access Pattern

```
View requests: index(row=1, col=0)
                    │
                    ▼
Model::data(index, Qt::DisplayRole)
                    │
                    ▼
        return m_data[1].text
                    │
                    ▼
        View shows: "Item 2"
```

---

## 📊 Table Model Structure

```
QAbstractTableModel
    │
    ├─ m_rows: QVector<Row>
    │
    ├─ rowCount() ────────┐
    ├─ columnCount() ─────┤
    └─ data(index)        │
           │              │
           └──────────────┼──────► View displays table
                          │
                    ┌─────┴─────┬─────┬─────┐
                    │  Col 0    │ Col 1│ Col 2│
                ────┼───────────┼─────┼──────┤
                Row 0│ Cell(0,0) │ ... │ ...  │
                Row 1│ Cell(1,0) │ ... │ ...  │
                Row 2│ Cell(2,0) │ ... │ ...  │
```

### Cell Access Pattern

```
View requests: index(row=1, col=2)
                    │
                    ▼
Model::data(index, Qt::DisplayRole)
                    │
                    ▼
        row = 1, column = 2
                    │
                    ▼
    return m_rows[1].columns[2]
                    │
                    ▼
        View shows cell value
```

---

## 🌲 Tree Model Structure

```
                        Root (Invalid Index)
                           │
        ┌──────────────────┼──────────────────┐
        │                  │                  │
    Node A              Node B              Node C
    row=0               row=1               row=2
    parent=Root         parent=Root         parent=Root
        │                  │
        ├─ Node A1         ├─ Node B1
        │  row=0           │  row=0
        │  parent=A        │  parent=B
        │                  │
        └─ Node A2         └─ Node B2
           row=1              row=1
           parent=A           parent=B
```

### Tree Node Structure

```cpp
struct Node {
    QString data;
    Node *parent;           ─┐
    QVector<Node*> children;│
                            │
    int row() const {       │  Used to find position
        return parent->     │  in parent's children
        children.indexOf(this);
    }                       ─┘
}
```

### Index Creation Flow

```
View wants Node B1's index
        │
        ▼
model->index(row=0, col=0, parent=NodeB_Index)
        │
        ▼
Get parent node from parent.internalPointer() → Node B
        │
        ▼
Get child: Node B->children[0] → Node B1
        │
        ▼
createIndex(0, 0, Node B1 pointer)
        │
        ▼
Returns QModelIndex with internalPointer = Node B1
```

### Parent Finding Flow

```
View wants parent of Node B1
        │
        ▼
model->parent(NodeB1_Index)
        │
        ▼
Get node from index.internalPointer() → Node B1
        │
        ▼
Get parent: Node B1->parent → Node B
        │
        ▼
Find row of Node B in Root's children → 1
        │
        ▼
createIndex(1, 0, Node B pointer)
        │
        ▼
Returns QModelIndex for Node B
```

---

## 🗄️ SQL Model Architecture

### QSqlQueryModel (Read-Only)

```
┌──────────────┐
│   Database   │
│              │
│  SELECT *    │
│  FROM users  │
│  WHERE ...   │
└──────┬───────┘
       │ Query
       ▼
┌──────────────┐
│ QSqlQuery    │
│ Model        │
│              │
│ Read-Only    │
└──────┬───────┘
       │
       ▼
┌──────────────┐
│  QTableView  │
│              │
│   Display    │
└──────────────┘
```

### QSqlTableModel (Editable)

```
┌──────────────────────────────────────────┐
│           Database Table                  │
│                                          │
│  ID │ Name    │ Age │ Department        │
│  ───┼─────────┼─────┼──────────         │
│  1  │ Alice   │ 25  │ IT                │
│  2  │ Bob     │ 30  │ HR                │
└──────────────┬───────────────────────────┘
               │
               │ SELECT * FROM table
               ▼
┌──────────────────────────────────────────┐
│        QSqlTableModel (Cache)            │
│                                          │
│  ┌────────────────────────────┐         │
│  │ Cached rows                │         │
│  │ Modified flags             │         │
│  │ Pending inserts/deletes    │         │
│  └────────────────────────────┘         │
│                                          │
│  setData() ─► Modify cache              │
│  insertRow() ─► Add to cache            │
│  removeRow() ─► Mark for delete         │
│                                          │
│  submitAll() ─────┐                     │
│  revertAll() ◄────┘                     │
└──────────────┬───────────────────────────┘
               │
               ▼
┌──────────────────────────────────────────┐
│           QTableView                     │
│                                          │
│  User sees cached data                   │
│  Can edit, changes stored in cache       │
└──────────────────────────────────────────┘
```

### QSqlRelationalTableModel (Foreign Keys)

```
Table: employees                    Table: departments
┌──────┬────────┬──────────┐       ┌────┬──────────┐
│ id   │ name   │ dept_id  │       │ id │ name     │
├──────┼────────┼──────────┤       ├────┼──────────┤
│ 1    │ Alice  │ 10       │───┐   │ 10 │ IT       │
│ 2    │ Bob    │ 20       │   │   │ 20 │ HR       │
└──────┴────────┴──────────┘   │   │ 30 │ Sales    │
                               │   └────┴──────────┘
                               │
                setRelation(2, QSqlRelation(
                │    "departments", "id", "name"))
                └──────────┐
                           ▼
              ┌────────────────────────┐
              │ QSqlRelationalTable    │
              │         Model          │
              │                        │
              │  Auto-joins tables     │
              └────────┬───────────────┘
                       │
                       ▼
              ┌────────────────────────┐
              │      View Displays:    │
              │                        │
              │ id │ name  │ dept_name│
              │ 1  │ Alice │ IT       │← Shows name, not ID!
              │ 2  │ Bob   │ HR       │
              └────────────────────────┘
                       ▲
                       │ When editing dept_name:
                       │ QSqlRelationalDelegate
                       │ shows dropdown with
                       └─ [IT, HR, Sales]
```

---

## 🔍 Proxy Model Architecture

### Simple Proxy Chain

```
┌──────────────┐
│ Source Model │
│              │
│ 100 items    │
└──────┬───────┘
       │
       ▼
┌──────────────┐
│ Proxy Model  │
│              │
│ Filter:      │
│ name         │
│ contains "A" │
└──────┬───────┘
       │
       ▼
┌──────────────┐
│    View      │
│              │
│ Shows 25     │
│ filtered     │
│ items        │
└──────────────┘
```

### Complex Proxy Chain

```
┌────────────────┐
│   Database     │
│   1000 rows    │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│ QSqlTableModel │  ◄─── Source data
└───────┬────────┘
        │
        ▼
┌────────────────┐
│  Proxy 1       │  ◄─── Filter by category
│  Category      │       (500 rows pass)
│  Filter        │
└───────┬────────┘
        │
        ▼
┌────────────────┐
│  Proxy 2       │  ◄─── Filter by price
│  Price Filter  │       (200 rows pass)
└───────┬────────┘
        │
        ▼
┌────────────────┐
│  Proxy 3       │  ◄─── Sort by name
│  Sort Proxy    │       (200 rows, sorted)
└───────┬────────┘
        │
        ▼
┌────────────────┐
│  View          │  ◄─── Shows 200 sorted, filtered items
└────────────────┘
```

### Proxy Index Mapping

```
SOURCE MODEL (5 items)
┌─────────────────┐
│ Row 0: "Apple"  │─────┐
│ Row 1: "Banana" │     │
│ Row 2: "Cherry" │─────┼──► PROXY MODEL (Filter: contains "a")
│ Row 3: "Date"   │     │    ┌──────────────────┐
│ Row 4: "Fig"    │     │    │ Row 0: "Banana"  │◄── Maps to source row 1
└─────────────────┘     └────│ Row 1: "Date"    │◄── Maps to source row 3
                              └──────────────────┘
                                       │
VIEW requests row 1 ───────────────────┘
        │
        ▼
Proxy index: row=1
        │
        ▼
proxy->mapToSource(index)
        │
        ▼
Source index: row=3 ("Date")
```

---

## 🔄 Complete Data Flow Example

### Scenario: User edits a cell in a filtered, sorted table

```
┌──────────────────────────────────────────────────────────┐
│ 1. User types in view                                    │
└────────────────┬─────────────────────────────────────────┘
                 │
                 ▼
┌──────────────────────────────────────────────────────────┐
│ 2. View calls setData(proxyIndex, "New Value")          │
└────────────────┬─────────────────────────────────────────┘
                 │
                 ▼
┌──────────────────────────────────────────────────────────┐
│ 3. Proxy maps index:                                     │
│    sourceIndex = mapToSource(proxyIndex)                 │
└────────────────┬─────────────────────────────────────────┘
                 │
                 ▼
┌──────────────────────────────────────────────────────────┐
│ 4. Proxy calls sourceModel->setData(sourceIndex, value) │
└────────────────┬─────────────────────────────────────────┘
                 │
                 ▼
┌──────────────────────────────────────────────────────────┐
│ 5. SQL Model updates cache                              │
└────────────────┬─────────────────────────────────────────┘
                 │
                 ▼
┌──────────────────────────────────────────────────────────┐
│ 6. SQL Model emits dataChanged(sourceIndex)             │
└────────────────┬─────────────────────────────────────────┘
                 │
                 ▼
┌──────────────────────────────────────────────────────────┐
│ 7. Proxy receives dataChanged, maps back:               │
│    proxyIndex = mapFromSource(sourceIndex)               │
└────────────────┬─────────────────────────────────────────┘
                 │
                 ▼
┌──────────────────────────────────────────────────────────┐
│ 8. Proxy emits dataChanged(proxyIndex)                  │
└────────────────┬─────────────────────────────────────────┘
                 │
                 ▼
┌──────────────────────────────────────────────────────────┐
│ 9. View updates cell display                            │
└──────────────────────────────────────────────────────────┘
                 │
                 ▼
┌──────────────────────────────────────────────────────────┐
│ 10. Later: model->submitAll() writes to database        │
└──────────────────────────────────────────────────────────┘
```

---

## 🎯 When to Use Which Model - Decision Tree

```
                     Start Here
                         │
                    Need data?
                         │
        ┌────────────────┴────────────────┐
        │                                 │
   From Database?                  In Memory?
        │                                 │
        YES                               │
        │                            How structured?
        │                                 │
        │               ┌─────────────────┼─────────────┐
        │               │                 │             │
        │            1D List?         2D Table?      Tree?
        │               │                 │             │
        │               NO                NO            NO
        │                                               │
    Need edit?                              Use QAbstractItemModel
        │                                   (Tree structure)
    ┌───┴───┐
    │       │
   YES      NO
    │       │
    │   QSqlQueryModel
    │   (Read-only, flexible SQL)
    │
    │
One table?
    │
┌───┴────┐
│        │
YES      NO (Multiple tables)
│        │
│    Write custom query
│    + QSqlQueryModel
│
Foreign keys?
│
┌───┴────┐
│        │
YES      NO
│        │
│    QSqlTableModel
│    (Simple editable table)
│
QSqlRelationalTableModel
(Handles foreign keys)


IN MEMORY BRANCH:
│
├─ 1D List?
│  │
│  ├─ Simple strings?
│  │  └─► QStringListModel
│  │
│  └─ Custom data?
│     └─► QAbstractListModel
│
├─ 2D Table?
│  └─► QAbstractTableModel
│
└─ Tree?
   └─► QAbstractItemModel


Need filtering/sorting?
│
└─► Wrap ANY model with QSortFilterProxyModel
```

---

## 💾 Memory Layout Comparison

### List Model
```
Memory: Linear array
Access: O(1)

m_items: [Item0][Item1][Item2][Item3][Item4]
          ↓      ↓      ↓      ↓      ↓
        Row 0  Row 1  Row 2  Row 3  Row 4
```

### Table Model
```
Memory: 2D array or vector of structs
Access: O(1)

m_rows[0]: [Cell0,0][Cell0,1][Cell0,2]
m_rows[1]: [Cell1,0][Cell1,1][Cell1,2]
m_rows[2]: [Cell2,0][Cell2,1][Cell2,2]
```

### Tree Model
```
Memory: Pointer-based tree
Access: O(depth)

          [Root]
          /  |  \
         /   |   \
     [A]    [B]   [C]
     / \     |
   [A1][A2] [B1]

Each node knows:
- parent pointer
- children list
- data
```

### SQL Model
```
Memory: Cached query results
Access: O(1) for cached, O(n) for query

┌────────────────────────┐
│ QSqlTableModel         │
│                        │
│ Cache:                 │
│  Row 0: [cached data]  │
│  Row 1: [cached data]  │
│  Row 2: [modified]     │◄─ Pending change
│  Row 3: [new]          │◄─ Pending insert
│                        │
│ On submitAll():        │
│  → Execute SQL         │
│  → Clear flags         │
└────────────────────────┘
```

---

## 🎨 Roles Visualization

```
Single Cell with Multiple Roles:

┌─────────────────────────────────────────┐
│  🔴 Text: "Important"                   │
│  ───────────────────────────────────   │
│  Qt::DisplayRole:     "Important"      │
│  Qt::EditRole:        "Important"      │
│  Qt::DecorationRole:  🔴 (red icon)    │
│  Qt::ForegroundRole:  Red color        │
│  Qt::BackgroundRole:  Yellow bg        │
│  Qt::FontRole:        Bold font        │
│  Qt::ToolTipRole:     "High priority"  │
│  Qt::UserRole:        Priority=1       │
└─────────────────────────────────────────┘

All this from ONE index in the model!
Each role returns different QVariant
```

---

**Happy Coding! 🚀**
