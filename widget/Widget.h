#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QAction>
#include <QMainWindow>
#include <QSystemTrayIcon>

#include "Sql.h"
#include "ini.h"
#include "debug.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(const QString &title, QWidget *parent = 0);

    virtual void preClose();

protected:
    void ok();
    void cancel();

signals:
    void s_ok();
    void s_cancel();
};


class CurrentWidget
{
    Widget *_widget;
    QAction *_action;

public:
    CurrentWidget();
    void clear();
    void setWidget(Widget *widget);
    void setAction(QObject *action);
    bool preClose();

private:
    void initNull();
};


class MainWindowDefault : public QMainWindow
{
    Q_OBJECT

    User _user;
    QString _browser;
    QWidget *_mainWidget;
    CurrentWidget _current;
    QSystemTrayIcon *_trayIcon;

public:
    explicit MainWindowDefault(QWidget *parent = 0);
    ~MainWindowDefault();

    User user() const;
    void setUser(const User &user);

    QString browser() const;

protected:
    void init();
    void loadUser();

    virtual void initUi() {}
    virtual QLabel *labelUser() const { return 0x0; }
    virtual QString getWindowLabel() const { return QString(); }
    virtual void update() {}
    virtual void loadSeanse() {}
    virtual void saveSeanse() {}

    void clickActionWidget(const bool &checked, Widget *widget, QObject *action);
    void openWidget(Widget *widget);
    void closeWidget();

    void openTaskInBrowser(const int &numberTask);

    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    void closeEvent(QCloseEvent *event);

    QIcon getIconWindow() const;

private:
    void initCodec();
    void createTrayIcon();

private slots:
    void slotActivateTrayIcon(const QSystemTrayIcon::ActivationReason &reason);
    void slotCancelWidget();
};

#endif // WIDGET_H
