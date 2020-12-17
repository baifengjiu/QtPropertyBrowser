#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "qteditorfactory.h"
#include "customeditorfactory.h"
#include "TestTableModel.h"
#include "TestDialog.h"
#include <QAbstractItemDelegate>
#include <QAction>

MainWindow::MainWindow()
    :QMainWindow(nullptr)
{
    ui = new Ui::MainWindow();
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
}

void MainWindow::selectedChanged(const QModelIndex& index)
{
    currentItem_ = model_->dataItem(index.row());
    QMap<QtProperty*, QString>::ConstIterator itProp = propertyToId.constBegin();
    while (itProp != propertyToId.constEnd()) {
        delete itProp.key();
        itProp++;
    }
    propertyToId.clear();
    idToProperty.clear();
    QtProperty* root1_ = groupManager_->addProperty("General");
    /* QtProperty**/ root2_ = groupManager_->addProperty("Extraction");
    QtProperty* root3_ = groupManager_->addProperty("Table List");
    QtProperty* root4_ = groupManager_->addProperty("description");

    {
        QtProperty* item1 = intManager_->addProperty("id");
        intManager_->setValue(item1, currentItem_->id);
        root1_->addSubProperty(item1);
        addProperty(item1);

        QtProperty* item2 = stringManager_->addProperty("code");
        stringManager_->setValue(item2, currentItem_->code);
        root1_->addSubProperty(item2);
        addProperty(item2);

        QtProperty* item3 = stringManager_->addProperty("name");
        stringManager_->setValue(item3, currentItem_->name);
        root1_->addSubProperty(item3);
        addProperty(item3);

        QtProperty* item4 = lineManager_->addProperty("unit");

        auto func = [](QString& value)->bool{
            TestDialog dlg;
            if(dlg.exec() == QDialog::Accepted)
            {
                value = dlg.getValue();
                return true;
            }
            return false;
        };
        lineManager_->addClickedHandle(item4, func);
        root1_->addSubProperty(item4);
        addProperty(item4);
    }

    {
        QtProperty* item7 = boolManager_->addProperty("isShow");
        root2_->addSubProperty(item7);
        addProperty(item7);

        QtProperty* item8 = enumManager_->addProperty("type");
        QStringList enumNames;
        enumNames << "UINT8" <<
            "INT8" <<
            "UINT16" <<
            "INT16" <<
            "UINT32" <<
            "INT32" <<
            "UINT64" <<
            "INT64" <<
            "FLOAT" <<
            "DOUBLE";
        enumManager_->setEnumNames(item8, enumNames);
        root2_->addSubProperty(item8);
        addProperty(item8);

        QtProperty* item11 = doubleManager_->addProperty("minimum");
        doubleManager_->setRange(item11, 0, 100);
        doubleManager_->setValue(item11, currentItem_->minimum);
        root2_->addSubProperty(item11);
        addProperty(item11);

        QtProperty* item12 = doubleManager_->addProperty("maximum");
        doubleManager_->setRange(item12, 0, 50);
        doubleManager_->setValue(item12, currentItem_->maximum);
        root2_->addSubProperty(item12);
        addProperty(item12);

        auto item = enumMapManager_->addProperty("map");
        QMap<int, QString> map{
            {0,"00"},
            {1,"01"},
            {2,"10"},
            {3,"11"}
        };

        enumMapManager_->setEnumNames(item, map);
        enumMapManager_->setValue(item, { 0,2 });
        root2_->addSubProperty(item);
        addProperty(item);
    }

    QList<QAction*> actions;
    {
        TestTableModel* model = new TestTableModel();
        QtProperty* item9 = tableManager_->addProperty("Table");
        ui->custombrowser->setLableVisible(item9, false);
        tableManager_->setHeaderVisible(item9, Qt::Orientation::Vertical, false);
        item9->setDescriptionToolTip("Table tool tip");
        tableManager_->setModel(item9, model);
        ValueDelegate* delegate = new ValueDelegate();
        QMap<uint8_t, QAbstractItemDelegate*> map;
        map.insert(1, delegate);
        tableManager_->setItemDelegates(item9, map);

        QAction* addAct = new QAction(QIcon(":/images/addItem.png"), "");
        QAction* delAct = new QAction(QIcon(":/images/deleteItem.png"), "");
        connect(addAct, &QAction::triggered, model, &TestTableModel::addRow);
        actions.append(addAct);
        actions.append(delAct);

        root3_->addSubProperty(item9);
        addProperty(item9);
    }

    {
        QtProperty* item10 = textManager_->addProperty("desc");
        textManager_->setValue(item10, currentItem_->textBrowser);
        ui->custombrowser->setLableVisible(item10, false);
        root4_->addSubProperty(item10);
        item10->setToolTip("Description");
        item10->setDescriptionToolTip("desc tool tip");
        addProperty(item10);
    }

    addProperty(root1_);
    addProperty(root2_);
    addProperty(root3_);
    addProperty(root4_);
    auto browserItem1 = ui->custombrowser->addProperty(root1_);
    auto browserItem2 = ui->custombrowser->addProperty(root2_);
    auto browserItem3 = ui->custombrowser->addProperty(root3_);
    auto browserItem4 = ui->custombrowser->addProperty(root4_);

    //需要放在最后，要等待其他控件的添加,不然不生效
    ui->custombrowser->addActions(browserItem3, actions);
    ui->custombrowser->setExpanded(browserItem1, true);
    ui->custombrowser->setExpanded(browserItem2, true);
    ui->custombrowser->setExpanded(browserItem3, true);
    ui->custombrowser->setExpanded(browserItem4, true);
}

void MainWindow::valueChanged(QtProperty* property, bool value)
{
    if (!propertyToId.contains(property))
        return;

    if (!currentItem_)
        return;

    QString id = propertyToId[property];
    if (id == QLatin1String("isShow")) {
        currentItem_->isShow = value;
    }
    model_->refresh(ui->tableView->selectionModel()->currentIndex().row());
}

void MainWindow::valueChanged(QtProperty* property, int value)
{
    if (!propertyToId.contains(property))
        return;

    if (!currentItem_)
        return;

    QString id = propertyToId[property];
    if (id == QLatin1String("id")) {
        currentItem_->id = value;
    }
    else if (id == "type") {
        currentItem_->type = Type(value);
        if (value == 1)
        {
            QtProperty* item10 = stringManager_->addProperty("test");
            stringManager_->setValue(item10, "3");
            addProperty(item10);
            root2_->addSubProperty(item10);
        }
        if (value == 0)
        {
            removeProperty(idToProperty.value("test"));
        }
    }

    model_->refresh(ui->tableView->selectionModel()->currentIndex().row());
}

void MainWindow::valueChanged(QtProperty* property, double value)
{
    if (!propertyToId.contains(property))
        return;

    if (!currentItem_)
        return;

    QString id = propertyToId[property];
    if (id == QLatin1String("minimum")) {
        currentItem_->minimum = value;
    }
    else if (id == QLatin1String("maximum")) {
        currentItem_->maximum = value;
        property->setToolTip("error");
        ui->custombrowser->setTipInfo(property, CustomBrowser::TipType::Error);
    }

    model_->refresh(ui->tableView->selectionModel()->currentIndex().row());
}

void MainWindow::valueChanged(QtProperty* property, const QString& value)
{
    if (!propertyToId.contains(property))
        return;

    if (!currentItem_)
        return;

    QString id = propertyToId[property];
    if (id == QLatin1String("name")) {
        currentItem_->name = value;
    }
    else if (id == "code") {
        currentItem_->code = value;
    }
    else if (id == "desc") {
        currentItem_->textBrowser = value;
    }
    else if (id == "Table") {
        currentItem_->table = value;
    }
    else if (id == "unit")
    {
        currentItem_->unit = value;
    }
    model_->refresh(ui->tableView->selectionModel()->currentIndex().row());
}

void MainWindow::valueChanged(QtProperty* property, const QList<int>& value)
{
}

void MainWindow::init()
{
    model_ = new TableModel(ui->tableView);
    ui->tableView->setModel(model_);

    connect(ui->tableView, &QTableView::clicked, this, &MainWindow::selectedChanged);

    boolManager_ = new QtBoolPropertyManager(this);
    intManager_ = new QtIntPropertyManager(this);
    doubleManager_ = new QtDoublePropertyManager(this);
    stringManager_ = new QtStringPropertyManager(this);
    enumManager_ = new QtEnumPropertyManager(this);
    groupManager_ = new QtGroupPropertyManager(this);
    tableManager_ = new TablePropertyManager(this);
    lineManager_ = new LineEditWithButtonPropertyManager(this);
    textManager_ = new TextEditPropertyManager(this);

    enumMapManager_ = new EnumMapPropertyManager(this);

    connect(boolManager_, SIGNAL(valueChanged(QtProperty*, bool)), this, SLOT(valueChanged(QtProperty*, bool)));
    connect(intManager_, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
    connect(doubleManager_, SIGNAL(valueChanged(QtProperty*, double)), this, SLOT(valueChanged(QtProperty*, double)));
    connect(stringManager_, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(valueChanged(QtProperty*, const QString&)));
    connect(enumManager_, SIGNAL(valueChanged(QtProperty*, int)), this, SLOT(valueChanged(QtProperty*, int)));
    connect(textManager_, SIGNAL(valueChanged(QtProperty*, const QString&)), this, SLOT(valueChanged(QtProperty*, const QString&)));
    connect(enumMapManager_, SIGNAL(valueChanged(QtProperty*, const QList<int>&)), this, SLOT(valueChanged(QtProperty*, const QList<int>&)));
    //connect(tableManager_, SIGNAL(valueChanged(QtProperty*, const QModelIndex&)), this, SLOT(valueChanged(QtProperty*, const QModelIndex&)));
    //connect(tableManager_, SIGNAL(clicked(QtProperty*, const QModelIndex&)), this, SLOT(clicked(QtProperty*, const QModelIndex&)));

    QtCheckBoxFactory* checkBoxFactory = new QtCheckBoxFactory(this);
    QtDoubleSpinBoxFactory* spinBoxFactory = new QtDoubleSpinBoxFactory(this);
    QtSpinBoxFactory* intFactory = new QtSpinBoxFactory(this);
    QtLineEditFactory* lineEditFactory = new QtLineEditFactory(this);
    QtEnumEditorFactory* comboBoxFactory = new QtEnumEditorFactory(this);
    TableEditorFactory* tableFactory = new TableEditorFactory(this);
    LineEditWithButtonEditorFactory* lineFactory = new LineEditWithButtonEditorFactory(this);
    TextEditFactory* textBrowserFactory = new TextEditFactory(this);
    EnumMapFactory* enumMapFactory = new EnumMapFactory(this);

    ui->custombrowser->setFactoryForManager(boolManager_, checkBoxFactory);
    ui->custombrowser->setFactoryForManager(intManager_, intFactory);
    ui->custombrowser->setFactoryForManager(doubleManager_, spinBoxFactory);
    ui->custombrowser->setFactoryForManager(stringManager_, lineEditFactory);
    ui->custombrowser->setFactoryForManager(enumManager_, comboBoxFactory);
    ui->custombrowser->setFactoryForManager(tableManager_, tableFactory);
    ui->custombrowser->setFactoryForManager(lineManager_, lineFactory);
    ui->custombrowser->setFactoryForManager(textManager_, textBrowserFactory);
    ui->custombrowser->setFactoryForManager(enumMapManager_, enumMapFactory);
}

void MainWindow::addProperty(QtProperty* property)
{
    propertyToId[property] = property->propertyName();
    idToProperty[property->propertyName()] = property;
}

void MainWindow::removeProperty(QtProperty* property)
{
    if (!property)
    {
        return;
    }
    propertyToId.remove(property);
    idToProperty.remove(property->propertyName());
    delete property;
}
