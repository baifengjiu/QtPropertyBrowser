#pragma once

#include "custompropertymanager.h"
#include <qteditorfactory.h>
#include <QAbstractTableModel>
#include <QScopedPointer>

/**
 * TableEditorFactory
 */
class TableEditorFactory : public QtAbstractEditorFactory<TablePropertyManager>
{
    Q_OBJECT
public:
    TableEditorFactory(QObject* parent = 0);
    ~TableEditorFactory();

protected:
    void connectPropertyManager(TablePropertyManager* manager) override;
    QWidget* createEditor(TablePropertyManager* manager, QtProperty* property, QWidget* parent) override;
    void disconnectPropertyManager(TablePropertyManager* manager) override;

protected Q_SLOTS:
    void slotEditorDestroyed(QObject* object);

private:
    struct Impl;
    QScopedPointer<Impl> data_;
};

/**
 * LineEditWithButtonEditorFactory
 */
class LineEditWithButtonEditorFactory : public QtAbstractEditorFactory<LineEditWithButtonPropertyManager>
{
    Q_OBJECT
public:
    LineEditWithButtonEditorFactory(QObject* parent = 0);
    ~LineEditWithButtonEditorFactory();

protected:
    void connectPropertyManager(LineEditWithButtonPropertyManager* manager) override;
    QWidget* createEditor(LineEditWithButtonPropertyManager* manager, QtProperty* property, QWidget* parent) override;
    void disconnectPropertyManager(LineEditWithButtonPropertyManager* manager) override;

protected Q_SLOTS:
    void slotPropertyChanged(QtProperty*, const QString&);
    void slotEditorDestroyed(QObject* object);
    void slotSetValue(const QString&);

private:

    struct Impl;
    QScopedPointer<Impl> data_;
};

/**
 * TextEditFactory
 */
class TextEditFactory : public QtAbstractEditorFactory<TextEditPropertyManager>
{
    Q_OBJECT
public:
    TextEditFactory(QObject* parent = 0);
    ~TextEditFactory();

protected:
    void connectPropertyManager(TextEditPropertyManager* manager) override;
    QWidget* createEditor(TextEditPropertyManager* manager, QtProperty* property, QWidget* parent) override;
    void disconnectPropertyManager(TextEditPropertyManager* manager) override;

private Q_SLOTS:
    void slotPropertyChanged(QtProperty*, const QString&);
    void slotSetValue(const QString&);
    void slotEditorDestroyed(QObject*);

private:

    struct Impl;
    QScopedPointer<Impl> data_;
};
