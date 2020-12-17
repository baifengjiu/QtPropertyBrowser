#include "lineeditwithbutton.h"

#include <QEnterEvent>
#include <QHBoxLayout>

LineEditWithButton::LineEditWithButton(QWidget* parent)
    : QLineEdit(parent)
{
    QHBoxLayout* layout = new QHBoxLayout(this);
    m_btn = new QPushButton(this);
    m_btn->setObjectName("LineEdit_Btn");
    m_btn->setCursor(Qt::ArrowCursor);
    m_btn->setFlat(true);
    m_btn->setText(QStringLiteral("···"));
    m_btn->setWindowOpacity(1);
    m_btn->setFixedSize(16, 16);
    m_btn->setMaximumWidth(16);
    m_pbClear = new QPushButton(this);
    m_pbClear->setObjectName("LineEdit_Clear");
    m_pbClear->setCursor(Qt::ArrowCursor);
    m_pbClear->setFlat(true);
    m_pbClear->setWindowOpacity(1);
    m_pbClear->setFixedSize(16, 16);
    m_pbClear->hide();

    setTextMargins(0, 2, 19, 0);

    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    layout->addWidget(m_pbClear);
    layout->addWidget(m_btn);
    layout->setContentsMargins(1, 0, 1, 0);
    layout->setSpacing(0);
    connect(m_btn, &QPushButton::clicked, this, &LineEditWithButton::buttonClicked);
    connect(this, &QLineEdit::textEdited, this, &LineEditWithButton::onTextChanged);
    connect(m_pbClear, &QPushButton::clicked, [=]() {
        setText("");
        emit textEdited("");
    });
    setFrame(false);
    m_btn->installEventFilter(this);
    m_pbClear->installEventFilter(this);
}

LineEditWithButton::~LineEditWithButton()
{
}

void LineEditWithButton::setClearButtonVisible(bool isVisible)
{
    m_pbClear->setVisible(isVisible);
    if (isVisible)
    {
        setTextMargins(0, 2, 35, 0);
        connect(this, &QLineEdit::textChanged, this, &LineEditWithButton::onTextChanged);
    }
    else
    {
        setTextMargins(0, 2, 19, 0);
        disconnect(this, &QLineEdit::textChanged, this, &LineEditWithButton::onTextChanged);
    }
}

void LineEditWithButton::setReadOnly(bool readOnly)
{
    QLineEdit::setReadOnly(readOnly);
    m_pbClear->setEnabled(!readOnly);
}

void LineEditWithButton::onTextChanged(const QString& str)
{
    setText(str);
}

void LineEditWithButton::resizeEvent(QResizeEvent* e)
{
    m_btn->setFixedHeight(e->size().height() - 2);
    QLineEdit::resizeEvent(e);
}

void LineEditWithButton::enterEvent(QEvent* e)
{
    m_enter = true;
    QEnterEvent* event = dynamic_cast<QEnterEvent*>(e);
    if (m_btn != childAt(event->pos()) || m_pbClear != childAt(event->pos()))
        QLineEdit::enterEvent(event);
}

bool LineEditWithButton::eventFilter(QObject* o, QEvent* e)
{
    if (o == m_btn)
    {
        if (e->type() == QEvent::Enter)
        {
            setCursor(Qt::ArrowCursor);
        }
        else if (e->type() == QEvent::Leave && m_enter == true)
        {
            setCursor(Qt::IBeamCursor);
        }
        else if (e->type() == QEvent::FocusIn)
        {
            setFocus();
        }
    }
    if (o == m_pbClear)
    {
        if (e->type() == QEvent::Enter)
        {
            setCursor(Qt::ArrowCursor);
        }
        else if (e->type() == QEvent::Leave && m_enter == true)
        {
            setCursor(Qt::IBeamCursor);
        }
        else if (e->type() == QEvent::FocusIn)
        {
            setFocus();
        }
    }
    return QLineEdit::eventFilter(o, e);
}

void LineEditWithButton::leaveEvent(QEvent* e)
{
    QLineEdit::leaveEvent(e);
    m_enter = false;
}