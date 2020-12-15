#include "customeditorfactory.h"
#include "lineeditwithbutton.h"
#include "enummapedit.h"

#include <QTableView>
#include <QHeaderView>
#include <QTextEdit>
#include <QDialog>

/**
 * TableEditorFactory
 */

struct TableEditorFactory::Impl {
    typedef QList<QTableView*> EditorList;
    typedef QMap<QtProperty*, EditorList> PropertyToEditorListMap;
    typedef QMap<QTableView*, QtProperty*> EditorToPropertyMap;

    PropertyToEditorListMap  createdEditors;
    EditorToPropertyMap editorToProperty;
};

TableEditorFactory::TableEditorFactory(QObject* parent)
    : QtAbstractEditorFactory<TablePropertyManager>(parent)
    , data_(new Impl)
{
}

TableEditorFactory::~TableEditorFactory()
{
    qDeleteAll(data_->editorToProperty.keys());
}

void TableEditorFactory::connectPropertyManager(TablePropertyManager* manager)
{
}

QWidget* TableEditorFactory::createEditor(TablePropertyManager* manager, QtProperty* property, QWidget* parent)
{
    QTableView* table = new QTableView(parent);
    table->setObjectName("EditorFactoryTable");
    table->horizontalHeader()->setStretchLastSection(true);
    table->setModel(manager->model(property));
    bool hv = manager->headerVisible(property, Qt::Orientation::Horizontal);
    table->horizontalHeader()->setVisible(hv);
    bool vv = manager->headerVisible(property, Qt::Orientation::Vertical);
    table->verticalHeader()->setVisible(vv);
    auto map = manager->itemDelegates(property);
    for (auto it = map.cbegin(); it != map.cend(); it++)
    {
        table->setItemDelegateForColumn(it.key(), it.value());
    }

    auto it = data_->createdEditors.find(property);
    if (it == data_->createdEditors.end())
    {
        it = data_->createdEditors.insert(property, Impl::EditorList());
    }
    it.value().append(table);
    data_->editorToProperty.insert(table, property);

    connect(table, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestroyed(QObject*)));
    connect(table, &QTableView::clicked, this, [manager, property](const QModelIndex& index) {
        manager->setSelectedIndex(property, index);
        emit manager->clicked(property, index);
    });
    connect(table, &QTableView::doubleClicked, this, [manager, property](const QModelIndex& index) {
        manager->setSelectedIndex(property, index);
        emit manager->dbClicked(property, index);
    });

    return table;
}

void TableEditorFactory::disconnectPropertyManager(TablePropertyManager* manager)
{
}

void TableEditorFactory::slotEditorDestroyed(QObject* object)
{
    const auto ecend = data_->editorToProperty.end();
    for (auto itEditor = data_->editorToProperty.begin(); itEditor != ecend; ++itEditor) {
        if (itEditor.key() == object) {
            QTableView* table = itEditor.key();
            QtProperty* property = itEditor.value();
            const auto pit = data_->createdEditors.find(property);
            if (pit != data_->createdEditors.end()) {
                pit.value().removeAll(table);
                if (pit.value().isEmpty())
                    data_->createdEditors.erase(pit);
            }
            data_->editorToProperty.erase(itEditor);
            return;
        }
    }
}

/**
 * LineEditWithButtonEditorFactory
 */
struct LineEditWithButtonEditorFactory::Impl {
    typedef QList<LineEditWithButton*> EditorList;
    typedef QMap<QtProperty*, EditorList> PropertyToEditorListMap;
    typedef QMap<LineEditWithButton*, QtProperty*> EditorToPropertyMap;

    PropertyToEditorListMap  createdEditors;
    EditorToPropertyMap editorToProperty;
};

LineEditWithButtonEditorFactory::LineEditWithButtonEditorFactory(QObject* parent)
    : QtAbstractEditorFactory<LineEditWithButtonPropertyManager>(parent)
    , data_(new Impl)
{
}

LineEditWithButtonEditorFactory::~LineEditWithButtonEditorFactory()
{
    qDeleteAll(data_->editorToProperty.keys());
}

void LineEditWithButtonEditorFactory::connectPropertyManager(LineEditWithButtonPropertyManager* manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty*, QString)), this, SLOT(slotPropertyChanged(QtProperty*, QString)));
}

QWidget* LineEditWithButtonEditorFactory::createEditor(LineEditWithButtonPropertyManager* manager, QtProperty* property, QWidget* parent)
{
    LineEditWithButton* lineEdit = new LineEditWithButton(parent);
    lineEdit->setClearButtonVisible(false);
    lineEdit->setText(manager->value(property));

    auto it = data_->createdEditors.find(property);
    if (it == data_->createdEditors.end())
    {
        it = data_->createdEditors.insert(property, Impl::EditorList());
    }
    it.value().append(lineEdit);
    data_->editorToProperty.insert(lineEdit, property);
    auto dlg = manager->popupDialog(property);

    connect(lineEdit, SIGNAL(textEdited(QString)), this, SLOT(slotSetValue(QString)));
    connect(lineEdit, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestroyed(QObject*)));
    connect(lineEdit, &LineEditWithButton::buttonClicked, [lineEdit, dlg]() {
        if (!dlg)
            return;
        if (dlg->exec() == QDialog::Accepted)
        {
            auto text = dlg->getValue();
            lineEdit->setText(text);
        }
    });
    return lineEdit;
}

void LineEditWithButtonEditorFactory::disconnectPropertyManager(LineEditWithButtonPropertyManager* manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty*, QString)), this, SLOT(slotPropertyChanged(QtProperty*, QString)));
}

void LineEditWithButtonEditorFactory::slotPropertyChanged(QtProperty* property, const QString& value)
{
    const auto it = data_->createdEditors.constFind(property);
    if (it == data_->createdEditors.constEnd())
        return;

    for (auto editor : it.value()) {
        if (editor->text() != value) {
            editor->blockSignals(true);
            editor->setText(value);
            editor->blockSignals(false);
        }
    }
}

void LineEditWithButtonEditorFactory::slotEditorDestroyed(QObject* object)
{
    const auto ecend = data_->editorToProperty.end();
    for (auto itEditor = data_->editorToProperty.begin(); itEditor != ecend; ++itEditor) {
        if (itEditor.key() == object) {
            LineEditWithButton* table = itEditor.key();
            QtProperty* property = itEditor.value();
            const auto pit = data_->createdEditors.find(property);
            if (pit != data_->createdEditors.end()) {
                pit.value().removeAll(table);
                if (pit.value().isEmpty())
                    data_->createdEditors.erase(pit);
            }
            data_->editorToProperty.erase(itEditor);
            return;
        }
    }
}

void LineEditWithButtonEditorFactory::slotSetValue(const QString& value)
{
    QObject* object = sender();
    const auto ecend = data_->editorToProperty.constEnd();
    for (auto itEditor = data_->editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object)
        {
            QtProperty* property = itEditor.value();
            auto manager = propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

/**
 * TextEditFactory
 */
struct TextEditFactory::Impl {
    typedef QList<QTextEdit*> EditorList;
    typedef QMap<QtProperty*, EditorList> PropertyToEditorListMap;
    typedef QMap<QTextEdit*, QtProperty*> EditorToPropertyMap;

    PropertyToEditorListMap  createdEditors;
    EditorToPropertyMap editorToProperty;
};

TextEditFactory::TextEditFactory(QObject* parent)
    : QtAbstractEditorFactory<TextEditPropertyManager>(parent)
    , data_(new Impl)
{
}

TextEditFactory::~TextEditFactory()
{
    qDeleteAll(data_->editorToProperty.keys());
}

void TextEditFactory::connectPropertyManager(TextEditPropertyManager* manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty*, QString)), this, SLOT(slotPropertyChanged(QtProperty*, QString)));
}

QWidget* TextEditFactory::createEditor(TextEditPropertyManager* manager, QtProperty* property, QWidget* parent)
{
    QTextEdit* editor = new QTextEdit(parent);
    editor->setReadOnly(false);
    editor->setText(manager->value(property));

    auto it = data_->createdEditors.find(property);
    if (it == data_->createdEditors.end())
    {
        it = data_->createdEditors.insert(property, Impl::EditorList());
    }
    it.value().append(editor);
    data_->editorToProperty.insert(editor, property);

    connect(editor, &QTextEdit::textChanged, this, [editor, this]() {
        slotSetValue(editor->toPlainText());
    });
    connect(editor, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestroyed(QObject*)));
    return editor;
}

void TextEditFactory::disconnectPropertyManager(TextEditPropertyManager* manager)
{
    disconnect(manager, SIGNAL(valueChanged(QtProperty*, QString)), this, SLOT(slotPropertyChanged(QtProperty*, QString)));
}

void TextEditFactory::slotPropertyChanged(QtProperty* property, const QString& value)
{
    const auto it = data_->createdEditors.constFind(property);
    if (it == data_->createdEditors.constEnd())
        return;

    for (QTextEdit* editor : it.value()) {
        if (editor->toPlainText() != value)
        {
            editor->blockSignals(true);
            editor->setText(value);
            editor->blockSignals(false);
        }
    }
}

void TextEditFactory::slotSetValue(const QString& value)
{
    QObject* object = sender();
    const auto ecend = data_->editorToProperty.constEnd();
    for (auto itEditor = data_->editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object)
        {
            QtProperty* property = itEditor.value();
            TextEditPropertyManager* manager = propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

void TextEditFactory::slotEditorDestroyed(QObject* object)
{
    const auto ecend = data_->editorToProperty.end();
    for (auto itEditor = data_->editorToProperty.begin(); itEditor != ecend; ++itEditor) {
        if (itEditor.key() == object) {
            auto widget = itEditor.key();
            QtProperty* property = itEditor.value();
            const auto pit = data_->createdEditors.find(property);
            if (pit != data_->createdEditors.end()) {
                pit.value().removeAll(widget);
                if (pit.value().isEmpty())
                    data_->createdEditors.erase(pit);
            }
            data_->editorToProperty.erase(itEditor);
            //itEditor.key()->deleteLater();
            return;
        }
    }
}

/**
 * EnumMapFactory
 */
struct EnumMapFactory::Impl {
    typedef QList<EnumMapEdit*> EditorList;
    typedef QMap<QtProperty*, EditorList> PropertyToEditorListMap;
    typedef QMap<EnumMapEdit*, QtProperty*> EditorToPropertyMap;

    PropertyToEditorListMap  createdEditors;
    EditorToPropertyMap editorToProperty;
};

EnumMapFactory::EnumMapFactory(QObject* parent)
    : QtAbstractEditorFactory<EnumMapPropertyManager>(parent)
    , data_(new Impl)
{
}
EnumMapFactory::~EnumMapFactory()
{
    qDeleteAll(data_->editorToProperty.keys());
}

void EnumMapFactory::connectPropertyManager(EnumMapPropertyManager* manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty*, const QList<int>&)), this, SLOT(slotPropertyChanged(QtProperty*, const QList<int>&)));
    connect(manager, SIGNAL(enumNamesChanged(QtProperty*, const QMap<int, QString>&)), this, SLOT(slotEnumNamesChanged(QtProperty*, const QMap<int, QString>&)));
}

QWidget* EnumMapFactory::createEditor(EnumMapPropertyManager* manager, QtProperty* property, QWidget* parent)
{
    EnumMapEdit* editor = new EnumMapEdit(parent);
    // editor->setReadOnly(true);

    auto it = data_->createdEditors.find(property);
    if (it == data_->createdEditors.end())
    {
        it = data_->createdEditors.insert(property, Impl::EditorList());
    }
    it.value().append(editor);
    data_->editorToProperty.insert(editor, property);

    auto names = manager->enumNames(property);
    auto values = manager->value(property);
    editor->setDropDownData(names);
    editor->setValues(values);
    connect(editor, SIGNAL(valueChanged(QList<int>)), this, SLOT(slotSetValue(QList<int>)));
    connect(editor, SIGNAL(destroyed(QObject*)), this, SLOT(slotEditorDestroyed(QObject*)));
    return editor;
}

void EnumMapFactory::disconnectPropertyManager(EnumMapPropertyManager* manager)
{
    connect(manager, SIGNAL(valueChanged(QtProperty*, const QList<int>&)), this, SLOT(slotPropertyChanged(QtProperty*, const QList<int>&)));
    connect(manager, SIGNAL(enumNamesChanged(QtProperty*, const QMap<int, QString>&)), this, SLOT(slotEnumNamesChanged(QtProperty*, const QMap<int, QString>&)));
    connect(manager, SIGNAL(sepChanged(QtProperty*, QStringList)), this, SLOT(slotSepChanged(QtProperty*, QStringList)));
}

void EnumMapFactory::slotPropertyChanged(QtProperty* property, const QList<int>& value)
{
    const auto it = data_->createdEditors.constFind(property);
    if (it == data_->createdEditors.constEnd())
        return;

    for (EnumMapEdit* editor : it.value()) {
        editor->blockSignals(true);
        editor->setValues(value);
        editor->blockSignals(false);
    }
}

void EnumMapFactory::slotSetValue(const QList<int>& value)
{
    QObject* object = sender();
    const auto ecend = data_->editorToProperty.constEnd();
    for (auto itEditor = data_->editorToProperty.constBegin(); itEditor != ecend; ++itEditor)
        if (itEditor.key() == object)
        {
            QtProperty* property = itEditor.value();
            EnumMapPropertyManager* manager = propertyManager(property);
            if (!manager)
                return;
            manager->setValue(property, value);
            return;
        }
}

void EnumMapFactory::slotEnumNamesChanged(QtProperty* property, const QMap<int, QString>& names)
{
    const auto it = data_->createdEditors.constFind(property);
    if (it == data_->createdEditors.constEnd())
        return;
    for (EnumMapEdit* editor : it.value()) {
        editor->blockSignals(true);
        editor->setDropDownData(names);
        editor->blockSignals(false);
    }
}

void EnumMapFactory::slotSepChanged(QtProperty* property, char sep)
{
    const auto it = data_->createdEditors.constFind(property);
    if (it == data_->createdEditors.constEnd())
        return;
    for (EnumMapEdit* editor : it.value()) {
        editor->blockSignals(true);
        editor->setSep(sep);
        editor->blockSignals(false);
    }
}

void EnumMapFactory::slotEditorDestroyed(QObject* object)
{
    const auto ecend = data_->editorToProperty.end();
    for (auto itEditor = data_->editorToProperty.begin(); itEditor != ecend; ++itEditor) {
        if (itEditor.key() == object) {
            auto widget = itEditor.key();
            QtProperty* property = itEditor.value();
            const auto pit = data_->createdEditors.find(property);
            if (pit != data_->createdEditors.end()) {
                pit.value().removeAll(widget);
                if (pit.value().isEmpty())
                    data_->createdEditors.erase(pit);
            }
            data_->editorToProperty.erase(itEditor);
            return;
        }
    }
}