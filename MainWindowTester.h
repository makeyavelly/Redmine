#ifndef MAINWINDOWTESTER_H
#define MAINWINDOWTESTER_H

#include "widget/Widget.h"

namespace Ui {
class MainWindowTester;
}

class MainWindowTester : public MainWindowDefault
{
    Q_OBJECT

    Ui::MainWindowTester *ui;
    int _testProject;
    int _defaultLevel;
    int _currentTest;
    int _currentTask;

public:
    explicit MainWindowTester(QWidget *parent = 0);
    ~MainWindowTester();
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *);
    bool eventFilter(QObject *object, QEvent *event);

private:
    void initUi();
    void update();
    void loadSeanse();
    void saveSeanse();
    QString getWindowLabel() const;

    void loadTestProject();
    void loadProjects();
    void loadTracks();
    void loadLevels();
    void loadUsers();
    void loadProjectForTesting();

    void updateSize();

    QPushButton *createButton(const QIcon &icon, const QString &name,
                              const char *slot, const QString &toolTip = QString());

private slots:
    void on_actionUpdate_triggered();
    void on_actionSettings_triggered(bool checked);

    void on_MainWindowTesterSplitter_splitterMoved(int, int);

    void on_tableTest_currentCellChanged(int currentRow, int, int, int);
    void on_tableTask_currentCellChanged(int currentRow, int, int, int);
    void slotTableCurrentCellChanged(int currentRow, int, int previosRow, int);
    void on_tableTest_cellDoubleClicked(int row, int);
    void on_tableTask_cellDoubleClicked(int row, int);

    void on_editName_editingFinished();
    void on_comboProject_currentIndexChanged(int index);
    void on_comboTrack_currentIndexChanged(int index);
    void on_comboLevel_currentIndexChanged(int index);
    void on_comboUser_currentIndexChanged(int index);
    void on_tableTest_cellChanged(int row, int column);

    void slotAddTest();
    void slotAddTask();

    void on_btCancelTask_clicked();
    void on_btReworkingTask_clicked();
    void on_actionExit_triggered();
    void on_editFind_textChanged(const QString &arg1);
    void on_btLevelDefault_clicked();
};

#endif // MAINWINDOWTESTER_H
