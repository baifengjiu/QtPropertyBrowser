#pragma once

#include "qtvariantproperty.h"
#include <QAbstractTableModel>
#include <QAbstractItemDelegate>
#include <QMap>
#include <QDialog>
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
class CustomDialog :public QDialog
{
public:
    CustomDialog(QWidget* parent = nullptr) :QDialog(parent) {}

    virtual QString getValue() const = 0;
};

class LineEditWithButtonPropertyManager : public QtAbstractPropertyManager
{
    Q_OBJECT
public:
    LineEditWithButtonPropertyManager(QObject* parent = nullptr);
    ~LineEditWithButtonPropertyManager();

    /**
     * @brief:     添加按钮点击触发的弹出框
     * @param[in]: QtProperty * property
     * @param[in]: CustomDialog * dlg   自定义的弹出框
     */
    void addPopupDialog(QtProperty* property, CustomDialog* dlg);
    CustomDialog* popupDialog(QtProperty* property);

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
