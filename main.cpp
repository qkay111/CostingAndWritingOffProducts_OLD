#include "mainwindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);   // подключение графического интерфейса
    QFile styleFile(":/style.css");
    styleFile.open(QFile::ReadOnly);
    app.setStyleSheet(styleFile.readAll());
    MainWindow mainWindow;  // создаём главное окно
    mainWindow.show();  // вызываем главное окно
    return app.exec();
}
