#include "debug.h"

#include <iostream>
#include <QMessageBox>
#include <QApplication>

void print(const QString &message)
{
    std::cout << message.toAscii().constData() << std::endl;
}

void printError(const QString &error)
{
    std::cerr << "[ERROR]: " << error.toAscii().constData() << std::endl;
}


void showError(const QString &error)
{
    printError(error);
    QMessageBox::critical(qApp->activeWindow(), "Ошибка", error);
}
