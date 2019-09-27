#ifndef WIDGETSETTINGS_H
#define WIDGETSETTINGS_H

#include "Widget.h"

namespace Ui {
class WidgetSettings;
}

class WidgetSettings : public Widget
{
    Q_OBJECT

public:
    explicit WidgetSettings(QWidget *parent = 0);
    ~WidgetSettings();

private slots:
    void on_btCheck_clicked();
    void on_btOk_clicked();
    void on_btCancel_clicked();

private:
    Ui::WidgetSettings *ui;

    void load();
    void save();
    bool isCorrectSettings();
    void preClose();
};

#endif // WIDGETSETTINGS_H
