#include "lexic.h"
#include <fstream>
#include <string> //подключение библиотек
#include <iostream>
#include <vector>
#include <stdlib.h>
using namespace std;


Lexic::Lexic()
{
    //cin_code.open("input.mod");
    cin_mainwords.open("mainwords.txt");
    out_code.open("temp.txt");
    variables = NULL;
    operations = NULL;
    constants = NULL;
    numstroke=1;
    keywords = NULL; //обнуление указателей на начала списков
    flagL=flagN=flagO=flagS=false; //установка начальных состояний автомата
    int k;
    char q; //символ типа операции
    cin_mainwords >> k >> q;
    string s;
    for (int i = 0; i < k; i++) //чтение ключевых слов из файла и помещение в список
    {
        cin_mainwords >> s;
        add1(keywords, s, i);
    }
    //print2(keywords, "k");
    cin_mainwords >> k;
    for (int i = 0; i < k; i++) //чтение операций из файла и помещение в список
    {
        int n;
        cin_mainwords >> n >> q;
        for (int j = 0; j < n; j++) //чтение операций типа ch
        {
            cin_mainwords >> s;
            add2(operations, s, q, j);
        }
    }
}
void Lexic::openfile(char *s)
{
    cin_code.open(s);
}
int Lexic::Lexanalys()
{
    while (!endfile()) //пока не достигнут конец файла
    {
      automat(getsym()/*читаем символ*/); //запускаем автомат
    }
    automat(' '); //запускаем автомат с пробельным символом для обработки содержимого магазина на последнем шаге
    printLex(); //вывод списков
    closefiles();
    return 1;
}
void Lexic::closefiles()
{
    cin_code.close();
    cin_mainwords.close();
    out_code.close();
}
int Lexic::add0(list1 *&head, string w) //добавление в список 1 типа при неизвестном номере
{
    list1 *temp = new list1; //создание нового элемента
    temp -> next = NULL;
    temp -> word = w; //присвоение новому элементу идентификатора w
    if (head) //если список не пуст
    {
        list1 *t;
        for (t=head; t -> next != NULL; t = t -> next);
        temp -> num = t -> num + 1;
        t -> next = temp; //добавление в хвост очереди
    }
    else
    {
        temp -> num = 0;
        head = temp;
    }
    return temp -> num; //возвращаем номер добавленного элемента
}

void Lexic::add1(list1 *&head, string w, int n) //добавление в очередь 1 типа при известном номере
{
    list1 *temp = new list1; //аналогично add0
    temp -> word = w;
    temp -> next = NULL;
    temp -> num = n;
    if (head)
    {
        list1 *t;
        for (t=head; t -> next != NULL; t = t -> next);
        t -> next = temp;
    }
    else
    {
        head = temp;
    }
}

void Lexic::add2(list2 *&head, string w, char id, int n) //добавление в список 2 типа
{
    list2 *temp = new list2;
    temp -> next = NULL;
    temp -> id = id;
    temp -> word = w;
    temp -> num = n;
    if (head)
    {
        list2 *t;
        for (t=head; t -> next != NULL; t = t -> next);
        t -> next = temp;
    }
    else
    {
        head = temp;
    }
}

bool Lexic::sym() //является ли символ «символом»
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == '_')) ? true : false;
}

bool Lexic::num() //является ли символ цифрой
{
    return (ch >= '0' && ch <= '9') ? true : false;
}

bool Lexic::oper() //является ли символ знаком операции
{
    return (ch =='"' || ch==':' || ch== ',' || ch=='.' || ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '>' || ch == '<' || ch == '&' || ch == '|' || ch == '=' || ch == ';' || /*ch == '(' || ch == ')' ||*/ ch == '{' || ch == '}') ? true : false;
}

bool Lexic::separ() //является ли символ разделителем
{
    return (ch == '"' || ch=='[' || ch==']' || ch== ',' || ch=='.' ||  ch == ' ' || ch == '\n' || ch == ')' || ch == '(' || ch == ';' || ch == '}' || ch == '{' || (!flagO && mp != "" && oper())) ? true : false;
}

void Lexic::mpclear() //очистка магазина
{
    mp = "";
}

void Lexic::InsertWord() //добавление ключевого слова или переменной
{
    //char ch1;
    string w;
    list1 *temp;
    for (temp=keywords; temp!=NULL; temp = temp -> next) //поиск в списке констант
    {
        if (temp -> word == mp)
        {
            w = "k";
            /*ch1 = '0' + temp -> num;
            w += ch1;*/
            w += " " + temp -> word + " " + numtostr(numstroke);
            Lexems.push_back(w);
            return;
        }
    }
    for (temp=variables; temp!=NULL; temp = temp -> next) //поиск в списке переменных
    {
        if (temp -> word == mp)
        {
            w = "j";
            w += " " + temp -> word + " " + numtostr(numstroke);
            Lexems.push_back(w);
            return;
        }
    }
    //int nnum = add0(variables, mp); //добавление новой переменной
    w = "j";
    w += " " + mp + " " + numtostr(numstroke);
    Lexems.push_back(w);
    //cout << w << endl;
}

void Lexic::InsertNumber() //добавление числовой константы
{
    //char ch1;
    string w;
    list1 *temp;
    for (temp=constants; temp!=NULL; temp = temp -> next) //поиск в списке констант
        if (temp -> word == mp)
        {
            w = "c";
            w += " " + temp -> word + " " + numtostr(numstroke);
            Lexems.push_back(w);
            return;
        }

    //int nnum = add0(constants, mp); //добавление новой константы
    w = "c";
    w += " " + mp + " " + numtostr(numstroke);
    Lexems.push_back(w);
}

void Lexic::InsertOper() //добавление операции
{
    //char ch1;
    string w;
    list2 *temp;
    for (temp=operations; temp != NULL; temp = temp -> next)
//поиск в списке операций
        if (temp -> word == mp)
        {
            w = temp -> id;
            w += " " + temp -> word + " " + numtostr(numstroke);
            Lexems.push_back(w);
            return;
        }
}

string Lexic::numtostr(int num)
{
    char buf[5];
    string temp="";
    itoa(num, buf, 10);
    for(int i=0 ; i<5 && buf[i]!='\0'; i++) temp+=buf[i];
    //temp+="\0";
    return temp;
}
void Lexic::Insert() //добавление токена в зависимости от его типа
{
    string w="";
    if (flagS) //если токен – ключевое слово или имя переменной
        InsertWord();

    if (flagN) //если токен – числовая константа
        InsertNumber();

    if (flagO) //если токен - операция
        InsertOper();

    if(flagL)
    {
        w = "l";
        w += " " + mp + " " + numtostr(numstroke);
        Lexems.push_back(w);
    }
    mpclear(); //очистка магазина
}

void Lexic::Error() //обработчик ошибок
{
    Lexems.push_back("Error"); //вывод сообщения об ошибке
    while (!separ() && cin_code.peek()!=EOF) //пропуск символов до следующего разделителя
        cin_code >> ch;
    mpclear();
}

void Lexic::automat(char get) //функция, реализующая действие автомата
{
    string w;
    static char last=' ';
    ch=get;
    int h=0;
    static int q=0;
    if(q==1)
    {
        numstroke++;
        q=0;
    }
    if(ch=='\n')
    {
        q=1;
    }
    if(ch=='"')
    {
        //Insert();
        ch=getsym();
        mp="\"";
        while(ch!='"')
        {
            mp+=ch;
            ch=getsym();
        }
        mp+=ch;
        flagL=true;
        flagS = flagN = flagO = false;
        Insert();
        flagL=false;
        mp=")";
        flagO=true;
        Insert();
        return;
    }
    if (mp.length() == 0) //если магазин пуст
    {
        last=ch;
        if (!(ch == ' ' || ch == '\n')) //если символ не пробел и не маркер конца строки
        {
            flagS = sym();
            flagN = num();
            flagO = oper(); //определяем тип символа
            mp += ch; //добавляем символ в магазин
            return;
        }
        else return;
    }
    if(ch=='*' && last=='(')
    {
        while(h!=1)
        {
            do
            {
                ch=getsym();
            }while(ch!='*');
            if(getsym()!=')') continue;
            else h=1;
        }
        h=0;
        return;
    }
    if (!separ() || (flagN && ch=='.')) //если символ - не разделитель
    {
        if ((sym() || num()) && flagS)
//если накапливаем «слово» и текущий символ – «символ» или цифра
            mp += ch;
        else if ((num() || ch=='.') && flagN)
            mp += ch;
//если накапливаем число и текущий символ - цифра или точка
        else if (oper() && flagO)
//если накапливаем операцию и текущий символ – символ операции
            mp += ch;
        else if (flagN && sym())
//если накапливаем число, а текущий символ – «символ», то ошибка
            Error();
        else
        {
            if(mp=="[")
            {
                w="i ";
                w += mp + " " + numtostr(numstroke);
                Lexems.push_back(w);
            }
            Insert();
            flagS = sym();
            flagN = num();
            flagO = oper();
            mp += ch; //добавляем символ в магазин
        }
        last = ch;
        return;
    }
    if (separ())
    {
        if (oper()) //если разделитель – символ операции
        {
            Insert();
            flagS = sym();
            flagN = num();
            flagO = oper();
            mp = ch;
        }
        else
        {
            Insert();
            if(ch=='(' || ch==')')
            {
                w="s ";
                w += ch;
                w+= " " + numtostr(numstroke);
                Lexems.push_back(w);
            }
            else if(ch=='[' || ch==']')
            {
                w="i ";
                w += ch;
                w+= " " + numtostr(numstroke);
                Lexems.push_back(w);
            }
        }
        last = ch;
    }
}

void Lexic::printLex()
{
    for(int i=0 ; i<(int)Lexems.size() ; i++)
        out_code << Lexems[i] << endl;
}
bool Lexic::endfile()
{
    return cin_code.peek()==EOF ? true : false;
}

char Lexic::getsym()
{
    char q;
    q=cin_code.get();
    return q;
}
