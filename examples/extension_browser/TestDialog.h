#pragma once
#include "customeditorfactory.h"
#include "ui_TestDialog.h"

class TestDialog :public CustomDialog
{
public:
    TestDialog(QWidget* parent = nullptr);
    ~TestDialog();

    QString getValue() const override;

private:
    Ui::TestDialog* ui;
};
