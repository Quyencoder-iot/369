// custom_delegate.h - Custom delegate để demo editor management

#ifndef CUSTOM_DELEGATE_H
#define CUSTOM_DELEGATE_H

#include <QStyledItemDelegate>
#include <QPointer>
#include <QHash>

class CustomDelegate : public QStyledItemDelegate
{
    Q_OBJECT
    
public:
    explicit CustomDelegate(QObject* parent = nullptr);
    ~CustomDelegate();
    
    // Override methods
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const override;
                         
    void setEditorData(QWidget* editor, const QModelIndex& index) const override;
    
    void setModelData(QWidget* editor, QAbstractItemModel* model,
                     const QModelIndex& index) const override;
                     
    void updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                             const QModelIndex& index) const override;
                             
    void paint(QPainter* painter, const QStyleOptionViewItem& option,
               const QModelIndex& index) const override;
    
    // Demo methods
    void showActiveEditors() const;
    int activeEditorCount() const;
    void clearEditorTracking();
    
signals:
    void logMessage(const QString& message) const;
    void editorCreated(QWidget* editor, const QModelIndex& index) const;
    void editorDestroyed(QWidget* editor) const;
    
private:
    // Track active editors
    mutable QHash<QPersistentModelIndex, QPointer<QWidget>> m_activeEditors;
    
    // Helper to emit log messages
    void log(const QString& msg) const;
};

#endif // CUSTOM_DELEGATE_H