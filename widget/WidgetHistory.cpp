#include "WidgetHistory.h"
#include "ui_WidgetHistory.h"

#include "Sql.h"
#include "debug.h"


const QString HEADER = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\"> "
                       "<html>"
                       "<head>"
                       "<meta name=\"qrichtext\" content=\"1\" />"
                       "<style type=\"text/css\"> "
                       "p, li { white-space: pre-wrap; } "
                       "</style>"
                       "</head>"
                       "<body style=\" font-family:'Cousine'; font-size:11pt; font-weight:400; font-style:normal;\">";
const QString END    = "</body>"
                       "</html>";


WidgetHistory::WidgetHistory(const int &task, QWidget *parent) :
    Widget("История", parent),
    ui(new Ui::WidgetHistory),
    task(task)
{
    ui->setupUi(this);
    load();
    update();
}

WidgetHistory::~WidgetHistory()
{
    delete ui;
}

void WidgetHistory::load()
{
    taskName = Sql::getTaskName(task);
    loadUsers();
    loadStatuses();
    loadHistory();
}

void WidgetHistory::loadUsers()
{
    users.clear();
    QSqlQuery q = Sql::select("SELECT id,lastname||' '||firstname FROM users ORDER BY id;");
    while (q.next()) {
        users.append(QPair<int, QString>(q.value(0).toInt(), q.value(1).toString()));
    }
}

void WidgetHistory::loadStatuses()
{
    statuses.clear();
    QSqlQuery q = Sql::select("SELECT id,name FROM issue_statuses ORDER BY id;");
    while (q.next()) {
        statuses.append(QPair<int, QString>(q.value(0).toInt(), q.value(1).toString()));
    }
}

void WidgetHistory::loadHistory()
{
    history.clear();
    QSqlQuery q = Sql::selectHistory(task);
    while (q.next()) {
        addHistory(q.value(0).toInt(), q.value(1).toDateTime(), q.value(2).toString(),
                   q.value(3).toString(), q.value(4).toString(), q.value(5).toString());
    }
}

void WidgetHistory::addHistory(const int &user, const QDateTime &date, const QString &property,
                               const QString &propKey, const QString &oldValue, const QString &newValue)
{
    QString elem = getProperty(property, propKey, oldValue, newValue);
    if (!elem.isEmpty()) {
        history.append(QString("<div style='border:1px solid black;padding-left:15px;padding-right:15px;margin:2px;'>"
                               "<p style='color:#404a2b;'><b><i>%1 [%2]:</i></b></p>"
                               "<p>%3</p></div>")
                       .arg(getUser(user))
                       .arg(date.toString(FORMAT_DATE_TIME))
                       .arg(elem));
    }
}

void WidgetHistory::update()
{
    ui->editHistory->setHtml(QString("%1%2%3%4")
                             .arg(HEADER)
                             .arg(QString("<p style='font-size:12pt;'><b>История для задачи "
                                          "<a href=http://%3:3000/issues/%1>№%1</a>: %2<b></p>")
                                  .arg(task)
                                  .arg(taskName)
                                  .arg(Sql::host()))
                             .arg(history.join(""))
                             .arg(END));
}


QString WidgetHistory::getUser(const int &id) const
{
    QPair<int, QString> user;
    foreach (user, users) {
        if (user.first == id) {
            return user.second;
        }
    }
    return QString();
}

QString WidgetHistory::getStatus(const int &id) const
{
    QPair<int, QString> status;
    foreach (status, statuses) {
        if (status.first == id) {
            return status.second;
        }
    }
    return QString();
}

QString WidgetHistory::getProperty(const QString &property, const QString &propKey,
                                   const QString &oldValue, const QString &newValue) const
{
    if (property == "attr") {
        if (propKey == "description") {
            return _getProperty("Изменилось описание", oldValue, newValue);
        }
        if (propKey == "status_id") {
            return _getProperty("Изменился статус", getStatus(oldValue.toInt()), getStatus(newValue.toInt()));
        }
        if (propKey == "done_ratio") {
            return _getProperty("Изменилась готовность", QString("%1 %").arg(oldValue), QString("%1 %").arg(newValue));
        }
        if (propKey == "checklist") {
            QString oldV = oldValue;
            QString newV = newValue;
            return _getProperty("Изменился чеклист",
                                oldV.replace(" [x]", "<br>   [x]").replace(" [ ]", "<br>   [ ]"),
                                newV.replace(" [x]", "<br>   [x]").replace(" [ ]", "<br>   [ ]"));
        }
    }
    return QString();
}

QString WidgetHistory::_getProperty(const QString &title, const QString &oldValue, const QString &newValue) const
{
    return QString("%1:"
                   "<p style='color:#8f8f8f;background-color:#efefef;'>   %2</p>"
                   "<p style='color:#004f00;background-color:#cfffcf;'>   %3</p>")
            .arg(title)
            .arg(oldValue)
            .arg(newValue);
}

void WidgetHistory::on_btHome_clicked()
{
    update();
}
