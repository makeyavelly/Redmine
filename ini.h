#ifndef INI_H
#define INI_H

#include <QString>
#include <QVariant>

#include <QMainWindow>
#include <QAction>
#include <QTabWidget>
#include <QSplitter>

namespace ini
{
    QVariant get(const QString &group, const QString &key,
                 const QVariant &defaultValue = QVariant());
    void set(const QString &group, const QString &key, const QVariant &value);

    void load(QMainWindow *widget);
    void save(QMainWindow *widget);

    void load(QAction *action);
    void save(QAction *action);

    void load(QTabWidget *tabWidget);
    void save(QTabWidget *tabWidget);

    void load(QSplitter *splitter);
    void save(QSplitter *splitter);
}

#endif // INI_H
