#ifndef MYEXCEPTIONS_H
#define MYEXCEPTIONS_H

#include <iostream>

using namespace std;

template <typename T>
class MyExceptions
{
public:
    static bool allocation(T*& ptr, unsigned int quantity, bool mode)   // обработка выделения памяти
    {
        if (quantity == 0)  // если надо выделить память под 0 элементов
        {
            ptr = nullptr;
            return false;
        }
        if (mode)   // выбор режима
        {
            if (quantity == 1)  // если выделить память под один элемент
                try
                {
                    ptr = new T;
                }
                catch (const bad_alloc& obj)    // проверка на выделение памяти
                {
                    obj.what();
                    abort();
                }
            else    // если выделить память под несколько элементов
                try
                {
                    ptr = new T[quantity];
                }
                catch (const bad_alloc& obj)    // проверка на выделение памяти
                {
                    obj.what();
                    abort();
                }
        }
        else
        {
            if (quantity == 1)  // если выделить память под один элемент
                try
                {
                    ptr = new T;
                }
                catch (const bad_alloc& obj)    // проверка на выделение памяти
                {
                    obj.what();
                    abort();
                    return false;
                }
            else    // если выделить память под несколько элементов
                try
                {
                    ptr = new T[quantity];
                }
                catch (const bad_alloc& obj)    // проверка на выделение памяти
                {
                    obj.what();
                    abort();
                    return false;
                }
        }
        return true;
    }
};

#endif // MYEXCEPTIONS_H
