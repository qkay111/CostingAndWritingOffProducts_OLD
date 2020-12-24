#ifndef PRODUCT_H
#define PRODUCT_H

#include <QString>

class product
{
private:
    unsigned int id;    // id продукта
    QString name;   // название продукта
    QString keyWord;    // ключевое слово
public:
    product();
    ~product();
    unsigned int get_id();
    QString get_name();
    QString get_keyWord();
    void set_id(const unsigned int);
    void set_name(const QString);
    void set_keyWord(const QString);
};

#endif // PRODUCT_H
