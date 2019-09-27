#include "ini.h"

#include <QApplication>
#include <QSettings>

namespace ini
{

QString getPath()
{
    return QString("%1/settings.ini").arg(qApp->applicationDirPath());
}

QVariant get(const QString &group, const QString &key, const QVariant &defaultValue)
{
    QSettings ini(getPath(), QSettings::IniFormat);
    ini.beginGroup(group);
    QVariant result = ini.value(key, defaultValue);
    ini.setValue(key, result);
    ini.endGroup();
    ini.sync();
    return result;
}

void set(const QString &group, const QString &key, const QVariant &value)
{
    QSettings ini(getPath(), QSettings::IniFormat);
    ini.beginGroup(group);
    ini.setValue(key, value);
    ini.endGroup();
    ini.sync();
}

void load(QMainWindow *mainWindow)
{
    mainWindow->setGeometry(get(mainWindow->objectName(), "x", 0).toInt(),
                            get(mainWindow->objectName(), "y", 0).toInt(),
                            get(mainWindow->objectName(), "width", 100).toInt(),
                            get(mainWindow->objectName(), "height", 100).toInt());
}

void save(QMainWindow *mainWindow)
{
    set(mainWindow->objectName(), "x", mainWindow->geometry().x());
    set(mainWindow->objectName(), "y", mainWindow->geometry().y());
    set(mainWindow->objectName(), "width", mainWindow->geometry().width());
    set(mainWindow->objectName(), "height", mainWindow->geometry().height());
}

void load(QAction *action)
{
    action->setChecked(get(action->objectName(), "check", false).toBool());
}

void save(QAction *action)
{
    set(action->objectName(), "check", action->isChecked());
}

void load(QTabWidget *tabWidget)
{
    tabWidget->setCurrentIndex(get(tabWidget->objectName(), "index", 0).toInt());
}

void save(QTabWidget *tabWidget)
{
    set(tabWidget->objectName(), "index", tabWidget->currentIndex());
}

void load(QSplitter *splitter)
{
    QList<int> sizes;
    QString size_t = get(splitter->objectName(), "sizes", "").toString();
    if (size_t.isEmpty()) {
        QStringList list;
        for (int i = 0; i < splitter->count(); ++i) {
            list.append("100");
        }
        size_t = list.join(";");
    }
    foreach (QString size, size_t.split(";")) {
        sizes.append(size.toInt());
    }
    splitter->setSizes(sizes);
}

void save(QSplitter *splitter)
{
    QStringList sizes;
    foreach (int size, splitter->sizes()) {
        sizes.append(QString::number(size));
    }
    set(splitter->objectName(), "sizes", sizes.join(";"));
}

}
