#pragma once
#include <qtpropertybrowser.h>
#include <QAction>

QT_BEGIN_NAMESPACE

class CustomBrowserPrivate;

class CustomBrowser : public QtAbstractPropertyBrowser
{
    Q_OBJECT
public:
    enum TipType
    {
        None,
        Info,
        Warring,
        Error
    };

public:
    CustomBrowser(QWidget* parent = nullptr);
    ~CustomBrowser();

    void setExpanded(QtBrowserItem* item, bool expanded);
    bool isExpanded(QtBrowserItem* item) const;

    void insertAction(QtBrowserItem* item, int index, QAction* action);
    void addActions(QtBrowserItem* item, QList<QAction*> actions);
    void removeAction(QtBrowserItem* item, QAction* action);

    /**
     * @brief:     设置当前提示类型
     * @param[in]: QtProperty * property
     * @param[in]: TipType type 提示信息类型
     * @return:    
     */
    void setTipInfo(QtProperty* property, TipType type);

    /**
     * @brief:     设置名称是否可见，默认可见
     * @param[in]: QtProperty * property
     * @param[in]: bool isVisible
     */
    void setLableVisible(QtProperty* property, bool isVisible);

Q_SIGNALS:
    void collapsed(QtBrowserItem* item);
    void expanded(QtBrowserItem* item);

protected:
    void itemInserted(QtBrowserItem* item, QtBrowserItem* afterItem) override;
    void itemRemoved(QtBrowserItem* item) override;
    // can be tooltip, statustip, whatsthis, name, icon, text.
    void itemChanged(QtBrowserItem* item) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    QScopedPointer<CustomBrowserPrivate> d_ptr;
    Q_DECLARE_PRIVATE(CustomBrowser)
        Q_DISABLE_COPY_MOVE(CustomBrowser)
        Q_PRIVATE_SLOT(d_func(), void slotUpdate())
        Q_PRIVATE_SLOT(d_func(), void slotEditorDestroyed())
        Q_PRIVATE_SLOT(d_func(), void slotToggled(bool))
};

QT_END_NAMESPACE
