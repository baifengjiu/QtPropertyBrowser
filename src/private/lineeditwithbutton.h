#pragma once
#include <QLineEdit>
#include <QPushButton>

class LineEditWithButton : public QLineEdit
{
    Q_OBJECT

public:
    LineEditWithButton(QWidget* parent = nullptr);
    ~LineEditWithButton();

    void setClearButtonVisible(bool isVisible);
    void setReadOnly(bool readOnly);

signals:
    void buttonClicked();

protected Q_SLOTS:
    void onTextChanged(const QString& str);

protected:
    void resizeEvent(QResizeEvent* e);
    void enterEvent(QEvent* e);
    bool eventFilter(QObject* o, QEvent* e);
    void leaveEvent(QEvent* e);

private:
    QPushButton* m_btn;
    QPushButton* m_pbClear;
    bool m_enter;
};
