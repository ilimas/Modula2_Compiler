#include "syntax.h"
#include <stack>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <clocale>
#include <fstream>
#include <string> //подключение библиотек
using namespace std;
ASTnode::ASTnode(int type, string text, ASTnode *child1, ASTnode *child2)
{
    parent = NULL;
    Type = type;
    Text = text;
    if(child1 != NULL) AddChild(child1);
    if(child2 != NULL) AddChild(child2);
}
void ASTnode::printtext()
{
    cout << this->Text;
}
ASTnode* ASTnode::retchild(int i)
{
    return this->childs[i];
}
string ASTnode::rettext()
{
    return this->Text;
}
string ASTnode::childtext(int i)
{
    return this->childs[i]->Text;
}
int ASTnode::AddChild(ASTnode *child)
{
    if(child==NULL) return 0;
    childs.push_back(child);
    child->parent = this;
    return 1;
}
void ASTnode::settext(string qwe)
{
    Text=qwe;
}
int ASTnode::curpar()
{
    if(this->parent==NULL) return 0;
    else return 1;
}
int ASTnode::rettype()
{
    return this->Type;
}
void ASTnode::setparent(ASTnode *qwe)
{
    parent=qwe;
}
void ASTnode::settype(int qwe)
{
    Type=qwe;
}
int ASTnode::RemoveChild(ASTnode *child)
{
    if (child->parent == this) child->parent = NULL;
    return 2;
}
void ASTnode::ASTremove(ASTnode *node)
{
    if(node==NULL) return;
    for(int i=0 ; i<(int)node->childs.size() ; i++)
    {
        ASTremove(node->childs[i]);
    }
    node->childs.clear();
    delete node;
}
void ASTnode::printnode(ASTnode *node)
{
    static int g=0;
    if(node==NULL) return;
    cout << node->Text <<endl;
    for(int i=0 ; i<(int)node->childs.size() ; i++)
    {
        for(int j=0 ; j<g ; j++) cout << " ";
        cout << "|";
        g++;
        printnode(node->childs[i]);
    }
    g--;
}
int ASTnode::childscount()
{
    return this->childs.size();
}


Syntax::Syntax()
{
    mod.open("gencode.asm");
    varnum=1;
    ERROR = 404;
    flag=0;
    NONTERMINAL = 101;
    UNKNOWN = 0;
    KEYWORD=1;
    CONSTANT = 2;
    IDENT = 3;
    ADD=11;
    SUB=12;
    MUL = 13;
    DIV = 14;
    MOD = 15;
    ASSIGN = 16;
    STR=71;
    SEPARATOR = 52;
    OR = 17;
    AND=18;
    EQ = 19;
    NE=20;
    NOT=21;
    PROCEDURE=200;
    SRAV=22;
    joknum=0;
    realvar=0;
    OPCIKLE=30;
    OPIF=31;
    OPELSE=32;
    OPELSIF=33;
    TERMINAL=100;
    root = NULL;
    fparam=0;
    metka=0;
    code.open("temp.txt", ios::in);
    currentproc="glob";
}
Syntax::~Syntax()
{
    mod.close();
    code.close();
    root->ASTremove(root);
}
void Syntax::printdata()
{
    mod << ".386" << endl;
    mod << ".model flat,stdcall" << endl;
    mod << "include \\masm32\\include\\masm32rt.inc" << endl;
    mod << "include \\masm32\\include\\msvcrt.inc" << endl;
    mod << "include \\masm32\\include\\kernel32.inc" << endl;

    mod << "includelib \\masm32\\lib\\masm32.lib" << endl;
    mod << "includelib \\masm32\\lib\\msvcrt.lib" << endl;
    mod << "includelib \\masm32\\lib\\kernel32.lib" << endl;
    mod << ".data" << endl;
    mod << "prnt_scan_real DB \"%f\",0" << endl;
    mod << "OutputFormat db \"%i\",0" << endl;
    for(int i =0 ; i<(int)TI.size() ; i++)
    {
        if(TI[i].proc!="glob" && TI[i].type!="PROCEDURE")
        {
            mod << "proc_" << TI[i].proc << " " << TI[i].var << " DD " << TI[i].zn << endl;
        }
        else if(TI[i].proc=="glob" && TI[i].type!="PROCEDURE")
        {
            if(TI[i].type=="string")
            {
                mod << "var_" << TI[i].kol << " DB " << TI[i].var << ",0" << endl;
            }
            else
            {
                switch(idtype(TI[i].var))
                {
                case 4:
                    mod << TI[i].var << " DD " << TI[i].zn << endl;
                    break;
                }
            }
        }
    }
    mod << ".code" << endl;
    for(int i=0 ; i<root->retchild(1)->childscount() ; i++)
    {
        if(root->retchild(1)->retchild(i)->rettext()=="PROCEDURE") nesquik(root->retchild(1)->retchild(i)->retchild(0));
    }
    mod << "start:" << endl;
    mod << "FINIT" << endl;
}
void Syntax::printjok()
{
    for(int i=0 ; i<(int)fi.size() ; i++)
    {
        if(ischislo(fi[i].jk1)==0)
        {
            mod << "MOV " << fi[i].jk2 << "," << fi[i].jk1 << endl;
        }
    }
}
void Syntax::ProvCikl(ASTnode *current)
{
    for(int j=0 ; j<current->childscount() ; j++)
    {
        if(current->retchild(j)->rettext()=="WHILE")
        {
            while_cikle(current->retchild(j));
        }
        else if(current->retchild(j)->rettext()=="IF")
        {
            ifcode(current->retchild(j));
        }
        else if(current->retchild(j)->rettext()!=":=")
        {
            opproc(current->retchild(j));
        }
        else
        {
            if(current->retchild(j)->retchild(1)->childscount()==0)
            {
                mod << "MOV eax," << current->retchild(j)->retchild(1)->rettext() << endl;
                mod << "MOV " << current->retchild(j)->retchild(0)->rettext() << ",eax" << endl;
            }
            else
            {
                setindex(current->retchild(j)->retchild(1));
                printjok();
                prisv(current->retchild(j));
                fi.clear();
                joknum=0;
                realvar=0;
            }
        }
    }
}
int Syntax::GenerCode()
{
    ASTnode *current=NULL;
    int i=root->childscount();
    printdata();
    for(int j=2 ; j<i-1 ; j++)
    {
        if(root->childtext(j)=="NACHINKA")
        {
            current=root->retchild(j);
            ProvCikl(current);
        }
    }
    mod << "invoke crt__getch" << endl;
    mod << "invoke ExitProcess,0" << endl;
    mod << "end start" << endl;
    mod << "END" << endl;
    return 1;
}
void Syntax::nesquik(ASTnode *temp)
{
    mod <<  temp->rettext() <<  " proc " << temp->retchild(0)->retchild(0)->rettext() << ":DWORD, " << temp->retchild(0)->retchild(1)->rettext() << ":DWORD" << endl;
    ProvCikl(temp->retchild(1));
    mod << "ret\n" <<  temp->rettext() << " ENDP" << endl;
}
void Syntax::while_cikle(ASTnode *temp)
{
    if(temp->retchild(0)->rettext()=="<")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "L" << metka << ": CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JGE " << "L" << metka+1 << endl;

    }
    else if(temp->retchild(0)->rettext()==">")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "L" << metka << ": CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JLE " << "L" << metka+1 << endl;
    }
    else if(temp->retchild(0)->rettext()=="<=")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "L" << metka << ": CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JG " << "L" << metka+1 << endl;
    }
    else if(temp->retchild(0)->rettext()==">=")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "L" << metka << ": CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JL " << "L" << metka+1 << endl;
    }
    else if(temp->retchild(0)->rettext()=="=")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "L" << metka << ": CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JNE " << "L" << metka+1 << endl;
    }
    else if(temp->retchild(0)->rettext()=="<>")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "L" << metka << ": CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JE " << "L" << metka+1 << endl;
    }
    ProvCikl(temp->retchild(1));
    mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
    mod << "JMP L" << metka << endl;
    metka++;
    mod << "L" << metka << ":" << endl;
}
void Syntax::ifcode(ASTnode *temp)
{
    if(temp->retchild(0)->rettext()=="<")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JGE " << "L" << metka << endl;

    }
    else if(temp->retchild(0)->rettext()==">")
    {
       mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JLE " << "L" << metka << endl;
    }
    else if(temp->retchild(0)->rettext()=="<=")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JG " << "L" << metka << endl;
    }
    else if(temp->retchild(0)->rettext()==">=")
    {
       mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JL " << "L" << metka << endl;
    }
    else if(temp->retchild(0)->rettext()=="=")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JNE " << "L" << metka << endl;
    }
    else if(temp->retchild(0)->rettext()=="<>")
    {
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "CMP edx," << temp->retchild(0)->retchild(1)->rettext()<< endl;
        mod << "JE " << "L" << metka << endl;
    }
    if(temp->childscount()>2 && temp->retchild(2)->rettext()=="ELSE")
    {
        ProvCikl(temp->retchild(1));
        mod << "JMP L" << metka+1 << endl;
        mod << "L" << metka << ":" << endl;
        ProvCikl(temp->retchild(3));
        mod << "L" << ++metka << ":" << endl;
    }
    else
    {
        ProvCikl(temp->retchild(1));
        mod << "MOV edx," << temp->retchild(0)->retchild(0)->rettext() << endl;
        mod << "L" << metka << ":" << endl;
        metka++;
    }
}

void Syntax::setindex(ASTnode *temp)
{
    static int n=1;
    if(temp->childscount()==1)
    {
        jok.jk1=temp->retchild(0)->rettext();
        jok.jk2="eax";
        fi.push_back(jok);
    }
    else
    for(int i=0 ; i<temp->childscount() ; i++)
    {
        setindex(temp->retchild(i));
        if(temp->retchild(i)->rettype()==IDENT && idstringtype(temp->retchild(i)->rettext())=="REAL")
        {
            realvar=1;
        }
        if(temp->retchild(i)->rettype()==IDENT)
        {
            jok.jk1=temp->retchild(i)->rettext();
            for(int j=0 ; j<(int)fi.size(); j++)
            {
                if(fi[j].jk1==temp->retchild(i)->rettext())
                {
                    n=0;
                    break;
                }
            }
            if(n)
            {
                switch(joknum)
                {
                case 0:
                    jok.jk2="eax";
                    break;
                case 1:
                    jok.jk2="ebx";
                    break;
                case 2:
                    jok.jk2="ecx";
                    break;
                case 3:
                    jok.jk2="edx";
                    break;
                }
                fi.push_back(jok);
                joknum++;
            }
            n=1;
        }
        else if(temp->retchild(i)->rettype()==CONSTANT)
        {
            jok.jk1=jok.jk2=temp->retchild(i)->rettext();
            fi.push_back(jok);
        }
    }
}
string Syntax::getindex(string s)
{
    for(int i=0 ; i<(int)fi.size() ; i++)
    {
        if(fi[i].jk1==s) return fi[i].jk2;
    }
    return "";
}
void Syntax::opproc(ASTnode *temp)
{
    if(temp->rettext()=="sqrt")
    {
        if(temp->retchild(0)->rettype()==IDENT && idstringtype(temp->retchild(0)->rettext())=="REAL")
        {
            realvar=1;
        }
        if(realvar==1)
        {
            mod << "MOV eax," << temp->retchild(0)->rettext() << endl;
            mod << "FLD eax" << endl;
            mod << "FSQRT" << endl;
            mod << "FSTP eax" << getindex(temp->retchild(0)->rettext()) << endl;
            mod << "MOV " << temp->retchild(0)->rettext() << ","<< "eax" << endl;
        }
        else
        {
            mod << "MOV eax," << temp->retchild(0)->rettext() << endl;
            mod << "FILD eax" << endl;
            mod << "FSQRT" << endl;
            mod << "FISTP eax" << getindex(temp->retchild(0)->rettext()) << endl;
            mod << "MOV " << temp->retchild(0)->rettext() << ","<< "eax" << endl;
        }
        realvar=0;
    }
    else if(temp->rettext()=="printreal")
    {
        mod << "printf(\"%f\"," << temp->retchild(0)->rettext() << ")" << endl;
    }
    else if(temp->rettext()=="printint")
    {
        mod << "printf(\"%i\"," << temp->retchild(0)->rettext() << ")" << endl;
    }
    else if(temp->rettext()=="scanint")
    {
        mod << "invoke crt_scanf, ADDR OutputFormat, ADDR " << temp->retchild(0)->rettext() << endl;
    }
    else if(temp->rettext()=="scanreal")
    {
        mod << "invoke crt_scanf, ADDR OutputFormat, ADDR " << temp->retchild(0)->rettext() << endl;
    }
    else if(temp->rettext()=="printstring")
    {
        mod << "printf("<< temp->retchild(0)->rettext() << ")" << endl;
    }
    else
    {
        mod << "invoke " << temp->rettext() << ", " << temp->retchild(0)->rettext() << "," << temp->retchild(1)->rettext() <<  endl;
    }
}
int Syntax::GSTPS(string s)
{
    for(int i=0 ; i < (int)TI.size() ; i++)
    {
        if(TI[i].var==s)
        {
            return TI[i].kol;
        }
    }
    return 0;
}
void Syntax::prisv(ASTnode *temp)
{
    if(temp->retchild(0)->rettype()!=IDENT && temp->retchild(0)->rettype()!=CONSTANT)
    {
        prisv(temp->retchild(0));
    }
    if(temp->retchild(1) && temp->retchild(1)->rettype()!=IDENT && temp->retchild(1)->rettype()!=CONSTANT)
    {
        prisv(temp->retchild(1));
    }
    if(realvar==1)
    {
        if(temp->rettext()=="-")
        {
            if(temp->childscount()==1)
            {
                temp->settext(temp->retchild(0)->rettext());
                mod << "NEG " << getindex(temp->retchild(0)->rettext()) << endl;
            }
            else
            {
                temp->settext(temp->retchild(0)->rettext());
                mod << "FSUB " << getindex(temp->retchild(0)->rettext()) << "," << getindex(temp->retchild(1)->rettext()) << endl;
            }
        }
        else if(temp->rettext()=="+")
        {
            temp->settext(temp->retchild(0)->rettext());
            mod << "FADD " << getindex(temp->retchild(0)->rettext()) << "," << getindex(temp->retchild(1)->rettext()) << endl;
        }
        else if(temp->rettext()=="*")
        {
            temp->settext(temp->retchild(0)->rettext());
            mod << "FMUL " << getindex(temp->retchild(0)->rettext()) << "," << getindex(temp->retchild(1)->rettext()) << endl;
        }
        else if(temp->rettext()=="/")
        {
            temp->settext(temp->retchild(0)->rettext());
            mod << "FDIV " << getindex(temp->retchild(0)->rettext()) << "," << getindex(temp->retchild(1)->rettext()) << endl;
        }
        else if(temp->rettext()==":=")
        {
            mod << "MOV " << temp->retchild(0)->rettext() << "," << getindex(temp->retchild(1)->rettext()) << endl;
            return;
        }
    }
    else
    {
        if(temp->rettext()=="-")
        {
            if(temp->childscount()==1)
            {
                temp->settext(temp->retchild(0)->rettext());
                mod << "NEG " << getindex(temp->retchild(0)->rettext()) << endl;
            }
            else
            {
                temp->settext(temp->retchild(0)->rettext());
                mod << "SUB " << getindex(temp->retchild(0)->rettext()) << "," << getindex(temp->retchild(1)->rettext()) << endl;
            }
        }
        else if(temp->rettext()=="+")
        {
            temp->settext(temp->retchild(0)->rettext());
            mod << "ADD " << getindex(temp->retchild(0)->rettext()) << "," << getindex(temp->retchild(1)->rettext()) << endl;
        }
        else if(temp->rettext()=="*")
        {
            temp->settext(temp->retchild(0)->rettext());
            mod << "IMUL " << getindex(temp->retchild(0)->rettext()) << "," << getindex(temp->retchild(1)->rettext()) << endl;
        }
        else if(temp->rettext()=="/")
        {
            temp->settext(temp->retchild(0)->rettext());
            mod << "IDIV " << getindex(temp->retchild(0)->rettext()) << "," << getindex(temp->retchild(1)->rettext()) << endl;
        }
        else if(temp->rettext()==":=")
        {
            mod << "MOV " << temp->retchild(0)->rettext() << "," << getindex(temp->retchild(1)->rettext()) << endl;
            return;
        }
    }
}
void Syntax::scancode()
{
    code >> t >> word >> page;
}
int Syntax::ischislo(string s)
{
    for (int i = 0; i < (int)s.length(); i++)
    {
        if(s[i]=='.') continue;
        if ((s[i] < '0') || (s[i] > '9'))  return 0;
    }
    return 1;
}
int Syntax::Synanalys()
{
    if(START()!=NULL)
    {
        return 1;
    }
    else return 0;
}
ASTnode* Syntax::START()
{
    ASTnode *prog = new ASTnode(NONTERMINAL, "PROGRAMM", NULL, NULL);
    root=prog;
    ASTnode *qwe = NULL;
    while(!code.eof())
    {
        try
        {
            qwe=MODULE();
            if(qwe==NULL)
            {
                //cout << word << endl<< "Syntax Error" << endl;
                return NULL;
            }
            prog->AddChild(qwe);
            scancode();
            if(word!=";") throw 10;
            qwe=O();
            prog->AddChild(qwe);
            if(word!="BEGIN")
            {
                throw 20;
                root->ASTremove(root);
                return NULL;
            }
            prog->AddChild(P());
            if(word!="END")
            {
                throw 21;
                root->ASTremove(root);
                return NULL;
            }
            prog->AddChild(new ASTnode(TERMINAL, word, NAME(), NULL));
            scancode();
            if(word!=".")
            {
                throw 4;
                root->ASTremove(root);
                return NULL;
            }
            code >> t;
        }
        catch(int i)
        {
            switch(i)
            {
                case 20: cout << "Expected BEGIN"; break;
                case 21: cout << "Expected END"; break;
                case 4: cout << "Expected ."; break;
                case 10: cout << "Expected ;"; break;
                case 12: cout << "Undeclared identifier"; break;
                case 2: cout << "Syntax error"; break;
                case 3: cout << "Expected number"; break;
                case 5: cout << "Expected OF"; break;
                case 6: cout << "Expected type of identifier"; break;
                case 7: cout << "Expected name"; break;
                default: break;
            }
            root->ASTremove(root);
            cout <<" String: " << page;
            return NULL;
        }
    }
    prog->printnode(prog);
    return prog;
}
ASTnode* Syntax::MODULE()
{
    ASTnode *name=NULL;
    scancode();
    if(word!="MODULE") return NULL;
    else
    {
        name = NAME();
        if(name==NULL) return NULL;
        return new ASTnode(KEYWORD, "MODULE", name, NULL);
    }
}
ASTnode* Syntax::NAME()
{
    scancode();
    nam=word;
    if(t!="j") throw 7;
    return new ASTnode(IDENT, word, NULL, NULL);
}
ASTnode* Syntax::O()
{
    string temp;
    ASTnode *ob=NULL;
    ob=new ASTnode(NONTERMINAL, "DECLARATION", NULL, NULL);
    scancode();
    while(word=="CONST" || word=="VAR" || word=="PROCEDURE")
    {
        if(word=="CONST") ob->AddChild(C());
        if(word=="VAR") ob->AddChild(V());
        if(word=="PROCEDURE") ob->AddChild(F());
        scancode();
    }
    if(ob->childscount()==0)
    {
        delete ob;
        return NULL;
    }
    return ob;
}
ASTnode* Syntax::P()
{
    ASTnode *temp=NULL;
    temp=new ASTnode(NONTERMINAL, "NACHINKA", NULL, NULL);
    scancode();
    while(t=="j" || word=="WHILE" || word=="IF")
    {
        if(t=="j") temp->AddChild(OO());
        else if(word=="WHILE" || word=="IF") temp->AddChild(PO());
        scancode();
    }
    if(temp->childscount()==0) return NULL;
    return temp;
}
ASTnode* Syntax::PO()
{
    if(word=="IF") return IFELSE();
    else if(word=="WHILE") return CIKL();
    else return NULL;
}
ASTnode* Syntax::IFELSE()
{
    ASTnode *temp=NULL;
    temp=new ASTnode(OPIF, "IF", NULL, NULL);
    scancode();
    temp->AddChild(BOOL());
    if(word!="THEN")
    {
        cout << "Error: Expected THEN";
        root->ASTremove(root);
        return NULL;
    }
    temp->AddChild(P());
    if(word=="ELSE")
    {
        temp->AddChild(new ASTnode(OPELSE, word, NULL, NULL));
        temp->AddChild(P());
    }
    else if(word=="ELSEIF")
    {
        while(word=="ELSEIF")
        {
            temp->AddChild(new ASTnode(OPELSIF, word, NULL, NULL));
            scancode();
            temp->AddChild(BOOL());
            if(word!="THEN")
            {
                cout << "Error: Expected THEN";
                root->ASTremove(root);
                return NULL;
            }
            temp->AddChild(P());
        }
        if(word=="ELSE")
        {
            temp->AddChild(new ASTnode(OPELSE, word, NULL, NULL));
            temp->AddChild(P());
        }
    }
    cout << word;
    if(word!="END")
    {
        cout << "Error: Expected END";
        root->ASTremove(root);
        return NULL;
    }
    return temp;
}
ASTnode* Syntax::CIKL()
{
    ASTnode *temp=NULL;
    temp=new ASTnode(OPCIKLE, "WHILE", NULL, NULL);
    scancode();
    temp->AddChild(BOOL());
    if(word!="DO")
    {
        cout << "Error: Expected DO";
        root->ASTremove(root);
        return NULL;
    }
    temp->AddChild(P());
    if(word!="END")
    {
        cout << "Error: Expected END";
        root->ASTremove(root);
        return NULL;
    }
    return temp;
}
ASTnode* Syntax::OO()
{
    ASTnode *temp=NULL;
    string ident;
    ident=word;
    scancode();
    if(word=="(")
    {
        temp=new ASTnode(PROCEDURE, ident, NULL, NULL);
        FACTPAR(temp);
        if(word!=")")
        {
            temp->ASTremove(temp);
            cout << "Error";
            return NULL;
        }
        scancode();
        if(word!=";")
        {
            temp->ASTremove(temp);
            cout << "Error";
            return NULL;
        }
    }
    else if(word==":=") temp=new ASTnode(ASSIGN, word, new ASTnode(IDENT, ident, NULL, NULL), A());
    if(word!=";") throw 10;
    return temp;
}
ASTnode* Syntax::A()
{
    ASTnode *temp=NULL;
    scancode();
    temp=ARIFM();
    return temp;
}
void Syntax::printid()
{
    for(int i=0 ; i<(int)TI.size() ; i++)
        cout << TI[i].var << " " << TI[i].type << " " << TI[i].kol << " " << TI[i].mas<< endl;
}
int Syntax::check(string w)
{
    if(w=="OR") return 17;
    if(w=="AND") return 18;
    if(w=="+") return 11;
    if(w=="-") return 12;
    if(w=="*") return 13;
    if(w=="/" || w=="DIV") return 14;
    if(w=="MOD") return 15;
    if(w=="j") return 3;
    if(w=="c") return 2;
    return 0;
}
ASTnode* Syntax::BOOL()
{
    ASTnode *temp=NULL;
    string w;
    temp=JOIN();
    while(word=="OR")
    {
        w=word;
        scancode();
        temp=new ASTnode(OR, w, temp, JOIN());
    }
    return temp;
}
ASTnode* Syntax::JOIN()
{
    ASTnode *temp=NULL;
    string w;
    temp=EQUA();
    while(word=="AND")
    {
        w=word;
        scancode();
        temp=new ASTnode(AND, w, temp, EQUA());
    }
    return temp;
}
ASTnode* Syntax::EQUA()
{
    ASTnode *temp=NULL;
    string w;
    temp=REL();
    while(word=="=" || word=="<>")
    {
        w=word;
        scancode();
        temp=new ASTnode(AND, w, temp, REL());
    }
    return temp;
}
ASTnode* Syntax::REL()
{
    ASTnode *temp=NULL;
    string w;
    temp=ARIFM();
    if(word=="<" || word==">" || word==">=" || word=="<=")
    {
        w=word;
        scancode();
        return new ASTnode(SRAV, w, temp, ARIFM());
    }
    else return temp;
}
ASTnode* Syntax::ARIFM()
{
    ASTnode *temp=NULL;
    string w;
    temp=term();
    while(word=="+" || word=="-")
    {
        w=word;
        scancode();
        temp=new ASTnode(check(w), w, temp, term());
    }
    return temp;
}
ASTnode* Syntax::term()
{
    ASTnode *temp=NULL;
    string w;
    temp=unary();
    while(word=="*" || word=="/" || word=="MOD" || word=="DIV")
    {
        w=word;
        scancode();
        temp=new ASTnode(check(w), w, temp, unary());
    }
    return temp;
}
ASTnode* Syntax::unary()
{
    if(word=="-")
    {
        scancode();
        return new ASTnode(SUB, "-", unary(), NULL);
    }
    else if(word=="NOT")
    {
        scancode();
        return new ASTnode(NOT, "NOT", unary(), NULL);
    }
    else return factor();
}
ASTnode* Syntax::factor()
{
    ASTnode *temp=NULL;
    string w;
    if(t=="c")
    {
        temp = new ASTnode(CONSTANT, word, NULL, NULL);
        scancode();
        return temp;
    }
    else if(t=="j")
    {
        w=word;
        scancode();
        if(idtype(w)==-1) throw 12;
        if(idtype(w)!=55) return new ASTnode(IDENT, w, NULL, NULL);
        else return offset(w);
    }
    else if(word=="(")
    {
        scancode();
        temp=BOOL();
        if(word==")")
        {
            scancode();
            return temp;
        }
    }
    return NULL;
}
ASTnode* Syntax::offset(string qwe)
{
    ASTnode *temp=NULL, *t1=NULL;
    //cout << word;
    if(word!="[")
    {
        //cout << "yes";
        throw 2;
        return NULL;
    }
    scancode();
    t1=new ASTnode(MUL, "*", new ASTnode(CONSTANT, word, NULL, NULL), new ASTnode(CONSTANT, sidtype(qwe), NULL, NULL));
    temp=new ASTnode(ADD, "+", new ASTnode(IDENT, qwe, NULL, NULL), t1);
    scancode();
    if(word!="]")
    {
        throw 2;
        return NULL;
    }
    scancode();
    return temp;
}
string Syntax::sidtype(string w)
{
    for(int i=0 ; i<(int)TI.size() ; i++)
    {
        if(TI[i].var==w)
        {
            if(TI[i].type=="INTEGER" || TI[i].type=="CARDINAL" || TI[i].type=="REAL")
            {
                return "4";
            }
            else if(TI[i].type=="CHAR") return "1";
        }
    }
    return " ";
}
int Syntax::idtype(string w)
{
    if(flag) return 4;
    else
    {
    for(int i=0 ; i<(int)TI.size() ; i++)
    {
        if(TI[i].var==w)
        {
            if(TI[i].mas==1) return 55;
            else if(TI[i].type=="INTEGER" || TI[i].type=="CARDINAL" || TI[i].type=="REAL" || TI[i].type=="CONST")
            {
                return 4;
            }
            else if(TI[i].type=="CHAR") return 1;
        }
    }
    }
    return -1;
}
string Syntax::idstringtype(string w)
{
    for(int i=0 ; i<(int)TI.size() ; i++)
    {
        if(TI[i].var==w)
        {
            return TI[i].type;
        }
    }
    return "-1";
}
void Syntax::FACTPAR(ASTnode *temp)
{
    int str=0;
    do
    {
        scancode();
        if(t=="l") str=1;
        else if(t!="j" && t!="c")
        {
            temp->ASTremove(temp);
            cout << "Error";
            return;
        }
        if(str==1)
        {
            temp->AddChild(new ASTnode(STR, word, NULL, NULL));
            str=0;
            if(TIcheck(word))
            {
                rty.kol=varnum;
                varnum++;
                rty.mas=0;
                rty.proc="glob";
                rty.var=HelpStr(word);
                rty.type="string";
                TI.push_back(rty);
            }
        }
        else if(t=="j") temp->AddChild(new ASTnode(IDENT, word, NULL, NULL));
        else if(t=="c") temp->AddChild(new ASTnode(CONSTANT, word, NULL, NULL));
        scancode();
    }
    while(word==",");
}
int Syntax::TIcheck(string s)
{
    for(int i=0 ; i<(int)TI.size() ; i++)
    {
        if(TI[i].var==s) return 0;
    }
    return 1;
}
string Syntax::HelpStr(string s)
{
    if(s=="endl")
    {
        return "\\n";
    }
    else return s;
}
ASTnode* Syntax::F()
{
    ASTnode *temp=NULL, *name=NULL, *parameters=NULL, *temp2=NULL, *endname=NULL;
    if(word!="PROCEDURE") return NULL;
    temp=new ASTnode(KEYWORD, "PROCEDURE", NULL, NULL);
    flag=1;
    name=NAME();
    rty.proc=currentproc;
    currentproc=word;
    rty.type="PROCEDURE";
    rty.mas=0;
    rty.kol=0;
    rty.var=nam;
    TI.push_back(rty);
    if(name==NULL)
    {
        delete name;
        delete temp;
        return NULL;
    }
    scancode();
    if(word=="(")
    {
        parameters=FORMALPAR();
        if(parameters==NULL)
        {
            delete name;
            delete temp;
            return NULL;
        }
        name->AddChild(parameters);
        scancode();
    }
    if(word!=";")
    {
        delete name;
        delete temp;
        delete parameters;
        return NULL;
    }
    temp2=O();
    name->AddChild(temp2);
    if(word!="BEGIN")
    {
        delete name;
        delete temp;
        delete parameters;
        delete temp2;
        return NULL;
    }

    name->AddChild(P());
    if(word!="END")
    {
        delete name;
        delete temp;
        delete parameters;
        delete temp2;
        return NULL;
    }
    endname=NAME();
    if(endname==NULL)
    {
        delete name;
        delete endname;
        delete temp;
        delete parameters;
        delete temp2;
        return NULL;
    }
    delete endname;
    /*scancode();
    if(word!=";")
    {
        delete name;
        delete endname;
        delete temp;
        delete parameters;
        delete temp2;
        return NULL;
    }*/
    flag=0;
    temp->AddChild(name);
    currentproc="glob";
    return temp;
}

ASTnode* Syntax::FORMALPAR()
{
    ASTnode *temp=NULL, *temp2=NULL, *par=NULL;
    temp=new ASTnode(NONTERMINAL, "PARAMETERS", NULL, NULL);
    fparam=1;
    do
    {
        scancode();
        if(word=="VAR")
        {
            scancode();
            par=new ASTnode(KEYWORD, word, NULL, NULL);
            temp->AddChild(par);
        }
        else
        {
            temp->AddChild(new ASTnode(KEYWORD, word, NULL, NULL));
        }
        scancode();
        if(word!=":")
        {
            delete temp;
            delete temp2;
            return NULL;
        }
        temp2=TYPE();
        if(temp2==NULL)
        {
            delete temp;
            delete temp2;
            return NULL;
        }
        par->AddChild(temp2);
        scancode();
    }
    while(word!=")");
    fparam=0;
    return temp;
}
ASTnode* Syntax::C()
{
    ASTnode *temp=NULL, *name=NULL, *value=NULL, *temp2=NULL;
    if(word!="CONST") return NULL;
    temp=new ASTnode(KEYWORD, "CONST", NULL, NULL);
    name=NAME();
    rty.proc=currentproc;
    rty.var=nam;
    rty.type="CONST";
    rty.mas=0;
    rty.kol=1;
    if(name==NULL)
    {
        delete name;
        delete temp;
        return NULL;
    }
    scancode();
    if(word!="=")
    {
        delete name;
        delete temp;
        return NULL;
    }
    value=C1();
    rty.zn=value->rettext();
    temp2=new ASTnode(ASSIGN, "=", name, value);
    temp->AddChild(temp2);
    scancode();
    if(word!=";")
    {
        delete name;
        delete temp;
        delete temp2;
        delete value;
        return NULL;
    }
    TI.push_back(rty);
    return temp;
}
ASTnode* Syntax::C1()
{
    ASTnode *temp=NULL;
    scancode();
    if(word=="-")
    {
        scancode();
        temp=new ASTnode(SUB, "-", CHISLO(), NULL);
        return temp;
    }
    else return CHISLO();
}
ASTnode* Syntax::CHISLO()
{
    if(!ischislo(word)) return NULL;
    return new ASTnode(CONSTANT, word, NULL, NULL);
}
ASTnode* Syntax::V()
{
    ASTnode *temp=NULL, *name=NULL;
    string ide;
    if(word!="VAR") return NULL;
    temp=new ASTnode(KEYWORD, "VAR", NULL, NULL);
    name=NAME();
    if(name==NULL) return NULL;
    temp->AddChild(name);
    scancode();
    /*while(word==",")
    {
        name=NAME();
        if(name==NULL)
        {
            delete temp;
            return NULL;
        }
        temp->AddChild(name);
        code >> t >> word;
    }*/
    if(word!=":") return NULL;
    temp->AddChild(TYPE());
    scancode();
    if(word!=";")
    {
        delete name;
        delete temp;
        return NULL;
    }
    return temp;
}
ASTnode* Syntax::TYPE()
{
    scancode();
    if(word!="INTEGER" && word!="REAL" && word!="CHAR" && word!="BOOLEAN" && word!="ARRAY") throw 6;
    if(word=="ARRAY")
    {
        if(fparam)
        {
            scancode();
            scancode();
            word="ARRAY OF " + word;
        }
        else
        {
            rty.var=nam;
            rty.mas=1;
            scancode();
            if(word!="[") throw 2;
            scancode();
            if(word!="0") throw 3;
            scancode();
            if(word!=".") throw 4;
            scancode();
            if(word!=".") throw 4;
            scancode();
            if(!ischislo(word)) throw 3;
            else rty.kol=atoi(word.c_str())+1;
            scancode();
            if(word!="]") throw 2;
            scancode();
            if(word!="OF") throw 5;
            scancode();
            if(t!="k") throw 6;
            else rty.type=word;
            word="ARRAY OF " + word;
            rty.proc=currentproc;
            rty.zn="?";
        }
    }
    else
    {
        rty.proc=currentproc;
        rty.var=nam;
        rty.type=word;
        rty.mas=0;
        rty.kol=1;
        rty.zn="?";
    }
    if(fparam==0) TI.push_back(rty);
    return new ASTnode(KEYWORD, word, NULL, NULL);
}
