#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QMessageBox>
#include <QCloseEvent>
#include <QScrollBar>
#include <QLineEdit>
#include <QDateTime>
#include <QDebug>

#include "ini.h"
#include "debug.h"
#include "Sql.h"

#include "widget/WidgetSettings.h"
#include "widget/WidgetHistory.h"


enum {
    COL_TASK_NAME,
    // NEW
    COL_TASK_CREATE,
    COL_TASK_UPDATE,
    COL_BT_WORKING,
    COL_BT_REJECT,
    // WORKING
    COL_TASK_BEGIN = COL_TASK_CREATE,
    COL_BT_TESTING = COL_BT_WORKING,
    COL_BT_NEW = COL_BT_REJECT,
    // TESTING
    COL_TASK_EXECUTE = COL_TASK_UPDATE,
    COL_BT_CLOSE = COL_BT_WORKING,
    COL_BT_REWORKING = COL_BT_REJECT
};


QString date_t(const QDateTime &dateTime)
{
    return dateTime.toString(FORMAT_DATE);
}

QString date_t(const QDate &date)
{
    return date.toString(FORMAT_DATE);
}


MainWindow::MainWindow(QWidget *parent) :
    MainWindowDefault(parent),
    ui(new Ui::MainWindow),
    isShowDescription(true),
    countNewTask(0),
    timerId(0)
{
    init();
    timerStart();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *)
{
    resizeAllTablesTask();
}

void MainWindow::timerEvent(QTimerEvent *)
{
    int count = Sql::getInt(QString("SELECT COUNT(*) FROM issues WHERE status_id IN (%1) AND assigned_to_id=%2;")
                            .arg(TASK_NEW)
                            .arg(user().id));
    if (count > countNewTask) {
        this->show();
        update();
        QMessageBox::about(this, "Предупреждение", "Новая задача");
    }
}

void MainWindow::initUi()
{
    ui->setupUi(this);
    this->setWindowIcon(getIconWindow());
    foreach (QTableWidget *table, QList<QTableWidget*>() << ui->tableNewTask
             << ui->tableWorkingTask << ui->tableTestingTask) {
        QObject::connect(table, SIGNAL(currentCellChanged(int,int,int,int)),
                         this, SLOT(slotTableCurrentCellChanged(int,int,int,int)), Qt::QueuedConnection);
        QObject::connect(table, SIGNAL(cellDoubleClicked(int,int)),
                         this, SLOT(slotTableCellDoubleClicked(int,int)));
    }
}

QLabel *MainWindow::labelUser() const
{
    return ui->labelUser;
}

void MainWindow::update()
{
    timerStop();
    updateTableNewTask();
    updateTableWorkingTask();
    updateTableTestingTask();
    resizeAllTablesTask();
    timerStart();
}

void MainWindow::saveSeanse()
{
    ini::save(this);
    ini::save(ui->tabWidget);
    ini::save(ui->splitterNewTask);
    ini::save(ui->splitterWorkingTask);
    ini::save(ui->splitterTestingTask);
}

void MainWindow::loadSeanse()
{
    ini::load(this);
    ini::load(ui->tabWidget);
    ini::load(ui->splitterNewTask);
    ini::load(ui->splitterWorkingTask);
    ini::load(ui->splitterTestingTask);
    isRequest = ini::get("DIALOG", "is_request", true).toBool();
}

QString MainWindow::getWindowLabel() const
{
    return "D";
}

void MainWindow::timerStart()
{
    timerId = startTimer(5000);
}

void MainWindow::timerStop()
{
    if (timerId > 0) {
        killTimer(timerId);
        timerId = 0;
    }
}

void MainWindow::updateCurrentTask()
{
    QTableWidget *currentTable = getCurrentTable();
    if (currentTable) {
        QTableWidgetItem *current = currentTable->currentItem();
        currentTable->setCurrentCell(-1, -1);
        currentTable->setCurrentItem(current);
    }
}

void MainWindow::updateTableNewTask()
{
    QTableWidget *table = ui->tableNewTask;
    createTableHeader(table, QStringList() << "Название" << "Поставлена" << "Обновлена" << "" << "");
    fillTableTask(table, TASK_NEW);
    countNewTask = table->rowCount();
    for (int row = 0; row < table->rowCount(); ++row) {
        int task = table->verticalHeaderItem(row)->text().toInt();
        table->setCellWidget(row, COL_BT_WORKING, createButton("В работу", QIcon("://next"),
                                                               task, st_TaskWorking));
        table->setCellWidget(row, COL_BT_REJECT, createButton("Отклонить", QIcon("://no"),
                                                              task, st_TaskReject));
    }
}

void MainWindow::updateTableWorkingTask()
{
    QTableWidget *table = ui->tableWorkingTask;
    createTableHeader(table, QStringList() << "Название" << "Выполняется с" << "Обновлена" << "" << "");
    fillTableTask(table, TASK_WORKING);
    for (int row = 0; row < table->rowCount(); ++row) {
        int task = table->verticalHeaderItem(row)->text().toInt();
        table->setCellWidget(row, COL_BT_TESTING, createButton("На тестирование", QIcon("://ok"),
                                                               task, st_TaskTesting));
        table->setCellWidget(row, COL_BT_NEW, createButton("Отложить", QIcon("://previous"),
                                                           task, st_TaskPause));
    }
}

void MainWindow::updateTableTestingTask()
{
    QTableWidget *table = ui->tableTestingTask;
    createTableHeader(table, QStringList() << "Название" << "Выполняется с" << "Отправлена на\nтестирование" << "" << "");
    fillTableTask(table, TASK_TESTING);
    for (int row = 0; row < table->rowCount(); ++row) {
        int task = table->verticalHeaderItem(row)->text().toInt();
        table->setCellWidget(row, COL_BT_CLOSE, createButton("Закрыть", QIcon("://yes"),
                                                             task, st_TaskClose));
        table->setCellWidget(row, COL_BT_REWORKING, createButton("В доработку", QIcon("://remove"),
                                                                 task, st_TaskReworking));
    }
}

void MainWindow::fillTableTask(QTableWidget *table, const QString &type)
{
    int row = 0;
    QSqlQuery q = Sql::selectTasks(type, user().id);
    while (q.next()) {
        table->setRowCount(++row);
        QTableWidgetItem *item = new QTableWidgetItem(q.value(0).toString());
        item->setTextAlignment(Qt::AlignHCenter | Qt::AlignTop);
        table->setVerticalHeaderItem(row - 1, item);
        item = new QTableWidgetItem(q.value(1).toString());
        //item->setToolTip(q.value(2).toString());
        item->setTextAlignment(Qt::AlignLeft | Qt::AlignTop);
        table->setItem(row - 1, COL_TASK_NAME, item);

        if (table == ui->tableNewTask) {
            table->setItem(row - 1, COL_TASK_CREATE, new QTableWidgetItem(date_t(q.value(3).toDateTime())));
            table->setItem(row - 1, COL_TASK_UPDATE, new QTableWidgetItem(date_t(q.value(4).toDateTime())));
            setRowTextColor(table, row - 1, getNewStatusColor(q.value(7).toInt()));
        }
        if (table == ui->tableWorkingTask) {
            table->setItem(row - 1, COL_TASK_BEGIN, new QTableWidgetItem(date_t(q.value(5).toDate())));
            table->setItem(row - 1, COL_TASK_UPDATE, new QTableWidgetItem(date_t(q.value(4).toDateTime())));
        }
        if (table == ui->tableTestingTask) {
            table->setItem(row - 1, COL_TASK_BEGIN, new QTableWidgetItem(date_t(q.value(5).toDate())));
            table->setItem(row - 1, COL_TASK_EXECUTE, new QTableWidgetItem(date_t(q.value(6).toDate())));
        }
    }
    if (row > 0) {
        table->setCurrentCell(0, 0);
    }
}

void MainWindow::resizeAllTablesTask()
{
    resizeTable(ui->tableNewTask, COL_TASK_NAME, 200);
    resizeTable(ui->tableWorkingTask, COL_TASK_NAME, 200);
    resizeTable(ui->tableTestingTask, COL_TASK_NAME, 200);
}

void MainWindow::createTableHeader(QTableWidget *table, const QStringList &labels)
{
    table->clear();
    table->setRowCount(0);
    table->setColumnCount(labels.count());
    table->setHorizontalHeaderLabels(labels);
}

Button *MainWindow::createButton(const QString &name, const QIcon &icon, const int &task, const int &status)
{
    Button *button = new Button(name, icon, task, status);
    QObject::connect(button, SIGNAL(clicked()), this, SLOT(slotChangeStatus()));
    return button;
}

QWidget *MainWindow::getInfoWidget(QObject *sender)
{
    if (sender == ui->tableNewTask) {
        return ui->widgetInfoNewTask;
    } else if (sender == ui->tableWorkingTask) {
        return ui->widgetInfoWorkingTask;
    } else if (sender == ui->tableTestingTask) {
        return ui->widgetInfoTestingTask;
    }
    return 0x0;
}

void MainWindow::loadInfoTask(const int &numberTask, QWidget *widgetInfo)
{
    if (widgetInfo) {
        clearLayout(widgetInfo->layout());
        QVBoxLayout *layout = new QVBoxLayout();
        widgetInfo->setLayout(layout);
        addWidgetIntoLayout(layout, createInfoTaskDescription(numberTask));
        addWidgetIntoLayout(layout, createInfoTaskPercent(numberTask));
        addWidgetIntoLayout(layout, createInfoTaskCheckList(numberTask));
        layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Expanding));
    }
}

void MainWindow::clearLayout(QLayout *layout)
{
    if (layout) {
        int i = layout->count();
        while (i > 0) {
            QLayoutItem *item = layout->itemAt(--i);
            if (item) {
                if (item->widget()) {
                    delete item->widget();
                }
                layout->removeItem(item);
            }
        }
        delete layout;
    }
}

QGroupBox *MainWindow::createInfoTaskDescription(const int &numberTask)
{
    QString description = Sql::getTaskDescription(numberTask);
    if (description.isEmpty()) {
        return 0x0;
    }
    QGroupBox *group = new QGroupBox("Описание");
    group->setCheckable(true);
    QVBoxLayout *layout = new QVBoxLayout(group);
    group->setLayout(layout);
    QLabel *label = new QLabel(description);
    layout->addWidget(label);
    label->setWordWrap(true);
    QFont font = label->font();
    font.setItalic(true);
    label->setFont(font);
    group->setChecked(isShowDescription);
    label->setVisible(isShowDescription);
    QObject::connect(group, SIGNAL(clicked(bool)), this, SLOT(slotHideDescription(bool)));
    return group;
}

QGroupBox *MainWindow::createInfoTaskCheckList(const int &numberTask)
{
    QList<Check *> list = Sql::getTaskCheckList(numberTask);
    if (list.isEmpty()) {
        return 0x0;
    }
    QGroupBox *group = new QGroupBox("Список задач");
    QVBoxLayout *layout = new QVBoxLayout(group);
    group->setLayout(layout);
    foreach (Check *check, list) {
        check->setParent(group);
        QObject::connect(check, SIGNAL(toggled(bool)), this, SLOT(slotChangeStatusCheck(bool)));
        layout->addWidget(check);
    }
    return group;
}

PercentScale *MainWindow::createInfoTaskPercent(const int &numberTask)
{
    PercentScale *scale = new PercentScale(numberTask, Sql::getTaskPercent(numberTask));
    QObject::connect(scale, SIGNAL(s_changeValue(int)), this, SLOT(slotChangeTaskPercent(int)));
    return scale;
}

void MainWindow::addWidgetIntoLayout(QLayout *layout, QWidget *widget)
{
    if (widget && layout) {
        layout->addWidget(widget);
    }
}

QTableWidget *MainWindow::getCurrentTable() const
{
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        return ui->tableNewTask;
    case 1:
        return ui->tableWorkingTask;
    case 2:
        return ui->tableTestingTask;
    }
    return 0x0;
}

int MainWindow::getCurrentTask() const
{
    QTableWidget *table = getCurrentTable();
    if (table && table->currentRow() >= 0) {
        return table->verticalHeaderItem(table->currentRow())->text().toInt();
    }
    return 0;
}


void MainWindow::on_actionUpdate_triggered()
{
    update();
}

void MainWindow::on_actionSettings_triggered(bool checked)
{
    WidgetSettings *widget = new WidgetSettings(this);
    QObject::connect(widget, SIGNAL(s_ok()), this, SLOT(slotOkSettingsWidget()));
    clickActionWidget(checked, widget, sender());
}

void MainWindow::on_actionHistory_triggered(bool checked)
{
    if (checked) {
        int currentTask = getCurrentTask();
        if (currentTask) {
            WidgetHistory *widget = new WidgetHistory(currentTask, this);
            clickActionWidget(checked, widget, sender());
        } else {
            QMessageBox::warning(this, "Ошибка", "Необходимо выбрать задачу");
            ui->actionHistory->setChecked(false);
        }
    } else {
        closeWidget();
    }
}

void MainWindow::on_actionExit_triggered()
{
    saveSeanse();
    qApp->exit(0);
}

void MainWindow::on_tabWidget_currentChanged(int)
{
    resizeAllTablesTask();
}

void MainWindow::slotTableCurrentCellChanged(const int &row, const int &, const int &previosRow, const int &)
{
    if (row >= 0) {
        QTableWidget *table = static_cast<QTableWidget*>(sender());
        if (previosRow >= 0 && previosRow < table->rowCount()) {
            table->item(previosRow, 0)->setIcon(QIcon());
        }
        if (row >= 0 && row < table->rowCount()) {
            table->item(row, 0)->setIcon(QIcon("://next"));
        }
        if (table && table->verticalHeaderItem(row)) {
            loadInfoTask(table->verticalHeaderItem(row)->text().toInt(), getInfoWidget(sender()));
        }
    }
}

void MainWindow::slotTableCellDoubleClicked(const int &row, const int &)
{
    if (row >= 0) {
        openTaskInBrowser(static_cast<QTableWidget*>(sender())->verticalHeaderItem(row)->text().toInt());
    }
}

void MainWindow::slotChangeStatusCheck(const bool &status)
{
    Check *check = static_cast<Check*>(sender());
    if (check) {
        Sql::setCheck(check->id(), status, user().id);
        updateCurrentTask();
    }
}

void MainWindow::slotChangeTaskPercent(const int &percent)
{
    Check *check = static_cast<Check*>(sender());
    if (check) {
        Sql::setTaskPercent(check->id(), percent, user().id);
    }
}

void MainWindow::slotOkSettingsWidget()
{
    isRequest = ini::get("DIALOG", "is_request", true).toBool();
    loadUser();
}

void MainWindow::slotChangeStatus()
{
    Button *button = static_cast<Button*>(sender());
    if (button) {
        bool isUpdate = true;
        if (button->status() == st_TaskClose && isRequest &&
                QMessageBox::question(this, "Предупреждение", "Вы уверены, что хотите закрыть задачу?",
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            isUpdate = false;
        }
        if (button->status() == st_TaskReject && isRequest &&
                QMessageBox::question(this, "Предупреждение", "Вы уверены, что хотите отклонить задачу?",
                                      QMessageBox::Yes | QMessageBox::No) == QMessageBox::No) {
            isUpdate = false;
        }
        if (isUpdate) {
            Sql::setStatus(button->task(), button->status(), user().id);
            update();
        }
    }
}

void MainWindow::slotHideDescription(bool isShow)
{
    QWidget *widget = static_cast<QWidget*>(sender())->layout()->itemAt(0)->widget();
    if (widget) {
        isShowDescription = isShow;
        widget->setVisible(isShow);
    }
}
