#include "mainwindow.h"
#include "myexceptions.h"
#include "ui_mainwindow.h"
#include <cstring>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MyExceptions<addProductWidget>::allocation(addForm, 1, true);   // создаём окно ввода
    addForm->setWindowFlags(Qt::FramelessWindowHint);   // чтобы не было Windows bar'a
    MyExceptions<findForm>::allocation(FindForm, 1, true);  // создаём окно поиска
    FindForm->setWindowFlags(Qt::FramelessWindowHint);  // чтобы не было Windows bar'a
    StepBack tmp;
    tmp.set_type(StepBack::typeOp::SORTID); // создаём первое действие
    stepBackBuf.push_back(tmp);
    setWindowTitle("Калькуляция и списание продуктов");

    ui->newFile->setShortcut(tr("Ctrl+N")); // горячие клавиши
    ui->openFile->setShortcut(tr("Ctrl+O"));    // горячие клавиши
    ui->saveFile->setShortcut(tr("Ctrl+S"));    // горячие клавиши
    ui->closeFile->setShortcut(tr("Ctrl+C"));   // горячие клавиши
    ui->back->setShortcut(tr("Ctrl+Z"));    // горячие клавиши
    ui->find->setShortcut(tr("Ctrl+F"));    // горячие клавиши

    ui->sortLabel->setAlignment(Qt::AlignCenter);   // обработка интерфейса
    ui->thereIsNotAFile->setAlignment(Qt::AlignCenter); // обработка интерфейса
    ui->thereIsNotAFile->setStyleSheet("font-size: 45px");  // обработка интерфейса

    ui->sortBox->setEditable(true); // выравнивание комбо-бокса
    ui->sortBox->lineEdit()->setReadOnly(true); // выравнивание комбо-бокса
    ui->sortBox->lineEdit()->setAlignment(Qt::AlignCenter); // выравнивание комбо-бокса
    for(int i = 0; i < ui->sortBox->count(); i++)   // выравнивание комбо-бокса
        ui->sortBox->setItemData(i, Qt::AlignCenter, Qt::TextAlignmentRole); // выравнивание комбо-бокса

    QPixmap pix(":/folder_orange.png");  // файл не открыт или не создан
    ui->emptyFolder->setPixmap(pix);    // файл не открыт или не создан
    ui->emptyFolder->setStyleSheet("background-color: rgba(220, 245, 224, 255)");   // файл не открыт или не создан

    connect(this, SIGNAL(send_main_ptr(QMainWindow*)), addForm, SLOT(onlyMainPtr(QMainWindow*)));   // обработка кнопок
    connect(addForm, SIGNAL(send_new_product(product*,productInfo*)), this, SLOT(get_new_product(product*,productInfo*)));  // обработка кнопок
    connect(this, SIGNAL(send_ptr_i_for_view(productInfo*)), addForm, SLOT(get_ptr_i_for_view(productInfo*)));  // обработка кнопок
    connect(this, SIGNAL(send_ptr_i_for_change(productInfo*)), addForm, SLOT(get_ptr_i_for_change(productInfo*)));  // обработка кнопок
    connect(addForm, SIGNAL(send_changed_product(product*, productInfo*)), this, SLOT(get_changed_product(product*,productInfo*))); // обработка кнопок
    connect(this, SIGNAL(send_main_ptr_for_find(QMainWindow*)), FindForm, SLOT(getMainPtr(QMainWindow*)));  // обработка кнопок
    connect(FindForm, SIGNAL(send_string_for_find(QString)), this, SLOT(get_string_for_find(QString))); // обработка кнопок
    connect(this, SIGNAL(send_index_for_find(int)), FindForm, SLOT(getIndex(int))); // обработка кнопок
    connect(this, SIGNAL(existing_name()), addForm, SLOT(existing_name())); // обработка кнопок

    fileNotUse();   // работа без файла
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::get_new_product(product* ptr, productInfo* ptr_i)  // создание нового продукта
{
    for (unsigned int i(0); i < listProduct.get_quantity(); i++)    // проверка на наличие такого же названия
    {
        if (listProduct.findID(i + 1)->ptr->get_name() == ptr->get_name())
        {
            emit existing_name();   // если такое название уже есть,повторно вызываем окно ввода
            return;
        }
    }
    listProduct.push(ptr, ptr_i);   // добавление продукта
    listProduct.set_quantity(listProduct.get_quantity() + 1); // обновление количества продуктов
    char tmpptr[15];
    bool tmpcheacker;
    tmpcheacker = _itoa_s(listProduct.get_quantity(), tmpptr, 15, 10);  // преобразовываем число в строку
    if (tmpcheacker == true)
    {
        QMessageBox::critical(this, "Ошибка!", "Ошибка преобразования числа в строку!");
        return;
    }
    ui->listWidget->addItem(tmpptr + (".   " + ptr->get_name()));   // добавление названия нового товара в список

    if (ui->sortBox->currentIndex() == 1)   // сортировка по названию
        nameSort();
    if (ui->sortBox->currentIndex() == 2)   // сортировка по ключевому слову
        keyWordSort();

    StepBack tmp;   // добавление действия "Создание нового продукта" в последние действия
    tmp.set_type(StepBack::typeOp::ADD);    // установка типа действия - добавление
    tmp.set_ptr(ptr);
    tmp.get_ptr()->set_id(listProduct.get_quantity());
    tmp.set_ptr_i(ptr_i);
    if (stepBackBuf.count() <= 6)
        stepBackBuf.push_back(tmp);
    else
    {
        stepBackBuf.pop_front();
        stepBackBuf.push_back(tmp);
    }
    StepBack tmptmp;    // добавление действия сортировки после добавления
    switch (ui->sortBox->currentIndex())
    {
    case 0:
        tmptmp.set_type(StepBack::typeOp::SORTID);  // вид сортировки
        break;
    case 1:
        tmptmp.set_type(StepBack::typeOp::SORTNAME);    // вид сортировки
        break;
    case 2:
        tmptmp.set_type(StepBack::typeOp::SORTKEYWORD); // вид сортировки
        break;
    }
    if (stepBackBuf.count() <= 6)   // проверка на заполненность буфера последних действий
        stepBackBuf.push_back(tmptmp);
    else
    {
        stepBackBuf.pop_front();
        stepBackBuf.push_back(tmptmp);
    }

    fileUse();  // возможность работы с файлом
}

void MainWindow::get_changed_product(product* _ptr, productInfo* _ptr_i)    // изменение продукта
{
    unsigned int tmpID = 0;
    for (int i(0); ((ui->listWidget->currentItem()->text()).toStdString())[i] != '.'; i++ )
        tmpID = tmpID * 10 + ((ui->listWidget->currentItem()->text()).toStdString())[i] - '0';
    for (unsigned int i(0); i < listProduct.get_quantity(); i++)    // проверка на наличие такого же названия
    {
        if (listProduct.findID(i + 1)->ptr->get_name() == _ptr->get_name())
        {
            if (listProduct.findID(tmpID)->ptr->get_name() == _ptr->get_name())
                continue;
            emit existing_name();   // если такое название уже есть,повторно вызываем окно ввода
            return;
        }
    }

    StepBack tmp;   // добавление действия "Изменение продукта" в последние действия
    tmp.set_type(StepBack::typeOp::CHANGE); // установка типа действия - изменение
    tmp.set_ptr(listProduct.findID(tmpID)->ptr);
    tmp.get_ptr()->set_id(tmpID);
    tmp.set_ptr_i(listProduct.findID(tmpID)->ptr_i);
    if (stepBackBuf.count() <= 6)
        stepBackBuf.push_back(tmp);
    else
    {
        stepBackBuf.pop_front();
        stepBackBuf.push_back(tmp);
    }
    StepBack tmptmp;    // добавление действия сортировки после изменения
    switch (ui->sortBox->currentIndex())
    {
    case 0:
        tmptmp.set_type(StepBack::typeOp::SORTID);  // вид сортировки
        break;
    case 1:
        tmptmp.set_type(StepBack::typeOp::SORTNAME);    // вид сортировки
        break;
    case 2:
        tmptmp.set_type(StepBack::typeOp::SORTKEYWORD); // вид сортировки
        break;
    }
    if (stepBackBuf.count() <= 6)   // проверка на заполненность буфера последних действий
        stepBackBuf.push_back(tmptmp);
    else
    {
        stepBackBuf.pop_front();
        stepBackBuf.push_back(tmptmp);
    }

    if (_ptr_i->get_name() != listProduct.findID(tmpID)->ptr->get_name())   // если название было изменено, то обновляем его в списке продуктов
    {
        listProduct.findID(tmpID)->ptr->set_name(_ptr_i->get_name());
        listProduct.findID(tmpID)->ptr_i->set_name(_ptr_i->get_name());
        char tmpptr[15];
        bool tmpcheacker;
        tmpcheacker = _itoa_s(tmpID, tmpptr, 15, 10);
        if (tmpcheacker == true)
        {
            QMessageBox::critical(this, "Ошибка!", "Ошибка преобразования числа в строку!");
            return;
        }
        ui->listWidget->currentItem()->setText(tmpptr + (".   " + _ptr->get_name()));
    }
    listProduct.findID(tmpID)->ptr->set_keyWord(_ptr_i->get_keyWord()); // обновление информации о продукте
    listProduct.findID(tmpID)->ptr_i->set_keyWord(_ptr_i->get_keyWord());   // обновление информации о продукте
    listProduct.findID(tmpID)->ptr_i->set_description(_ptr_i->get_description());   // обновление информации о продукте
    listProduct.findID(tmpID)->ptr_i->set_purchasePrice(_ptr_i->get_purchasePrice());   // обновление информации о продукте
    listProduct.findID(tmpID)->ptr_i->set_sellingPrice(_ptr_i->get_sellingPrice()); // обновление информации о продукте
    listProduct.findID(tmpID)->ptr_i->set_tax(_ptr_i->get_tax());   // обновление информации о продукте
    listProduct.findID(tmpID)->ptr_i->set_shelfLife(_ptr_i->get_shelfLife());   // обновление информации о продукте
    listProduct.findID(tmpID)->ptr_i->set_picProduct(_ptr_i->get_picProduct()); // обновление информации о продукте

    if (ui->sortBox->currentIndex() == 1)   // сортировка по названию
        nameSort();
    if (ui->sortBox->currentIndex() == 2)   // сортировка по ключевому слову
        keyWordSort();
}

void MainWindow::on_newFile_triggered() // обработка нажатия на кнопку "Новый"
{
    if (!listProduct.get_adress().isNull()) // если уже работаем с другим файлом, то предлагаем его закрыть
    {
        on_closeFile_triggered();
    }
    QString tmp = nullptr;
    tmp = QFileDialog::getSaveFileName(this);   // путь для создания папки для хранения
    QDir dirName;
    if (!dirName.mkdir(tmp))    // создание папки
    {
        QMessageBox::critical(this, "Ошибка!", "Ошибка создания каталога!");
        return;
    }
    QFile fout(tmp + "/main.cawop");    // создание файла с основной информацие о продуктах
    if (!fout.open(QIODevice::WriteOnly))
    {
        QMessageBox::critical(this, "Ошибка!", "Ошибка создания файла!");
        return;
    }
    unsigned int file_quantity = 0u;
    fout.write((char*)&file_quantity, sizeof(file_quantity));   // заносим количество продуктов
    fout.flush();
    listProduct.set_quantity(0);
    listProduct.set_adress(tmp);    // добаление адреса нового файла
    filePartiallyUse(); // начало работы с файлом
    fout.close();
}

void MainWindow::on_openFile_triggered()    // обработка нажатия на кнопку "Открыть"
{
    if (!listProduct.get_adress().isNull()) // если уже работаем с другим файлом, то предлагаем его закрыть
    {
        on_closeFile_triggered();
    }
    QString path = QFileDialog::getExistingDirectory(this, tr("Open file"));    // ввод пути к файлу
    if (path.isNull())
    {
        QMessageBox::critical(this, "Ошибка!", "Ошибка открытия файла!");
        return;
    }
    QFile file(path + "/main.cawop");   // открытие файла
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Ошибка!", "Ошибка открытия файла!");
        return;
    }
    unsigned int file_quantity;
    file.read((char*)&file_quantity, sizeof(file_quantity));    // получение количества продуктов в файле
    listProduct.set_adress(path);   // добаление адреса открытого файла
    for (unsigned int i(0); i < file_quantity; i++) // считываем основную информацию о продуктах из файла
    {
        file.seek(10 + i * 80 + i * 30);
        product* productPtr = nullptr;
        MyExceptions<product>::allocation(productPtr, 1, true);
        char tmpText1[80] = {};
        file.read(tmpText1, 80);
        productPtr->set_name(tmpText1);
        file.seek(10 + (i + 1) * 80 + i * 30);
        char tmpText2[30] = {};
        file.read(tmpText2, 30);
        productPtr->set_keyWord(tmpText2);
        listProduct.push(productPtr, nullptr);
        listProduct.set_quantity(listProduct.get_quantity() + 1);
        delete productPtr;
    }
    file.flush();
    file.close();
    for (unsigned int i(0); i < file_quantity; i++) // добавление названий продуктов в список продуктов
    {
        char tmpptr[15];
        bool tmpcheacker;
        tmpcheacker = _itoa_s(i + 1, tmpptr, 15, 10);
        if (tmpcheacker == true)
        {
            QMessageBox::critical(this, "Ошибка!", "Ошибка преобразования числа в строку!");
            return;
        }
        ui->listWidget->addItem(tmpptr + (".   " + listProduct.findID(i + 1)->ptr->get_name()));
    }
    if (file_quantity == 0) // если нет товаров, то файл - новый
        filePartiallyUse();
    else
        fileUse();
}

void MainWindow::on_saveFile_triggered()    // обработка нажатия на кнопку "Сохранить"
{
    QFile fs(listProduct.get_adress() + "/main.cawop"); // открытие файла
    if (!fs.open(QIODevice::ReadWrite))
    {
        QMessageBox::critical(this, "Ошибка!", "Ошибка открытия файла!");
        return;
    }
    unsigned int file_quantity;
    fs.read((char*)&file_quantity, sizeof(file_quantity));  // считываем старое количество продуктов
    if (file_quantity == 0) // если не было продуктов, то файл - новый
    {
        fs.seek(0);
        file_quantity = listProduct.get_quantity();
        fs.write((char*)&file_quantity, sizeof(file_quantity)); // запись нового количества продуктов в файл
        for (unsigned int i(0); i < listProduct.get_quantity(); i++)    // запись информации о продуктах в файл
        {
            fs.seek(10 + i * 80 + i * 30);
            fs.write(listProduct.findID(i + 1)->ptr->get_name().toStdString().c_str(),
            strlen(listProduct.findID(i + 1)->ptr->get_name().toStdString().c_str()));
            fs.seek(10 + (i + 1) * 80 + i * 30);
            fs.write(listProduct.findID(i + 1)->ptr->get_keyWord().toStdString().c_str(),
            strlen(listProduct.findID(i + 1)->ptr->get_keyWord().toStdString().c_str()));
            QDir dirName;
            QString tmpAdress = listProduct.get_adress() + "/" + listProduct.findID(i + 1)->ptr_i->get_name();
            if (!dirName.mkdir(tmpAdress))  // создание каталога под каждый файл
            {
                QMessageBox::critical(this, "Ошибка!", "Ошибка создания каталога!");
                fs.flush();
                fs.close();
                return;
            }
            QFile fout(tmpAdress + "/info.cawopi");
            if (!fout.open(QIODevice::WriteOnly))   // запись всей информации в файл
            {
                QMessageBox::critical(this, "Ошибка!", "Ошибка создания файла!");
                fs.flush();
                fs.close();
                return;
            }
            fout.write(listProduct.findID(i + 1)->ptr_i->get_name().toStdString().c_str(),
            strlen(listProduct.findID(i + 1)->ptr_i->get_name().toStdString().c_str()));
            fout.seek(80);
            fout.write(listProduct.findID(i + 1)->ptr_i->get_keyWord().toStdString().c_str(),
            strlen(listProduct.findID(i + 1)->ptr_i->get_keyWord().toStdString().c_str()));
            fout.seek(110);
            if (!listProduct.findID(i + 1)->ptr_i->get_description().isNull())
                fout.write(listProduct.findID(i + 1)->ptr_i->get_description().toStdString().c_str(),
                strlen(listProduct.findID(i + 1)->ptr_i->get_description().toStdString().c_str()));
            fout.seek(430);
            double tmpValue = listProduct.findID(i + 1)->ptr_i->get_purchasePrice();
            fout.write((char*)&tmpValue, sizeof(tmpValue));
            fout.seek(460);
            tmpValue = listProduct.findID(i + 1)->ptr_i->get_sellingPrice();
            fout.write((char*)&tmpValue, sizeof(tmpValue));
            fout.seek(490);
            if (listProduct.findID(i + 1)->ptr_i->get_tax() != 0.0)
            {
                tmpValue = listProduct.findID(i + 1)->ptr_i->get_tax();
                fout.write((char*)&tmpValue, sizeof(tmpValue));
            }
            fout.seek(515);
            unsigned int _tmpValue_ = listProduct.findID(i + 1)->ptr_i->get_shelfLife();
            fout.write((char*)&_tmpValue_, sizeof(_tmpValue_));
            if (!listProduct.findID(i + 1)->ptr_i->get_picProduct().isNull())
                QFile::copy(listProduct.findID(i + 1)->ptr_i->get_picProduct(), tmpAdress + "/pic.jpg");
            fout.flush();
            fout.close();
        }
    }
    else    // если продукты уже были в файле
    {
        fs.flush();
        fs.close();
        for (unsigned int i(0); i < listProduct.get_quantity(); i++)
            if (listProduct.findID(i + 1)->ptr_i == nullptr)    // проверка на наличие информации об этом товаре
            {   // если всей информации о товаре нет, то считываем её из старого файла
                productInfo** tmp = &(listProduct.findID(i + 1)->ptr_i);
                MyExceptions<productInfo>::allocation(*tmp, 1, true);
                QFile file(listProduct.get_adress() + "/" + listProduct.findID(i + 1)->ptr->get_name() + "/info.cawopi");
                if (!file.open(QIODevice::ReadWrite))
                {
                    QMessageBox::critical(this, "Ошибка!", "Ошибка открытия файла!");
                    return;
                }
                char tmpText1[80] = {};
                file.read(tmpText1, 80);
                (*tmp)->set_name(tmpText1);
                file.seek(80);
                char tmpText2[30] = {};
                file.read(tmpText2, 30);
                (*tmp)->set_keyWord(tmpText2);
                file.seek(110);
                char tmpText3[320] = {};
                file.read(tmpText3, 320);
                (*tmp)->set_description(tmpText3);
                file.seek(430);
                double tmpdouble;
                file.read((char*)&tmpdouble, 8);
                (*tmp)->set_purchasePrice(tmpdouble);
                file.seek(460);
                file.read((char*)&tmpdouble, 8);
                (*tmp)->set_sellingPrice(tmpdouble);
                file.seek(490);
                file.read((char*)&tmpdouble, 8);
                (*tmp)->set_tax(tmpdouble);
                file.seek(515);
                unsigned int tmpint;
                file.read((char*)&tmpint, 4);
                (*tmp)->set_shelfLife(tmpint);
                (*tmp)->set_picProduct(listProduct.get_adress() + "/" + listProduct.findID(i + 1)->ptr->get_name() + "/pic.jpg");
                file.flush();
                file.close();
            }
        QString oldAdress = listProduct.get_adress();   // старый адрес
        QDir newFolder(oldAdress); // новая директория
        newFolder.cdUp();
        if (!newFolder.mkdir("tmp"))    // запись в новый файл
        {
            QMessageBox::critical(this, "Ошибка!", "Ошибка создания каталога!");
            return;
        }
        QFile fout(newFolder.path() + "/tmp" + "/main.cawop");
        if (!fout.open(QIODevice::WriteOnly))
        {
            QMessageBox::critical(this, "Ошибка!", "Ошибка создания файла!");
            return;
        }
        unsigned int file_quantity = 0u;
        fout.write((char*)&file_quantity, sizeof(file_quantity));
        fout.flush();
        fout.close();

        listProduct.set_adress(newFolder.path() + "/tmp");
        on_saveFile_triggered();
        QDir newName(oldAdress);
        QString newNameFolder = newName.dirName();
        QDir toRemove(oldAdress);   // удаление старого файла
        toRemove.removeRecursively();   // удаление старого файла
        newFolder.rename("tmp", newNameFolder); // переименование новго файла
        listProduct.set_adress(newFolder.path() + "/" + newNameFolder);
    }
}

void MainWindow::on_closeFile_triggered()   // обработка нажатия на кнопку "Закрыть"
{
    if (!listProduct.get_adress().isNull()) // желание пользователя сохранить файл
    {
        int result = QMessageBox::question(this, "Сохранение", "Вы желаете сохранить текущий файл?", QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes)
            on_saveFile_triggered();
    }
    ui->listWidget->clear();    // очистка списка названий продуктов
    listProduct.Delete();   // очитска контейнера продуктов
    stepBackBuf.clear();    // очистка буфера последних действий
    fileNotUse();   // работа без файла
}

void MainWindow::fileNotUse()   // режим работы интерфейса без выбранного файла
{
    ui->listWidget->setStyleSheet("background-color: rgba(220, 245, 224, 255);");

    ui->emptyFolder->show();
    ui->thereIsNotAFile->show();

    ui->addPushButton->setEnabled(false);
    ui->changePushButton->setEnabled(false);
    ui->deletePushButton->setEnabled(false);
    ui->sortButton->setEnabled(false);
    ui->aboutButton->setEnabled(false);
    ui->sortBox->setEnabled(false);
    ui->saveFile->setEnabled(false);
    ui->closeFile->setEnabled(false);
    ui->back->setEnabled(false);
    ui->find->setEnabled(false);

    ui->sortBox->setCurrentIndex(0);

    ui->sortLabel->hide();
    ui->addPushButton->hide();
    ui->changePushButton->hide();
    ui->deletePushButton->hide();
    ui->sortButton->hide();
    ui->aboutButton->hide();
    ui->sortBox->hide();
}

void MainWindow::filePartiallyUse() // начальный режим работы интерфейса с файлом
{
    ui->listWidget->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.471, y1:0.625, x2:1, y2:0,"
                                  "stop:0 rgba(207, 219, 209, 255), stop:1 rgba(220, 245, 224, 255));");

    ui->emptyFolder->hide();
    ui->thereIsNotAFile->hide();

    ui->addPushButton->setEnabled(true);
    ui->saveFile->setEnabled(true);
    ui->closeFile->setEnabled(true);
    ui->back->setEnabled(true);

    ui->changePushButton->setEnabled(false);
    ui->deletePushButton->setEnabled(false);
    ui->sortButton->setEnabled(false);
    ui->aboutButton->setEnabled(false);
    ui->sortBox->setEnabled(false);
    ui->find->setEnabled(false);

    ui->sortLabel->show();
    ui->addPushButton->show();
    ui->changePushButton->show();
    ui->deletePushButton->show();
    ui->sortButton->show();
    ui->aboutButton->show();
    ui->sortBox->show();
}

void MainWindow::fileUse()  // режим работы интерфейса с файлом
{
    ui->listWidget->setStyleSheet("background-color: qlineargradient(spread:pad, x1:0.471, y1:0.625, x2:1, y2:0,"
                                  "stop:0 rgba(207, 219, 209, 255), stop:1 rgba(220, 245, 224, 255));");

    ui->emptyFolder->hide();
    ui->thereIsNotAFile->hide();

    ui->addPushButton->setEnabled(true);
    ui->saveFile->setEnabled(true);
    ui->closeFile->setEnabled(true);
    ui->changePushButton->setEnabled(true);
    ui->deletePushButton->setEnabled(true);
    ui->sortButton->setEnabled(true);
    ui->aboutButton->setEnabled(true);
    ui->sortBox->setEnabled(true);
    ui->back->setEnabled(true);
    ui->find->setEnabled(true);

    ui->sortLabel->show();
    ui->addPushButton->show();
    ui->changePushButton->show();
    ui->deletePushButton->show();
    ui->sortButton->show();
    ui->aboutButton->show();
    ui->sortBox->show();
}

void MainWindow::on_addPushButton_clicked() // обработка нажатия на кновку "Добавить"
{
    emit send_main_ptr(this);
    addForm->show();
    this->hide();
}

void MainWindow::on_changePushButton_clicked()  // обработка нажатия на кновку "Изменить"
{
    bool flag = true;
    for (unsigned int i(0); i < listProduct.get_quantity(); i++)    // проверка на выделение продукта
        if (ui->listWidget->isItemSelected(ui->listWidget->item(i)))
            flag = false;
    if (flag)   // если продукт не выбран
    {
        QMessageBox::information(this, "Предупреждение!", "Вы не выбрали элемент!");
        return;
    }
    unsigned int tmpID = 0; // получение номера продукта
    for (int i(0); ((ui->listWidget->currentItem()->text()).toStdString())[i] != '.'; i++ )
        tmpID = tmpID * 10 + ((ui->listWidget->currentItem()->text()).toStdString())[i] - '0';
    if (listProduct.findID(tmpID)->ptr_i == nullptr)    // проверка на наличие информации об этом товаре
    {   // если всей информации нет, то считываем её из файла
        productInfo** tmp = &(listProduct.findID(tmpID)->ptr_i);
        MyExceptions<productInfo>::allocation(*tmp, 1, true);
        QFile file(listProduct.get_adress() + "/" + listProduct.findID(tmpID)->ptr->get_name() + "/info.cawopi");
        if (!file.open(QIODevice::ReadWrite))
        {
            QMessageBox::critical(this, "Ошибка!", "Ошибка открытия файла!");
            return;
        }
        char tmpText1[80] = {};
        file.read(tmpText1, 80);
        (*tmp)->set_name(tmpText1);
        file.seek(80);
        char tmpText2[30] = {};
        file.read(tmpText2, 30);
        (*tmp)->set_keyWord(tmpText2);
        file.seek(110);
        char tmpText3[320] = {};
        file.read(tmpText3, 320);
        (*tmp)->set_description(tmpText3);
        file.seek(430);
        double tmpdouble;
        file.read((char*)&tmpdouble, 8);
        (*tmp)->set_purchasePrice(tmpdouble);
        file.seek(460);
        file.read((char*)&tmpdouble, 8);
        (*tmp)->set_sellingPrice(tmpdouble);
        file.seek(490);
        file.read((char*)&tmpdouble, 8);
        (*tmp)->set_tax(tmpdouble);
        file.seek(515);
        unsigned int tmpint;
        file.read((char*)&tmpint, 4);
        (*tmp)->set_shelfLife(tmpint);
        (*tmp)->set_picProduct(listProduct.get_adress() + "/" + listProduct.findID(tmpID)->ptr->get_name() + "/pic.jpg");
        file.flush();
        file.close();
    }
    emit send_main_ptr(this);
    emit send_ptr_i_for_change(listProduct.findID(tmpID)->ptr_i);
    addForm->show();    // вызываем окно ввода/вывода информации о продукте
    this->hide();   // скрываем текущее окно
}

void MainWindow::on_deletePushButton_clicked()  // обработка нажатия на кновку "Удалить"
{
    bool flag = true;
    for (unsigned int i(0); i < listProduct.get_quantity(); i++)    // проверка на выделение продукта
        if (ui->listWidget->isItemSelected(ui->listWidget->item(i)))
            flag = false;
    if (flag)
    {
        QMessageBox::information(this, "Предупреждение!", "Вы не выбрали элемент!");
        return;
    }
    unsigned int tmpID = 0; // получение номера продукта
    for (int i(0); ((ui->listWidget->currentItem()->text()).toStdString())[i] != '.'; i++ )
        tmpID = tmpID * 10 + ((ui->listWidget->currentItem()->text()).toStdString())[i] - '0';

    StepBack tmp;   // добавление действия "Удаление продукта" в последние действия
    tmp.set_type(StepBack::typeOp::POP);    // установка типа действия - удаление
    tmp.set_ptr(listProduct.findID(tmpID)->ptr);
    tmp.get_ptr()->set_id(tmpID);
    tmp.set_ptr_i(listProduct.findID(tmpID)->ptr_i);
    if (stepBackBuf.count() <= 6)
        stepBackBuf.push_back(tmp);
    else
    {
        stepBackBuf.pop_front();
        stepBackBuf.push_back(tmp);
    }
    StepBack tmptmp;    // добавление действия сортировки после удаления
    switch (ui->sortBox->currentIndex())
    {
    case 0:
        tmptmp.set_type(StepBack::typeOp::SORTID);  // вид сортировки
        break;
    case 1:
        tmptmp.set_type(StepBack::typeOp::SORTNAME);    // вид сортировки
        break;
    case 2:
        tmptmp.set_type(StepBack::typeOp::SORTKEYWORD); // вид сортировки
        break;
    }
    if (stepBackBuf.count() <= 6)   // проверка на заполненность буфера последних действий
        stepBackBuf.push_back(tmptmp);
    else
    {
        stepBackBuf.pop_front();
        stepBackBuf.push_back(tmptmp);
    }

    if (listProduct.pop(tmpID)) // удаление продукта из контейнера
    {
        unsigned int tmpValueID = 1;
        while (listProduct.findID(tmpID + 1)) // уменьшение индекса в последующих элементах, если они есть
        {
            listProduct.findID(tmpID++ + 1)->id--;
        }
        ui->listWidget->clear();
        while (tmpValueID < tmpID)  // обновление названий продуктов в списке продуктов
        {
            char tmpptr[15];
            bool tmpcheacker;
            tmpcheacker = _itoa_s(tmpValueID, tmpptr, 15, 10);
            if (tmpcheacker == true)
            {
                QMessageBox::critical(this, "Ошибка!", "Ошибка преобразования числа в строку!");
                return;
            }
            ui->listWidget->addItem(tmpptr + (".   " + listProduct.findID(tmpValueID++)->ptr->get_name()));
        }
        if (listProduct.get_quantity() == 0u)   // если был удалён последний продукт, то ограничиваем интерфейс
            filePartiallyUse();
        else
            if (ui->sortBox->currentIndex() == 1)   // сортировка по названию
                nameSort();
            else
                if (ui->sortBox->currentIndex() == 2)   // сортировка по ключевому слову
                    keyWordSort();
    }
}

void MainWindow::on_aboutButton_clicked()   // обработка нажатия на кновку "Подробнее"
{
    bool flag = true;
    for (unsigned int i(0); i < listProduct.get_quantity(); i++)    // проверка на выделение продукта
        if (ui->listWidget->isItemSelected(ui->listWidget->item(i)))
            flag = false;
    if (flag)
    {
        QMessageBox::information(this, "Предупреждение!", "Вы не выбрали элемент!");
        return;
    }
    unsigned int tmpID = 0; // получение номера продукта
    for (int i(0); ((ui->listWidget->currentItem()->text()).toStdString())[i] != '.'; i++ )
        tmpID = tmpID * 10 + ((ui->listWidget->currentItem()->text()).toStdString())[i] - '0';
    if (listProduct.findID(tmpID)->ptr_i == nullptr)    // проверка на наличие информации об этом товаре
    {   // если основной информации нет, то считываем её из файла
        productInfo** tmp = &(listProduct.findID(tmpID)->ptr_i);
        MyExceptions<productInfo>::allocation(*tmp, 1, true);
        QFile file(listProduct.get_adress() + "/" + listProduct.findID(tmpID)->ptr->get_name() + "/info.cawopi");
        if (!file.open(QIODevice::ReadWrite))
        {
            QMessageBox::critical(this, "Ошибка!", "Ошибка открытия файла!");
            return;
        }
        char tmpText1[80] = {};
        file.read(tmpText1, 80);
        (*tmp)->set_name(tmpText1);
        file.seek(80);
        char tmpText2[30] = {};
        file.read(tmpText2, 30);
        (*tmp)->set_keyWord(tmpText2);
        file.seek(110);
        char tmpText3[320] = {};
        file.read(tmpText3, 320);
        (*tmp)->set_description(tmpText3);
        file.seek(430);
        double tmpdouble;
        file.read((char*)&tmpdouble, 8);
        (*tmp)->set_purchasePrice(tmpdouble);
        file.seek(460);
        file.read((char*)&tmpdouble, 8);
        (*tmp)->set_sellingPrice(tmpdouble);
        file.seek(490);
        file.read((char*)&tmpdouble, 8);
        (*tmp)->set_tax(tmpdouble);
        file.seek(515);
        unsigned int tmpint;
        file.read((char*)&tmpint, 4);
        (*tmp)->set_shelfLife(tmpint);
        (*tmp)->set_picProduct(listProduct.get_adress() + "/" + listProduct.findID(tmpID)->ptr->get_name() + "/pic.jpg");
        file.flush();
        file.close();
    }
    emit send_main_ptr(this);
    emit send_ptr_i_for_view(listProduct.findID(tmpID)->ptr_i);
    addForm->show();    // вызываем окно вывода информации о продукте
    this->hide();   // скрываем текущее окно
}

void MainWindow::on_sortButton_clicked()    // обработка нажатия на кновку "Отсортировать"
{
    int index = ui->sortBox->currentIndex();    // получение индекса сортировки
    StepBack tmp;
    switch (index)
    {
    case 0:
        idSort();   // сортировка по времени добавления
        tmp.set_type(StepBack::typeOp::SORTID); // установка вида сортировки
        break;
    case 1:
        nameSort(); // сортировка по названию продукта
        tmp.set_type(StepBack::typeOp::SORTNAME);   // установка вида сортировки
        break;
    case 2:
        keyWordSort();  // сортировка по ключевому слову
        tmp.set_type(StepBack::typeOp::SORTKEYWORD);    // установка вида сортировки
        break;
    }
    if (stepBackBuf.count() <= 6)   // проверка на заполненность буфера последних действий
        stepBackBuf.push_back(tmp);
    else
    {
        stepBackBuf.pop_front();
        stepBackBuf.push_back(tmp);
    }
}

void MainWindow::idSort()   // сортировка по времени добавления
{
    ui->listWidget->clear();
    unsigned int tmpID = listProduct.get_quantity();
    unsigned int tmpValueID = 1;
    while (tmpValueID <= tmpID)
    {
        char tmpptr[15];
        bool tmpcheacker;
        tmpcheacker = _itoa_s(tmpValueID, tmpptr, 15, 10);
        if (tmpcheacker == true)
        {
            QMessageBox::critical(this, "Ошибка!", "Ошибка преобразования числа в строку!");
            return;
        }
        ui->listWidget->addItem(tmpptr + (".   " + listProduct.findID(tmpValueID++)->ptr->get_name()));
    }
}

void MainWindow::nameSort() // сортировка по названию продуктов
{
    ui->listWidget->clear();
    unsigned int tmpID = listProduct.get_quantity();
    unsigned int tmpValueID = 0;
    QList<unsigned int> listOfID;
    product forSort;
    while (tmpValueID < tmpID)
    {
        int i = 1;
        do
        {
            forSort.set_name(listProduct.findID(i)->ptr->get_name());
            forSort.set_id(i);
        } while (listOfID.contains(i++));
        for (unsigned int i(0); i < tmpID; i++)
        {
            if (forSort.get_name() > listProduct.findID(i + 1)->ptr->get_name() && !listOfID.contains(i + 1))
            {
                forSort.set_name(listProduct.findID(i + 1)->ptr->get_name());
                forSort.set_id(i + 1);
            }
        }
        listOfID.append(forSort.get_id());
        char tmpptr[15];
        bool tmpcheacker;
        tmpcheacker = _itoa_s(forSort.get_id(), tmpptr, 15, 10);
        if (tmpcheacker == true)
        {
            QMessageBox::critical(this, "Ошибка!", "Ошибка преобразования числа в строку!");
            return;
        }
        ui->listWidget->addItem(tmpptr + (".   " + forSort.get_name()));
        tmpValueID++;
    }
}

void MainWindow::keyWordSort()  // сортировка по ключевому слову продуктов
{
    ui->listWidget->clear();
    unsigned int tmpID = listProduct.get_quantity();
    unsigned int tmpValueID = 0;
    QList<unsigned int> listOfID;
    product forSort;
    while (tmpValueID < tmpID)
    {
        int i = 1;
        do
        {
            forSort.set_name(listProduct.findID(i)->ptr->get_name());
            forSort.set_keyWord(listProduct.findID(i)->ptr->get_keyWord());
            forSort.set_id(i);
        } while (listOfID.contains(i++));
        for (unsigned int i(0); i < tmpID; i++)
        {
            if (forSort.get_keyWord() > listProduct.findID(i + 1)->ptr->get_keyWord() && !listOfID.contains(i + 1))
            {
                forSort.set_name(listProduct.findID(i + 1)->ptr->get_name());
                forSort.set_keyWord(listProduct.findID(i + 1)->ptr->get_keyWord());
                forSort.set_id(i + 1);
            }
        }
        listOfID.append(forSort.get_id());
        char tmpptr[15];
        bool tmpcheacker;
        tmpcheacker = _itoa_s(forSort.get_id(), tmpptr, 15, 10);
        if (tmpcheacker == true)
        {
            QMessageBox::critical(this, "Ошибка!", "Ошибка преобразования числа в строку!");
            return;
        }
        ui->listWidget->addItem(tmpptr + (".   " + forSort.get_name()));
        tmpValueID++;
    }
}

void MainWindow::on_author_triggered()  // обработка нажатия на кновку "Автор"
{
    QMessageBox::about(this, "Автор программы", "Программу разработал студент группы 850502:\n"
                      "Домбровский Тимур Андреевич");
}

void MainWindow::on_find_triggered()    // обработка нажатия на кновку "Найти"
{
    send_main_ptr_for_find(this);
    send_index_for_find(ui->sortBox->currentIndex());
    FindForm->show();   // вызываем окно ввода строки для поиска
    this->setEnabled(false);    // текущее окно недоступно для ввода
}

void MainWindow::get_string_for_find(QString str)   // получение строки для поиска
{
    StepBack tmp;
    int index = ui->sortBox->currentIndex();    // получение индекса сортировки
    unsigned int count = listProduct.get_quantity();
    switch (index)
    {
    case 0:
        tmp.set_type(StepBack::typeOp::FINDID); // установка вида сортировки
        tmp.set_findString(str);
        ui->listWidget->clear();
        idSort();
        for (unsigned int i(0); i < count; i++) // поиск среди времени добавления продуктов
            if (!ui->listWidget->item(i)->text().contains(str))
            {
                delete ui->listWidget->item(i--);
                count--;
            }
        break;
    case 1:
        tmp.set_type(StepBack::typeOp::FINDNAME);   // установка вида сортировки
        tmp.set_findString(str);
        ui->listWidget->clear();
        nameSort();
        for (unsigned int i(0); i < count; i++) // поиск среди названия продуктов
            if (!ui->listWidget->item(i)->text().contains(str))
            {
                delete ui->listWidget->item(i--);
                count--;
            }
        break;
    case 2:
        tmp.set_type(StepBack::typeOp::FINDKEYWORD);    // установка вида сортировки
        tmp.set_findString(str);
        ui->listWidget->clear();
        keyWordSort();
        for (unsigned int i(0); i < count; i++) // поиск среди ключевого слова продуктов
        {
            unsigned int tmpID = 0;
            for (int j(0); ((ui->listWidget->item(i)->text()).toStdString())[j] != '.'; j++ )
                tmpID = tmpID * 10 + ((ui->listWidget->item(i)->text()).toStdString())[j] - '0';
            if (!listProduct.findID(tmpID)->ptr->get_keyWord().contains(str))
            {
                delete ui->listWidget->item(i--);
                count--;
            }
        }
        break;
    }
    if (stepBackBuf.count() <= 6)   // проверка на заполнение буфера последних действий
        stepBackBuf.push_back(tmp);
    else
    {
        stepBackBuf.pop_front();
        stepBackBuf.push_back(tmp);
    }
}

void MainWindow::on_back_triggered()    // обработка нажатия на кновку "Назад"
{
    if (stepBackBuf.isEmpty())  // если буфер пуст
        return;
    StepBack tmp;
    unsigned int tmpValueID = 1;
    tmp = stepBackBuf.last();
    if (stepBackBuf.count() != 1    // проверка на тип последнего действия
        && tmp.get_type() != StepBack::typeOp::ADD
        && tmp.get_type() != StepBack::typeOp::CHANGE
        && tmp.get_type() != StepBack::typeOp::POP)
    {
        stepBackBuf.pop_back();
        tmp = stepBackBuf.last();
    }
    switch (tmp.get_type())
    {
    case StepBack::typeOp::ADD :    // если последнее действие - добавление
        listProduct.pop(tmp.get_ptr()->get_id());   // удаление продукта
        for (unsigned int i(tmp.get_ptr()->get_id()); i < listProduct.get_quantity(); i++)  // уменьшение индексов последующих элементов
            listProduct.findID(i)->id = listProduct.findID(i)->id - 1;
        switch (ui->sortBox->currentIndex())
        {
        case 0:
            idSort();   // тип сортировки
            break;
        case 1:
            nameSort(); // тип сортировки
            break;
        case 2:
            keyWordSort();  // тип сортировки
            break;
        }
        if (listProduct.get_quantity() == 0)    // если продуктов не осталось
            filePartiallyUse();
        break;
    case StepBack::typeOp::CHANGE : // если последнее действие - изменение
        listProduct.pop(tmp.get_ptr()->get_id());   // удаление нового продукта
        tmpValueID = tmp.get_ptr()->get_id();
        while (listProduct.findID(tmpValueID + 1)) // уменьшение индекса в последующих элементах, если они есть
        {
            listProduct.findID(tmpValueID++ + 1)->id--;
        }
        listProduct.insert(tmp.get_ptr(), tmp.get_ptr_i(), tmp.get_ptr()->get_id());    // вставка старого продукта
        switch (ui->sortBox->currentIndex())
        {
        case 0:
            idSort();   // тип сортировки
            break;
        case 1:
            nameSort(); // тип сортировки
            break;
        case 2:
            keyWordSort();  // тип сортировки
            break;
        }
        break;
    case StepBack::typeOp::POP :    // если последнее действие - удаление
        listProduct.insert(tmp.get_ptr(), tmp.get_ptr_i(), tmp.get_ptr()->get_id());    // вставка удаленного продукта
        switch (ui->sortBox->currentIndex())
        {
        case 0:
            idSort();   // тип сортировки
            break;
        case 1:
            nameSort(); // тип сортировки
            break;
        case 2:
            keyWordSort();  // тип сортировки
            break;
        }
        if (listProduct.get_quantity() != 0)    // если если продукты ещё остались
            fileUse();
        break;
    case StepBack::typeOp::SORTID : // если последнее действие - сортировка по времени добавления
        idSort();   // сортировка по времени добавления
        ui->sortBox->setCurrentIndex(0);
        break;
    case StepBack::typeOp::SORTNAME :   // если последнее действие - сортировка по названию
        nameSort(); // сортировка по названию
        ui->sortBox->setCurrentIndex(1);
        break;
    case StepBack::typeOp::SORTKEYWORD :    // если последнее действие - сортировка по ключевому слову
        keyWordSort();  // сортировка по ключевому слову
        ui->sortBox->setCurrentIndex(2);
        break;
    case StepBack::typeOp::FINDID : // если последнее действие - поиск по времени добавления
        ui->listWidget->clear();
        idSort();
        for (unsigned int i(0), count = listProduct.get_quantity(); i < count; i++)
            if (!ui->listWidget->item(i)->text().contains(tmp.get_findString()))
            {
                delete ui->listWidget->item(i--);
                count--;
            }
        break;
    case StepBack::typeOp::FINDNAME :   // если последнее действие - поиск по названию
        ui->listWidget->clear();
        nameSort();
        for (unsigned int i(0), count = listProduct.get_quantity(); i < count; i++)
            if (!ui->listWidget->item(i)->text().contains(tmp.get_findString()))
            {
                delete ui->listWidget->item(i--);
                count--;
            }
        break;
    case StepBack::typeOp::FINDKEYWORD :    // если последнее действие - поиск по ключевому слову
        ui->listWidget->clear();
        keyWordSort();
        for (unsigned int i(0), count = listProduct.get_quantity(); i < count; i++)
        {
            unsigned int tmpID = 0;
            for (int j(0); ((ui->listWidget->item(i)->text()).toStdString())[j] != '.'; j++ )
                tmpID = tmpID * 10 + ((ui->listWidget->item(i)->text()).toStdString())[j] - '0';
            if (!listProduct.findID(tmpID)->ptr->get_keyWord().contains(tmp.get_findString()))
            {
                delete ui->listWidget->item(i--);
                count--;
            }
        }
        break;
    }
    if (stepBackBuf.count() != 1)
        stepBackBuf.pop_back();
    tmp = stepBackBuf.last();
    if (tmp.get_type() != StepBack::typeOp::ADD &&  // проверка на предыдущее действие
        tmp.get_type() != StepBack::typeOp::CHANGE &&
        tmp.get_type() != StepBack::typeOp::POP)
    {
        tmp.set_type(StepBack::typeOp::SORTID);
        delete tmp.get_ptr();
        tmp.set_ptr(nullptr);
        delete tmp.get_ptr_i();
        tmp.set_ptr_i(nullptr);
        stepBackBuf.push_back(tmp);
    }
}
