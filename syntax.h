#ifndef SYNTAX_H
#define SYNTAX_H
#include <stack>
#include <vector>
#include <iostream>
#include <clocale>
#include <fstream>
#include <string> //подключение библиотек
using namespace std;
class ASTnode
{
    int Type;
    string Text;
    ASTnode *parent;
    vector <ASTnode*> childs;
public:
    ASTnode(int type, string text, ASTnode *child1, ASTnode *child2);
    void printtext();
    int AddChild(ASTnode *child);
    void settext(string qwe);
    int curpar();
    ASTnode *retchild(int);
    string childtext(int);
    void setparent(ASTnode *qwe);
    void settype(int qwe);
    int rettype();
    string rettext();
    int RemoveChild(ASTnode *child);
    void ASTremove(ASTnode *node);
    void printnode(ASTnode *node);
    int childscount();
};
class Syntax
{
    ASTnode *root;
    fstream code;
    ofstream mod;
    string word, t, nam, page, currentproc;
    struct tableid
    {
        string var, type, proc, zn;
        int mas, kol;
    }rty;
    struct forindex
    {
        string jk1, jk2;
    }jok;
    vector <forindex> fi;
    vector <tableid> TI;
    int flag;
    int realvar;
    int metka;
    int varnum;
    int joknum;
    int
    UNKNOWN,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    STR,
    ASSIGN,
    KEYWORD,
    SEPARATOR,
    IDENT,
    CONSTANT,
    PROGRAMM,
    PROCEDURE,
    ERROR,
    NONTERMINAL,
    OR,
    AND,
    EQ,
    NE,
    NOT,
    SRAV,
    OPCIKLE,
    OPIF,
    OPELSE,
    OPELSIF,
    TERMINAL,
    fparam;
public:
    Syntax();
    ~Syntax();
    void scancode();
    string idstringtype(string);
    void while_cikle(ASTnode *);
    int GSTPS(string);
    string HelpStr(string);
    int TIcheck(string);
    int Synanalys();
    void nesquik(ASTnode *);
    void ifcode(ASTnode *);
    void ProvCikl(ASTnode *);
    void printjok();
    int ischislo(string);
    //int typeof(string);
    ASTnode* START();
    ASTnode* MODULE();
    ASTnode* NAME();
    ASTnode *O();
    ASTnode *P();
    void setindex(ASTnode *);
    string getindex(string);
    void printdata();
    ASTnode *PO();
    void opproc(ASTnode*);
    ASTnode *IFELSE();
    ASTnode *CIKL();
    ASTnode *OO();
    ASTnode *A();
    void printid();
    int check(string w);
    ASTnode *BOOL();
    ASTnode *JOIN();
    ASTnode *EQUA();
    ASTnode *REL();
    ASTnode *ARIFM();
    ASTnode *term();
    ASTnode *unary();
    ASTnode *factor();
    ASTnode *offset(string qwe);
    string sidtype(string w);
    int idtype(string w);
    void FACTPAR(ASTnode *);
    ASTnode *F();
    void prisv(ASTnode*);
    ASTnode *FORMALPAR();
    ASTnode *C();
    ASTnode *C1();
    ASTnode* CHISLO();
    ASTnode* V();
    ASTnode* TYPE();
    int GenerCode();
};
#endif
