#include <iostream>
#include "lexic.h"
#include "syntax.h"
using namespace std;

int main(int argc, char* argv[])
{
    Lexic L;
    Syntax S;
    int a;
    setlocale(0, "rus");
    cout << " акую программу компилировать(1 или 2)?\n";
    cin >> a;
    if (a == 1)// если передаем аргументы, то argc будет больше 1(в зависимости от кол-ва аргументов)
    {
        L.openfile("input.mod");// вывод второй строки из массива указателей на строки(нумераци€ в строках начинаетс€ с 0 )
    }
    else if(a==2)
    {
        L.openfile("input2.mod");
    }
    if(L.Lexanalys())
    {
        if(S.Synanalys())
        {
            if(S.GenerCode())
            {
                cout << " омпил€ци€ завершена!!!" << endl;
            }
        }
    }
    return 0;
}
