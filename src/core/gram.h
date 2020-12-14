#ifndef symFirst
#define symFirst 
#include<cstdio>
#include<string>
#include<cstdlib>
#include<vector>
#include<map>
#include<set>
#include<iostream>
#include <algorithm>
#include "global.h"
using namespace  std;
#define CDEBUG 0
#define YDEBUG 0
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
	string ss;
	int type;
	int line;
	int no;
	word(int types=INITIAL,string ms ="",int mline=0,int mno = 0) {
		type = types;
		s = ms;
		line = mline;
		no = mno;
		for (auto i : ms) {
			if (i >= 'A' && i <= 'Z') i = i - 'A' + 'a';
			ss.push_back(i);
		}
	}
	bool operator == (const word& mwd) const {
		return mwd.ss == ss && mwd.line == line && mwd.no == no;
	}
	bool operator < (const word& mwd) const {
		if (mwd.line == line) return no < mwd.no;
		return line < mwd.line;
	}
};
void addWord(int num,string s,int mode = CDEBUG);
void getsym(FILE* fp);
void check(word wd,int mode =0);
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
word next(int mode=1);
void addSen(string s = "", int mode = YDEBUG);
void program();
#endif //
