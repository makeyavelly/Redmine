#ifndef WIDGETSETTINGSTESTER_H
#define WIDGETSETTINGSTESTER_H

#include <QListWidgetItem>

#include "Widget.h"

namespace Ui {
class WidgetSettingsTester;
}

class WidgetSettingsTester : public Widget
{
    Q_OBJECT

    Ui::WidgetSettingsTester *ui;
    int _testProject;
    QList<int> _projects;
    QList<int> _users;

public:
    explicit WidgetSettingsTester(QWidget *parent = 0);
    ~WidgetSettingsTester();

private:
    void preClose();

    void load();
    void loadProjectsFromDb();
    void loadUsersFromDb();
    void updateParentIntoTree();

    void save();

    void setLabelTestProjectColor(const QColor &color);

private slots:
    void on_btSetTestProject_clicked();
    void on_treeProject_itemChanged(QTreeWidgetItem *item, int column);
    void on_listUsers_itemChanged(QListWidgetItem *item);
};

#endif // WIDGETSETTINGSTESTER_H
