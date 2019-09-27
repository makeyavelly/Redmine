#ifndef WIDGETHISTORY_H
#define WIDGETHISTORY_H

#include <QDateTime>

#include "Widget.h"

namespace Ui {
class WidgetHistory;
}

class WidgetHistory : public Widget
{
    Q_OBJECT

public:
    explicit WidgetHistory(const int &task, QWidget *parent = 0);
    ~WidgetHistory();

private:
    Ui::WidgetHistory *ui;
    int task;
    QString taskName;
    QStringList history;
    QList< QPair<int, QString> > users;
    QList< QPair<int, QString> > statuses;

    void load();
    void loadUsers();
    void loadStatuses();
    void loadHistory();

    void addHistory(const int &user, const QDateTime &date, const QString &property,
                    const QString &propKey, const QString &oldValue, const QString &newValue);
    void update();

    QString getUser(const int &id) const;
    QString getStatus(const int &id) const;
    QString getProperty(const QString &property, const QString &propKey,
                        const QString &oldValue, const QString &newValue) const;
    QString _getProperty(const QString &title, const QString &oldValue, const QString &newValue) const;

private slots:
    void on_btHome_clicked();
};

#endif // WIDGETHISTORY_H
