#include "productinfo.h"

productInfo::productInfo()
    : description()
    , shelfLife(0)
    , purchasePrice(0.0)
    , sellingPrice(0.0)
    , tax(0.0)
    , product()
{
}

productInfo::~productInfo()
{
    shelfLife = purchasePrice = sellingPrice = tax = 0;
    if (!description.isNull())
        description.clear();
}

QString productInfo::get_description()
{
    return description;
}

unsigned int productInfo::get_shelfLife()
{
    return shelfLife;
}

double productInfo::get_purchasePrice()
{
    return purchasePrice;
}

double productInfo::get_sellingPrice()
{
    return sellingPrice;
}

double productInfo::get_tax()
{
    return tax;
}

QString productInfo::get_picProduct()
{
    return picProduct;
}

void productInfo::set_description(const QString var)
{
    description = var;
}

void productInfo::set_shelfLife(const unsigned int var)
{
    shelfLife = var;
}

void productInfo::set_purchasePrice(const double var)
{
    purchasePrice = var;
}

void productInfo::set_sellingPrice(const double var)
{
    sellingPrice = var;
}

void productInfo::set_tax(const double var)
{
    tax = var;
}

void productInfo::set_picProduct(const QString var)
{
    picProduct = var;
}
