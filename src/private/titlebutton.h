#pragma once

#include <QWidget>
#include <QPushButton>
#include <QIcon>
#include <QEvent>
#include <memory.h>

class TitleButton :public QPushButton
{
    Q_OBJECT
public:
    TitleButton(QWidget* QWidget = nullptr);
    ~TitleButton();

    void setExpand(bool expand);
    bool isExpand() const;

    void insertAction(int index, QAction* action);
    void addActions(QList<QAction*> actions);
    void removeAction(QAction* action);

protected:
    void paintEvent(QPaintEvent* event) override;
    void actionEvent(QActionEvent* event) override;

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};
