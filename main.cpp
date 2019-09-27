#include <QApplication>

#include "ini.h"
#include "MainWindow.h"
#include "MainWindowTester.h"

#define MODE_DEVELOPER
#define MODE_TESTER

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setApplicationName("Redmine");

#ifdef MODE_TESTER
    bool isTester = (ini::get("MODE", "TESTER", "on").toString() == "on");
    if (isTester) {
        (new MainWindowTester())->show();
    }
#endif

#ifdef MODE_DEVELOPER
    bool isDeveloper = (ini::get("MODE", "DEVELOPER", "on").toString() == "on");
    if (isDeveloper) {
        (new MainWindow())->show();
    }
#endif

    return a.exec();
}
