import sys
from PySide6.QtWidgets import (QApplication, QMainWindow, QTableView, QListView, QTreeView, 
                               QVBoxLayout, QWidget, QTabWidget, QLineEdit, QLabel, QHBoxLayout)
from PySide6.QtCore import QSortFilterProxyModel, Qt
from custom_list_model import CustomListModel
from custom_table_model import CustomTableModel
from custom_tree_model import CustomTreeModel
from sql_model import create_connection, CustomSqlModel

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Qt Models Tutorial")
        self.resize(800, 600)

        self.tabs = QTabWidget()
        self.setCentralWidget(self.tabs)

        self.create_list_tab()
        self.create_table_tab()
        self.create_tree_tab()
        self.create_sql_tab()

    def create_list_tab(self):
        # Tab 1: List Model + Proxy Model (Filter)
        tab = QWidget()
        layout = QVBoxLayout()
        
        # Filter Input
        filter_layout = QHBoxLayout()
        filter_layout.addWidget(QLabel("Filter:"))
        filter_input = QLineEdit()
        filter_layout.addWidget(filter_input)
        layout.addLayout(filter_layout)

        # Model Setup
        data = ["Apple", "Banana", "Cherry", "Date", "Elderberry", "Fig", "Grape"]
        source_model = CustomListModel(data)
        
        # Proxy Model Setup
        proxy_model = QSortFilterProxyModel()
        proxy_model.setSourceModel(source_model)
        proxy_model.setFilterCaseSensitivity(Qt.CaseInsensitive)
        
        # Connect filter input
        filter_input.textChanged.connect(proxy_model.setFilterFixedString)

        # View Setup
        list_view = QListView()
        list_view.setModel(proxy_model)
        layout.addWidget(list_view)
        
        tab.setLayout(layout)
        self.tabs.addTab(tab, "List & Proxy Model")

    def create_table_tab(self):
        # Tab 2: Table Model
        tab = QWidget()
        layout = QVBoxLayout()
        
        data = [
            ["Alice", 30, "Engineer"],
            ["Bob", 25, "Designer"],
            ["Charlie", 35, "Manager"]
        ]
        model = CustomTableModel(data)
        
        table_view = QTableView()
        table_view.setModel(model)
        layout.addWidget(table_view)
        
        tab.setLayout(layout)
        self.tabs.addTab(tab, "Table Model")

    def create_tree_tab(self):
        # Tab 3: Tree Model
        tab = QWidget()
        layout = QVBoxLayout()
        
        data = """
Project A
    Task 1
    Task 2
        Subtask 2.1
Project B
    Task 3
"""
        model = CustomTreeModel(data)
        
        tree_view = QTreeView()
        tree_view.setModel(model)
        tree_view.expandAll()
        layout.addWidget(tree_view)
        
        tab.setLayout(layout)
        self.tabs.addTab(tab, "Tree Model")

    def create_sql_tab(self):
        # Tab 4: SQL Model
        tab = QWidget()
        layout = QVBoxLayout()
        
        if not create_connection():
            return
            
        model = CustomSqlModel()
        
        table_view = QTableView()
        table_view.setModel(model)
        layout.addWidget(table_view)
        
        tab.setLayout(layout)
        self.tabs.addTab(tab, "SQL Model")

if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
