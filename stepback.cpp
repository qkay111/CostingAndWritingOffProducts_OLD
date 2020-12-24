#include "myexceptions.h"
#include "stepback.h"

StepBack::StepBack()
{
    type = typeOp::ADD;
    ptr = ptr_i = nullptr;
}

StepBack::StepBack(typeOp t, product* p, productInfo* p_i, QString str)
    : type(t)
    , findString(str)
{
    if (p != nullptr)
    {
        MyExceptions<product>::allocation(ptr, 1, true);
        *ptr = *p;
    }
    if (p_i != nullptr)
    {
        MyExceptions<productInfo>::allocation(ptr_i, 1, true);
        *ptr_i = *p_i;
    }
}

StepBack::StepBack(const StepBack& obj) // конструктор копирования
{
    type = obj.get_type();
    findString = obj.get_findString();
    if (obj.get_ptr() != nullptr)
    {
        MyExceptions<product>::allocation(ptr, 1, true);
        *ptr = *(obj.get_ptr());
    }
    else
        ptr = nullptr;
    if (obj.get_ptr_i() != nullptr)
    {
        MyExceptions<productInfo>::allocation(ptr_i, 1, true);
        *ptr_i = *(obj.get_ptr_i());
    }
    else
        ptr_i = nullptr;
}

StepBack::~StepBack()
{
    delete ptr;
    delete ptr_i;
}

product* StepBack::get_ptr() const
{
    return ptr;
}

productInfo* StepBack::get_ptr_i() const
{
    return ptr_i;
}

QString StepBack::get_findString() const
{
    return findString;
}

StepBack::typeOp StepBack::get_type() const
{
    return type;
}

void StepBack::set_ptr(product* p)
{
    if (p != nullptr)
    {
        MyExceptions<product>::allocation(ptr, 1, true);
        *ptr = *p;
    }
    else
        ptr = p;
}

void StepBack::set_ptr_i(productInfo* p_i)
{
    if (p_i != nullptr)
    {
        MyExceptions<productInfo>::allocation(ptr_i, 1, true);
        *ptr_i = *p_i;
    }
    else
        ptr_i = p_i;
}

void StepBack::set_findString(QString str)
{
    findString = str;
}

void StepBack::set_type(typeOp var)
{
    type = var;
}

StepBack& StepBack::operator = (const StepBack& obj)
{
    type = obj.get_type();
    findString = obj.get_findString();
    if (obj.get_ptr() != nullptr)
    {
        if (ptr != nullptr)
            delete ptr;
        MyExceptions<product>::allocation(ptr, 1, true);
        *ptr = *(obj.get_ptr());
    }
    else
        ptr = nullptr;
    if (obj.get_ptr_i() != nullptr)
    {
        if (ptr_i != nullptr)
            delete ptr_i;
        MyExceptions<productInfo>::allocation(ptr_i, 1, true);
        *ptr_i = *(obj.get_ptr_i());
    }
    else
        ptr_i = nullptr;
    return *this;
}
