#include "WidgetSettingsTester.h"
#include "ui_WidgetSettingsTester.h"

WidgetSettingsTester::WidgetSettingsTester(QWidget *parent) :
    Widget("Настройки", parent),
    ui(new Ui::WidgetSettingsTester)
{
    ui->setupUi(this);
    ini::load(ui->splitterSettingsTester);
    load();
}

WidgetSettingsTester::~WidgetSettingsTester()
{
    ini::save(ui->splitterSettingsTester);
    delete ui;
}

void WidgetSettingsTester::preClose()
{
    save();
}

void WidgetSettingsTester::load()
{
    _testProject = ini::get("WidgetSettingsTester", "TestProject", 0).toInt();
    _projects = loadListIntFromFile("projects");
    _users = loadListIntFromFile("users");
    loadProjectsFromDb();
    loadUsersFromDb();
    updateParentIntoTree();
}

void WidgetSettingsTester::loadProjectsFromDb()
{
    bool isExistsTestProject = false;

    QTreeWidget *tree = ui->treeProject;
    tree->clear();
    foreach (Project project, Sql::getProjects()) {
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setData(0, RoleId, project.id);
        item->setData(0, RoleParent, project.parent);
        item->setText(0, project.name);
        item->setCheckState(0, _projects.contains(project.id) ? Qt::Checked : Qt::Unchecked);
        tree->addTopLevelItem(item);

        if (project.id == _testProject) {
            ui->labelTestProject->setText(project.name);
            isExistsTestProject = true;
        }
    }

    if (!isExistsTestProject) {
        ui->labelTestProject->setText("Не выбран проект для тестовых задач");
        setLabelTestProjectColor(QColor("#cf0000"));
    }
}

void WidgetSettingsTester::loadUsersFromDb()
{
    QListWidget *list = ui->listUsers;
    list->clear();
    foreach (User user, Sql::getUsers()) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(RoleId, user.id);
        item->setText(user.title);
        item->setCheckState(_users.contains(user.id) ? Qt::Checked : Qt::Unchecked);
        list->addItem(item);
    }
}

void WidgetSettingsTester::updateParentIntoTree()
{
    QTreeWidget *tree = ui->treeProject;
    int i = 0;
    while (i < tree->topLevelItemCount()) {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        int parent = item->data(0, RoleParent).toInt();
        if (parent == 0) {
            ++i;
        } else {
            QTreeWidgetItem *parentItem = findTreeWidgetItem(tree, parent, 0, RoleId);
            if (parentItem) {
                parentItem->addChild(tree->takeTopLevelItem(i));
            } else {
                ++i;
            }
        }
    }
}

void WidgetSettingsTester::save()
{
    saveListIntIntoFile(_projects, "projects");
    saveListIntIntoFile(_users, "users");
}

void WidgetSettingsTester::setLabelTestProjectColor(const QColor &color)
{
    QPalette palette = ui->labelTestProject->palette();
    palette.setColor(QPalette::WindowText, color);
    ui->labelTestProject->setPalette(palette);
}

void WidgetSettingsTester::on_btSetTestProject_clicked()
{
    QTreeWidgetItem *item = ui->treeProject->currentItem();
    if (item) {
        _testProject = item->data(0, RoleId).toInt();
        ui->labelTestProject->setText(item->text(0));
        setLabelTestProjectColor(Qt::black);
        ini::set("WidgetSettingsTester", "TestProject", _testProject);
    }
}

void WidgetSettingsTester::on_treeProject_itemChanged(QTreeWidgetItem *item, int column)
{
    if (item && column == 0) {
        if (item->checkState(0) == Qt::Checked) {
            _projects.append(item->data(0, RoleId).toInt());
        } else {
            _projects.removeAll(item->data(0, RoleId).toInt());
        }
    }
}

void WidgetSettingsTester::on_listUsers_itemChanged(QListWidgetItem *item)
{
    if (item) {
        if (item->checkState() == Qt::Checked) {
            _users.append(item->data(RoleId).toInt());
        } else {
            _users.removeAll(item->data(RoleId).toInt());
        }
    }
}
