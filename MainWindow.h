#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QTableWidget>
#include <QLabel>
#include <QGroupBox>
#include <QSpinBox>

#include "Types.h"
#include "widget/Widget.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public MainWindowDefault
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resizeEvent(QResizeEvent *);
    void timerEvent(QTimerEvent *);

private:
    Ui::MainWindow *ui;
    bool isRequest;
    bool isShowDescription;
    int countNewTask;
    int timerId;

    void initUi();
    QLabel *labelUser() const;
    void update();
    void saveSeanse();
    void loadSeanse();
    QString getWindowLabel() const;

    void timerStart();
    void timerStop();

    void updateCurrentTask();

    void updateTableNewTask();
    void updateTableWorkingTask();
    void updateTableTestingTask();
    void fillTableTask(QTableWidget *table, const QString &type);
    void resizeAllTablesTask();

    void createTableHeader(QTableWidget *table, const QStringList &labels);
    Button *createButton(const QString &name, const QIcon &icon, const int &task, const int &status);
    QWidget *getInfoWidget(QObject *sender);

    void loadInfoTask(const int &numberTask, QWidget *widgetInfo);
    void clearLayout(QLayout *layout);
    QGroupBox *createInfoTaskDescription(const int &numberTask);
    QGroupBox *createInfoTaskCheckList(const int &numberTask);
    PercentScale *createInfoTaskPercent(const int &numberTask);
    void addWidgetIntoLayout(QLayout *layout, QWidget *widget);

    QTableWidget *getCurrentTable() const;
    int getCurrentTask() const;

private slots:
    void on_actionUpdate_triggered();
    void on_actionSettings_triggered(bool checked);
    void on_actionHistory_triggered(bool checked);
    void on_actionExit_triggered();
    void on_tabWidget_currentChanged(int);

    void slotTableCurrentCellChanged(const int &row, const int &, const int &previosRow, const int &);
    void slotTableCellDoubleClicked(const int &row, const int &);
    void slotChangeStatusCheck(const bool &status);
    void slotChangeTaskPercent(const int &percent);

    void slotOkSettingsWidget();
    void slotChangeStatus();

    void slotHideDescription(bool isShow);
};

#endif // MAINWINDOW_H
