#include "WidgetSettings.h"
#include "ui_WidgetSettings.h"

#include <QMessageBox>

#include "Sql.h"
#include "debug.h"
#include "ini.h"

WidgetSettings::WidgetSettings(QWidget *parent) :
    Widget("Настройка", parent),
    ui(new Ui::WidgetSettings)
{
    ui->setupUi(this);
    load();
}

WidgetSettings::~WidgetSettings()
{
    delete ui;
}

void WidgetSettings::on_btCheck_clicked()
{
    if (isCorrectSettings()) {
        QMessageBox::about(this, "Уведомление", "Настройки корректные");
    }
}

void WidgetSettings::on_btOk_clicked()
{
    if (isCorrectSettings()) {
        save();
        ok();
    }
}

void WidgetSettings::on_btCancel_clicked()
{
    preClose();
    cancel();
}

void WidgetSettings::load()
{
    ui->editHost->setText(Sql::host());
    ui->editDatabase->setText(Sql::database());
    ui->editUser->setText(Sql::username());
    ui->editPassword->setText(Sql::password());

    ui->editLogin->setText(ini::get("USER", "name").toString());

    ui->checkIsRequest->setChecked(ini::get("DIALOG", "is_request", true).toBool());
}

void WidgetSettings::save()
{
    ini::set("CONNECTION", "host", ui->editHost->text());
    ini::set("CONNECTION", "database", ui->editDatabase->text());
    ini::set("CONNECTION", "login", ui->editUser->text());
    ini::set("CONNECTION", "password", ui->editPassword->text());

    ini::set("USER", "name", ui->editLogin->text());

    ini::set("DIALOG", "is_request", ui->checkIsRequest->isChecked());
}

bool WidgetSettings::isCorrectSettings()
{
    try {
        Sql::connect(ui->editHost->text(), ui->editDatabase->text(),
                     ui->editUser->text(), ui->editPassword->text());
        Sql::loadUser(ui->editLogin->text());
    } catch (const QString &error) {
        showError(error);
        return false;
    }
    return true;
}

void WidgetSettings::preClose()
{
    try {
        Sql::connect();
        Sql::loadUser(ini::get("USER", "NAME").toString());
    } catch (const QString &error) {
        printError(error);
    }
}
