#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "accounting.h"
#include "addproductwidget.h"
#include "findform.h"
#include "product.h"
#include "productinfo.h"
#include "stepback.h"
#include <QMainWindow>
#include <QVector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void get_new_product(product*, productInfo*);   // добавление нового продукта
    void get_changed_product(product*, productInfo*);   // изменение продукта
    void get_string_for_find(QString);  // поиск по строке
    void on_newFile_triggered();    // обработка нажатия на кнопку "Новый"
    void on_openFile_triggered();   // обработка нажатия на кнопку "Открыть"
    void on_saveFile_triggered();   // обработка нажатия на кнопку "Сохранить"
    void on_closeFile_triggered();  // обработка нажатия на кнопку "Закрыть"
    void on_addPushButton_clicked();    // обработка нажатия на кнопку "Добавить"
    void on_changePushButton_clicked(); // обработка нажатия на кнопку "Изменить"
    void on_deletePushButton_clicked(); // обработка нажатия на кнопку "Удалить"
    void on_aboutButton_clicked();  // обработка нажатия на кнопку "Подробнее"
    void on_sortButton_clicked();   // обработка нажатия на кнопку "Отсортировать"
    void on_author_triggered(); // обработка нажатия на кнопку "Автор"
    void on_find_triggered();   // обработка нажатия на кнопку "Найти"
    void on_back_triggered();   // обработка нажатия на кнопку "Назад"

signals:
    void send_main_ptr(QMainWindow*);   // передача указателя на MainWindow
    void send_main_ptr_for_find(QMainWindow*);  //передача указателя на MainWindow для поиска
    void send_ptr_i_for_view(productInfo*); // передача информации о продукте для просмотра
    void send_ptr_i_for_change(productInfo*);   // передача информации о продукте для изменения
    void send_index_for_find(int);  // передача индекса для поиска
    void existing_name();   // проверка на существование такого названия

private:
    Ui::MainWindow *ui; // графический интерфейс
    addProductWidget* addForm;  // окно ввода и вывода информации о продукте
    findForm* FindForm; // окно для ввода строки для поиска
    accounting<product, productInfo> listProduct;   // контейнер продуктов
    QVector<StepBack> stepBackBuf;  // список последних действий
    void fileNotUse();  // состояние интерфейса до работы с файлом
    void filePartiallyUse();    // состояние интерфейса в начале работы с файлом
    void fileUse(); // состояние интерфейса во время работы с файлом
    void idSort();  // сортировка по времени добавления продукта
    void nameSort();    // сортировка по названию продукта
    void keyWordSort(); // сортировка по ключевому слову продукта
};

#endif // MAINWINDOW_H
