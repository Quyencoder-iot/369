#include "custom_list_model.h"

CustomListModel::CustomListModel(const QStringList &data, QObject *parent)
    : QAbstractListModel(parent), m_data(data)
{
}

int CustomListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.count();
}

QVariant CustomListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_data.size())
        return QVariant();

    if (role == Qt::DisplayRole)
        return m_data.at(index.row());

    return QVariant();
}

void CustomListModel::addItem(const QString &item)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_data.append(item);
    endInsertRows();
}
