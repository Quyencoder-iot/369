/**
 * ============================================
 * EXAMPLE 3: FILE SYSTEM TREE APPLICATION
 * ============================================
 * 
 * C++ Model: FileSystemTreeModel (QAbstractItemModel)
 * QML View: FileSystemTreeView.qml
 * 
 * PURPOSE:
 * Demonstrate how to integrate a C++ TREE MODEL with QML
 * 
 * KEY DIFFERENCES from List/Table:
 * - Hierarchical data (parent-child relationships)
 * - QModelIndex for node references
 * - parent() method
 * - index() method for navigation
 * - Recursive structure
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "models/FileSystemTreeModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QGuiApplication app(argc, argv);
    
    // ============================================
    // Create QML Engine
    // ============================================
    QQmlApplicationEngine engine;
    
    // ============================================
    // Create C++ Tree Model
    // ============================================
    FileSystemTreeModel treeModel;
    
    // Build sample file system tree
    // Root
    //  ├─ Documents/
    //  │   ├─ report.pdf
    //  │   └─ notes.txt
    //  ├─ Pictures/
    //  │   ├─ vacation.jpg
    //  │   └─ family.png
    //  └─ code.cpp
    
    QModelIndex root = treeModel.rootIndex();
    
    // Add folders
    treeModel.addItem("Documents", true, root);
    treeModel.addItem("Pictures", true, root);
    treeModel.addItem("Music", true, root);
    
    // Add files to Documents (row 0)
    QModelIndex docsIndex = treeModel.index(0, 0, root);
    treeModel.addItem("report.pdf", false, docsIndex);
    treeModel.addItem("notes.txt", false, docsIndex);
    treeModel.addItem("budget.xlsx", false, docsIndex);
    
    // Add files to Pictures (row 1)
    QModelIndex picsIndex = treeModel.index(1, 0, root);
    treeModel.addItem("vacation.jpg", false, picsIndex);
    treeModel.addItem("family.png", false, picsIndex);
    
    // Add files to Music (row 2)
    QModelIndex musicIndex = treeModel.index(2, 0, root);
    treeModel.addItem("song1.mp3", false, musicIndex);
    treeModel.addItem("song2.mp3", false, musicIndex);
    
    // Add root-level file
    treeModel.addItem("README.md", false, root);
    
    // ============================================
    // Expose to QML
    // ============================================
    engine.rootContext()->setContextProperty("treeModel", &treeModel);
    
    // ============================================
    // Load QML
    // ============================================
    const QUrl url(QStringLiteral("qrc:/qml/FileSystemTreeView.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    
    engine.load(url);
    
    return app.exec();
}

/**
 * ============================================
 * 📚 SUMMARY - TREE MODEL INTEGRATION
 * ============================================
 * 
 * 1. TREE MODEL vs LIST/TABLE:
 *    LIST/TABLE:
 *    - Flat structure
 *    - Simple row index
 *    - rowCount() only
 *    
 *    TREE:
 *    - Hierarchical structure
 *    - QModelIndex for each node
 *    - rowCount(parent) - children of parent
 *    - parent() - get parent node
 *    - index(row, col, parent) - get child
 * 
 * 2. QModelIndex CONCEPT:
 *    - Unique reference to a tree node
 *    - Contains: row, column, parent pointer
 *    - Invalid index = root node
 *    - Used for ALL tree operations
 *    
 *    Example:
 *      QModelIndex root = QModelIndex();  // Invalid = root
 *      QModelIndex child = model.index(0, 0, root);  // First child
 *      QModelIndex grandchild = model.index(0, 0, child);  // Child of child
 * 
 * 3. TREE NAVIGATION:
 *    Get root:
 *      QModelIndex root = QModelIndex();  // Invalid index
 *    
 *    Get child:
 *      QModelIndex child = model.index(row, 0, parent);
 *    
 *    Get parent:
 *      QModelIndex parent = model.parent(child);
 *    
 *    Check if has children:
 *      bool hasChildren = model.rowCount(parent) > 0;
 * 
 * 4. REQUIRED OVERRIDES:
 *    ✅ rowCount(parent) - Number of children
 *    ✅ columnCount(parent) - Usually 1 for trees
 *    ✅ data(index, role) - Node data
 *    ✅ index(row, col, parent) - Get child index
 *    ✅ parent(index) - Get parent index
 *    ✅ roleNames() - Role mapping
 * 
 * 5. PARENT-CHILD OPERATIONS:
 *    Add child:
 *      model.addItem(name, isFolder, parentIndex)
 *    
 *    Remove node:
 *      model.removeItem(nodeIndex)
 *    
 *    Get children:
 *      int count = model.rowCount(parentIndex)
 *      for (int i = 0; i < count; ++i) {
 *          QModelIndex child = model.index(i, 0, parentIndex);
 *      }
 * 
 * 6. QML TREE VIEW:
 *    - Recursive delegate component
 *    - Indent based on depth
 *    - Expand/collapse state
 *    - Loader for recursive loading
 * 
 * 7. EXPAND/COLLAPSE:
 *    State stored in C++ (e.g., QSet<QModelIndex>)
 *    QML calls: treeModel.toggleExpanded(index)
 *    QML shows/hides children based on state
 * 
 * ============================================
 * 💡 LEARNING POINTS
 * ============================================
 * 
 * ★ Tree model MUST override:
 *   - rowCount(parent)  ← Parent parameter!
 *   - index(row, col, parent)  ← Create child index
 *   - parent(child)  ← Get parent index
 * 
 * ★ QModelIndex is NOT an int!
 *   - It's an opaque reference
 *   - Pass between C++ and QML as QVariant
 *   - Use model.index() to get it
 * 
 * ★ Root node = invalid QModelIndex:
 *   QModelIndex root;  // Invalid by default
 *   QModelIndex();     // Explicit invalid
 * 
 * ★ Navigate tree:
 *   QModelIndex child = model.index(row, 0, parent);
 *   QModelIndex parent = model.parent(child);
 *   int childCount = model.rowCount(parent);
 * 
 * ============================================
 */
