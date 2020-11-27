#include "TableModel.h"

TableModel::TableModel(QObject* parent)
    :QAbstractTableModel(parent)
{
    headers_ << "id" << "name" << "code" << "unit" << "table" << "isShow" << "type" << "minimum"
        << "maximum" << "textBrowser";

    for (int i = 0; i < 10; i++)
    {
        TableDataItem* item = new TableDataItem;
        item->id = i;
        item->code = QString("Code_%1").arg(i);
        item->name = QString("Name_%1").arg(i);
        item->unit = QString("unit_%1").arg(i);
        item->table = QString("Table_%1").arg(i);
        item->isShow = i % 2;
        item->type = Type(i % 11);
        item->minimum = i;
        item->maximum = i * 10;
        item->textBrowser = QString("Text Browser_%1").arg(i);
        datas_.append(item);
    }
}

TableModel::~TableModel()
{
}

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) return QVariant();

    if (role == Qt::DisplayRole)
    {
        auto item = datas_.value(index.row());
        switch (index.column())
        {
            case 0: return item->id;
            case 1: return item->code;
            case 2: return item->name;
            case 3: return item->unit;
            case 4: return item->table;
            case 5: return item->isShow;
            case 6: return item->type;
            case 7: return item->minimum;
            case 8: return item->maximum;
            case 9: return item->textBrowser;
        }
    }
    return QVariant();
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section > columnCount())
        return QVariant();
    if (role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal)
    {
        return headers_.value(section);
    }
    return QVariant();
}

int TableModel::rowCount(const QModelIndex&) const
{
    return datas_.count();
}

int TableModel::columnCount(const QModelIndex&) const
{
    return headers_.count();
}

TableDataItem* TableModel::dataItem(int row)
{
    if (row > rowCount())
    {
        return nullptr;
    }
    return datas_.value(row);
}

void TableModel::refresh(int row)
{
    auto topLeft = index(row, 0);
    auto bottomRight = index(row, columnCount() - 1);
    dataChanged(topLeft, bottomRight, { Qt::DisplayRole });
}