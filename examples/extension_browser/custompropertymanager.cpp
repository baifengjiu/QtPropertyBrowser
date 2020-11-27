#include "custompropertymanager.h"

struct TablePropertyManager::Data
{
    QAbstractTableModel* model{ nullptr };
    QMap<uint8_t, QAbstractItemDelegate*> delegates;
    QMap<Qt::Orientation, bool> headerVisible;
};

TablePropertyManager::TablePropertyManager(QObject* parent)
    : QtAbstractPropertyManager(parent)
{
}

TablePropertyManager::~TablePropertyManager()
{
    clear();
}

void TablePropertyManager::setModel(QtProperty* property, QAbstractTableModel* model)
{
    propertyToValue_[property].model = model;
}

QAbstractTableModel* TablePropertyManager::model(const QtProperty* property) const
{
    const auto& data = propertyToValue_.value(property);
    return data.model;
}

void TablePropertyManager::setHeaderVisible(const QtProperty* property, Qt::Orientation orientation, bool isVisible)
{
    propertyToValue_[property].headerVisible[orientation] = isVisible;
}

bool TablePropertyManager::headerVisible(const QtProperty* property, Qt::Orientation orientation) const
{
    const auto& data = propertyToValue_.value(property);

    return data.headerVisible.value(orientation, true);
}

void TablePropertyManager::setItemDelegates(QtProperty* property, QMap<uint8_t, QAbstractItemDelegate*> delegates)
{
    propertyToValue_[property].delegates = delegates;
}

QMap<uint8_t, QAbstractItemDelegate*> TablePropertyManager::itemDelegates(QtProperty* property)
{
    const auto& data = propertyToValue_.value(property);
    return data.delegates;
}

void TablePropertyManager::initializeProperty(QtProperty* property)
{
    propertyToValue_[property] = TablePropertyManager::Data();
}

void TablePropertyManager::uninitializeProperty(QtProperty* property)
{
    propertyToValue_.remove(property);
}

/**
 * LineEditWithButtonPropertyManager
 */
struct LineEditWithButtonPropertyManager::Data
{
    CustomDialog* dlg{ nullptr };
    QString val;
};

LineEditWithButtonPropertyManager::LineEditWithButtonPropertyManager(QObject* parent)
    : QtAbstractPropertyManager(parent)
{
}
LineEditWithButtonPropertyManager::~LineEditWithButtonPropertyManager()
{
    clear();
}

void LineEditWithButtonPropertyManager::addPopupDialog(QtProperty* property, CustomDialog* dlg)
{
    propertyToValue_[property].dlg = dlg;
}

CustomDialog* LineEditWithButtonPropertyManager::popupDialog(QtProperty* property)
{
    return propertyToValue_.value(property).dlg;
}

void LineEditWithButtonPropertyManager::setValue(QtProperty* property, const QString& val)
{
    auto value = propertyToValue_.value(property).val;
    if (value == val)
        return;

    propertyToValue_[property].val = val;

    emit propertyChanged(property);
    emit valueChanged(property, val);
}

QString LineEditWithButtonPropertyManager::value(const QtProperty* property) const
{
    return propertyToValue_.value(property).val;
}

void LineEditWithButtonPropertyManager::initializeProperty(QtProperty* property)
{
    propertyToValue_[property] = LineEditWithButtonPropertyManager::Data();
}

void LineEditWithButtonPropertyManager::uninitializeProperty(QtProperty* property)
{
    propertyToValue_.remove(property);
}

/**
 * TextEditPropertyManager
 */
struct TextEditPropertyManager::Data
{
    QString val;
};

TextEditPropertyManager::TextEditPropertyManager(QObject* parent)
    : QtAbstractPropertyManager(parent)
{
}

TextEditPropertyManager::~TextEditPropertyManager()
{
    clear();
}

QString TextEditPropertyManager::value(const QtProperty* property) const
{
    return propertyToValue_.value(property).val;
}

void TextEditPropertyManager::setValue(QtProperty* property, const QString& val)
{
    auto value = propertyToValue_.value(property).val;
    if (value == val)
        return;

    propertyToValue_[property].val = val;

    emit propertyChanged(property);
    emit valueChanged(property, val);
}

void TextEditPropertyManager::initializeProperty(QtProperty* property)
{
    propertyToValue_[property] = TextEditPropertyManager::Data();
}

void TextEditPropertyManager::uninitializeProperty(QtProperty* property)
{
    propertyToValue_.remove(property);
}