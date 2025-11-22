#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QListView>
#include <QTableView>
#include <QTreeView>
#include <QLabel>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlError>

#include "custom_list_model.h"
#include "custom_table_model.h"
#include "custom_tree_model.h"

bool createConnection() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        QMessageBox::critical(nullptr, "Cannot open database",
            "Unable to establish a database connection.\n"
            "This example needs SQLite support. Please read "
            "the Qt SQL driver documentation for information how "
            "to build it.\n\n"
            "Click Cancel to exit.", QMessageBox::Cancel);
        return false;
    }

    QSqlQuery query;
    query.exec("CREATE TABLE person (id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "firstname VARCHAR(20), lastname VARCHAR(20))");
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Danny', 'Young')");
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Christine', 'Holgin')");
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Lars', 'Gordon')");
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Roberto', 'Robitaille')");
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Maria', 'Papadopoulos')");

    return true;
}

QWidget* createListTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QHBoxLayout *filterLayout = new QHBoxLayout;
    filterLayout->addWidget(new QLabel("Filter:"));
    QLineEdit *filterInput = new QLineEdit;
    filterLayout->addWidget(filterInput);
    layout->addLayout(filterLayout);

    QStringList data = {"Apple", "Banana", "Cherry", "Date", "Elderberry", "Fig", "Grape"};
    CustomListModel *sourceModel = new CustomListModel(data, tab);

    QSortFilterProxyModel *proxyModel = new QSortFilterProxyModel(tab);
    proxyModel->setSourceModel(sourceModel);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    QObject::connect(filterInput, &QLineEdit::textChanged, proxyModel, &QSortFilterProxyModel::setFilterFixedString);

    QListView *listView = new QListView;
    listView->setModel(proxyModel);
    layout->addWidget(listView);

    return tab;
}

QWidget* createTableTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QVector<PersonData> data = {
        {"Alice", 30, "Engineer"},
        {"Bob", 25, "Designer"},
        {"Charlie", 35, "Manager"}
    };
    CustomTableModel *model = new CustomTableModel(data, tab);

    QTableView *tableView = new QTableView;
    tableView->setModel(model);
    layout->addWidget(tableView);

    return tab;
}

QWidget* createTreeTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    QString data = 
        "Project A\n"
        "    Task 1\n"
        "    Task 2\n"
        "        Subtask 2.1\n"
        "Project B\n"
        "    Task 3";
        
    CustomTreeModel *model = new CustomTreeModel(data, tab);

    QTreeView *treeView = new QTreeView;
    treeView->setModel(model);
    treeView->expandAll();
    layout->addWidget(treeView);

    return tab;
}

QWidget* createSqlTab() {
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);

    if (createConnection()) {
        QSqlTableModel *model = new QSqlTableModel(tab);
        model->setTable("person");
        model->setEditStrategy(QSqlTableModel::OnFieldChange);
        model->select();

        model->setHeaderData(0, Qt::Horizontal, "ID");
        model->setHeaderData(1, Qt::Horizontal, "First Name");
        model->setHeaderData(2, Qt::Horizontal, "Last Name");

        QTableView *tableView = new QTableView;
        tableView->setModel(model);
        layout->addWidget(tableView);
    }

    return tab;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Qt Models Tutorial (C++)");
    window.resize(800, 600);

    QTabWidget *tabs = new QTabWidget;
    window.setCentralWidget(tabs);

    tabs->addTab(createListTab(), "List & Proxy Model");
    tabs->addTab(createTableTab(), "Table Model");
    tabs->addTab(createTreeTab(), "Tree Model");
    tabs->addTab(createSqlTab(), "SQL Model");

    window.show();

    return app.exec();
}
