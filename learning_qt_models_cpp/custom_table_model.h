#ifndef CUSTOM_TABLE_MODEL_H
#define CUSTOM_TABLE_MODEL_H

#include <QAbstractTableModel>
#include <QVector>
#include <QVariant>

struct PersonData {
    QString name;
    int age;
    QString role;
};

class CustomTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CustomTableModel(const QVector<PersonData> &data, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    QVector<PersonData> m_data;
    QStringList m_headers;
};

#endif // CUSTOM_TABLE_MODEL_H
