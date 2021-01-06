#include "titlebutton.h"
#include <QLayout>
#include <QToolBar>
#include <QPainter>
#include <QStyleOption>
#include <QActionEvent>

struct TitleButton::Impl {
    bool expand{ false };
    bool pressed{ false };
    QHBoxLayout* layout{ nullptr };
    QToolBar* toolBar{ nullptr };
};

TitleButton::TitleButton(QWidget* parent)
    :QPushButton(parent)
    , impl_(std::make_shared<Impl>())
{
    impl_->layout = new QHBoxLayout(this);
    impl_->layout->setMargin(0);
    impl_->layout->setSpacing(0);
    impl_->layout->addStretch(1);

    impl_->toolBar = new QToolBar(this);
    impl_->toolBar->setObjectName("TitleButtonToolBar");
    impl_->layout->addWidget(impl_->toolBar, 0, Qt::AlignRight);
}

TitleButton::~TitleButton()
{
}

void TitleButton::setExpand(bool expand)
{
    impl_->expand = expand;

    update();
}

bool TitleButton::isExpand() const
{
    return impl_->expand;
}

void TitleButton::insertAction(int index, QAction* action)
{
    auto before = impl_->toolBar->actions().value(index);
    if (before)
        impl_->toolBar->insertAction(before, action);
}

void TitleButton::addActions(QList<QAction*> actions)
{
    impl_->toolBar->addActions(actions);
}

void TitleButton::removeAction(QAction* action)
{
    impl_->toolBar->removeAction(action);
}

void TitleButton::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    QStyleOptionButton opt;
    this->initStyleOption(&opt);
    opt.text.clear();
    style()->drawControl(QStyle::CE_PushButton, &opt, &p, this);

    QStyleOptionViewItem itemOpt;
    itemOpt.initFrom(this);
    auto indent = style()->pixelMetric(QStyle::PM_TreeViewIndentation, &itemOpt, this);
    do
    {
        itemOpt.rect = QRect(0, 0, indent, height());
        itemOpt.viewItemPosition = QStyleOptionViewItem::Beginning;
        itemOpt.state = QStyle::State_Children;

        if (impl_->expand)
            itemOpt.state |= QStyle::State_Open;
        style()->drawPrimitive(QStyle::PE_IndicatorBranch, &itemOpt, &p, this);
    } while (false);

    auto rect = QRect(opt.rect.left() + indent, opt.rect.top(), opt.rect.width() - 40, opt.rect.height());
    QTextOption to;
    to.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    p.save();
    p.setPen(opt.palette.windowText().color());
    p.drawText(rect, text(), to);
    p.restore();
}

void TitleButton::actionEvent(QActionEvent* event)
{
    if (event->type() == QEvent::ActionAdded)
    {
    }
    else if (event->type() == QEvent::ActionRemoved)
    {
    }
}