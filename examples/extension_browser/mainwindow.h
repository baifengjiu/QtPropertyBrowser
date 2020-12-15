#pragma once

#include "tablemodel.h"
#include "custompropertymanager.h"
#include "qtpropertymanager.h"
#include <QtProperty>
#include <QMainWindow>
#include <QMap>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

private Q_SLOTS:
    void selectedChanged(const QModelIndex& index);
    void valueChanged(QtProperty* property, bool value);
    void valueChanged(QtProperty* property, int value);
    void valueChanged(QtProperty* property, double value);
    void valueChanged(QtProperty* property, const QString& value);
    void valueChanged(QtProperty* property, const QList<int>& value);

private:
    void init();
    /**
     * @brief:     ÃÌº” Ù–‘
     * @param[in]: QtProperty * property  Ù–‘
     * @return:
     */
    void addProperty(QtProperty* property);
    void removeProperty(QtProperty* property);

private:
    Ui::MainWindow* ui;
    TableModel* model_;

    TableDataItem* currentItem_;

    QtIntPropertyManager* intManager_;
    QtDoublePropertyManager* doubleManager_;
    QtStringPropertyManager* stringManager_;
    QtBoolPropertyManager* boolManager_;
    QtEnumPropertyManager* enumManager_;
    QtGroupPropertyManager* groupManager_;

    TablePropertyManager* tableManager_;
    LineEditWithButtonPropertyManager* lineManager_;
    TextEditPropertyManager* textManager_;
    EnumMapPropertyManager* enumMapManager_;

    QtProperty* root2_;

    QMap<QtProperty*, QString> propertyToId;
    QMap<QString, QtProperty*> idToProperty;
    QMap<QString, bool> idToExpanded;
};
