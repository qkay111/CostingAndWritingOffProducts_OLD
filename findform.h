#ifndef FINDFORM_H
#define FINDFORM_H

#include <QWidget>
#include <QMainWindow>
#include <QString>

namespace Ui {
class findForm;
}

class findForm : public QWidget
{
    Q_OBJECT

public:
    explicit findForm(QWidget *parent = 0);
    ~findForm();

signals:
    void send_string_for_find(QString); // передача строки для поиска

public slots:
    void getMainPtr(QMainWindow*);  // получение указателя на MainWindow
    void getIndex(int); // получение индекса сортировки

private slots:
    void on_pushButton_clicked();   // обработка нажатия на кнопку "Поиск"

private:
    Ui::findForm *ui;   // графический интерфейс
    QMainWindow* mainWindow;    // указатель на главное окно
};

#endif // FINDFORM_H
