from PySide6.QtCore import QAbstractListModel, Qt

class CustomListModel(QAbstractListModel):
    def __init__(self, data=None):
        super().__init__()
        self._data = data or []

    def rowCount(self, parent=None):
        return len(self._data)

    def data(self, index, role=Qt.DisplayRole):
        if not index.isValid():
            return None
        
        if role == Qt.DisplayRole:
            return self._data[index.row()]
        
        if role == Qt.DecorationRole:
            # Ví dụ: trả về màu sắc hoặc icon tùy logic
            pass
            
        return None

    # Phương thức để thêm dữ liệu
    def add_item(self, item):
        self.beginInsertRows(self.index(0).parent(), self.rowCount(), self.rowCount())
        self._data.append(item)
        self.endInsertRows()
