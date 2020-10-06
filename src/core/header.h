#define  _CRT_SECURE_NO_WARNINGS
#ifndef symFirst
#define symFirst 
#include<cstdio>
#include<string>
#include<cstdlib>
#include<vector>
#include<map>
#include<iostream>
using namespace  std;
#define INITIAL -3
#define ERROR -2
#define IDENFR 0
#define INTCON 1
#define CHARCON 2
#define STRCON 3
#define CONSTTK 4
#define INTTK 5
#define CHARTK 6
#define VOIDTK 7
#define MAINTK 8
#define IFTK 9
#define ELSETK 10
#define SWITCHTK 11
#define CASETK 12
#define DEFAULTTK 13
#define WHILETK 14
#define FORTK 15
#define SCANFTK 16
#define PRINTFTK 17
#define RETURNTK 18
#define PLUS 19
#define MINU 20
#define MULT 21
#define DIV 22
#define LSS 23
#define LEQ 24
#define GRE 25
#define GEQ 26
#define EQL 27
#define NEQ 28
#define COLON 29
#define ASSIGN 30
#define SEMICN 31
#define COMMA 32
#define LPARENT 33
#define RPARENT 34
#define LBRACK 35
#define RBRACK 36
#define LBRACE 37
#define RBRACE 38
#define ISALPHA(x) ((x)>='A'&&(x)<='Z'||x>='a'&&x<='z'||x=='_')
#define ISDIGIT(x) ((x)>='0'&&x<='9')
#define ISOP(x) ((x)=='+'||(x)=='-'||(x)=='*'||(x)=='/')    
#define ISCHAR(x) (ISALPHA(x)||ISDIGIT(x)||ISOP(x))
#define ISSTR(x) ((x)>=35&&(x)<=126||(x)==32||(x)==33)
#define LOWER(x) (((x)>='A'&&(x)<='Z'?(x)-'A'+'a':(x)))
extern string id[39];
struct word {
	string s;
	int type;
	word(int types=INITIAL,string ms ="") {
		type = types;
		s = ms;
	}
};
void error(string s ="");
void addWord(int num,string s,int mode = 0);
void getsym(FILE* fp);
void check(word wd,int mode =1);
/*语法分析*/
#define INTFUN     0
#define CHARFUN    1
#define VOIDFUN    2
#define ISTYPE(x) ((x)==CHARTK||(x)==INTTK)
#define ISCONST(x)	((x)==CONSTTK)
#define ISSEN(x) ((x)==WHILETK||(x)==FORTK||(x)==IFTK||(x)==IDENFR||(x)==SCANFTK||(x)==PRINTFTK||(x)==SWITCHTK||(x)==RETURNTK||(x)==SEMICN||(x)==LBRACE)
#define ISCON(x) ((x)==CHARCON||(x)==INTCON||(x)==PLUS||(x)==MINU)
#define ISRELATEOP(x) ((x)==LSS||(x)==LEQ||(x)==GRE||(x)==GEQ||(x)==NEQ||(x)==EQL)
#define HASRETURN(x) ((x)==INTFUN||(x)==CHARFUN)
#define NORETURN(x) ((x)==VOIDFUN)
#define BRACK  		\
	do { \
			wd = next(); \
			noSignInt(); \
			if (wd.type != RBRACK) error(); \
			wd = next(); \
	} while (0)
#define EBRACK	\
		wd = next(); \
		expression(); \
		if(wd.type !=RBRACK) error(); \
		wd = next();
struct symbol {
	int type;
	int layer;
	string name;
	symbol(int itype=INITIAL,int ilayer=0,string iname="") {
		type = itype;
		layer = ilayer;
		name = iname;
	}
	bool operator ==(const symbol & x) const{
		return type == x.type && name == x.name;
	}
	bool operator <(const symbol& x) const {
		if (type == x.type) return name < x.name;
		return type < x.type;
	}
};
struct hashSymbol {
	size_t operator()(const symbol &x) const{
		return hash<int>()(x.layer) ^ hash<int>()(x.type) ^ hash<string>()(x.name);
	}
};
word next(int mode=1);
void addSen(string s ="",int mode=1);
void cons_Var();
void no_Has();
void noSignInt();
void program();
void consExplain();
void consDef();
void con();
void varExplain();
void varDef();
void varIniDef();
void varNoIniDef();
void hasReturnFun();
void noReturnFun();
void mainFun();
void declareHead();
void parList();
void compSen();
void senList();
void sentence();
void repSen();
void condSen();
void hasCallSen();
void noCallSen();
void assignSen();
void readSen();
void writeSen();
void situaSen();
void returnSen();
void condition();
void expression();
void item();
void factor();
void stepLen();
void valParList();
void str();
void caseTable();
void subCase();
void defaul();
void integer();
#endif //
