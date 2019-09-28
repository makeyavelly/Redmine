#include "debug.h"

#include <iostream>
#include <QMessageBox>
#include <QApplication>

void print(const QString &message)
{
    std::cout << message.toLocal8Bit().constData() << std::endl;
}

void printError(const QString &error)
{
    std::cerr << "[ERROR]: " << error.toLocal8Bit().constData() << std::endl;
}


void showError(const QString &error)
{
    printError(error);
    QMessageBox::critical(qApp->activeWindow(), "Ошибка", error);
}
