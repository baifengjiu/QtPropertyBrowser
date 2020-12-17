#pragma once
#include <QDialog>
#include "ui_TestDialog.h"

class TestDialog :public QDialog
{
public:
    TestDialog(QWidget* parent = nullptr);
    ~TestDialog();

    QString getValue() const;

private:
    Ui::TestDialog* ui;
};
