#include <fstream>
#include <string> //подключение библиотек

using namespace std;

ifstream cin_code ("input.txt"); //файл с исходной программой
ifstream cin_key ("key.txt"); //файл с ключевыми словами исходного языка
ofstream cout_lexem ("lexem.txt"); //файл для выходного потока лексем
ofstream cout_exp ("explain.txt"); //файл для соответствий обозначений
ofstream cout_lex ("inp.txt"); //входной файл для синтаксического анализатора
ofstream cout_res ("res.txt"); //файл с переменными и константами для синт. анализ.

const int Lex_Max = 1000; //константа – максимальное число токенов

string mg; //магазин автомата
string lexem[Lex_Max]; //массив токенов
int num_lex; //количество токенов

bool flagS, flagN, flagO; //флаги накопления слова, числа и оператора соответственно

char ch;

struct queue1 //очередь 1 типа
{
    string word; //идентификатор
    int num; //номер в списке
    queue1 *next; //ссылка на следующий
} *list_v, *list_c, *list_k; // списки переменных, констант и ключевых слов

struct queue2 //очерель 2 типа
{
    string word; //идентификатор
    char id; //символ типа операции
    int num; //номер операции среди операций того же типа
    queue2 *next; //ссылка на следующий элемент
} *list_o; //список операций

int push0(queue1 *&head, string w)
//добавление в очередь 1 типа при неизвестном номере
{
    queue1 *temp = new queue1; //создание нового элемента
    temp -> next = NULL;
    temp -> word = w; //присвоение новому элементу идентификатора w

    if (head) //если список не пуст
    {
        queue1 *t;
        for (t = head; t -> next != NULL; t = t -> next);
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

void push1(queue1 *&head, string w, int n)
//добавление в очередь 1 типа при известном номере
{
    queue1 *temp = new queue1; //аналогично push0
    temp -> word = w;
    temp -> next = NULL;
    temp -> num = n;
    if (head)
    {
        queue1 *t;
        for (t = head; t -> next != NULL; t = t -> next);
        t -> next = temp;
    }
    else
    {
        head = temp;
    }
}

void push2(queue2 *&head, string w, char id, int n)
//добавление в очередь 2 типа
{
    queue2 *temp = new queue2;
    temp -> next = NULL;
    temp -> id = id;
    temp -> word = w;
    temp -> num = n;
    if (head)
    {
        queue2 *t;
        for (t = head; t -> next != NULL; t = t -> next);
        t -> next = temp;
    }
    else
    {
        head = temp;
    }
}

void init() //инициализация
{
    list_v = NULL;
    list_c = NULL;
    list_k = NULL;
    list_o = NULL; //обнуление указателей на начала списков
    int k;
    char ch;
    cin_key >> k >> ch;
    string s;
    for (int i = 0; i < k; i++) //чтение ключевых слов из файла и помещение в список
    {
        cin_key >> s;
        push1(list_k, s, i);
    }
    cin_key >> k;
    for (int i = 0; i < k; i++) //чтение операций из файла и помещение в список
    {
        int n;
        cin_key >> n >> ch;
        for (int j = 0; j < n; j++) //чтение операций типа ch
        {
            cin_key >> s;
            push2(list_o, s, ch, j);
        }
    }

}

bool Is_Symb(char ch) //является ли символ «символом»
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || (ch == '_')) ? true : false;
}

bool Is_Num(char ch) //является ли символ цифрой
{
    return (ch >= '0' && ch <= '9') ? true : false;
}

bool Is_Oper(char ch) //является ли символ знаком операции
{
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '>' || ch == '<' || ch == '&' || ch == '|' || ch == '=' || ch == ';' || ch == '(' || ch == ')' || ch == '{' || ch == '}') ? true : false;
}

bool Is_Separ(char ch) //является ли символ разделителем
{
    return (ch == ' ' || ch == '\n' || ch == ')' || ch == '(' || ch == ';' || ch == '}' || ch == '{' || (!flagO && mg != "" && Is_Oper(ch))) ? true : false;
}

void Clear_mg(string &mg) //очистка магазина
{
    mg = "";
}

void InsertWord(string mg) //добавление ключевого слова или переменной
{
    bool flag = false;
    for (queue1 *temp = list_k; temp; temp = temp -> next) //поиск в списке констант
        if (temp -> word == mg)
        {
            flag = true;
            char ch1 = '0' + temp -> num;
            lexem[num_lex] = "k";
            lexem[num_lex] += ch1;
            lexem[num_lex] += "   [" + temp -> word + "]";
            num_lex++;
            return;
        }
    for (queue1 *temp = list_v; temp; temp = temp -> next) //поиск в списке переменных
        if (temp -> word == mg)
        {
            flag = true;
            lexem[num_lex] = "j";
            char ch1 = '0' + temp -> num;
            lexem[num_lex] += ch1;
            lexem[num_lex] += "   [" + temp -> word + "]";
            num_lex++;
            return;
        }

    int nnum = push0(list_v, mg); //добавление новой переменной
    lexem[num_lex] = "j";
    char ch1 = '0' + nnum;
    lexem[num_lex] += ch1;
    lexem[num_lex] += "   [" + mg + "]";
    num_lex++;
}

void InsertNumber(string mg) //добавление числовой константы
{
    bool flag = false;
    for (queue1 *temp = list_c; temp; temp = temp -> next) //поиск в списке констант
        if (temp -> word == mg)
        {
            flag = true;
            lexem[num_lex] = "c";
            char ch1 = '0' + temp -> num;
            lexem[num_lex] += ch1;
            lexem[num_lex] += "   [" + temp -> word + "]";
            num_lex++;
            return;
        }

    int nnum = push0(list_c, mg); //добавление новой константы
    lexem[num_lex] = "c";
    char ch1 = '0' + nnum;
    lexem[num_lex] += ch1;
    lexem[num_lex] += "   [" + mg + "]";
    num_lex++;
}

void InsertOper(string mg) //добавление операции
{
    for (queue2 *temp = list_o; temp != NULL; temp = temp -> next)
//поиск в списке операций
        if (temp -> word == mg)
        {
            lexem[num_lex] = temp -> id;
            char ch1 = '0' + temp -> num;
            lexem[num_lex] += ch1;
            lexem[num_lex] += "   [" + temp -> word + "]";
            num_lex++;
            return;
        }
}


void Insert(string &mg, char ch) //добавление токена в зависимости от его типа
{
    if (flagS) //если токен – ключевое слово или имя переменной
        InsertWord(mg);

    if (flagN) //если токен – числовая константа
        InsertNumber(mg);

    if (flagO) //если токен - операция
        InsertOper(mg);

    Clear_mg(mg); //очистка магазина
}

void Error(char &s) //обработчик ошибок
{
    lexem[num_lex++] = "Error";
//вывод сообщения об ошибке
    while (!Is_Separ(ch) && cin_code.peek()!=EOF)
//пропуск символов до следующего разделителя
        cin_code >> ch;
        Clear_mg(mg);
}

void automat(string &mg, char ch) //функция, реализующая действие автомата
{
    if (mg.length() == 0) //если магазин не пуст
    {
        if (!(ch == ' ' || ch == '\n')) //если символ не пробел и не маркер конца строки
        {
            flagS = Is_Symb(ch);
            flagN = Is_Num(ch);
            flagO = Is_Oper(ch); //определяем тип символа
            mg += ch; //добавляем символ в магазин
            return;
        }
        else
            return;
    }

    if (!Is_Separ(ch)) //если символ - разделитель
    {
        if ((Is_Symb(ch) || Is_Num(ch)) && flagS)
//если накапливаем «слово» и текущий символ – «символ» или цифра
            mg += ch;
        else
            if (Is_Num(ch) && flagN)
//если накапливаем число и текущий символ - цифра
                mg += ch;
            else
                if (Is_Oper(ch) && flagO)
//если накапливаем операцию и текущий символ – символ операции
                    mg += ch;
                else
                    if (flagN && Is_Symb(ch))
//если накапливаем число, а текущий символ – «символ», то ошибка
                        Error(ch);
                    else
                    {
                        Insert(mg, ch);
                        flagS = Is_Symb(ch);
                        flagN = Is_Num(ch);
                        flagO = Is_Oper(ch);
                        mg += ch; //добавляем символ в магазин
                    }
        return;
    }
    if (Is_Separ(ch))
    {
        if (Is_Oper(ch)) //если разделитель – символ операции
        {
            Insert(mg, ' ');
            flagS = Is_Symb(ch);
            flagN = Is_Num(ch);
            flagO = Is_Oper(ch);
            mg = ch;
        }
        else
            Insert(mg, ch);
    }
}

void print2(queue1 *head, string s) //вывод элементов очереди типа 1
{
    for (head; head; head = head -> next)
        cout_exp << head -> word << "   " << s << head -> num << endl;
}

void print3(queue2 *head) //вывод элементов очереди типа 2
{
    for (head; head; head = head -> next)
        cout_exp << head -> word << "   " << head -> id << head -> num << endl;
}

void print(queue1 *list_k, queue2 *list_o, queue1 *list_v, queue1 *list_c)
{
//вывод списков переменных, констант, ключевых слов и операций
    cout_exp << "Переменные" << endl << endl;
    print2(list_v, "j");
    cout_exp << endl << "Константы" << endl << endl;
    print2(list_c, "c");
    cout_exp << endl << "Ключевые слова" << endl << endl;
    print2(list_k, "k");
    cout_exp << endl << "Операции" << endl << endl;
    print3(list_o);
}

int main()
{
    init(); //инициализация
    while (cin_code.peek()!=EOF) //пока не достигнут конец файла
    {
        cin_code.get(ch); //читаем символ
        automat(mg, ch); //запускаем автомат
    }
    automat(mg, ' ');
//запускаем автомат с пробельным символом для обработки содержимого магазина на последнем //шаге
    print(list_k, list_o, list_v, list_c); //вывод списков

    for(int i = 0; i < num_lex; i++)
        cout_lexem << lexem[i] << endl;

    cout_lex << num_lex << "\n";
    for(int i = 0; i < num_lex; i++)
//создание входного файла для синтаксического анализатора
        cout_lex << lexem[i][0] << lexem[i][1] << endl;

    cout_res << push0(list_v, "") << endl;
//создание файла с перечнем переменных и констант для синт. анализ.
    for (queue1 *t = list_v; t -> next; t = t -> next)
        cout_res << t -> word << endl;
    cout_res << endl << push0(list_c, "") << endl;
    for (queue1 *t = list_c; t -> next; t = t -> next)
        cout_res << t -> word << endl;
    return 0;
}
