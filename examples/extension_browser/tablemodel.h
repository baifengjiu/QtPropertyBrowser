#pragma once
#include <QAbstractTableModel>
#include <QList>
#include <QString>

enum Type :uint8_t {
    T_UINT8,
    T_INT8,
    T_UINT16,
    T_INT16,
    T_UINT32,
    T_INT32,
    T_UINT64,
    T_INT64,
    T_FLOAT,
    T_DOUBLE
};

struct TableDataItem
{
    uint16_t id;
    QString name;
    QString code;
    QString unit;
    QString table;
    bool  isShow;
    Type type;
    double minimum;
    double maximum;
    QString textBrowser;
};

class TableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TableModel(QObject* parent = nullptr);
    ~TableModel();

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    TableDataItem* dataItem(int row);
    void refresh(int row);

private:
    QStringList headers_;
    QList<TableDataItem*> datas_;
};
