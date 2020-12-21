#pragma once

#include "qtvariantproperty.h"
#include <QAbstractTableModel>
#include <QAbstractItemDelegate>
#include <QMap>
#include <QString>

/**
 * TablePropertyManager
 */
class TablePropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT
public:
    TablePropertyManager(QObject* parent = nullptr);
    ~TablePropertyManager();

    void setModel(QtProperty* property, QAbstractTableModel* model);
    QAbstractTableModel* model(const QtProperty* property) const;

    void setHeaderVisible(const QtProperty* property, Qt::Orientation orientation, bool isVisible);
    bool headerVisible(const QtProperty* property, Qt::Orientation orientation) const;

    void setItemDelegates(QtProperty* property, QMap<uint8_t, QAbstractItemDelegate*> delegates);
    QMap<uint8_t, QAbstractItemDelegate*> itemDelegates(QtProperty* property);

    void setColWidth(QtProperty* property, QMap<uint8_t, uint8_t> colWidths);
    QMap<uint8_t, uint8_t> itemColWidth(QtProperty* property);

    void setSelectedIndex(QtProperty* property, const QModelIndex& index);
    QModelIndex selectedIndex(QtProperty* property);

Q_SIGNALS:
    void valueChanged(QtProperty* property, const QModelIndex& index);
    void dbClicked(QtProperty* property, const QModelIndex& index);
    void clicked(QtProperty* property, const QModelIndex& index);

protected:
    void initializeProperty(QtProperty* property) override;
    void uninitializeProperty(QtProperty* property) override;

private:
    struct Data;
    QMap<const QtProperty*, Data> propertyToValue_;
};

/**
 * LineEditWithButtonPropertyManager
 */
class LineEditWithButtonPropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT
public:
    LineEditWithButtonPropertyManager(QObject* parent = nullptr);
    ~LineEditWithButtonPropertyManager();

    /**
     * @brief:     Add event triggered by button click
     * @param[in]: QtProperty * property
     * @param[in]: std::function<bool(QString&)>  bool:Confirm/Cancel QString:out parameter.Determine the modified value.
     */
    void addClickedHandle(QtProperty* property, std::function<bool(QString&)>);
    std::function<bool(QString&)> clickedHuandle(QtProperty* property);

    void setReadOnly(QtProperty* property, bool readOnly);
    bool readOnly(QtProperty* property);

    QString value(const QtProperty* property) const;

public Q_SLOTS:
    void setValue(QtProperty* property, const QString& val);

Q_SIGNALS:
    void valueChanged(QtProperty* property, const QString& val);
    void readOnlyChanged(QtProperty* property, bool readOnly);

protected:
    void initializeProperty(QtProperty* property) override;
    void uninitializeProperty(QtProperty* property) override;

private:
    struct Data;
    QMap<const QtProperty*, Data> propertyToValue_;
};

/**
 * TextEditPropertyManager
 */
class TextEditPropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT
public:
    TextEditPropertyManager(QObject* parent = 0);
    ~TextEditPropertyManager();

    QString value(const QtProperty* property) const;

public Q_SLOTS:
    void setValue(QtProperty* property, const QString& val);

Q_SIGNALS:
    void valueChanged(QtProperty* property, const QString& val);

protected:
    void initializeProperty(QtProperty* property) override;
    void uninitializeProperty(QtProperty* property) override;

private:
    struct Data;
    QMap<const QtProperty*, Data> propertyToValue_;
};

/**
 * EnumMapPropertyManager
 */
class EnumMapPropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT
public:
    EnumMapPropertyManager(QObject* parent);
    ~EnumMapPropertyManager();

    QList<int> value(const QtProperty* property) const;
    QMap<int, QString> enumNames(const QtProperty* property) const;
    char sep(const QtProperty* property) const;

public Q_SLOTS:
    void setEnumNames(QtProperty* property, const QMap<int, QString>& names);
    void setValue(QtProperty* property, const QList<int>& value);
    void setSep(QtProperty* property, const char sep); // Separator, default ','

Q_SIGNALS:
    void valueChanged(QtProperty* property, const QList<int>& value);
    void enumNamesChanged(QtProperty* property, const QMap<int, QString>& names);
    void sepChanged(QtProperty* property, char sep);

protected:
    void initializeProperty(QtProperty* property) override;
    void uninitializeProperty(QtProperty* property) override;

private:
    struct Data;
    QMap<const QtProperty*, Data> propertyToValue_;
};
