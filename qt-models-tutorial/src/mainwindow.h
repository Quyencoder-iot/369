#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QStackedWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onMenuItemClicked();
    void runExample(const QString &exampleName);

private:
    void setupUI();
    void createMenuItems();
    void addMenuItem(const QString &title, const QString &description, const QString &executable);
    
    QListWidget *m_menuList;
    QStackedWidget *m_contentStack;
    QWidget *m_welcomeWidget;
    
    struct ExampleItem {
        QString title;
        QString description;
        QString executable;
    };
    
    QList<ExampleItem> m_examples;
};

#endif // MAINWINDOW_H