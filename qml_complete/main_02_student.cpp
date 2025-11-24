/**
 * ============================================
 * EXAMPLE 2: STUDENT TABLE APPLICATION
 * ============================================
 * 
 * C++ Model: StudentTableModel (QAbstractTableModel)
 * QML View: StudentTableView.qml
 * 
 * PURPOSE:
 * Demonstrate how to integrate a C++ TABLE MODEL with QML
 * 
 * KEY DIFFERENCES from ListModel:
 * - 2D data (rows × columns)
 * - columnCount() method
 * - Column-specific roles
 * - TableView in QML (not ListView)
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "models/StudentTableModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    
    QGuiApplication app(argc, argv);
    
    // ============================================
    // Create QML Engine
    // ============================================
    QQmlApplicationEngine engine;
    
    // ============================================
    // Create C++ Table Model
    // ============================================
    StudentTableModel studentModel;
    
    // Add sample students
    studentModel.addStudent("Nguyễn Văn A", 20, "A", "nguyenvana@email.com", "0901234567");
    studentModel.addStudent("Trần Thị B", 21, "B+", "tranthib@email.com", "0912345678");
    studentModel.addStudent("Lê Văn C", 19, "A+", "levanc@email.com", "0923456789");
    studentModel.addStudent("Phạm Thị D", 22, "C", "phamthid@email.com", "0934567890");
    studentModel.addStudent("Hoàng Văn E", 20, "B", "hoangvane@email.com", "0945678901");
    
    // ============================================
    // Expose to QML
    // ============================================
    engine.rootContext()->setContextProperty("studentModel", &studentModel);
    
    // ============================================
    // Load QML
    // ============================================
    const QUrl url(QStringLiteral("qrc:/qml/StudentTableView.qml"));
    
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
 * 📚 SUMMARY - STUDENT TABLE INTEGRATION
 * ============================================
 * 
 * 1. TABLE MODEL vs LIST MODEL:
 *    LIST MODEL:
 *    - 1D data (single column)
 *    - Only rowCount()
 *    - ListView in QML
 *    
 *    TABLE MODEL:
 *    - 2D data (rows × columns)
 *    - rowCount() AND columnCount()
 *    - TableView in QML
 *    - Column headers
 * 
 * 2. DATA ACCESS:
 *    C++ data(index, role):
 *      - index.row() → Row number
 *      - index.column() → Column number (TABLE ONLY!)
 *      - role → Which data (name, age, grade, etc.)
 *    
 *    QML:
 *      model.studentId  // Role-based access
 *      model.name
 *      model.age
 * 
 * 3. ROLES MAPPING:
 *    C++ roles (roleNames):
 *      IdRole → "studentId"
 *      NameRole → "name"
 *      AgeRole → "age"
 *      GradeRole → "grade"
 *      EmailRole → "email"
 *      PhoneRole → "phone"
 * 
 * 4. TABLE VIEW REQUIREMENTS:
 *    ✅ columnCount() implementation
 *    ✅ Column-specific data() logic
 *    ✅ HorizontalHeaderView in QML
 *    ✅ columnWidthProvider function
 * 
 * 5. EDITABLE TABLE:
 *    Override setData() in C++
 *      → QML TextField can edit cells
 *      → Changes propagate back to C++
 * 
 * 6. STATISTICS:
 *    Q_INVOKABLE methods:
 *      - gradeACount()
 *      - averageAge()
 *    
 *    Called from QML:
 *      studentModel.gradeACount()
 *      studentModel.averageAge()
 * 
 * ============================================
 * 💡 LEARNING POINTS
 * ============================================
 * 
 * ★ Table model MUST override:
 *   - rowCount()
 *   - columnCount()  ← New!
 *   - data(index, role)  ← Check index.column()
 *   - roleNames()
 * 
 * ★ Optional for editing:
 *   - flags() → return Qt::ItemIsEditable
 *   - setData() → Handle cell edits
 * 
 * ★ QML TableView setup:
 *   TableView {
 *       model: cppTableModel
 *       columnWidthProvider: function(col) { ... }
 *       delegate: Rectangle { /* row, column available */ }
 *   }
 * 
 * ============================================
 */
