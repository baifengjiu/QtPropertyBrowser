#include "custompropertymanager.h"
#include <algorithm>    // equal

struct TablePropertyManager::Data
{
    QAbstractTableModel* model{ nullptr };
    QMap<uint8_t, QAbstractItemDelegate*> delegates;
    QMap<Qt::Orientation, bool> headerVisible;
    QModelIndex index;
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
    connect(model, &QAbstractTableModel::dataChanged, this,
            [property, this](const QModelIndex& topLeft,
                             const QModelIndex& bottomRight,
                             const QVector<int>& roles = QVector<int>())
    {
        if (roles.contains(Qt::DisplayRole))
            emit valueChanged(property, topLeft);
    });
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

void TablePropertyManager::setSelectedIndex(QtProperty* property, const QModelIndex& index)
{
    propertyToValue_[property].index = index;
}

QModelIndex TablePropertyManager::selectedIndex(QtProperty* property)
{
    const auto& data = propertyToValue_.value(property);
    return data.index;
}

void TablePropertyManager::initializeProperty(QtProperty* property)
{
    propertyToValue_[property] = TablePropertyManager::Data();
}

void TablePropertyManager::uninitializeProperty(QtProperty* property)
{
    const auto& data = propertyToValue_.value(property);
    propertyToValue_.remove(property);
    disconnect(data.model);
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

/**
 * EnumMapPropertyManager
 */
struct EnumMapPropertyManager::Data
{
    QList<int> values;
    QMap<int, QString> enumNames;
    char sep = ',';
};

EnumMapPropertyManager::EnumMapPropertyManager(QObject* parent)
    : QtAbstractPropertyManager(parent)
{

}

EnumMapPropertyManager::~EnumMapPropertyManager()
{
    clear();
}

QList<int> EnumMapPropertyManager::value(const QtProperty* property) const
{
    const auto& value = propertyToValue_.value(property);
    return value.values;
}

QMap<int, QString> EnumMapPropertyManager::enumNames(const QtProperty* property) const
{
    const auto& value =  propertyToValue_.value(property);
    return value.enumNames;
}

char EnumMapPropertyManager::sep(const QtProperty* property) const
{
    const auto& value = propertyToValue_.value(property);
    return value.sep;
}

void EnumMapPropertyManager::setEnumNames(QtProperty* property, const QMap<int, QString>& names)
{
    auto it = propertyToValue_.find(property);
    if(it == propertyToValue_.end())
        return;
    auto& data = it.value();

    if(data.enumNames == names)
        return;

    data.enumNames = names;
    data.values.clear();

    emit enumNamesChanged(property, data.enumNames);

    emit propertyChanged(property);
    emit valueChanged(property, data.values);
}


void EnumMapPropertyManager::setValue(QtProperty* property, const QList<int>& value)
{
    auto it = propertyToValue_.find(property);
    if (it == propertyToValue_.end())
        return;

    auto& data = it.value();

    if(data.values == value)
        return;

    data.values = value;

    emit propertyChanged(property);
    emit valueChanged(property, data.values);
}

void EnumMapPropertyManager::setSep(QtProperty* property, const char sep)
{
    auto it = propertyToValue_.find(property);
    if (it == propertyToValue_.end())
        return;
    auto& data = it.value();

    if (data.sep == sep)
        return;
    data.sep = sep;

    emit sepChanged(property, sep);
}


void EnumMapPropertyManager::initializeProperty(QtProperty* property)
{
    propertyToValue_[property] = EnumMapPropertyManager::Data();
}

void EnumMapPropertyManager::uninitializeProperty(QtProperty* property)
{
    propertyToValue_.remove(property);
}
