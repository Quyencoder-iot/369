// custom_model.cpp - Implementation của custom model

#include "custom_model.h"
#include <QDebug>

CustomModel::CustomModel(QObject* parent)
    : QStandardItemModel(parent)
{
    qDebug() << "CustomModel constructed at" << this << "with parent" << parent;
    populateTestData();
}

CustomModel::~CustomModel()
{
    qDebug() << "CustomModel destructor called at" << this;
    emit logMessage(QString("CustomModel at %1 is being destroyed").arg((qintptr)this, 0, 16));
    
    // Check tracked items
    int validItems = 0;
    for (const auto& itemPtr : m_trackedItems) {
        if (itemPtr) validItems++;
    }
    qDebug() << "Tracked items still valid:" << validItems << "of" << m_trackedItems.size();
}

void CustomModel::populateTestData()
{
    setHorizontalHeaderLabels({"Name", "Value", "Type"});
    
    for (int row = 0; row < 5; ++row) {
        QList<QStandardItem*> items;
        
        QStandardItem* nameItem = new QStandardItem(QString("Item %1").arg(row));
        QStandardItem* valueItem = new QStandardItem(QString::number(row * 10));
        QStandardItem* typeItem = new QStandardItem("Standard");
        
        items << nameItem << valueItem << typeItem;
        appendRow(items);
        
        // Track first item of each row
        trackItem(nameItem);
    }
    
    emit logMessage("CustomModel populated with test data");
}

void CustomModel::trackItem(QStandardItem* item)
{
    m_trackedItems.append(item);
    qDebug() << "Tracking item" << item << "text:" << item->text();
}

void CustomModel::demonstrateItemOwnership()
{
    emit logMessage("\n=== Demonstrating Item Ownership ===");
    
    // Create items
    QStandardItem* parent = new QStandardItem("Parent Item");
    QStandardItem* child1 = new QStandardItem("Child 1");
    QStandardItem* child2 = new QStandardItem("Child 2");
    
    emit logMessage(QString("Created parent at %1").arg((qintptr)parent, 0, 16));
    emit logMessage(QString("Created child1 at %1").arg((qintptr)child1, 0, 16));
    emit logMessage(QString("Created child2 at %1").arg((qintptr)child2, 0, 16));
    
    // Add to model - model now owns parent
    appendRow(parent);
    emit logMessage("Added parent to model - model now owns it");
    
    // Add children - parent now owns them
    parent->appendRow(child1);
    parent->appendRow(child2);
    emit logMessage("Added children to parent - parent now owns them");
    
    // Track for demo
    trackItem(parent);
    trackItem(child1);
    trackItem(child2);
    
    emit logMessage("IMPORTANT: Do NOT delete these items manually!");
}

void CustomModel::demonstrateIndexPointers()
{
    emit logMessage("\n=== Demonstrating Index and Pointers ===");
    
    // Get index
    QModelIndex index = this->index(0, 0);
    emit logMessage(QString("Got index for (0,0): valid=%1").arg(index.isValid()));
    
    // Get item from index
    QStandardItem* item = itemFromIndex(index);
    if (item) {
        emit logMessage(QString("Item from index: %1 text='%2'")
            .arg((qintptr)item, 0, 16)
            .arg(item->text()));
    }
    
    // Get item directly
    QStandardItem* directItem = this->item(0, 1);
    if (directItem) {
        emit logMessage(QString("Direct item(0,1): %1 text='%2'")
            .arg((qintptr)directItem, 0, 16)
            .arg(directItem->text()));
    }
    
    // Index remains valid even if model changes
    emit logMessage("Indexes are temporary - don't store them long term!");
    emit logMessage("Use QPersistentModelIndex for long-term storage");
    
    QPersistentModelIndex persistentIndex(index);
    emit logMessage(QString("Created persistent index: valid=%1").arg(persistentIndex.isValid()));
}

void CustomModel::demonstrateParentChild()
{
    emit logMessage("\n=== Demonstrating Parent-Child Relations ===");
    
    // Get root item
    QStandardItem* root = invisibleRootItem();
    emit logMessage(QString("Invisible root item: %1").arg((qintptr)root, 0, 16));
    
    // Check parent of existing item
    QStandardItem* item = this->item(0, 0);
    if (item) {
        QStandardItem* parent = item->parent();
        emit logMessage(QString("Item parent: %1 (should be root)").arg((qintptr)parent, 0, 16));
        emit logMessage(QString("Same as root? %1").arg(parent == root ? "Yes" : "No"));
    }
    
    // Create hierarchy
    QStandardItem* level1 = new QStandardItem("Level 1");
    QStandardItem* level2 = new QStandardItem("Level 2");
    QStandardItem* level3 = new QStandardItem("Level 3");
    
    appendRow(level1);
    level1->appendRow(level2);
    level2->appendRow(level3);
    
    emit logMessage("Created 3-level hierarchy");
    emit logMessage("Each level owns its children");
}

bool CustomModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    emit logMessage(QString("setData called: index(%1,%2) value='%3' role=%4")
        .arg(index.row())
        .arg(index.column())
        .arg(value.toString())
        .arg(role));
    
    bool result = QStandardItemModel::setData(index, value, role);
    
    if (result) {
        emit logMessage("Data set successfully");
    }
    
    return result;
}

bool CustomModel::removeRows(int row, int count, const QModelIndex& parent)
{
    emit logMessage(QString("removeRows called: row=%1 count=%2").arg(row).arg(count));
    
    // Check tracked items before removal
    emit logMessage("Checking tracked items before removal...");
    for (int i = row; i < row + count && i < m_trackedItems.size(); ++i) {
        if (m_trackedItems[i]) {
            emit logMessage(QString("Item %1 about to be deleted").arg(i));
        }
    }
    
    bool result = QStandardItemModel::removeRows(row, count, parent);
    
    if (result) {
        emit logMessage(QString("Successfully removed %1 rows").arg(count));
        
        // Check tracked items after removal
        int nullCount = 0;
        for (const auto& itemPtr : m_trackedItems) {
            if (!itemPtr) nullCount++;
        }
        emit logMessage(QString("QPointers nullified: %1").arg(nullCount));
    }
    
    return result;
}