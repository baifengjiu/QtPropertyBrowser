#pragma once

#include <QString>
#include <QAbstractItemModel>
#include <QList>
#include <QStyledItemDelegate>

class TestTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    TestTableModel(QObject* parent = nullptr);
    ~TestTableModel();

    Qt::ItemFlags flags(const QModelIndex& index) const;
    QVariant data(const QModelIndex& index, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

public Q_SLOTS:
    void addRow();
    void removeRow(int row);

private:
    QStringList m_headers;
    QList<QVariantList> m_cells;
};


class ValueDelegate : public QStyledItemDelegate
{
public:
    ValueDelegate(QObject* parent = nullptr);
    ~ValueDelegate();

    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* wid, const  QModelIndex& index)  const override;
    void setModelData(QWidget* wid, QAbstractItemModel* model, const  QModelIndex& index)  const override;
    void updateEditorGeometry(QWidget* editor, const  QStyleOptionViewItem& option, const  QModelIndex& index) const override;
};
