#include "addproductwidget.h"
#include "myexceptions.h"
#include "ui_addproductwidget.h"
#include <QMessageBox>
#include <QPixmap>
#include <QFileDialog>
#include <cstring>

addProductWidget::addProductWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::addProductWidget)
{
    ui->setupUi(this);
    changeMode = false; // начальный режим работы
    allOk = true;   // начальный режим работы
    MyExceptions<product>::allocation(ptr, 1, true);
    MyExceptions<productInfo>::allocation(ptr_i, 1, true);
    backPtr = nullptr;  // указатель на MainWindow
    setWindowTitle("Продукт");

    ui->productPic->setAlignment(Qt::AlignCenter);
    ui->nameLabel->setAlignment(Qt::AlignCenter);
    ui->keyWordLabel->setAlignment(Qt::AlignCenter);
    ui->descriptionLabel->setAlignment(Qt::AlignCenter);
    ui->purchasePriceLabel->setAlignment(Qt::AlignCenter);
    ui->sellingPriceLabel->setAlignment(Qt::AlignCenter);
    ui->taxLabel->setAlignment(Qt::AlignCenter);
    ui->profitabilityLabel->setAlignment(Qt::AlignCenter);
    ui->profitabilityValueLabel->setAlignment(Qt::AlignCenter);
    ui->shelfLifeLabel->setAlignment(Qt::AlignCenter);

    ui->productPic->setStyleSheet("background-color: white;"
                                  "color: black;"
                                  "font-size: 35px");
    ui->nameLabel->setStyleSheet("background-color: none;"
                                 "border: 1px solid white;"
                                 "font-size: 21px;");
    ui->keyWordLabel->setStyleSheet("background-color: none;"
                                    "border: 1px solid white;"
                                    "font-size: 21px;");
    ui->descriptionLabel->setStyleSheet("background-color: none;"
                                        "border: 1px solid white;"
                                        "font-size: 21px;");
    ui->purchasePriceLabel->setStyleSheet("background-color: none;"
                                          "border: 1px solid white;"
                                            "font-size: 19px;");
    ui->sellingPriceLabel->setStyleSheet("background-color: none;"
                                         "border: 1px solid white;"
                                         "font-size: 19px;");
    ui->taxLabel->setStyleSheet("background-color: none;"
                                "border: 1px solid white;"
                                "font-size: 21px;");
    ui->profitabilityLabel->setStyleSheet("background-color: none;"
                                          "border: 1px solid white;"
                                          "font-size: 21px;");
    ui->profitabilityValueLabel->setStyleSheet("background-color: none;"
                                               "border: 1px solid white;"
                                               "font-size: 21px;");
    ui->shelfLifeLabel->setStyleSheet("background-color: none;"
                                      "border: 1px solid white;"
                                      "font-size: 19px");

    connect(ui->descriptionEdit, SIGNAL(textChanged()), this, SLOT(onTextChanged()));   // обработка кнопок

    QRegExp exp1("[A-Za-zА-Яа-я ]{1,30}");  // установка валидатора
    ui->nameEdit->setValidator(new QRegExpValidator(exp1, this));   // установка валидатора
    QRegExp exp2("[A-Za-zА-Яа-я]{1,10}");   // установка валидатора
    ui->keyWordEdit->setValidator(new QRegExpValidator(exp2, this));    // установка валидатора
    QRegExp exp3("[0-9]{1,6}|[0-9]{1,6}[.]{1}[0-9]{1,6}");  // установка валидатора
    ui->purchasePriceEdit->setValidator(new QRegExpValidator(exp3, this));  // установка валидатора
    ui->sellingPriceEdit->setValidator(new QRegExpValidator(exp3, this));   // установка валидатора
    QRegExp exp4("[0-9]{1,2}|[0-9]{1,2}[.]{1}[0-9]{1,7}");  // установка валидатора
    ui->taxEdit->setValidator(new QRegExpValidator(exp4, this));    // установка валидатора
    QRegExp exp5("[0-9]{1,8}"); // установка валидатора
    ui->shelfLifeEdit->setValidator(new QRegExpValidator(exp5, this));  // установка валидатора

    ui->productPic->setText("No Image");
}

addProductWidget::~addProductWidget()
{
    delete ptr;
    delete ptr_i;
    delete ui;
}

void addProductWidget::onlyMainPtr(QMainWindow* ptr)    // получение указателя на MainWindow
{
    backPtr = ptr;
}

void addProductWidget::existing_name()  // установка режима работы
{
    allOk = false;
}

void addProductWidget::get_ptr_i_for_view(productInfo* pointer) // получение информации о продукте для просмотра
{
    double a = pointer->get_purchasePrice(), b = pointer->get_sellingPrice(), c = pointer->get_tax();
    double res = (b - b * c / 100.0) / a;   // расчёт рентабельности
    char resstr[15];
    sprintf(resstr, "%lf", res);
    ui->profitabilityValueLabel->setText(resstr);   // заполнение интерфейса информацией
    ui->nameEdit->setText(pointer->get_name());
    ui->keyWordEdit->setText(pointer->get_keyWord());   // заполнение интерфейса информацией
    ui->descriptionEdit->setText(pointer->get_description());
    sprintf(resstr, "%lf", pointer->get_purchasePrice());
    ui->purchasePriceEdit->setText(resstr); // заполнение интерфейса информацией
    sprintf(resstr, "%lf", pointer->get_sellingPrice());
    ui->sellingPriceEdit->setText(resstr);  // заполнение интерфейса информацией
    sprintf(resstr, "%lf", pointer->get_tax());
    ui->taxEdit->setText(resstr);   // заполнение интерфейса информацией
    char tmpptr[15];
    bool tmpcheacker;
    tmpcheacker = _itoa_s(pointer->get_shelfLife(), tmpptr, 15, 10);
    if (tmpcheacker == true)
    {
        QMessageBox::critical(this, "Ошибка!", "Ошибка преобразования числа в строку!");
        return;
    }
    ui->shelfLifeEdit->setText(tmpptr); // заполнение интерфейса информацией
    if (!pointer->get_picProduct().isNull())    // заполнение интерфейса информацией
    {
        QPixmap pix(pointer->get_picProduct());
        if (pix.isNull())
        {
            ui->productPic->setText("No Image");
        }
        else
        {
            int w = ui->productPic->width();
            int h = ui->productPic->height();
            ui->productPic->clear();
            ui->productPic->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
        }
    }
    ui->backButton->setEnabled(false);  // обработка интерфейса
    ui->addPicButton->setEnabled(false);    // обработка интерфейса
    ui->nameEdit->setEnabled(false);    // обработка интерфейса
    ui->keyWordEdit->setEnabled(false); // обработка интерфейса
    ui->descriptionEdit->setEnabled(false); // обработка интерфейса
    ui->purchasePriceEdit->setEnabled(false);   // обработка интерфейса
    ui->sellingPriceEdit->setEnabled(false);    // обработка интерфейса
    ui->taxEdit->setEnabled(false); // обработка интерфейса
    ui->shelfLifeEdit->setEnabled(false);   // обработка интерфейса
}

void addProductWidget::get_ptr_i_for_change(productInfo* pointer)   // получение информации о продукте для изменения
{
    double a = pointer->get_purchasePrice(), b = pointer->get_sellingPrice(), c = pointer->get_tax();
    double res = (b - b * c / 100.0) / a;   // расчёт рентабельности
    char resstr[15];
    sprintf(resstr, "%lf", res);
    ui->profitabilityValueLabel->setText(resstr);   // заполнение интерфейса информацией

    ui->nameEdit->setText(pointer->get_name()); // заполнение интерфейса информацией
    ui->keyWordEdit->setText(pointer->get_keyWord());   // заполнение интерфейса информацией
    ui->descriptionEdit->setText(pointer->get_description());   // заполнение интерфейса информацией
    sprintf(resstr, "%lf", pointer->get_purchasePrice());
    ui->purchasePriceEdit->setText(resstr); // заполнение интерфейса информацией
    sprintf(resstr, "%lf", pointer->get_sellingPrice());
    ui->sellingPriceEdit->setText(resstr);  // заполнение интерфейса информацией
    sprintf(resstr, "%lf", pointer->get_tax());
    ui->taxEdit->setText(resstr);   // заполнение интерфейса информацией
    char tmpptr[15];
    bool tmpcheacker;
    tmpcheacker = _itoa_s(pointer->get_shelfLife(), tmpptr, 15, 10);
    if (tmpcheacker == true)
    {
        QMessageBox::critical(this, "Ошибка!", "Ошибка преобразования числа в строку!");
        return;
    }
    ui->shelfLifeEdit->setText(tmpptr); // заполнение интерфейса информацией
    if (!pointer->get_picProduct().isNull())    // заполнение интерфейса информацией
    {
        QPixmap pix(pointer->get_picProduct());
        if (pix.isNull())
        {
            ui->productPic->setText("No Image");
        }
        else
        {
            int w = ui->productPic->width();
            int h = ui->productPic->height();
            ui->productPic->clear();
            ui->productPic->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
        }
    }

    ptr->set_name(pointer->get_name());
    ptr_i->set_name(pointer->get_name());
    ptr->set_keyWord(pointer->get_keyWord());
    ptr_i->set_keyWord(pointer->get_keyWord());
    ptr_i->set_description(pointer->get_description());
    ptr_i->set_purchasePrice(pointer->get_purchasePrice());
    ptr_i->set_sellingPrice(pointer->get_sellingPrice());
    ptr_i->set_tax(pointer->get_tax());
    ptr_i->set_shelfLife(pointer->get_shelfLife());
    ptr_i->set_picProduct(pointer->get_picProduct());

    changeMode = true;  // установка режима работы
}

void addProductWidget::onTextChanged()  // установка валидации
{
    if(ui->descriptionEdit->toPlainText().length() > 150)
        ui->descriptionEdit->setText(ui->descriptionEdit->toPlainText().left(150));
}

void addProductWidget::on_nameEdit_editingFinished()    // обработка ввода имени
{
    ptr->set_name(ui->nameEdit->text());
    ptr_i->set_name(ui->nameEdit->text());
}

void addProductWidget::on_keyWordEdit_editingFinished() // обработка ввода ключевого слова
{
    ptr->set_keyWord(ui->keyWordEdit->text());
    ptr_i->set_keyWord(ui->keyWordEdit->text());
}

void addProductWidget::on_descriptionEdit_textChanged() // обработка ввода описания
{
    ptr_i->set_description(ui->descriptionEdit->toPlainText());
}

void addProductWidget::on_purchasePriceEdit_editingFinished()   // обработка ввода цены закупки
{
    ptr_i->set_purchasePrice(ui->purchasePriceEdit->text().toDouble());
    if (ptr_i->get_purchasePrice() > 0 && ptr_i->get_sellingPrice() > 0)
    {
        double a = ptr_i->get_purchasePrice(), b = ptr_i->get_sellingPrice(), c = ptr_i->get_tax();
        double res = (b - b * c / 100.0) / a;
        char resstr[15];
        sprintf(resstr, "%lf", res);
        ui->profitabilityValueLabel->setText(resstr);
    }
}

void addProductWidget::on_sellingPriceEdit_editingFinished()    // обработка ввода цены продажи
{
    ptr_i->set_sellingPrice(ui->sellingPriceEdit->text().toDouble());
    if (ptr_i->get_purchasePrice() > 0.0 && ptr_i->get_sellingPrice() > 0.0)
    {
        double a = ptr_i->get_purchasePrice(), b = ptr_i->get_sellingPrice(), c = ptr_i->get_tax();
        double res = (b - b * c / 100.0) / a;
        char resstr[15];
        sprintf(resstr, "%lf", res);
        ui->profitabilityValueLabel->setText(resstr);
    }
}

void addProductWidget::on_taxEdit_editingFinished() // обработка ввода налога
{
    ptr_i->set_tax(ui->taxEdit->text().toDouble());
    if (ptr_i->get_purchasePrice() > 0.0 && ptr_i->get_sellingPrice() > 0.0)
    {
        double a = ptr_i->get_purchasePrice(), b = ptr_i->get_sellingPrice(), c = ptr_i->get_tax();
        double res = (b - b * c / 100.0) / a;
        char resstr[15];
        sprintf(resstr, "%lf", res);
        ui->profitabilityValueLabel->setText(resstr);
    }
}

void addProductWidget::on_shelfLifeEdit_editingFinished()   // обработка ввода срока годности
{
    ptr_i->set_shelfLife(ui->shelfLifeEdit->text().toUInt());
}

void addProductWidget::on_addPicButton_clicked()    // обработка добавления изображения
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open image"), "", tr("(*.jpg)"));
    QPixmap pix(path);
    if (path.isNull())
    {
        QMessageBox::critical(this, "Ошибка!", "Ошибка открытия изображения!");
        return;
    }
    int w = ui->productPic->width();
    int h = ui->productPic->height();
    ui->productPic->clear();
    ui->productPic->setPixmap(pix.scaled(w, h, Qt::KeepAspectRatio));
    ptr_i->set_picProduct(path);
}

void addProductWidget::on_backButton_clicked()  // обработка нажатия на кнопку "Подтвердить"
{
    if (ptr->get_name().isNull() || ptr->get_keyWord().isNull() || ptr_i->get_purchasePrice() == 0.0 ||
       ptr_i->get_sellingPrice() == 0.0 || ptr_i->get_shelfLife() == 0.0)   // проверка на ввод обязательных полей
    {
        QMessageBox::information(this, "Ошибка!", "Введены не все обязательные элементы!");
        ui->nameLabel->setStyleSheet("background-color: none;"
                                     "border: 1px solid red;"
                                     "font-size: 21px;");
        ui->keyWordLabel->setStyleSheet("background-color: none;"
                                         "border: 1px solid red;"
                                         "font-size: 21px;");
        ui->purchasePriceLabel->setStyleSheet("background-color: none;"
                                              "border: 1px solid red;"
                                              "font-size: 19px;");
        ui->sellingPriceLabel->setStyleSheet("background-color: none;"
                                             "border: 1px solid red;"
                                             "font-size: 19px;");
        ui->shelfLifeLabel->setStyleSheet("background-color: none;"
                                          "border: 1px solid red;"
                                          "font-size: 19px;");
        return;
    }
    ui->keyWordLabel->setStyleSheet("background-color: none;"
                                    "border: 1px solid white;"
                                    "font-size: 21px;");
    ui->purchasePriceLabel->setStyleSheet("background-color: none;"
                                          "border: 1px solid white;"
                                          "font-size: 19px;");
    ui->sellingPriceLabel->setStyleSheet("background-color: none;"
                                         "border: 1px solid white;"
                                         "font-size: 19px;");
    ui->shelfLifeLabel->setStyleSheet("background-color: none;"
                                      "border: 1px solid white;"
                                      "font-size: 19px;");
    if (changeMode == false)    // проверка на режим работы
        emit send_new_product(ptr, ptr_i);
    else
        emit send_changed_product(ptr, ptr_i);
    if (!allOk) // проверка на режим работы
    {
        QMessageBox::information(this, "Предупреждение!", "Товар с таким названием уже существует!");
        ui->nameLabel->setStyleSheet("background-color: none;"
                                     "border: 1px solid red;"
                                     "font-size: 21px;");
        allOk = true;
        return;
    }
    ui->nameLabel->setStyleSheet("background-color: none;"
                                  "border: 1px solid white;"
                                  "font-size: 21px;");
    ui->productPic->setText("No Image");
    ui->nameEdit->clear();
    ui->keyWordEdit->clear();
    ui->descriptionEdit->clear();
    ui->purchasePriceEdit->clear();
    ui->sellingPriceEdit->clear();
    ui->taxEdit->clear();
    ui->profitabilityValueLabel->setText("0");
    ui->shelfLifeEdit->clear();
    backPtr->show();    // вызываем главное окно
    this->hide(); // скрываем текущее окно
    delete ptr; // очистка временной информации
    delete ptr_i;   // очистка временной информации
    MyExceptions<product>::allocation(ptr, 1, true);
    MyExceptions<productInfo>::allocation(ptr_i, 1, true);
    backPtr = nullptr;  // сброс режима работы
    changeMode = false; // сброс режима работы
    allOk = true;   // сброс режима работы
}

void addProductWidget::on_exitButton_clicked()  // обработка нажатия на кнопку "Назад"
{
    ui->nameLabel->setStyleSheet("background-color: none;"
                                  "border: 1px solid white;"
                                  "font-size: 21px;");
    ui->keyWordLabel->setStyleSheet("background-color: none;"
                                    "border: 1px solid white;"
                                    "font-size: 21px;");
    ui->purchasePriceLabel->setStyleSheet("background-color: none;"
                                          "border: 1px solid white;"
                                          "font-size: 19px;");
    ui->sellingPriceLabel->setStyleSheet("background-color: none;"
                                         "border: 1px solid white;"
                                         "font-size: 19px;");
    ui->shelfLifeLabel->setStyleSheet("background-color: none;"
                                      "border: 1px solid white;"
                                      "font-size: 19px;");
    ui->taxLabel->setStyleSheet("background-color: none;"
                               "border: 1px solid white;"
                               "font-size: 21px;");

    ui->backButton->setEnabled(true);
    ui->addPicButton->setEnabled(true);
    ui->nameEdit->setEnabled(true);
    ui->keyWordEdit->setEnabled(true);
    ui->descriptionEdit->setEnabled(true);
    ui->purchasePriceEdit->setEnabled(true);
    ui->sellingPriceEdit->setEnabled(true);
    ui->taxEdit->setEnabled(true);
    ui->shelfLifeEdit->setEnabled(true);

    ui->productPic->setText("No Image");
    ui->nameEdit->clear();
    ui->keyWordEdit->clear();
    ui->descriptionEdit->clear();
    ui->purchasePriceEdit->clear();
    ui->sellingPriceEdit->clear();
    ui->taxEdit->clear();
    ui->profitabilityValueLabel->setText("0");
    ui->shelfLifeEdit->clear();
    backPtr->show();    // вызываем главное окно
    this->hide(); // скрываем текущее окно
    delete ptr; // очистка временной информации
    delete ptr_i;   // очистка временной информации
    MyExceptions<product>::allocation(ptr, 1, true);
    MyExceptions<productInfo>::allocation(ptr_i, 1, true);
    backPtr = nullptr;  // сброс режима работы
    changeMode = false; // сброс режима работы
    allOk = true;   // сброс режима работы
}
