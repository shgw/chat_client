#include "settingdialog.h"
#include "ui_settingdialog.h"
#include "mainwindow.h"
#include <qsettings.h>
#define INI_FILENAME    "./ChatClient.ini"

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    QSettings readini(INI_FILENAME, QSettings::IniFormat);
    QString str = readini.value("NETWORK/IP").toString();
    ui->lineEdit->setText(str);
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::on_saveBtn_clicked()
{

    QString str = ui->lineEdit->text();
    QSettings readini(INI_FILENAME, QSettings::IniFormat);
    readini.setValue( "NETWORK/IP", str);
    m_parent->SetServerIP( str );
    close();
}

void SettingDialog::on_cancelBtn_clicked()
{
    close();
}


void SettingDialog::SetParents(MainWindow* parent)
{
    this->m_parent = parent;
}
