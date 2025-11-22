#include "custom_table_model.h"

CustomTableModel::CustomTableModel(const QVector<PersonData> &data, QObject *parent)
    : QAbstractTableModel(parent), m_data(data)
{
    m_headers << "Name" << "Age" << "Role";
}

int CustomTableModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_data.count();
}

int CustomTableModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_headers.count();
}

QVariant CustomTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto &person = m_data.at(index.row());
        switch (index.column()) {
        case 0: return person.name;
        case 1: return person.age;
        case 2: return person.role;
        default: return QVariant();
        }
    }
    return QVariant();
}

QVariant CustomTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section < m_headers.size())
                return m_headers.at(section);
        } else {
            return QString::number(section + 1);
        }
    }
    return QVariant();
}
