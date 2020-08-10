#ifndef LEXIC_H
#define LEXIC_H
#include <fstream>
#include <string> //подключение библиотек
#include <iostream>
#include <vector>
using namespace std;
class Lexic
{
    ifstream cin_code; //файл с исходной программой
    ifstream cin_mainwords; //файл с ключевыми словами исходного языка
    ofstream out_code;
    vector <string> Lexems; //массив лексем
    string mp; //автомат
    bool flagS, flagN, flagO, flagL; //флаги накопления слова, числа и оператора соответственно
    char ch; //входящий символ
    struct list1 //список ключевых слов и т.д.
    {
        string word; //идентификатор
        int num; //номер в списке
        list1 *next; //ссылка на следующий
    } *variables, *constants, *keywords; // списки переменных, констант и ключевых слов

    struct list2 //список операций
    {
        string word; //идентификатор
        char id; //символ типа операции
        int num; //номер операции среди операций того же типа
        list2 *next; //ссылка на следующий элемент
    } *operations; //список операций
    int numstroke;
public:
    Lexic();
    int add0(list1 *&head, string w); //добавление в список 1 типа при неизвестном номере
    void add1(list1 *&head, string w, int n); //добавление в очередь 1 типа при известном номере
    void add2(list2 *&head, string w, char id, int n); //добавление в список 2 типа
    bool sym(); //является ли символ «символом»
    string numtostr(int);
    void openfile(char *);
    bool num(); //является ли символ цифрой
    bool oper(); //является ли символ знаком операции
    bool separ(); //является ли символ разделителем
    void mpclear(); //очистка магазина
    void InsertWord(); //добавление ключевого слова или переменной
    void InsertNumber(); //добавление числовой константы
    void InsertOper(); //добавление операции
    void Insert(); //добавление токена в зависимости от его типа
    void Error(); //обработчик ошибок
    void automat(char get); //функция, реализующая действие автомата
    void printLex();
    bool endfile();
    char getsym();
    int Lexanalys();
    void closefiles();
};


#endif
