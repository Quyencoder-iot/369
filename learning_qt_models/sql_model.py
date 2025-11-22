from PySide6.QtSql import QSqlDatabase, QSqlQuery, QSqlTableModel
from PySide6.QtWidgets import QMessageBox

def create_connection():
    db = QSqlDatabase.addDatabase("QSQLITE")
    db.setDatabaseName(":memory:")
    if not db.open():
        QMessageBox.critical(None, "Cannot open database",
                             "Unable to establish a database connection.\n"
                             "This example needs SQLite support. Please read "
                             "the Qt SQL driver documentation for information how "
                             "to build it.\n\n"
                             "Click Cancel to exit.", QMessageBox.Cancel)
        return False

    query = QSqlQuery()
    query.exec("CREATE TABLE person (id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "firstname VARCHAR(20), lastname VARCHAR(20))")
    
    # Insert some data
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Danny', 'Young')")
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Christine', 'Holgin')")
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Lars', 'Gordon')")
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Roberto', 'Robitaille')")
    query.exec("INSERT INTO person (firstname, lastname) VALUES ('Maria', 'Papadopoulos')")
    
    return True

class CustomSqlModel(QSqlTableModel):
    def __init__(self, db=QSqlDatabase()):
        super().__init__(db=db)
        self.setTable("person")
        self.setEditStrategy(QSqlTableModel.OnFieldChange)
        self.select()
        
        self.setHeaderData(0, 1, "ID") # Qt.Horizontal is default role
        self.setHeaderData(1, 1, "First Name")
        self.setHeaderData(2, 1, "Last Name")
