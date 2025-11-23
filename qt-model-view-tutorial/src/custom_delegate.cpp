// custom_delegate.cpp - Implementation của custom delegate

#include "custom_delegate.h"
#include <QComboBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QPainter>
#include <QDebug>

CustomDelegate::CustomDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
    qDebug() << "CustomDelegate constructed at" << this;
    log(QString("CustomDelegate created at %1").arg((qintptr)this, 0, 16));
}

CustomDelegate::~CustomDelegate()
{
    qDebug() << "CustomDelegate destructor at" << this;
    log(QString("CustomDelegate at %1 being destroyed").arg((qintptr)this, 0, 16));
    log(QString("Active editors being tracked: %1").arg(m_activeEditors.size()));
}

QWidget* CustomDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option,
                                    const QModelIndex& index) const
{
    log(QString("\n=== Creating editor for index(%1,%2) ===")
        .arg(index.row()).arg(index.column()));
    
    QWidget* editor = nullptr;
    
    // Create different editors based on column
    switch (index.column()) {
        case 0: {
            // ComboBox for first column
            QComboBox* combo = new QComboBox(parent);
            combo->addItems({"Option A", "Option B", "Option C", "Option D"});
            combo->setEditable(true);
            editor = combo;
            log("Created QComboBox editor");
            break;
        }
        
        case 1: {
            // SpinBox for second column
            QSpinBox* spinBox = new QSpinBox(parent);
            spinBox->setRange(0, 1000);
            spinBox->setSingleStep(10);
            editor = spinBox;
            log("Created QSpinBox editor");
            break;
        }
        
        case 2: {
            // Default line edit
            editor = QStyledItemDelegate::createEditor(parent, option, index);
            log("Created default QLineEdit editor");
            break;
        }
        
        case 3: {
            // Date editor
            QDateEdit* dateEdit = new QDateEdit(parent);
            dateEdit->setCalendarPopup(true);
            dateEdit->setDate(QDate::currentDate());
            editor = dateEdit;
            log("Created QDateEdit editor");
            break;
        }
        
        default:
            editor = QStyledItemDelegate::createEditor(parent, option, index);
            break;
    }
    
    if (editor) {
        log(QString("Editor created at %1 with parent %2")
            .arg((qintptr)editor, 0, 16)
            .arg((qintptr)parent, 0, 16));
        
        // Track the editor
        m_activeEditors[QPersistentModelIndex(index)] = editor;
        
        // Connect to track destruction
        connect(editor, &QObject::destroyed, this, [this, editor, index]() {
            log(QString("Editor at %1 destroyed for index(%2,%3)")
                .arg((qintptr)editor, 0, 16)
                .arg(index.row()).arg(index.column()));
            emit editorDestroyed(editor);
            
            // Remove from tracking
            m_activeEditors.remove(QPersistentModelIndex(index));
        });
        
        emit editorCreated(editor, index);
    }
    
    return editor;
}

void CustomDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
    if (!editor) return;
    
    log(QString("Setting editor data for index(%1,%2)")
        .arg(index.row()).arg(index.column()));
    
    QString value = index.model()->data(index, Qt::EditRole).toString();
    
    // Set data based on editor type
    if (QComboBox* combo = qobject_cast<QComboBox*>(editor)) {
        combo->setCurrentText(value);
        log(QString("Set ComboBox text to '%1'").arg(value));
    }
    else if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor)) {
        spinBox->setValue(value.toInt());
        log(QString("Set SpinBox value to %1").arg(value.toInt()));
    }
    else if (QDateEdit* dateEdit = qobject_cast<QDateEdit*>(editor)) {
        dateEdit->setDate(QDate::fromString(value, Qt::ISODate));
        log(QString("Set DateEdit to %1").arg(value));
    }
    else {
        // Default behavior
        QStyledItemDelegate::setEditorData(editor, index);
    }
}

void CustomDelegate::setModelData(QWidget* editor, QAbstractItemModel* model,
                                const QModelIndex& index) const
{
    if (!editor || !model) return;
    
    log(QString("Setting model data from editor for index(%1,%2)")
        .arg(index.row()).arg(index.column()));
    
    // Get data based on editor type
    if (QComboBox* combo = qobject_cast<QComboBox*>(editor)) {
        QString value = combo->currentText();
        model->setData(index, value, Qt::EditRole);
        log(QString("Set model data to '%1' from ComboBox").arg(value));
    }
    else if (QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor)) {
        int value = spinBox->value();
        model->setData(index, value, Qt::EditRole);
        log(QString("Set model data to %1 from SpinBox").arg(value));
    }
    else if (QDateEdit* dateEdit = qobject_cast<QDateEdit*>(editor)) {
        QString value = dateEdit->date().toString(Qt::ISODate);
        model->setData(index, value, Qt::EditRole);
        log(QString("Set model data to '%1' from DateEdit").arg(value));
    }
    else {
        // Default behavior
        QStyledItemDelegate::setModelData(editor, model, index);
    }
}

void CustomDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
    if (!editor) return;
    
    // Default geometry update
    editor->setGeometry(option.rect);
    
    // Log only for first column to reduce spam
    if (index.column() == 0) {
        log(QString("Updated editor geometry: %1x%2 at (%3,%4)")
            .arg(option.rect.width())
            .arg(option.rect.height())
            .arg(option.rect.x())
            .arg(option.rect.y()));
    }
}

void CustomDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                         const QModelIndex& index) const
{
    if (!painter) return;
    
    // Custom painting for specific columns
    painter->save();
    
    if (index.column() == 0) {
        // Highlight first column
        painter->fillRect(option.rect, QColor(240, 240, 255));
    }
    else if (index.column() == 1) {
        // Draw progress bar for numeric values
        bool ok;
        int value = index.data().toInt(&ok);
        if (ok && value >= 0 && value <= 100) {
            QStyleOptionProgressBar progressBarOption;
            progressBarOption.rect = option.rect;
            progressBarOption.minimum = 0;
            progressBarOption.maximum = 100;
            progressBarOption.progress = value;
            progressBarOption.text = QString("%1%").arg(value);
            progressBarOption.textVisible = true;
            
            QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                             &progressBarOption, painter);
            painter->restore();
            return;
        }
    }
    
    painter->restore();
    
    // Default painting
    QStyledItemDelegate::paint(painter, option, index);
}

void CustomDelegate::showActiveEditors() const
{
    log(QString("\n=== Active Editors Report ==="));
    log(QString("Total tracked editors: %1").arg(m_activeEditors.size()));
    
    int validCount = 0;
    for (auto it = m_activeEditors.begin(); it != m_activeEditors.end(); ++it) {
        if (it.value()) {
            validCount++;
            const QPersistentModelIndex& idx = it.key();
            log(QString("Editor at index(%1,%2): %3")
                .arg(idx.row())
                .arg(idx.column())
                .arg(it.value()->metaObject()->className()));
        }
    }
    
    log(QString("Valid editors: %1").arg(validCount));
    log(QString("Destroyed editors (null pointers): %1").arg(m_activeEditors.size() - validCount));
}

int CustomDelegate::activeEditorCount() const
{
    int count = 0;
    for (const auto& editor : m_activeEditors) {
        if (editor) count++;
    }
    return count;
}

void CustomDelegate::clearEditorTracking()
{
    log("Clearing editor tracking hash");
    int oldSize = m_activeEditors.size();
    m_activeEditors.clear();
    log(QString("Removed %1 tracked editors").arg(oldSize));
}

void CustomDelegate::log(const QString& msg) const
{
    emit logMessage(msg);
    qDebug() << "[CustomDelegate]" << msg;
}