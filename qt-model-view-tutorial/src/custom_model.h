// custom_model.h - Custom model để demo pointer management

#ifndef CUSTOM_MODEL_H
#define CUSTOM_MODEL_H

#include <QStandardItemModel>
#include <QPointer>

class CustomModel : public QStandardItemModel
{
    Q_OBJECT
    
public:
    explicit CustomModel(QObject* parent = nullptr);
    ~CustomModel();
    
    // Demo methods
    void demonstrateItemOwnership();
    void demonstrateIndexPointers();
    void demonstrateParentChild();
    
    // Override để log
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex()) override;
    
signals:
    void logMessage(const QString& message);
    
private:
    void populateTestData();
    void trackItem(QStandardItem* item);
    
    // Tracking items for demo
    QList<QPointer<QStandardItem>> m_trackedItems;
};

#endif // CUSTOM_MODEL_H