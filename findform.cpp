#include "findform.h"
#include "ui_findform.h"

findForm::findForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::findForm)
{
    ui->setupUi(this);
    mainWindow = nullptr;
}

findForm::~findForm()
{
    delete ui;
}

void findForm::getMainPtr(QMainWindow *ptr)
{
    mainWindow = ptr;
}

void findForm::getIndex(int var)
{
    QRegExp var_is_null("[0-9]{1,30}"); // установка валидации
    QRegExp var_is_one("[A-Za-zА-Яа-я ]{1,30}");    // установка валидации
    QRegExp var_is_two("[A-Za-zА-Яа-я]{1,10}"); // установка валидации
    switch (var)
    {
    case 0:
        ui->lineEdit->setValidator(new QRegExpValidator(var_is_null, this)); // установка валидации
        break;
    case 1:
        ui->lineEdit->setValidator(new QRegExpValidator(var_is_one, this)); // установка валидации
        break;
    case 2:
        ui->lineEdit->setValidator(new QRegExpValidator(var_is_two, this)); // установка валидации
        break;
    }
}

void findForm::on_pushButton_clicked()
{
    if (ui->lineEdit->text().count() == 0)  // если пользователь ничего не ввёл
    {
        ui->lineEdit->clear();
        mainWindow->setEnabled(true);
        this->hide();
        return;
    }
    send_string_for_find(ui->lineEdit->text());
    ui->lineEdit->clear();
    mainWindow->setEnabled(true);
    this->hide();
}
