#include "enummapedit.h"

#include <QPushButton>
#include <QLineEdit>
#include <QMenu>
#include <QHBoxLayout>
#include <QEvent>
#include <QAction>
#include <QWidgetAction>
#include <QCheckBox>
#include <QButtonGroup>
#include <QStylePainter>
#include <QStyleOption>
#include <QResizeEvent>
#include <QListView>

EnumMapEdit::EnumMapEdit(QWidget *parent)
    : QWidget(parent)
    , m_menu(nullptr)
    , m_btnDropDown(nullptr)
    , m_leContent(nullptr)
    , m_sep(',')
{
    initContent();
    initLayout();
    initConnect();
}

EnumMapEdit::~EnumMapEdit()
{
}

void EnumMapEdit::setReadOnly(bool bEdit)
{
    m_menu->setEnabled(!bEdit);
    m_btnDropDown->setEnabled(!bEdit);
}

void EnumMapEdit::setSep(const char sep)
{
    if (sep == m_sep)
    {
        return;
    }
    QString content = m_leContent->text();

    if (content.isEmpty())
    {
        return;
    }

    QStringList dataList = m_parseCallBackFunc(content);
    m_sep = sep;
    content = m_showCallBackFunc(dataList);
    m_leContent->setText(content);
}

char EnumMapEdit::sep() const
{
    return m_sep;
}

void EnumMapEdit::setShowWayCallBack(CallBackFunc callBack, ParseCallBackFunc parseCallBack)
{
    m_showCallBackFunc = callBack;
    m_parseCallBackFunc = parseCallBack;
}

bool EnumMapEdit::eventFilter(QObject * watched, QEvent * event)
{
    if (watched == m_menu)
    {
        if (event->type() == QEvent::Show)
        {
            subMenuActionShow();
            moveAndresizeMenu();
        }
        else if (event->type() == QEvent::Close)
        {
            QPoint pos = QCursor::pos();
            QPoint menuGlobalPos = m_menu->mapToGlobal(QPoint(0, 0));
            QRect menuGlobalRect = QRect(menuGlobalPos, QSize(m_menu->size()));
            if (menuGlobalRect.contains(pos))
            {
                event->ignore();
                return true;
            }
        }
    }
    if (event->type() == QEvent::FocusIn)
    {
        setFocus(Qt::MouseFocusReason);
    }
    else if (watched == m_leContent && event->type() == QEvent::Resize)
    {
        auto resizeEvent = static_cast<QResizeEvent*>(event);
        m_btnDropDown->setFixedHeight(resizeEvent->size().height());
    }

    return QWidget::eventFilter(watched, event);
}

void EnumMapEdit::paintEvent(QPaintEvent *e)
{
    QStylePainter painter(this);
    QStyleOption opt;
    opt.initFrom(this);
    opt.rect = rect();
    painter.drawPrimitive(QStyle::PE_Widget, opt);

    QWidget::paintEvent(e);
}

void EnumMapEdit::setDropDownData(QMap<int, QString> enumMaps)
{
    m_menu->clear();
    QMap<int, QString>::iterator iter = enumMaps.begin();
    for (iter; iter != enumMaps.end(); iter++)
    {
        int key = iter.key();
        QString subSystemName = iter.value();

        QWidgetAction* action = new QWidgetAction(m_menu);
        QWidget* widget = new QWidget(m_menu);
        QHBoxLayout* layout = new QHBoxLayout(widget);
        layout->setMargin(0);
        QCheckBox *box = new QCheckBox(widget);
        box->setObjectName("LineEdit_Item");
        box->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        m_buttonGroup->addButton(box);
        action->setDefaultWidget(widget);
        layout->addWidget(box);
        box->setCheckable(true);
        box->setChecked(false);
        box->setText(QString("%1").arg(subSystemName));
        box->setProperty("id", key);
        action->setData(key);
        m_menu->addAction(action);
    }
    m_enumMaps = enumMaps;
}

void EnumMapEdit::initContent()
{
    m_btnDropDown = new QPushButton(this);
    m_btnDropDown->setObjectName("LineEdit_Drop");
    m_btnDropDown->setText(" ... ");
    m_btnDropDown->installEventFilter(this);
    m_leContent = new QLineEdit(this);
    m_leContent->installEventFilter(this);
    m_buttonGroup = new QButtonGroup(this);
    m_buttonGroup->setExclusive(false);
    m_leContent->setReadOnly(true);
    m_menu = new QMenu(this);
    m_menu->setObjectName("LineEdit_DropMenu");
    m_menu->installEventFilter(this);
    m_menu->hide();

    m_showCallBackFunc = [this](QStringList dataList)
    {
        QString content = dataList.join(m_sep);
        return content;
    };

    m_parseCallBackFunc = [this](QString data)
    {
        QStringList dataList = data.split(m_sep, QString::SkipEmptyParts);
        return dataList;
    };
}

void EnumMapEdit::initLayout()
{
    m_btnDropDown->setMaximumWidth(16);

    QHBoxLayout* hMainLayout = new QHBoxLayout(this);
    hMainLayout->setContentsMargins(0, 0, 0, 0);
    hMainLayout->setMargin(0);
    hMainLayout->setSpacing(0);
    hMainLayout->addWidget(m_leContent);

    QHBoxLayout* hLineLayout = new QHBoxLayout(m_leContent);
    m_leContent->setTextMargins(0, 2, m_btnDropDown->width(), 0);
    hLineLayout->setMargin(0);
    hLineLayout->setContentsMargins(1, 0, 0, 0);
    hLineLayout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hLineLayout->setSpacing(0);
    hLineLayout->addWidget(m_btnDropDown);
}

void EnumMapEdit::initConnect()
{
    connect(m_btnDropDown, &QPushButton::clicked, m_menu, &QMenu::show);
    connect(m_buttonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(slotCheckBoxFunction(QAbstractButton*)));
}

void EnumMapEdit::subMenuActionShow()
{
    QString content = m_leContent->text();
    QStringList dataList = m_parseCallBackFunc(content);
    auto actions = m_menu->actions();

    QList<QAction*> selectActionList;
    QList<QAction*> unselectActionList;
    for (auto action : actions)
    {
        QWidgetAction* wa = dynamic_cast<QWidgetAction*>(action);
        if (!wa) continue;

        QCheckBox* box = wa->defaultWidget()->findChild<QCheckBox*>();
        box->setFixedWidth(this->width());
        if (!box) continue;

        box->setChecked(false);
        action->setParent(nullptr);
        unselectActionList.append(action);
        for (const auto& data : dataList)
        {
            QString actionContent = box->text();
            if (actionContent == data)
            {
                box->setChecked(true);
                unselectActionList.removeOne(action);
                selectActionList.append(action);
                break;
            }
        }
    }

    auto compareFunc = [=](QAction* firstAction, QAction* secondAction)
    {
        QString firstSystemId = firstAction->data().toString();
        QString secondSystemId = secondAction->data().toString();
        if (firstSystemId < secondSystemId)
        {
            return true;
        }
        else
        {
            return false;
        }
    };
    qSort(selectActionList.begin(), selectActionList.end(), compareFunc);
    qSort(unselectActionList.begin(), unselectActionList.end(), compareFunc);

    m_menu->clear();
    m_menu->addActions(selectActionList);
    m_menu->addActions(unselectActionList);
}

void EnumMapEdit::moveAndresizeMenu()
{
    QPoint globalPos = m_leContent->mapToGlobal(QPoint(0, m_leContent->height()));
    m_menu->setGeometry(globalPos.x(), globalPos.y(), m_leContent->width(), m_menu->height());
}

void EnumMapEdit::slotCheckBoxFunction(QAbstractButton* button)
{
    QCheckBox* box = dynamic_cast<QCheckBox*>(button);
    if (!box) return;

    QString content = m_leContent->text();
    QStringList dataList = m_parseCallBackFunc(content);

    if (box->checkState() == Qt::Unchecked)
    {
        dataList.removeOne(box->text());
    }
    else if (box->checkState() == Qt::Checked)
    {
        dataList.append(box->text());
    }
    content = m_showCallBackFunc(dataList);
    m_leContent->setText(content);
    emit valueChanged(getValues());
}

QList<int> EnumMapEdit::getValues()
{
    QList<int> valus;
    auto actions = m_menu->actions();
    if (actions.isEmpty()) return valus;

    for (auto action : actions)
    {
        QWidgetAction* wa = dynamic_cast<QWidgetAction*>(action);
        if (wa)
        {
            QCheckBox* box = wa->defaultWidget()->findChild<QCheckBox*>();
            if (box && box->isChecked())
            {
                valus << box->property("id").toInt();
            }
        }
    }
    return valus;
}

void EnumMapEdit::setValues(QList<int> values)
{
    QStringList dataList;
    for (auto value : values)
    {
        if (m_enumMaps.contains(value))
        {
            dataList.append(m_enumMaps.value(value));
        }
    }
    m_leContent->setText(m_showCallBackFunc(dataList));
    if (!m_menu->isHidden())
    {
        subMenuActionShow();
    }
}