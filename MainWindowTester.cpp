#include "MainWindowTester.h"
#include "ui_MainWindowTester.h"

#include <QDateTime>

#include "ini.h"
#include "widget/WidgetSettingsTester.h"

enum {
    COL_TEST_NAME,
    COL_TEST_DATE,

    COL_TEST_COUNT
};

enum {
    COL_TASK_NAME,

    COL_TASK_COUNT
};


MainWindowTester::MainWindowTester(QWidget *parent) :
    MainWindowDefault(parent),
    ui(new Ui::MainWindowTester),
    _currentTest(0),
    _currentTask(0)
{
    init();
    ui->editComment->installEventFilter(this);
    startTimer(5000);
}

MainWindowTester::~MainWindowTester()
{
    delete ui;
}

void MainWindowTester::showEvent(QShowEvent *event)
{
    MainWindowDefault::showEvent(event);
    updateSize();
}

void MainWindowTester::resizeEvent(QResizeEvent *)
{
    updateSize();
}

bool MainWindowTester::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->editComment && event->type() == QEvent::FocusOut) {
        if (_currentTask > 0) {
            Sql::setTaskDescription(_currentTask, ui->editComment->toPlainText().trimmed()
                                    .replace(QChar(0), "")
                                    .replace("'", "''"));
        }
    }
    return MainWindowDefault::eventFilter(object, event);
}

void MainWindowTester::initUi()
{
    ui->setupUi(this);
    this->setWindowIcon(getIconWindow());
    ui->widgetButtons->hide();
    QObject::connect(ui->tableTest, SIGNAL(currentCellChanged(int,int,int,int)),
                     this, SLOT(slotTableCurrentCellChanged(int,int,int,int)));
    QObject::connect(ui->tableTask, SIGNAL(currentCellChanged(int,int,int,int)),
                     this, SLOT(slotTableCurrentCellChanged(int,int,int,int)));
}

void MainWindowTester::update()
{
    _currentTest = 0;
    _currentTask = 0;
    loadTestProject();
    loadProjects();
    loadTracks();
    loadLevels();
    loadUsers();
    loadProjectForTesting();
}

void MainWindowTester::loadSeanse()
{
    ini::load(this);
    ini::load(ui->MainWindowTesterSplitter);
}

void MainWindowTester::saveSeanse()
{
    ini::save(this);
    ini::save(ui->MainWindowTesterSplitter);
}

QString MainWindowTester::getWindowLabel() const
{
    return "T";
}

void MainWindowTester::loadTestProject()
{
    _testProject = ini::get("WidgetSettingsTester", "TestProject", 0).toInt();
}

void MainWindowTester::loadProjects()
{
    QComboBox *combo = ui->comboProject;
    combo->clear();
    combo->addItem("Проект по умолчанию", _testProject);
    QList<int> projects = loadListIntFromFile("projects");
    foreach (Project project, Sql::getProjects()) {
        if (projects.contains(project.id)) {
            combo->addItem(project.name, project.id);
        }
    }
}

void MainWindowTester::loadTracks()
{
    QComboBox *combo = ui->comboTrack;
    combo->clear();
    foreach (Track track, Sql::getTracks()) {
        combo->addItem(track.name, track.id);
    }
}

void MainWindowTester::loadLevels()
{
    _defaultLevel = ini::get("WidgetSettingsTester", "DefaultLevel", 0).toInt();
    QComboBox *combo = ui->comboLevel;
    combo->clear();
    foreach (Level level, Sql::getLevels()) {
        QString name = QString();
        for (int i = 1; i < level.level; ++i) {
            name += "    ";
        }
        name += level.name;
        combo->addItem(name, level.name);
    }
}

void MainWindowTester::loadUsers()
{
    QComboBox *combo = ui->comboUser;
    combo->clear();
    combo->addItem("Исполнитель не назначен", 0);
    QList<int> users = loadListIntFromFile("users");
    foreach (User user, Sql::getUsers()) {
        if (users.contains(user.id)) {
            combo->addItem(user.title, user.id);
        }
    }
}

void MainWindowTester::loadProjectForTesting()
{
    QTableWidget *table = ui->tableTest;
    createTableHeader(table, QStringList() << "Задача" << "Дата создания");
    QSqlQuery q = Sql::select(QString("SELECT id,subject,created_on FROM issues "
                                      "WHERE project_id=%1 AND tracker_id=%2 AND author_id=%3 "
                                      "AND status_id IN (%4) ORDER BY created_on;")
                              .arg(_testProject)
                              .arg(TrackTest)
                              .arg(user().id)
                              .arg(TASK_OPEN));
    while (q.next()) {
        int row = insertRowIntoTable(table);
        table->setVerticalHeaderItem(row, new QTableWidgetItem(q.value(0).toString()));
        table->item(row, COL_TEST_NAME)->setData(RoleId, q.value(0).toInt());
        table->item(row, COL_TEST_NAME)->setText(q.value(1).toString());
        table->item(row, COL_TEST_DATE)->setText(q.value(2).toDateTime().toString(FORMAT_DATE));
        table->item(row, COL_TEST_DATE)->setFlags(table->item(row, COL_TEST_DATE)->flags() &
                                                  ~Qt::ItemIsEditable);
    }
    int row = insertRowIntoTable(table);
    if (table->columnCount() > 1) {
        table->setSpan(row, 0, 1, table->columnCount());
    }
    table->setCellWidget(row, 0, createButton(QIcon("://add"), "Добавить", SLOT(slotAddTest())));
    table->setVerticalHeaderItem(row, new QTableWidgetItem(QString()));
    if (table->rowCount() > 0) {
        table->setCurrentCell(0, 0);
    }
}

void MainWindowTester::updateSize()
{
    resizeTable(ui->tableTest, COL_TEST_NAME, 200);
    resizeTable(ui->tableTask, COL_TASK_NAME, 200);
}

QPushButton *MainWindowTester::createButton(const QIcon &icon, const QString &name,
                                            const char *slot, const QString &toolTip)
{
    QPushButton *bt = new QPushButton(icon, name);
    bt->setToolTip(toolTip);
    bt->setFlat(true);
    QObject::connect(bt, SIGNAL(clicked()), this, slot);
    return bt;
}

void MainWindowTester::on_actionUpdate_triggered()
{
    update();
}

void MainWindowTester::on_actionSettings_triggered(bool checked)
{
    WidgetSettingsTester *widget = new WidgetSettingsTester(this);
    clickActionWidget(checked, widget, sender());
    if (!checked) {
        update();
    }
}

void MainWindowTester::on_MainWindowTesterSplitter_splitterMoved(int, int)
{
    updateSize();
}

void MainWindowTester::on_tableTest_currentCellChanged(int currentRow, int, int, int)
{
    _currentTask = 0;
    QTableWidget *table = ui->tableTask;
    createTableHeader(table, QStringList() << "Задача");
    if (currentRow >= 0 && currentRow < ui->tableTest->rowCount()) {
        _currentTest = ui->tableTest->item(currentRow, COL_TEST_NAME)->data(RoleId).toInt();
        QSqlQuery q = Sql::select(QString("SELECT id,subject,status_id,assigned_to_id FROM issues "
                                          "WHERE parent_id=%1 AND id != parent_id ORDER BY created_on;")
                                  .arg(_currentTest));
        while (q.next()) {
            int row = insertRowIntoTable(table);
            table->setVerticalHeaderItem(row, new QTableWidgetItem(q.value(0).toString()));
            table->item(row, COL_TASK_NAME)->setData(RoleId, q.value(0).toInt());
            table->item(row, COL_TASK_NAME)->setText(q.value(1).toString());
            int status = q.value(2).toInt();
            table->item(row, COL_TASK_NAME)->setData(RoleStatus, status);
            if (q.value(3).toInt() > 0) {
                table->item(row, COL_TASK_NAME)->setToolTip(Sql::getStatusName(status));
                setRowColor(table, row, getStatusColor(status));
            } else {
                table->item(row, COL_TASK_NAME)->setToolTip("Исполнитель не назначен");
                setRowColor(table, row, QColor("#ffcfcf"));
            }
        }
    } else {
        _currentTest = 0;
    }
    int row = insertRowIntoTable(table);
    if (table->columnCount() > 1) {
        table->setSpan(row, 0, 1, table->columnCount());
    }
    table->setCellWidget(row, 0, createButton(QIcon("://add"), "Добавить", SLOT(slotAddTask())));
    table->setVerticalHeaderItem(row, new QTableWidgetItem(QString()));
    if (table->rowCount() > 0) {
        table->setCurrentCell(0, 0);
    }
    updateSize();
}

void MainWindowTester::on_tableTask_currentCellChanged(int currentRow, int, int, int)
{
    if (currentRow >= 0) {
        _currentTask = ui->tableTask->item(currentRow, COL_TASK_NAME)->data(RoleId).toInt();
        ui->editName->setText(ui->tableTask->item(currentRow, COL_TASK_NAME)->text());
        ui->editComment->setText(Sql::getTaskDescription(_currentTask));
        ui->comboProject->setCurrentIndex(findDataInCombo(ui->comboProject, Sql::getTaskProject(_currentTask)));
        ui->comboTrack->setCurrentIndex(findDataInCombo(ui->comboTrack, Sql::getTaskTrack(_currentTask)));
        ui->comboLevel->setCurrentIndex(findDataInCombo(ui->comboLevel, Sql::getTaskLevel(_currentTask)));
        ui->comboUser->setCurrentIndex(findDataInCombo(ui->comboUser, Sql::getTaskUser(_currentTask)));

        int status = ui->tableTask->item(currentRow, COL_TASK_NAME)->data(RoleStatus).toInt();
        if (status == st_TaskTesting) {
            ui->widgetButtons->show();
        } else {
            ui->widgetButtons->hide();
        }
    } else {
        _currentTask = 0;
        ui->editName->clear();
        ui->editComment->clear();
        ui->comboProject->setCurrentIndex(0);
        ui->comboLevel->setCurrentIndex(0);
        ui->comboUser->setCurrentIndex(0);
    }
}

void MainWindowTester::slotTableCurrentCellChanged(int currentRow, int, int previosRow, int)
{
    QTableWidget *table = static_cast<QTableWidget*>(sender());
    if (previosRow >= 0 && previosRow < table->rowCount() - 1) {
        table->item(previosRow, 0)->setIcon(QIcon());
    }
    if (currentRow >= 0 && currentRow < table->rowCount() - 1) {
        table->item(currentRow, 0)->setIcon(QIcon("://next"));
    }
}

void MainWindowTester::on_tableTest_cellDoubleClicked(int row, int)
{
    if (row >= 0) {
        int task = ui->tableTest->item(row, COL_TEST_NAME)->data(RoleId).toInt();
        openTaskInBrowser(task);
    }
}

void MainWindowTester::on_tableTask_cellDoubleClicked(int row, int)
{
    if (row >= 0) {
        int task = ui->tableTask->item(row, COL_TASK_NAME)->data(RoleId).toInt();
        openTaskInBrowser(task);
    }
}

void MainWindowTester::on_editName_editingFinished()
{
    if (_currentTask > 0) {
        Sql::setTaskName(_currentTask, ui->editName->text());
        ui->tableTask->item(ui->tableTask->currentRow(), COL_TASK_NAME)->setText(ui->editName->text());
        updateSize();
    }
}

void MainWindowTester::on_comboProject_currentIndexChanged(int index)
{
    if (index >= 0 && _currentTask > 0) {
        int project = ui->comboProject->itemData(index).toInt();
        Sql::setTaskProject(_currentTask, project);
    }
}

void MainWindowTester::on_comboTrack_currentIndexChanged(int index)
{
    if (index >= 0 && _currentTask > 0) {
        int track = ui->comboTrack->itemData(index).toInt();
        Sql::setTaskTrack(_currentTask, track);
    }
}

void MainWindowTester::on_comboLevel_currentIndexChanged(int index)
{
    if (index >= 0 && _currentTask > 0) {
        Sql::setTaskLevel(_currentTask, ui->comboLevel->itemData(index).toString());
    }
}

void MainWindowTester::on_comboUser_currentIndexChanged(int index)
{
    if (index >= 0 && _currentTask > 0) {
        int user = ui->comboUser->itemData(index).toInt();
        int row = ui->tableTask->currentRow();
        int status = ui->tableTask->item(row, COL_TASK_NAME)->data(RoleStatus).toInt();
        if (user > 0) {
            ui->tableTask->item(row, COL_TASK_NAME)->setToolTip(Sql::getStatusName(status));
            setRowColor(ui->tableTask, row, getStatusColor(status));
        } else {
            ui->tableTask->item(row, COL_TASK_NAME)->setToolTip("Исполнитель не назначен");
            setRowColor(ui->tableTask, row, QColor("#ffcfcf"));
        }
        Sql::setTaskUser(_currentTask, user);
    }
}

void MainWindowTester::on_tableTest_cellChanged(int row, int column)
{
    if (row >= 0 && column == COL_TEST_NAME) {
        Sql::setTaskName(_currentTest, ui->tableTest->item(row, column)->text());
        updateSize();
    }
}

void MainWindowTester::slotAddTest()
{
    QDate date = QDate::currentDate();
    QString name = QString("Тестирование от %1").arg(date.toString(FORMAT_DATE));
    int id = Sql::createTask(_testProject, TrackTest, name, st_TaskNew, user().id, 0);
    Sql::setTaskLevel(id, ui->comboLevel->itemData(_defaultLevel).toString());

    QTableWidget *table = ui->tableTest;
    int row = insertRowIntoTable(table, table->rowCount() - 1);
    table->setVerticalHeaderItem(row, new QTableWidgetItem(QString::number(id)));
    table->item(row, COL_TEST_NAME)->setData(RoleId, id);
    table->item(row, COL_TEST_NAME)->setText(name);
    table->item(row, COL_TEST_DATE)->setText(date.toString(FORMAT_DATE));
    table->item(row, COL_TEST_DATE)->setFlags(table->item(row, COL_TEST_DATE)->flags() &
                                              ~Qt::ItemIsEditable);
    if (table->rowCount() > 0) {
        table->setCurrentCell(row, COL_TEST_NAME);
    }
}

void MainWindowTester::slotAddTask()
{
    if (_currentTest > 0) {
        QString name = "Новая ошибка";
        int id = Sql::createTask(_testProject, TrackError, name, st_TaskNew, user().id, 0, _currentTest);
        Sql::setTaskLevel(id, ui->comboLevel->itemData(_defaultLevel).toString());

        QTableWidget *table = ui->tableTask;
        int row = insertRowIntoTable(table, table->rowCount() - 1);
        table->setVerticalHeaderItem(row, new QTableWidgetItem(QString::number(id)));
        table->item(row, COL_TASK_NAME)->setData(RoleId, id);
        table->item(row, COL_TASK_NAME)->setText(name);
        if (table->rowCount() > 0) {
            table->setCurrentCell(row, COL_TEST_NAME);
        }
    }
}

void MainWindowTester::on_btCancelTask_clicked()
{
    QTableWidget *table = ui->tableTask;
    int row = table->currentRow();
    if (_currentTask && row >= 0 && row < table->rowCount() - 1) {
        Sql::setStatus(_currentTask, st_TaskClose, user().id);
        table->item(row, COL_TASK_NAME)->setData(RoleStatus, st_TaskClose);
        table->item(row, COL_TASK_NAME)->setToolTip(Sql::getStatusName(st_TaskClose));
        setRowColor(table, row, getStatusColor(st_TaskClose));
    }
}

void MainWindowTester::on_btReworkingTask_clicked()
{
    QTableWidget *table = ui->tableTask;
    int row = table->currentRow();
    if (_currentTask && row >= 0 && row < table->rowCount() - 1) {
        Sql::setStatus(_currentTask, st_TaskReworking, user().id);
        table->item(row, COL_TASK_NAME)->setData(RoleStatus, st_TaskReworking);
        table->item(row, COL_TASK_NAME)->setToolTip(Sql::getStatusName(st_TaskReworking));
        setRowColor(table, row, getStatusColor(st_TaskReworking));
    }
}

void MainWindowTester::on_actionExit_triggered()
{
    saveSeanse();
    qApp->exit(0);
}

void MainWindowTester::on_editFind_textChanged(const QString &arg1)
{
    int task = arg1.toInt();
    int parent = Sql::getInt(QString("SELECT root_id FROM issues WHERE id=%1;").arg(task));
    if (parent > 0) {
        for (int i = 0; i < ui->tableTest->rowCount()-1; ++i) {
            int id = ui->tableTest->item(i, COL_TEST_NAME)->data(RoleId).toInt();
            if (id == parent) {
                ui->tableTest->setCurrentCell(i, COL_TEST_NAME);
                if (parent != task) {
                    for (int j = 0; j < ui->tableTask->rowCount()-1; ++j) {
                        if (ui->tableTask->item(j, COL_TASK_NAME)->data(RoleId).toInt() == task) {
                            ui->tableTask->setCurrentCell(j, COL_TASK_NAME);
                            return;
                        }
                    }
                }
                return;
            }
        }
    }
}

void MainWindowTester::on_btLevelDefault_clicked()
{
    _defaultLevel = ui->comboLevel->currentIndex();
    ini::set("WidgetSettingsTester", "DefaultLevel", _defaultLevel);
}
