#include "Widget.h"

#include <QTextCodec>
#include <QProcess>
#include <QPainter>
#include <QCloseEvent>

Widget::Widget(const QString &title, QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle(title);
}

void Widget::preClose()
{}

void Widget::ok()
{
    emit s_ok();
}

void Widget::cancel()
{
    emit s_cancel();
}


CurrentWidget::CurrentWidget()
{
    initNull();
}

void CurrentWidget::clear()
{
    if (_action) {
        _action->setChecked(false);
    }
    initNull();
}

void CurrentWidget::setWidget(Widget *widget)
{
    this->_widget = widget;
}

void CurrentWidget::setAction(QObject *action)
{
    this->_action = static_cast<QAction*>(action);
}

bool CurrentWidget::preClose()
{
    if (_widget) {
        _widget->preClose();
        return true;
    }
    return false;
}

void CurrentWidget::initNull()
{
    _widget = 0x0;
    _action = 0x0;
}



QString MainWindowDefault::browser() const
{
    return _browser;
}

MainWindowDefault::MainWindowDefault(QWidget *parent) :
    QMainWindow(parent)
{
    initCodec();
}

MainWindowDefault::~MainWindowDefault()
{
}

User MainWindowDefault::user() const
{
    return _user;
}

void MainWindowDefault::setUser(const User &user)
{
    _user = user;
}

void MainWindowDefault::init()
{
    initUi();
    createTrayIcon();
    _mainWidget = this->centralWidget();
    _browser = ini::get("BROWSER", "NAME", "firefox").toString();
    try {
        Sql::connect();
        loadUser();
        update();
    } catch (const QString &error) {
        showError(error);
    }
}

void MainWindowDefault::showEvent(QShowEvent *)
{
    loadSeanse();
}

void MainWindowDefault::hideEvent(QHideEvent *)
{
    saveSeanse();
}

void MainWindowDefault::closeEvent(QCloseEvent *event)
{
    this->hide();
    event->ignore();
}

QIcon MainWindowDefault::getIconWindow() const
{
    QPixmap pixmap("://icon");
    QString label = getWindowLabel();
    if (!label.isEmpty()) {
        QPainter painter(&pixmap);
        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(36);
        painter.setFont(font);
        QPen pen = painter.pen();
        pen.setColor(QColor("#a51414"));
        painter.setPen(pen);
        painter.drawText(pixmap.rect(), Qt::AlignCenter, label);
    }
    return QIcon(pixmap);
}

void MainWindowDefault::initCodec()
{
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#if QT_VERSION_MAJOR < 5
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#endif
}

void MainWindowDefault::loadUser()
{
    setUser(Sql::loadUser(ini::get("USER", "NAME", "").toString()));
    QLabel *label = labelUser();
    if (label) {
        label->setText(user().title);
        label->setToolTip(user().name);
    }
}

void MainWindowDefault::createTrayIcon()
{
    _trayIcon = new QSystemTrayIcon(getIconWindow(), this);
    _trayIcon->show();
    QObject::connect(_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
                     this, SLOT(slotActivateTrayIcon(QSystemTrayIcon::ActivationReason)));
}

void MainWindowDefault::clickActionWidget(const bool &checked, Widget *widget, QObject *action)
{
    if (checked) {
        _current.setAction(action);
        openWidget(widget);
    } else {
        closeWidget();
    }
}

void MainWindowDefault::openWidget(Widget *widget)
{
    QObject::connect(widget, SIGNAL(s_ok()), this, SLOT(slotCancelWidget()));
    QObject::connect(widget, SIGNAL(s_cancel()), this, SLOT(slotCancelWidget()));
    _mainWidget->setParent(0x0);
    _current.setWidget(widget);
    this->setCentralWidget(widget);
}

void MainWindowDefault::closeWidget()
{
    if (_current.preClose()) {
        slotCancelWidget();
    }
}

void MainWindowDefault::openTaskInBrowser(const int &numberTask)
{
    QProcess::startDetached(QString("%3 http://%1:3000/issues/%2")
                            .arg(Sql::host())
                            .arg(numberTask)
                            .arg(browser()));
}

void MainWindowDefault::slotActivateTrayIcon(const QSystemTrayIcon::ActivationReason &reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (this->isHidden()) {
            this->show();
        } else {
            this->hide();
        }
    }
}

void MainWindowDefault::slotCancelWidget()
{
    delete this->centralWidget();
    this->setCentralWidget(_mainWidget);
    _current.clear();
}
