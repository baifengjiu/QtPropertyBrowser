#include "custombrowser.h"
#include "titlebutton.h"
#include <QMap>
#include <QLabel>
#include <QWidget>
#include <QGridLayout>
#include <QToolButton>
#include <QTimer>
#include <QScrollArea>
#include <QTextBrowser>
#include <QStyle>

QT_BEGIN_NAMESPACE

class CustomBrowserPrivate
{
    CustomBrowser* q_ptr;
    Q_DECLARE_PUBLIC(CustomBrowser)

public:
    void init(QWidget* parent);

    void propertyInserted(QtBrowserItem* index, QtBrowserItem* afterIndex);
    void propertyRemoved(QtBrowserItem* index);
    void propertyChanged(QtBrowserItem* index);
    QWidget* createEditor(QtProperty* property, QWidget* parent) const
    {
        return q_ptr->createEditor(property, parent);
    }

    void setTipInfo(QtProperty* property, CustomBrowser::TipType type);
    void setTipStyle(QWidget* w, CustomBrowser::TipType type);
    void setLableVisible(QtProperty* property, bool isVisible);

    void slotEditorDestroyed();
    void slotUpdate();
    void slotToggled(bool checked);
    bool eventFilter(QObject* watched, QEvent* event);

    struct WidgetItem
    {
        QWidget* widget{ nullptr }; // can be null
        TitleButton* button{ nullptr }; // expandable button for items with children
        QLabel* label{ nullptr }; // main label with property name
        QWidget* container{ nullptr }; // container which is expanded when the button is clicked
        QGridLayout* layout{ nullptr }; // layout in container
        WidgetItem* parent{ nullptr };
        QList<WidgetItem*> children;
        bool expanded{ false };
    };
private:
    void updateLater();
    void updateItem(WidgetItem* item);
    void insertRow(QGridLayout* layout, int row) const;
    void removeRow(QGridLayout* layout, int row) const;
    int gridRow(WidgetItem* item) const;
    int gridSpan(WidgetItem* item) const;
    void setExpanded(WidgetItem* item, bool expanded);
    void insertAction(WidgetItem* item, int index, QAction* action);
    void addActions(WidgetItem* item, QList<QAction*> actions);
    void removeAction(WidgetItem* item, QAction* action);
    TitleButton* createTitleButton(QWidget* panret = 0) const;

    QMap<QtBrowserItem*, WidgetItem*> m_indexToItem;
    QMap<WidgetItem*, QtBrowserItem*> m_itemToIndex;
    QMap<QWidget*, WidgetItem*> m_widgetToItem;
    QMap<QObject*, WidgetItem*> m_titleToItem;
    QMap<QWidget*, QtBrowserItem*> m_focusProxyToIndex;
    QMap<QtProperty*, bool> m_lableVisible;
    QMap<QtProperty*, CustomBrowser::TipType> m_tipType;
    QGridLayout* m_mainLayout = nullptr;
    QList<WidgetItem*> m_children;
    QList<WidgetItem*> m_recreateQueue;
    QLabel* tipTitle_{ nullptr };
    QTextBrowser* tipMsg_{ nullptr };
    QWidget* currentItem_{ nullptr };
};

void CustomBrowserPrivate::init(QWidget* parent)
{
    QVBoxLayout* layout = new QVBoxLayout(parent);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QScrollArea* scroll5 = new QScrollArea(parent);
    scroll5->setWidgetResizable(true);
    QWidget* propertyWdiget = new QWidget(scroll5);
    m_mainLayout = new QGridLayout(propertyWdiget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    QLayoutItem* item = new QSpacerItem(0, 0, QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_mainLayout->addItem(item, 0, 0);
    scroll5->setWidget(propertyWdiget);
    layout->addWidget(scroll5, 1);

    ////提示框
    QWidget* w = new QWidget(parent);
    w->setObjectName("TipInfoWidget");
    QVBoxLayout* tipLayout = new QVBoxLayout(w);
    tipLayout->setContentsMargins(3, 3, 3, 3);
    tipTitle_ = new QLabel(w);
    tipLayout->addWidget(tipTitle_);
    tipMsg_ = new QTextBrowser(w);
    tipLayout->addWidget(tipMsg_, 1);
    w->setFixedHeight(100);

    layout->addWidget(w);
}

void CustomBrowserPrivate::propertyInserted(QtBrowserItem* index, QtBrowserItem* afterIndex)
{
    WidgetItem* afterItem = m_indexToItem.value(afterIndex);
    WidgetItem* parentItem = m_indexToItem.value(index->parent());

    WidgetItem* newItem = new WidgetItem();
    newItem->parent = parentItem;

    QGridLayout* layout = 0;
    QWidget* parentWidget = 0;
    int row = -1;
    if (!afterItem) {
        row = 0;
        if (parentItem)
            parentItem->children.insert(0, newItem);
        else
            m_children.insert(0, newItem);
    }
    else {
        row = gridRow(afterItem) + gridSpan(afterItem);
        if (parentItem)
            parentItem->children.insert(parentItem->children.indexOf(afterItem) + 1, newItem);
        else
            m_children.insert(m_children.indexOf(afterItem) + 1, newItem);
    }

    if (!parentItem) {
        layout = m_mainLayout;
        layout->setSpacing(0);
        parentWidget = q_ptr;
    }
    else {
        if (!parentItem->container)
        {
            m_recreateQueue.removeAll(parentItem);
            WidgetItem* grandParent = parentItem->parent;
            QGridLayout* l = 0;
            const int oldRow = gridRow(parentItem);
            if (grandParent) {
                l = grandParent->layout;
            }
            else {
                l = m_mainLayout;
            }
            QFrame* container = new QFrame();
            container->setFrameShape(QFrame::Shape::NoFrame);
            container->setFrameShadow(QFrame::Shadow::Plain);

            parentItem->container = container;
            parentItem->button = createTitleButton();
            m_titleToItem[parentItem->button] = parentItem;
            q_ptr->connect(parentItem->button, SIGNAL(clicked(bool)), q_ptr, SLOT(slotToggled(bool)));
            parentItem->layout = new QGridLayout();
            container->setLayout(parentItem->layout);
            if (parentItem->label) {
                l->removeWidget(parentItem->label);
                delete parentItem->label;
                parentItem->label = 0;
            }
            int span = 2;
            l->addWidget(parentItem->button, oldRow, 0, 1, span);
            updateItem(parentItem);
        }
        layout = parentItem->layout;
        layout->setContentsMargins(0, 6, 0, 6);
        layout->setSpacing(9);
        parentWidget = parentItem->container;
    }

    bool isCreateLable = m_lableVisible.value(index->property(), true);
    if (isCreateLable)
    {
        newItem->label = new QLabel(parentWidget);
        newItem->label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        newItem->label->setAlignment(Qt::AlignmentFlag::AlignLeft | Qt::AlignmentFlag::AlignVCenter);
    }

    newItem->widget = createEditor(index->property(), parentWidget);
    if (newItem->widget) {
        auto type = m_tipType.value(index->property());
        if (type != CustomBrowser::TipType::None)
        {
            setTipStyle(newItem->widget, type);
        }
        m_tipType.remove(index->property());
        QObject::connect(newItem->widget, SIGNAL(destroyed()), q_ptr, SLOT(slotEditorDestroyed()));
        m_widgetToItem[newItem->widget] = newItem;
        newItem->widget->setObjectName("ValueWidget");

        auto w = newItem->widget;
        if (auto proxy = newItem->widget->focusProxy())
        {
            w = proxy; // QCheckBox
        }
        w->installEventFilter(q_ptr);
        m_focusProxyToIndex[w] = index;
    }

    insertRow(layout, row);
    int span = 2;
    if (isCreateLable && newItem->widget)
        span = 1;

    if (isCreateLable)
    {
        layout->addWidget(newItem->label, row, 0, 1, span);
    }

    if (newItem->widget)
    {
        layout->addWidget(newItem->widget, row, span == 2 ? 0 : 1, 1, span);
    }

    m_itemToIndex[newItem] = index;
    m_indexToItem[index] = newItem;

    updateItem(newItem);
}

void CustomBrowserPrivate::propertyRemoved(QtBrowserItem* index)
{
    WidgetItem* item = m_indexToItem.value(index);

    m_indexToItem.remove(index);
    m_itemToIndex.remove(item);
    m_lableVisible.remove(index->property());
    m_tipType.remove(index->property());

    WidgetItem* parentItem = item->parent;

    const int row = gridRow(item);

    if (parentItem)
        parentItem->children.removeAt(parentItem->children.indexOf(item));
    else
        m_children.removeAt(m_children.indexOf(item));

    const int colSpan = gridSpan(item);

    m_titleToItem.remove(item->button);

    if (item->widget)
        delete item->widget;
    if (item->label)
        delete item->label;
    if (item->button)
        delete item->button;
    if (item->container)
        delete item->container;

    if (!parentItem) {
        removeRow(m_mainLayout, row);
        if (colSpan > 1)
            removeRow(m_mainLayout, row);
    }
    else if (parentItem->children.count() != 0) {
        removeRow(parentItem->layout, row);
        if (colSpan > 1)
            removeRow(parentItem->layout, row);
    }
    else {
        const WidgetItem* grandParent = parentItem->parent;
        QGridLayout* l = 0;
        if (grandParent) {
            l = grandParent->layout;
        }
        else {
            l = m_mainLayout;
        }

        const int parentRow = gridRow(parentItem);
        const int parentSpan = gridSpan(parentItem);

        l->removeWidget(parentItem->button);
        l->removeWidget(parentItem->container);
        delete parentItem->button;
        delete parentItem->container;
        parentItem->button = 0;
        parentItem->container = 0;
        parentItem->layout = 0;
        if (!m_recreateQueue.contains(parentItem))
            m_recreateQueue.append(parentItem);
        if (parentSpan > 1)
            removeRow(l, parentRow + 1);

        updateLater();
    }
    m_recreateQueue.removeAll(item);

    delete item;
}

void CustomBrowserPrivate::propertyChanged(QtBrowserItem* index)
{
    WidgetItem* item = m_indexToItem.value(index);
    updateItem(item);
}

void CustomBrowserPrivate::setTipInfo(QtProperty* property, CustomBrowser::TipType type)
{
    QList<QtBrowserItem*> items = q_ptr->items(property);
    for (auto item : items)
    {
        WidgetItem* w = m_indexToItem.value(item);
        if (w && w->widget)
        {
            setTipStyle(w->widget, type);
        }
    }
    if (items.empty())
    {
        m_tipType.insert(property, type);
    }
}

void CustomBrowserPrivate::setTipStyle(QWidget* w, CustomBrowser::TipType type)
{
    w->style()->unpolish(w);
    switch (type)
    {
        case CustomBrowser::None:
            w->setProperty("level", "None");
            break;
        case CustomBrowser::Info:
            w->setProperty("level", "Info");
            break;
        case CustomBrowser::Warring:
            w->setProperty("level", "Warring");
            break;
        case CustomBrowser::Error:
            w->setProperty("level", "Error");
            break;
        default:
            w->setProperty("level", "None");
            break;
    }
    w->style()->polish(w);
}

void CustomBrowserPrivate::setLableVisible(QtProperty* property, bool isVisible)
{
    m_lableVisible[property] = isVisible;
}

void CustomBrowserPrivate::slotEditorDestroyed()
{
    QWidget* editor = qobject_cast<QWidget*>(q_ptr->sender());
    if (!editor)
        return;
    if (!m_widgetToItem.contains(editor))
        return;
    m_widgetToItem[editor]->widget = 0;

    m_widgetToItem.remove(editor);
    auto w = editor;
    if (auto proxy = editor->focusProxy())
    {
        w = proxy;
    }
    if (m_focusProxyToIndex.contains(w))
        m_focusProxyToIndex.remove(w);
}

void CustomBrowserPrivate::slotUpdate()
{
    for (WidgetItem* item : qAsConst(m_recreateQueue)) {
        WidgetItem* parent = item->parent;
        QWidget* w = 0;
        QGridLayout* l = 0;
        const int oldRow = gridRow(item);
        if (parent) {
            w = parent->container;
            l = parent->layout;
        }
        else {
            w = q_ptr;
            l = m_mainLayout;
        }

        int span = 2;
        item->label = new QLabel(w);
        item->label->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
        l->addWidget(item->label, oldRow, 0, 1, span);
        updateItem(item);
    }
    m_recreateQueue.clear();
}

void CustomBrowserPrivate::slotToggled(bool checked)
{
    WidgetItem* item = m_titleToItem.value(q_ptr->sender());
    if (!item)
        return;

    setExpanded(item, !item->expanded);

    if (item->expanded)
        emit q_ptr->expanded(m_itemToIndex.value(item));
    else
        emit q_ptr->collapsed(m_itemToIndex.value(item));
}

bool CustomBrowserPrivate::eventFilter(QObject* watched, QEvent* event)
{
    auto t = event->type();
    if (t == QEvent::FocusIn)
    {
        auto widget = qobject_cast<QWidget*>(watched);
        if (currentItem_ && widget == currentItem_)
        {
            return false;
        }
        if (widget && m_focusProxyToIndex.keys().contains(widget))
        {
            currentItem_ = widget;
            auto index = m_focusProxyToIndex.value(widget);
            if (!index)
                return false;
            QtProperty* p = index->property();
            if (!p)
                return false;
            tipTitle_->setText(p->propertyName());
            tipMsg_->setText(p->descriptionToolTip());
        }
    }
    return false;
}

void CustomBrowserPrivate::updateLater()
{
    QTimer::singleShot(0, q_ptr, SLOT(slotUpdate()));
}

void CustomBrowserPrivate::updateItem(WidgetItem* item)
{
    QtProperty* property = m_itemToIndex[item]->property();
    if (item->button) {
        QFont font = item->button->font();
        font.setUnderline(property->isModified());
        item->button->setFont(font);
        item->button->setText(property->propertyName());
        item->button->setStatusTip(property->statusTip());
        item->button->setWhatsThis(property->whatsThis());
        item->button->setEnabled(property->isEnabled());
    }
    if (item->label) {
        QFont font = item->label->font();
        font.setUnderline(property->isModified());
        item->label->setFont(font);
        item->label->setText(property->propertyName());
        item->label->setStatusTip(property->statusTip());
        item->label->setWhatsThis(property->whatsThis());
        item->label->setEnabled(property->isEnabled());
    }
    if (item->widget) {
        QFont font = item->widget->font();
        font.setUnderline(false);
        item->widget->setFont(font);
        item->widget->setEnabled(property->isEnabled());
        const QString valueToolTip = property->valueToolTip();
        item->widget->setToolTip(valueToolTip.isEmpty() ? property->valueText() : valueToolTip);
    }
}

void CustomBrowserPrivate::insertRow(QGridLayout* layout, int row) const
{
    QMap<QLayoutItem*, QRect> itemToPos;
    int idx = 0;
    while (idx < layout->count()) {
        int r, c, rs, cs;
        layout->getItemPosition(idx, &r, &c, &rs, &cs);
        if (r >= row) {
            itemToPos[layout->takeAt(idx)] = QRect(r + 1, c, rs, cs);
        }
        else {
            idx++;
        }
    }

    for (auto it = itemToPos.constBegin(), icend = itemToPos.constEnd(); it != icend; ++it) {
        const QRect r = it.value();
        layout->addItem(it.key(), r.x(), r.y(), r.width(), r.height());
    }
}

void CustomBrowserPrivate::removeRow(QGridLayout* layout, int row) const
{
    QMap<QLayoutItem*, QRect> itemToPos;
    int idx = 0;
    while (idx < layout->count()) {
        int r, c, rs, cs;
        layout->getItemPosition(idx, &r, &c, &rs, &cs);
        if (r > row) {
            itemToPos[layout->takeAt(idx)] = QRect(r - 1, c, rs, cs);
        }
        else {
            idx++;
        }
    }

    for (auto it = itemToPos.constBegin(), icend = itemToPos.constEnd(); it != icend; ++it) {
        const QRect r = it.value();
        layout->addItem(it.key(), r.x(), r.y(), r.width(), r.height());
    }
}

int CustomBrowserPrivate::gridRow(WidgetItem* item) const
{
    QList<WidgetItem*> siblings;
    if (item->parent)
        siblings = item->parent->children;
    else
        siblings = m_children;

    int row = 0;
    for (WidgetItem* sibling : qAsConst(siblings)) {
        if (sibling == item)
            return row;
        row += gridSpan(sibling);
    }
    return -1;
}

int CustomBrowserPrivate::gridSpan(WidgetItem* item) const
{
    if (item->container && item->expanded)
        return 2;
    return 1;
}

void CustomBrowserPrivate::setExpanded(WidgetItem* item, bool expanded)
{
    if (item->expanded == expanded)
        return;

    if (!item->container)
        return;

    item->expanded = expanded;
    const int row = gridRow(item);
    WidgetItem* parent = item->parent;
    QGridLayout* l = 0;
    if (parent)
        l = parent->layout;
    else
        l = m_mainLayout;

    if (expanded) {
        insertRow(l, row + 1);
        l->addWidget(item->container, row + 1, 0, 1, 2);
        item->container->show();
    }
    else {
        l->removeWidget(item->container);
        item->container->hide();
        removeRow(l, row + 1);
    }
    item->button->setExpand(expanded);
}

void CustomBrowserPrivate::insertAction(WidgetItem* item, int index, QAction* action)
{
    if (item->button)
    {
        item->button->insertAction(index, action);
    }
}

void CustomBrowserPrivate::addActions(WidgetItem* item, QList<QAction*> actions)
{
    if (item->button)
    {
        item->button->addActions(actions);
    }
}

void CustomBrowserPrivate::removeAction(WidgetItem* item, QAction* action)
{
    if (item->button)
    {
        item->button->removeAction(action);
    }
}

TitleButton* CustomBrowserPrivate::createTitleButton(QWidget* parent) const
{
    TitleButton* button = new TitleButton(parent);
    button->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed));
    button->setCheckable(true);
    //   button->setChecked(true);
    return button;
}

CustomBrowser::CustomBrowser(QWidget* parent)
    : QtAbstractPropertyBrowser(parent)
    , d_ptr(new CustomBrowserPrivate)
{
    d_ptr->q_ptr = this;

    d_ptr->init(this);
}

CustomBrowser::~CustomBrowser()
{
    const QMap<CustomBrowserPrivate::WidgetItem*, QtBrowserItem*>::ConstIterator icend = d_ptr->m_itemToIndex.constEnd();
    for (QMap<CustomBrowserPrivate::WidgetItem*, QtBrowserItem*>::ConstIterator it = d_ptr->m_itemToIndex.constBegin(); it != icend; ++it)
        delete it.key();
}

void CustomBrowser::setExpanded(QtBrowserItem* item, bool expanded)
{
    CustomBrowserPrivate::WidgetItem* itm = d_ptr->m_indexToItem.value(item);
    if (itm)
        d_ptr->setExpanded(itm, expanded);
}

bool CustomBrowser::isExpanded(QtBrowserItem* item) const
{
    CustomBrowserPrivate::WidgetItem* itm = d_ptr->m_indexToItem.value(item);
    if (itm)
        return itm->expanded;
    return false;
}

void CustomBrowser::insertAction(QtBrowserItem* item, int index, QAction* action)
{
    CustomBrowserPrivate::WidgetItem* itm = d_ptr->m_indexToItem.value(item);
    if (itm)
        d_ptr->insertAction(itm, index, action);
}

void CustomBrowser::addActions(QtBrowserItem* item, QList<QAction*> actions)
{
    CustomBrowserPrivate::WidgetItem* itm = d_ptr->m_indexToItem.value(item);
    if (itm)
        d_ptr->addActions(itm, actions);
}

void CustomBrowser::removeAction(QtBrowserItem* item, QAction* action)
{
    CustomBrowserPrivate::WidgetItem* itm = d_ptr->m_indexToItem.value(item);
    if (itm)
        d_ptr->removeAction(itm, action);
}

void CustomBrowser::setTipInfo(QtProperty* property, TipType type)
{
    d_ptr->setTipInfo(property, type);
}

void CustomBrowser::setLableVisible(QtProperty* property, bool isVisible)
{
    d_ptr->setLableVisible(property, isVisible);
}

void CustomBrowser::itemInserted(QtBrowserItem* item, QtBrowserItem* afterItem)
{
    d_ptr->propertyInserted(item, afterItem);
}

void CustomBrowser::itemRemoved(QtBrowserItem* item)
{
    d_ptr->propertyRemoved(item);
}

void CustomBrowser::itemChanged(QtBrowserItem* item)
{
    d_ptr->propertyChanged(item);
}

bool CustomBrowser::eventFilter(QObject* watched, QEvent* event)
{
    d_ptr->eventFilter(watched, event);
    return QWidget::eventFilter(watched, event);
}

QT_END_NAMESPACE

#include "moc_custombrowser.cpp"