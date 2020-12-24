#ifndef ADDPRODUCTWIDGET_H
#define ADDPRODUCTWIDGET_H

#include <QWidget>
#include "product.h"
#include "productinfo.h"
#include <QMainWindow>

namespace Ui {
class addProductWidget;
}

class addProductWidget : public QWidget
{
    Q_OBJECT

public:
    explicit addProductWidget(QWidget *parent = 0);
    ~addProductWidget();

private:
    Ui::addProductWidget *ui;   // графический интерфейс
    product* ptr;   // основная информация о продукте
    productInfo* ptr_i; // вся информация о продукте
    QMainWindow* backPtr;   // указатель на главное окно
    bool changeMode, allOk; // режимы работы окна

signals:
    void send_new_product(product*, productInfo*);  // передача информации о новом продукте
    void send_changed_product(product*, productInfo*);  // передача информации об изменении продукта

public slots:
    void onlyMainPtr(QMainWindow*); // получение указателя на главное окно
    void get_ptr_i_for_view(productInfo*);  // получение всей информации о товаре для просмотра
    void get_ptr_i_for_change(productInfo*);    // получение всей информации о товаре для изменения
    void existing_name();   // обработка уже имеющегося названия

private slots:
    void onTextChanged();   // обработка работы с интерфейсом
    void on_nameEdit_editingFinished(); // обработка работы с интерфейсом
    void on_keyWordEdit_editingFinished();  // обработка работы с интерфейсом
    void on_descriptionEdit_textChanged();  // обработка работы с интерфейсом
    void on_purchasePriceEdit_editingFinished();    // обработка работы с интерфейсом
    void on_sellingPriceEdit_editingFinished(); // обработка работы с интерфейсом
    void on_taxEdit_editingFinished();  // обработка работы с интерфейсом
    void on_backButton_clicked();   // обработка работы с интерфейсом
    void on_shelfLifeEdit_editingFinished();    // обработка работы с интерфейсом
    void on_addPicButton_clicked(); // обработка работы с интерфейсом
    void on_exitButton_clicked();   // обработка работы с интерфейсом
};

#endif // ADDPRODUCTWIDGET_H
