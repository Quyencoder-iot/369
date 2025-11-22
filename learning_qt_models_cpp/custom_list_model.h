#ifndef CUSTOM_LIST_MODEL_H
#define CUSTOM_LIST_MODEL_H

#include <QAbstractListModel>
#include <QStringList>

class CustomListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit CustomListModel(const QStringList &data, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addItem(const QString &item);

private:
    QStringList m_data;
};

#endif // CUSTOM_LIST_MODEL_H
