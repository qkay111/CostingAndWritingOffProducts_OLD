#include "product.h"

product::product()
    : id(0)
    , name()
    , keyWord()
{
}

product::~product()
{
    id = 0;
    if (!name.isNull())
        name.clear();
    if (!keyWord.isNull())
        keyWord.clear();
}

unsigned int product::get_id()
{
    return id;
}

QString product::get_name()
{
    return name;
}

QString product::get_keyWord()
{
    return keyWord;
}

void product::set_id(const unsigned int var)
{
    id = var;
}

void product::set_name(const QString var)
{
    name = var;
}

void product::set_keyWord(const QString var)
{
    keyWord = var;
}
