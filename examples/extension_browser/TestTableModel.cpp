#include "TestTableModel.h"
#include <QComboBox>

TestTableModel::TestTableModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    m_headers << tr("Value") << tr("Description");

    for (int i = 0; i < 1; i++)
    {
        m_cells.append({ i + 1,QString("desc_%1").arg(i) });
    }
}

TestTableModel::~TestTableModel()
{
}

Qt::ItemFlags TestTableModel::flags(const QModelIndex& index) const
{
    return Qt::ItemIsEditable | Qt::ItemIsEnabled;
}

bool TestTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid() && !value.isValid())
        return false;

    if (role == Qt::EditRole)
    {
        int row = index.row();
        int col = index.column();
        m_cells[row][col] = value;
    }
    return false;
}

QVariant TestTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int col = index.column();
    switch (role)
    {
        case Qt::DisplayRole:
            return m_cells[row][col];
            break;
        case Qt::TextAlignmentRole:
            return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
            break;
        default:
            break;
    }
    return QVariant();
}

QVariant TestTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Orientation::Horizontal)
    {
        switch (role)
        {
            case Qt::DisplayRole:
                return QVariant(m_headers.at(section));
                break;
            case Qt::TextAlignmentRole:
                return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
                break;
        }
    }
    return QVariant();
}

int TestTableModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_cells.size();
}

int TestTableModel::columnCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_headers.size();
}

void TestTableModel::removeRow(int row)
{
    if (row > rowCount())
    {
        return;
    }

    beginRemoveRows(QModelIndex(),row,row);
    m_cells.removeAt(row);
    endRemoveRows();
}

void TestTableModel::addRow()
{
    int row = rowCount();
    beginInsertRows(QModelIndex(), row, row);
    m_cells.append({ row,QString("desc_%1").arg(row) });
    endInsertRows();
}

ValueDelegate::ValueDelegate(QObject* parent)
    : QStyledItemDelegate(parent)
{
}

ValueDelegate::~ValueDelegate()
{
}

QWidget* ValueDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    QComboBox* editor = new QComboBox(parent);
    editor->addItems({ "None","Addition","ArcCosine","ArcSin","ArcTangent","Cosine","Division" });
    return editor;
}

void ValueDelegate::setEditorData(QWidget* wid, const QModelIndex& index) const
{
    auto value = index.model()->data(index, Qt::DisplayRole);
    QComboBox* editor = qobject_cast<QComboBox*>(wid);
    editor->setCurrentText(value.toString());
}

void ValueDelegate::setModelData(QWidget* wid, QAbstractItemModel* model, const QModelIndex& index) const
{
    QComboBox* editor = qobject_cast<QComboBox*>(wid);
    QString value = editor->currentText();
    model->setData(index, value, Qt::EditRole);
}

void ValueDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    editor->setGeometry(option.rect);
}