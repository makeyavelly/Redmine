#include "Sql.h"

#include <QApplication>
#include "ini.h"
#include "debug.h"

Sql *sql = new Sql();

void Sql::connect(const QString &host, const QString &database, const QString &user, const QString &password)
{
    sql->_connect(host, database, user, password);
}

void Sql::connect()
{
    sql->_connect(ini::get("CONNECTION", "host", "127.0.0.1").toString(),
                   ini::get("CONNECTION", "database", "redmine").toString(),
                   ini::get("CONNECTION", "login", "postgres").toString(),
                   ini::get("CONNECTION", "password", "").toString());
}

QSqlQuery Sql::select(const QString &query)
{
    return sql->_select(query);
}

QVariant Sql::getValue(const QString &query)
{
    QSqlQuery q = Sql::select(query);
    if (q.first()) {
        return q.value(0);
    }
    return QVariant();
}

QString Sql::getString(const QString &query)
{
    return Sql::getValue(query).toString();
}

int Sql::getInt(const QString &query)
{
    return Sql::getValue(query).toInt();
}

User Sql::loadUser(const QString &userName)
{
    return sql->_loadUser(userName);
}

QString Sql::host()
{
    return sql->db.hostName();
}

QString Sql::database()
{
    return sql->db.databaseName();
}

QString Sql::username()
{
    return sql->db.userName();
}

QString Sql::password()
{
    return sql->db.password();
}

QSqlQuery Sql::selectTasks(const QString &type, const uint &idUser)
{
    return Sql::select(QString("SELECT id,subject,description,created_on,updated_on,start_date,due_date,status_id "
                               "FROM issues WHERE status_id IN (%1) AND assigned_to_id = %2 "
                               "ORDER BY updated_on DESC;")
                       .arg(type)
                       .arg(idUser));
}

QString Sql::getTaskName(const int &task)
{
    return Sql::getString(QString("SELECT subject FROM issues WHERE id=%1;").arg(task));
}

QString Sql::getTaskDescription(const int &task)
{
    QString description = Sql::getString(QString("SELECT description FROM issues WHERE id='%1';").arg(task));
    int n = 0;
    int i = description.indexOf("# ");
    while (i >= 0) {
        description = description.remove(i, 1);
        description.insert(i + 1, QString("%1. ").arg(++n));
        i = description.indexOf("# ");
    }
    return description;
}

int Sql::getTaskPercent(const int &task)
{
    return Sql::getInt(QString("SELECT done_ratio FROM issues WHERE id='%1';").arg(task));
}

int Sql::getTaskProject(const int &task)
{
    return Sql::getInt(QString("SELECT project_id FROM issues WHERE id='%1';").arg(task));
}

int Sql::getTaskTrack(const int &task)
{
    return Sql::getInt(QString("SELECT tracker_id FROM issues WHERE id='%1';").arg(task));
}

QString Sql::getTaskLevel(const int &task)
{
    return Sql::getString(QString("SELECT value FROM custom_values "
                                  "WHERE customized_type='Issue' AND customized_id=%1 AND custom_field_id=6;")
                          .arg(task));
}

int Sql::getTaskUser(const int &task)
{
    return Sql::getInt(QString("SELECT assigned_to_id FROM issues WHERE id='%1';").arg(task));
}

QList<Check *> Sql::getTaskCheckList(const int &task)
{
    QList<Check *> list;
    QSqlQuery q = Sql::select(QString("SELECT id, subject, is_done FROM checklists "
                                      "WHERE issue_id='%1' ORDER BY position;")
                              .arg(task));
    while (q.next()) {
        list.append(new Check(q.value(0).toInt(), q.value(1).toString(), q.value(2).toBool()));
    }
    return list;
}

QString Sql::getStatusName(const int &status)
{
    return Sql::getString(QString("SELECT name FROM issue_statuses WHERE id=%1;").arg(status));
}

void Sql::setStatus(const int &task, const int &status, const int &user)
{
    sql->_setStatus(task, status, user);
}

void Sql::setCheck(const int &task, const bool &status, const int &user)
{
    sql->_setCheck(task, status, user);
}

void Sql::setTaskPercent(const int &task, const int &percent, const int &user)
{
    sql->_setTaskPercent(task, percent, user);
}

void Sql::setTaskName(const int &task, const QString &name)
{
    Sql::select(QString("UPDATE issues SET subject='%2', updated_on=now() WHERE id=%1;")
                .arg(task)
                .arg(name));
}

void Sql::setTaskDescription(const int &task, const QString &description)
{
    Sql::select(QString("UPDATE issues SET description='%2', updated_on=now() WHERE id=%1;")
                .arg(task)
                .arg(description));
}

void Sql::setTaskProject(const int &task, const int &project)
{
    Sql::select(QString("UPDATE issues SET project_id=%2, updated_on=now() WHERE id=%1;")
                .arg(task)
                .arg(project));
}

void Sql::setTaskTrack(const int &task, const int &track)
{
    Sql::select(QString("UPDATE issues SET tracker_id=%2, updated_on=now() WHERE id=%1;")
                .arg(task)
                .arg(track));
}

void Sql::setTaskLevel(const int &task, const QString &level)
{
    Sql::select(QString("DELETE FROM custom_values WHERE customized_id=%1; "
                        "INSERT INTO custom_values (customized_type,customized_id,custom_field_id,value) "
                        "VALUES ('Issue',%1,6,'%2');")
                .arg(task)
                .arg(level));
}

void Sql::setTaskUser(const int &task, const int &user)
{
    Sql::select(QString("UPDATE issues SET assigned_to_id=%2, updated_on=now() WHERE id=%1;")
                .arg(task)
                .arg(user));
}

int Sql::createTask(const int &project, const int &track, const QString &name, const int status,
                    const int &author, const int &user, const int &parentTask)
{
    int rgt = 1;
    if (parentTask > 0) {
        Sql::select(QString("UPDATE issues SET rgt=rgt+2 WHERE id=%1;").arg(parentTask));
        rgt = Sql::getInt(QString("SELECT rgt FROM issues WHERE id=%1;").arg(parentTask));
    }
    int task = Sql::getInt(QString("INSERT INTO issues (root_id,parent_id,tracker_id,project_id,subject,status_id,"
                                   "assigned_to_id,author_id,created_on,updated_on,start_date,done_ratio,priority_id,"
                                   "lft,rgt) "
                                   "VALUES (%1,%1,%2,%3,'%4',%5,%6,%7,now(),now(),now(),0,2,%8-2,%8-1) "
                                   "RETURNING id;")
                           .arg(parentTask > 0 ? QString::number(parentTask) : "NULL")
                           .arg(track)
                           .arg(project)
                           .arg(name)
                           .arg(status)
                           .arg(user > 0 ? QString::number(user) : "NULL")
                           .arg(author)
                           .arg(rgt));
    if (parentTask <= 0) {
        Sql::select(QString("UPDATE issues SET root_id=id WHERE id=%1;").arg(task));
    }
    return task;
}

QString Sql::getBool(const bool &b)
{
    return b ? "TRUE" : "FALSE";
}

QSqlQuery Sql::selectHistory(const int &task)
{
    return sql->_selectHistory(task);
}

QVector<Project> Sql::getProjects()
{
    QVector<Project> projects;
    QSqlQuery q = Sql::select("SELECT id,name,parent_id FROM projects ORDER BY name;");
    while (q.next()) {
        projects.append(Project(q.value(0).toInt(),
                                q.value(1).toString(),
                                q.value(2).toInt()));
    }
    return projects;
}

QVector<Track> Sql::getTracks()
{
    QVector<Track> tracks;
    QSqlQuery q = Sql::select("SELECT id,name FROM trackers ORDER BY name;");
    while (q.next()) {
        tracks.append(Track(q.value(0).toInt(),
                            q.value(1).toString()));
    }
    return tracks;
}

QVector<Level> Sql::getLevels()
{
    QVector<Level> levels;
    QString str = Sql::getString("SELECT possible_values FROM custom_fields WHERE id=6;");
    foreach (QString line, str.split("\n")) {
        if (line.left(2) == "- ") {
            line = line.remove(0, 2);
            int level = 0;
            int i = 0;
            while (QRegExp("\\d+\\.").indexIn(line, i) == i) {
                ++level;
                i = line.indexOf(".", i) + 1;
            }
            levels.append(Level(level, line));
        }
    }
    return levels;
}

QVector<User> Sql::getUsers()
{
    QVector<User> users;
    QSqlQuery q = Sql::select("SELECT \"login\",id,lastname||' '||firstname AS title "
                              "FROM users WHERE \"login\"!='' ORDER BY title;");
    while (q.next()) {
        users.append(User(q.value(0).toString(),
                          q.value(1).toInt(),
                          q.value(2).toString()));
    }
    return users;
}


Sql::Sql()
{
    db = QSqlDatabase::addDatabase("QPSQL", qApp->applicationName());
}

void Sql::_connect(const QString &host, const QString &database, const QString &user, const QString &password)
{
    if (db.isOpen()) {
        db.close();
    }
    db.setPort(5432);
    db.setHostName(host);
    db.setDatabaseName(database);
    db.setUserName(user);
    db.setPassword(password);
    db.setConnectOptions(QString("application_name=%1").arg(qApp->applicationName()));
    if (!db.open()) {
        throw db.lastError().text();
    }
}

QSqlQuery Sql::_select(const QString &query)
{
    QSqlQuery q(db);
    if (!q.exec(query)) {
        printError(q.lastError().text());
    }
    return q;
}

User Sql::_loadUser(const QString &userName)
{
    User user(userName);
    QSqlQuery q = select(QString("SELECT id, firstname, lastname FROM users WHERE login = '%1';")
                         .arg(userName));
    if (q.first()) {
        user.id = q.value(0).toUInt();
        user.title = QString("%1 %2").arg(q.value(1).toString()).arg(q.value(2).toString());
    } else {
        user.title = "Неизвестный пользователь";
        throw QString("Пользователя %1 не существует")
                .arg(userName);
    }
    return user;
}

void Sql::_setStatus(const int &task, const int &status, const int &user)
{
    int oldStatus = Sql::getInt(QString("SELECT status_id FROM issues WHERE id='%1';")
                                .arg(task));
    _select(QString("UPDATE issues SET status_id='%2', updated_on=now() WHERE id='%1';")
             .arg(task)
             .arg(status));
    __addHistoryStatus(task, user, oldStatus, status);
    if (status == st_TaskWorking) {
        __startTask(task);
    }
    if (status == st_TaskTesting) {
        __executeTask(task, user);
    }
    if (status == st_TaskClose) {
        __closeTask(task);
    }
}

void Sql::__startTask(const int &task)
{
    _select(QString("UPDATE issues SET start_date=now() WHERE id='%1' AND start_date IS NULL;")
             .arg(task));
}

void Sql::__executeTask(const int &task, const int &user)
{
    _setTaskPercent(task, 100, user);
    _select(QString("UPDATE issues SET due_date=now() WHERE id='%1';")
             .arg(task));
}

void Sql::__closeTask(const int &task)
{
    _select(QString("UPDATE issues SET closed_on=now() WHERE id='%1';")
             .arg(task));
}

void Sql::_setCheck(const int &task, const int &status, const int &user)
{
    int taskId = Sql::getInt(QString("SELECT issue_id FROM checklists WHERE id='%1'").arg(task));
    QString oldCheck = _getCheck(taskId);
    _select(QString("UPDATE checklists SET is_done=%2, updated_at=now() WHERE id='%1';")
             .arg(task)
             .arg(Sql::getBool(status)));
    QString newCheck = _getCheck(taskId);
    __addHistoryCheck(taskId, user, oldCheck, newCheck);
    int count = Sql::getInt(QString("SELECT COUNT(id) FROM checklists WHERE issue_id='%1';").arg(taskId));
    int countDone = Sql::getInt(QString("SELECT COUNT(id) FROM checklists "
                                        "WHERE issue_id='%1' AND is_done=TRUE;").arg(taskId));
    if (count > 0) {
        _setTaskPercent(taskId, 100 * countDone / count, user);
//        Sql::select(QString("UPDATE issues SET done_ratio='%2' "
//                            "WHERE id='%1';").arg(taskId).arg(100 * countDone / count));
    }
}

QString Sql::_getCheck(const int &task)
{
    return Sql::getString(QString("SELECT string_agg(CASE WHEN is_done=TRUE THEN '[x] ' ELSE '[ ] ' END || subject, "
                                  "', ' ORDER BY position) FROM checklists WHERE issue_id='%1';")
                          .arg(task));
}

void Sql::_setTaskPercent(const int &task, const int &percent, const int &user)
{
    int oldPercent = Sql::getInt(QString("SELECT done_ratio FROM issues WHERE id='%1';").arg(task));
    if (oldPercent != percent) {
        _select(QString("UPDATE issues SET done_ratio=%2 WHERE id='%1';").arg(task).arg(percent));
        __addHistoryPercent(task, user, oldPercent, percent);
        int parent = Sql::getInt(QString("SELECT parent_id FROM issues WHERE id='%1';").arg(task));
        if (parent > 0) {
            int parentPercent = Sql::getInt(QString("SELECT AVG(done_ratio) FROM issues WHERE parent_id=%1;").arg(parent));
            _setTaskPercent(parent, parentPercent, user);
        }
    }
}

void Sql::__addHistory(const int &task, const int &user, const QString &property, const QString &key,
                       const QString &oldValue, const QString &newValue)
{
    if (oldValue == newValue) {
        return;
    }
    int id = Sql::getInt(QString("INSERT INTO journals (journalized_id,journalized_type,user_id,created_on) "
                                 "VALUES (%1,'Issue',%2,now()) RETURNING id;")
                         .arg(task)
                         .arg(user));
    if (id > 0) {
        _select(QString("INSERT INTO journal_details (journal_id,property,prop_key,old_value,value) "
                        "VALUES (%1,'%2','%3','%4','%5');")
                .arg(id)
                .arg(property)
                .arg(key)
                .arg(oldValue)
                .arg(newValue));
    }
}

void Sql::__addHistoryStatus(const int &task, const int &user, const int &oldValue, const int &newValue)
{
    __addHistory(task, user, "attr", "status_id", QString::number(oldValue), QString::number(newValue));
}

void Sql::__addHistoryPercent(const int &task, const int &user, const int &oldValue, const int &newValue)
{
    __addHistory(task, user, "attr", "done_ratio", QString::number(oldValue), QString::number(newValue));
}

void Sql::__addHistoryCheck(const int &task, const int &user, const QString &oldValue, const QString &newValue)
{
    __addHistory(task, user, "attr", "checklist", oldValue, newValue);
}

QSqlQuery Sql::_selectHistory(const int &task)
{
    return _select(QString("SELECT user_id,t1.created_on,property,prop_key,old_value,value "
                           "FROM journals t1 JOIN journal_details t2 ON t1.id=t2.journal_id "
                           "JOIN users t3 ON t1.user_id=t3.id WHERE journalized_id=%1 ORDER BY created_on DESC;")
                   .arg(task));
}
