#ifndef STEPBACK_H
#define STEPBACK_H

#include "product.h"
#include "productinfo.h"
#include <QString>

class StepBack
{
public:
    enum class typeOp{ ADD, CHANGE, POP, SORTID, SORTNAME,              // тип последнего действия
                       SORTKEYWORD, FINDID, FINDNAME, FINDKEYWORD };
    StepBack();
    StepBack(typeOp, product* = nullptr, productInfo* = nullptr, QString = "");
    StepBack(const StepBack&);
    ~StepBack();
    product* get_ptr() const;
    productInfo* get_ptr_i() const;
    QString get_findString() const;
    typeOp get_type() const;
    void set_ptr(product* = nullptr);
    void set_ptr_i(productInfo* = nullptr);
    void set_findString(QString = "");
    void set_type(typeOp);
    StepBack& operator = (const StepBack&);
private:
    product* ptr;   // основная информация о продукте
    productInfo* ptr_i; // вся информация о продукте
    QString findString; // строка поиска
    typeOp type;    // тип последнего действия
};

#endif // STEPBACK_H
