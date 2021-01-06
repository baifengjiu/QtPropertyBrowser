#pragma once

#include <QWidget>
#include <QMap>
#include <functional>

typedef std::function<QString(QStringList)> CallBackFunc;
typedef std::function<QStringList(QString)> ParseCallBackFunc;

class QMenu;
class QLineEdit;
class QToolButton;
class QButtonGroup;
class QAbstractButton;

class EnumMapEdit : public QWidget
{
    Q_OBJECT

public:
    EnumMapEdit(QWidget* parent = nullptr);
    virtual ~EnumMapEdit();
    void setReadOnly(bool bEdit);

    void setSep(const char sep);
    char sep() const;

    void setShowWayCallBack(CallBackFunc callBack, ParseCallBackFunc parseCallBack);
    QList<int> getValues();

public slots:
    void setValues(QList<int> values);
    void setDropDownData(QMap<int, QString> enumMaps);

signals:
    void valueChanged(QList<int> valus);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void paintEvent(QPaintEvent* e) override;

private:
    void initContent();
    void initLayout();
    void initConnect();
    void subMenuActionShow();
    void moveAndresizeMenu();

private slots:
    void slotCheckBoxFunction(QAbstractButton* button);

private:
    QLineEdit* m_leContent;
    QToolButton* m_btnDropDown;
    QMenu* m_menu;
    CallBackFunc m_showCallBackFunc;
    ParseCallBackFunc m_parseCallBackFunc;
    QButtonGroup* m_buttonGroup;
    QMap<int, QString> m_enumMaps;
    char     m_sep;
};