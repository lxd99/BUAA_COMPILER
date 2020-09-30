#define  _CRT_SECURE_NO_WARNINGS
#ifndef symFirst
#define symFirst 
#include<cstdio>
#include<string>
#include<cstdlib>
#include<vector>
using namespace  std;
#define ERROR -1
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
};
void error(int num, string s = "");
void add(int num, string s, int pt = 1);
void getsym(FILE* fp);
#endif //
