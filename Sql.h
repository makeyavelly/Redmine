#ifndef SQL_H
#define SQL_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "Types.h"

class Sql
{
    QSqlDatabase db;

public:
    Sql();

    static void connect(const QString &host, const QString &database,
                   const QString &user, const QString &password);
    static void connect();
    static QSqlQuery select(const QString &query);
    static QVariant getValue(const QString &query);
    static QString getString(const QString &query);
    static int getInt(const QString &query);
    static User loadUser(const QString &userName);

    static QString host();
    static QString database();
    static QString username();
    static QString password();

    static QSqlQuery selectTasks(const QString &type, const uint &idUser);
    static QString getTaskName(const int &task);
    static QString getTaskDescription(const int &task);
    static int getTaskPercent(const int &task);
    static int getTaskProject(const int &task);
    static int getTaskTrack(const int &task);
    static QString getTaskLevel(const int &task);
    static int getTaskUser(const int &task);
    static QList<Check *> getTaskCheckList(const int &task);
    static QString getStatusName(const int &status);

    static void setStatus(const int &task, const int &status, const int &user);
    static void setCheck(const int &task, const bool &status, const int &user);
    static void setTaskPercent(const int &task, const int &percent, const int &user);

    static void setTaskName(const int &task, const QString &name);
    static void setTaskDescription(const int &task, const QString &description);
    static void setTaskProject(const int &task, const int &project);
    static void setTaskTrack(const int &task, const int &track);
    static void setTaskLevel(const int &task, const QString &level);
    static void setTaskUser(const int &task, const int &user);

    static int createTask(const int &project, const int &track, const QString &name, const int status,
                          const int &author, const int &user, const int &parentTask = 0);

    static QString getBool(const bool &b);

    static QSqlQuery selectHistory(const int &task);

    static QVector<Project> getProjects();
    static QVector<Track> getTracks();
    static QVector<Level> getLevels();
    static QVector<User> getUsers();

private:
    void _connect(const QString &host, const QString &database,
                   const QString &user, const QString &password);
    QSqlQuery _select(const QString &query);
    User _loadUser(const QString &userName);

    void _setStatus(const int &task, const int &status, const int &user);
    void __startTask(const int &task);
    void __executeTask(const int &task, const int &user);
    void __closeTask(const int &task);

    void _setCheck(const int &task, const int &status, const int &user);
    QString _getCheck(const int &task);

    void _setTaskPercent(const int &task, const int &percent, const int &user);

    void __addHistory(const int &task, const int &user, const QString &property, const QString &key,
                      const QString &oldValue, const QString &newValue);
    void __addHistoryStatus(const int &task, const int &user, const int &oldValue, const int &newValue);
    void __addHistoryPercent(const int &task, const int &user, const int &oldValue, const int &newValue);
    void __addHistoryCheck(const int &task, const int &user, const QString &oldValue, const QString &newValue);

    QSqlQuery _selectHistory(const int &task);
};

#endif // SQL_H
