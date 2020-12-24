#ifndef PRODUCTINFO_H
#define PRODUCTINFO_H

#include "product.h"
#include <QString>

class productInfo : public product
{
private:
    QString description;    // описание продукта
    unsigned int shelfLife; // срок годности
    double purchasePrice;   // цена закупки
    double sellingPrice;    // цена продажи
    double tax; // налог на продукт
    QString picProduct; // изображение продукта
public:
    productInfo();
    ~productInfo();
    QString get_description();
    unsigned int get_shelfLife();
    double get_purchasePrice();
    double get_sellingPrice();
    double get_tax();
    QString get_picProduct();
    void set_description(const QString);
    void set_shelfLife(const unsigned int);
    void set_purchasePrice(const double);
    void set_sellingPrice(const double);
    void set_tax(const double);
    void set_picProduct(const QString);
};

#endif // PRODUCTINFO_H
