#include "TestDialog.h"

TestDialog::TestDialog(QWidget* parent)
    :QDialog(parent)
{
    ui = new Ui::TestDialog();
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &TestDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &TestDialog::reject);
}
TestDialog::~TestDialog()
{
}

QString TestDialog::getValue() const
{
    auto txt = ui->textBrowser->toPlainText();
    return txt;
}
