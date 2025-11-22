from PySide6.QtCore import QAbstractTableModel, Qt

class CustomTableModel(QAbstractTableModel):
    def __init__(self, data=None):
        super().__init__()
        # Data là list của list (2D array)
        self._data = data or []
        self._headers = ["Name", "Age", "Role"]

    def rowCount(self, parent=None):
        return len(self._data)

    def columnCount(self, parent=None):
        return len(self._headers)

    def data(self, index, role=Qt.DisplayRole):
        if not index.isValid():
            return None

        if role == Qt.DisplayRole:
            return self._data[index.row()][index.column()]
        
        return None

    def headerData(self, section, orientation, role):
        if role == Qt.DisplayRole:
            if orientation == Qt.Horizontal:
                return self._headers[section]
            else:
                return str(section + 1)
        return None
