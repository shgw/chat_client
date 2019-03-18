#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
class MainWindow;

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = nullptr);
    ~SettingDialog();

private slots:
    void on_saveBtn_clicked();

    void on_cancelBtn_clicked();

private:
    Ui::SettingDialog *ui;
    MainWindow* m_parent;
    void (MainWindow::*func)();


public:
    void SetCBF(void (MainWindow::*func)());
    void SetParents(MainWindow* parent);
};

#endif // SETTINGDIALOG_H
