#ifndef ACCOUNTING_H
#define ACCOUNTING_H

#include "myexceptions.h"
#include <QString>

template <typename type1, typename type2>
class accounting
{
public:
    struct node // узел контейнера
    {
        type1* ptr;
        type2* ptr_i;
        node* next;
        node* back;
        unsigned int id;
    };

    class accountingIterator    // итератор для контейнера
    {
    private:
        node* ptr;  // указатель на node
    public:
        accountingIterator()
        {
            ptr = nullptr;
        }
        node* get_ptr()
        {
            return ptr;
        }
        void set_ptr(node* var)
        {
            ptr = var;
        }
        accountingIterator& operator ++ (int)
        {
            if (ptr != nullptr)
                ptr = ptr->next;
            return *this;
        }
        accountingIterator& operator -- (int)
        {
            if (ptr != nullptr && ptr->back != nullptr)
                ptr = ptr->back;
            return *this;
        }
        const type2& operator * ()
        {
            return ptr->ptr_i;
        }
        node* operator -> ()
        {
            return ptr;
        }
        bool operator == (accountingIterator& var)
        {
            if (ptr == var.ptr)
                return true;
            return false;
        }
        bool operator != (accountingIterator& var)
        {
            if (ptr != var.ptr)
                return true;
            return false;
        }
    };

    accounting();
    ~accounting();
    unsigned int get_quantity()
    {
        return quantity;
    }
    QString get_adress()
    {
        return adress;
    }
    void set_quantity(const unsigned int var)
    {
        quantity = var;
    }
    void set_adress(const QString var)
    {
        adress = var;
    }
    bool push(type1*, type2* = nullptr);    // добавление узла
    bool pop(unsigned int); // удаление узла
    bool insert(type1*, type2*, unsigned int);  // вствка по индексу
    node* findID(unsigned int); // поиск по ID
    bool Delete();  // очистка контейнера

private:
    unsigned int quantity;  // количество элементов
    QString adress; // адрес для работы с файловой системой
    node *head; // указатель на node
    accountingIterator begin, end;  // итераторы
public:
    accountingIterator& get_begin() { return begin; }
    accountingIterator& get_end() { return end; }
};

template <typename type1, typename type2>
accounting<type1, type2>::accounting()
    : quantity(0)
    , adress()
    , head(nullptr)
{
    end.set_ptr(nullptr);   // установка значения для end итератора
}

template <typename type1, typename type2>
accounting<type1, type2>::~accounting()
{
    quantity = 0;
    if (!adress.isNull())
        adress.clear();
    Delete();
}

template <typename type1, typename type2>
bool accounting<type1, type2>::push(type1* ptr1, type2* ptr2)
{
    if (head == nullptr)    // если нет узлов
    {
        MyExceptions<node>::allocation(head, 1, true);  // выделяем память
        head->ptr = nullptr;
        head->ptr_i = nullptr;
        head->next = nullptr;
        head->back = nullptr;
        head->id = quantity + 1;
        begin.set_ptr(head); // устанавливаем положение для begin итератора
        if (ptr1 != nullptr)
        {
            MyExceptions<type1>::allocation(head->ptr, 1, true);
            *(head->ptr) = *ptr1;
        }
        else
            head->ptr = nullptr;
        if (ptr2 != nullptr)
        {
            MyExceptions<type2>::allocation(head->ptr_i, 1, true);
            *(head->ptr_i) = *ptr2;
        }
        else
            head->ptr_i = nullptr;
        return true;
    }
    node* tmp = head;
    while (tmp->next != nullptr)    // переходим на последний элемент
        tmp = tmp->next;
    tmp->next = new node { nullptr, nullptr, nullptr, tmp, quantity + 1 };  // выделяем память
    if (ptr1 != nullptr)
    {
        MyExceptions<type1>::allocation(tmp->next->ptr, 1, true);
        *(tmp->next->ptr) = *ptr1;
    }
    else
        tmp->next->ptr = nullptr;
    if (ptr2 != nullptr)
    {
        MyExceptions<type2>::allocation(tmp->next->ptr_i, 1, true);
        *(tmp->next->ptr_i) = *ptr2;
    }
    else
        tmp->next->ptr_i = nullptr;
    end.set_ptr(tmp->next);
    return true;
}

template <typename type1, typename type2>
bool accounting<type1, type2>::insert(type1* ptr, type2* ptr_i, unsigned int place)
{
    if (place > quantity)   // проверка на превышение количества элементов в контейнере
        place = quantity + 1;
    push(nullptr);
    quantity++; // увеличение количества элементов
    for (unsigned int i(quantity), j = i - 1; i > place; i--, j--)  // сдвиг всех элементов на одну позицию
    {
        findID(i)->ptr = findID(j)->ptr;
        findID(i)->ptr_i = findID(j)->ptr_i;
    }
    if (ptr != nullptr) // очищаем старое значение
    {
        MyExceptions<type1>::allocation(findID(place)->ptr, 1, true);
        *(findID(place)->ptr) = *ptr;
    }
    else
        findID(place)->ptr = nullptr;
    if (ptr_i != nullptr)   // очищаем старое значение
    {
        MyExceptions<type2>::allocation(findID(place)->ptr_i, 1, true);
        *(findID(place)->ptr_i) = *ptr_i;
    }
    return true;
}

template <typename type1, typename type2>
typename accounting<type1, type2>::node* accounting<type1, type2>::findID(unsigned int var)
{
    node* tmp = head;
    while (tmp != nullptr)  // пока не найдём нужный нам элемент
    {
        if (tmp->id == var)
            return tmp;
        tmp = tmp->next;
    }
    return nullptr;
}

template <typename type1, typename type2>
bool accounting<type1, type2>::pop(unsigned int var)
{
    node* tmpptr = head;
    while (tmpptr != nullptr)   // пока не прошли все узлы контейнера
    {
        if (tmpptr->id == var)  // если нужный нам узел
        {
            if (tmpptr == head) // если удаляемый элемент = head
            {
                head = head->next;
                if (head != nullptr)
                    head->back = nullptr;
                begin.set_ptr(head);    // обновляем позицию begin итератора
            }
            delete tmpptr->ptr; // очищаем память
            delete tmpptr->ptr_i;   // очищаем память
            if (tmpptr->next != nullptr)
                tmpptr->next->back = tmpptr->back;
            if (tmpptr->back != nullptr)
                tmpptr->back->next = tmpptr->next;
            delete tmpptr;  // удаляем узел
            tmpptr = nullptr;
            quantity--; // уменьшаем количество
            return true;
        }
        tmpptr = tmpptr->next; // переходим к следующему элементу
    }
    return false;
}

template <typename type1, typename type2>
bool accounting<type1, type2>::Delete()
{
    while (head != nullptr) // пока не прошли все узлы
    {
        node* tmp = head;
        head = head->next;
        delete tmp; // удаляем узел
    }
    quantity = 0;   // обнуляем количество
    adress.clear(); // сбрасываем путь к файлу
    begin.set_ptr(nullptr); // обнуляем итератор
    return true;
}

#endif // ACCOUNTING_H
