// mainwindow.h - Main window cho demo

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <memory>

QT_BEGIN_NAMESPACE
class QTabWidget;
class QTextEdit;
class QStandardItemModel;
class QTableView;
class QTreeView;
class QListView;
class QItemSelectionModel;
class QSortFilterProxyModel;
QT_END_NAMESPACE

class CustomModel;
class CustomDelegate;
class PointerManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupUI();
    void createModelTab();
    void createViewTab();
    void createDelegateTab();
    void createMemoryTab();
    void connectSignals();
    
    // UI Components
    QTabWidget* m_tabWidget;
    QTextEdit* m_logOutput;
    
    // Model tab
    QTableView* m_modelTableView;
    QTreeView* m_modelTreeView;
    CustomModel* m_customModel;
    QStandardItemModel* m_standardModel;
    
    // View tab
    QTableView* m_viewTable1;
    QTableView* m_viewTable2;
    QListView* m_viewList;
    QPointer<QItemSelectionModel> m_sharedSelection;
    
    // Delegate tab
    QTableView* m_delegateTableView;
    std::unique_ptr<CustomDelegate> m_customDelegate;
    
    // Memory management demo
    PointerManager* m_pointerManager;
    
    // Proxy model
    QSortFilterProxyModel* m_proxyModel;
    
private slots:
    // Model demos
    void demoModelOwnership();
    void demoModelSharing();
    void demoItemPointers();
    void demoModelCleanup();
    
    // View demos
    void demoViewModelRelation();
    void demoSharedSelection();
    void demoViewComponents();
    void demoProxyModel();
    
    // Delegate demos
    void demoDelegateLifecycle();
    void demoCustomEditors();
    void demoDelegateSwapping();
    
    // Memory management
    void demoSmartPointers();
    void demoQPointer();
    void demoMemoryLeaks();
    void demoBestPractices();
    
    // Logging
    void log(const QString& message);
    void clearLog();
};

#endif // MAINWINDOW_H