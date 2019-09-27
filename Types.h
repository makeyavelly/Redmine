#ifndef TYPES_H
#define TYPES_H

#include <QString>
#include <QStringList>
#include <QPushButton>
#include <QCheckBox>
#include <QSlider>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidget>
#include <QComboBox>
#include <QFile>
#include <QTextEdit>

enum StatusTask
{
    st_TaskNew = 1, // Новая
    st_TaskWorking = 2, // В работе
    st_TaskTesting = 7, // Ожидает тестирования
    st_TaskReworking = 4, // В доработку
    st_TaskClose = 5, // Закрыта
    st_TaskReject = 6, // Отклонена
    st_TaskPause = 9 // Отложена
};

enum TrackTask
{
    TrackError = 1, // Ошибка
    TrackTest = 6 // Тестирование
};


const QString TASK_NEW = QString("%1,%2,%3").arg(st_TaskNew).arg(st_TaskReworking).arg(st_TaskPause);
const QString TASK_WORKING = QString::number(st_TaskWorking);
const QString TASK_TESTING = QString::number(st_TaskTesting);
const QString TASK_OPEN = QString("1,2,4,7,9");

const QString FORMAT_DATE = "dd.MM.yy";
const QString FORMAT_DATE_TIME = "hh.mm dd.MM.yy";

const int RoleId = 33;
const int RoleParent = 34;
const int RoleStatus = 35;


class User
{
public:
    uint id;
    QString name;
    QString title;

    User(const QString &name = QString(), const uint &id = 0, const QString &title = QString());
};


class Project
{
public:
    int id;
    int parent;
    QString name;

    Project(const int &id = 0, const QString &name = QString(), const int &parent = 0);
};


class Track
{
public:
    int id;
    QString name;
    Track(const int &id = 0, const QString &name = QString());
};


class Level
{
public:
    int level;
    QString name;
    Level(const int &level = 0, const QString &name = QString());
};


class Button : public QPushButton
{
    int _task;
    int _status;

public:
    Button(const QString &name, const QIcon &icon, const int &task, const int &status);

    int task() const;
    int status() const;
};


class Check : public QCheckBox
{
    int _id;

public:
    Check(const int &id = 0, const QString &name = QString(), const bool &done = false, QWidget *parent = 0x0);

    int id() const;
    bool done();
};


class PercentScale : public QWidget
{
Q_OBJECT

private:
    static const int STEP = 5;
    static const int TICK = 2;

public:
    explicit PercentScale(const int &id = 0, const int &value = 0, QWidget *parent = 0x0);
    ~PercentScale();

    int id() const;
    int value() const;
    void setValue(const int &newValue);

signals:
    void s_changeValue(const int &);

private slots:
    void slotSetValue(const int &newValue);
    void slotChangeValue();

private:
    int _id;
    QSlider *_slider;
    QLabel *_labelPercent;
    QPushButton *_btOk;
};


QTreeWidgetItem *findTreeWidgetItem(QTreeWidgetItem *parent, const QString &value,
                      const int &column = 0, const int &role = Qt::DisplayRole);
QTreeWidgetItem *findTreeWidgetItem(QTreeWidget *tree, const QString &value,
                      const int &column = 0, const int &role = Qt::DisplayRole);
QTreeWidgetItem *findTreeWidgetItem(QTreeWidget *tree, const int &value,
                      const int &column = 0, const int &role = Qt::DisplayRole);

QList<int> loadListIntFromFile(const QString &fileName);
void saveListIntIntoFile(const QList<int> &list, const QString &fileName);

void createTableHeader(QTableWidget *table, const QStringList &labels);
int insertRowIntoTable(QTableWidget *table, const int &row = -1);
void resizeTable(QTableWidget *table, const int &notFixColumn, const int &minWidthFixColumn);
void setRowColor(QTableWidget *table, const int &row, const QColor &color);
void setRowTextColor(QTableWidget *table, const int &row, const QColor &color);

int findDataInCombo(QComboBox *combo, const QString &value);
int findDataInCombo(QComboBox *combo, const int &value);

const QColor getStatusColor(const int &status);
const QColor getNewStatusColor(const int &status);

#endif // TYPES_H
