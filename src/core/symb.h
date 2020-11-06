#include<string>
#include<vector>
using namespace std;

#ifndef SYMBOLS
#define  SYMBOLS
#define WASYMB 'a'
#define DUPNAME 'b'
#define NONAME 'c'
#define PARNUM 'd'
#define PARTYPE 'e'
#define IFTYPE 'f'
#define NOHASRET 'g'
#define HASNORET 'h'
#define ARRYINT 'i'
#define MODCONS 'j'
#define SSEMICN 'k'
#define SRPARENT 'l'
#define SRBRACK 'm'
#define ARRYNUM 'n'
#define CONTYPE 'o'
#define SDEFAULT 'p'
#define OTHER 'z'
//
#define INIT -1
#define CTYPE 1
#define ITYPE 2
#define VTYPE 3
#define GETTYPE(x) ((x)==INTTK?ITYPE:((x)==CHARTK?CTYPE:VTYPE))
//

#define ARRAYF 1
#define VARF 2
#define FUNF 3
#define CONSF 4
#define ISEXPB(x) ((x)==INTCON||(x)==PLUS||(x)==MINU||(x)==IDENFR||(x)==LPARENT||(x)==CHARCON)
#define CHECKWD(ctype) \
	do{\
		if (wd.type != IDENFR) error(wd,s); \
		(ctype) = symtab.findt(wd.ss, scope) ; \
		if ((ctype) < 0) error(wd, s, NONAME);\
	}while (0)
#define STRTINT(s,n) \
	do{\
		for(auto i : (s)) \
			(n) = (n) * 10 + i - '0';\
	}while(0);
#define DARRAY(n,m,x) \
	do {\
		(x) =new int*[(n)]; \
		for(int i=0;i<n;i++) (x)[i] = new int[(m)]; \
	} while(0)
#define BRACK(n) \
	do { \
			wd = next(); \
			noSignInt(n); \
			if (wd.type != RBRACK) error(wd,s,SRBRACK); \
			wd = next(); \
	} while (0)
#define EBRACK(scope,ctype)	\
	do{ \
		wd = next(); \
		expression(scope,ctype); \
		if(ctype!=ITYPE) error(wd,s,ARRYINT);\
		if(wd.type !=RBRACK) error(wd,s,SRBRACK); \
		wd = next();\
	}while(0)
union Value {
	void* abst;
	int* sa;
	int** da;
	int var;
};
void error(word mwd, string s = "", int mode = 'a', int isout = WASYMB);
struct symbol{
	word wd;
	int type;
	int form;
	int retcnt;
	//int layer;
	vector<int> size;
	vector <symbol*>  parList;
	symbol* scope;
	union Value value;
	symbol() { type = INIT; scope = 0;  retcnt = 0; }
	bool operator ==(const symbol& x) const {
		return wd.ss == x.wd.ss && scope == x.scope;
	}
	bool operator <(const symbol& x) const {
		return wd < x.wd;
	}
};
struct symbtable {
	/*map<symbol, int> ma;*/
	map<symbol*, map<string, symbol*> > fma;
	bool insert(symbol *symb) {
		if (fma.find(symb->scope) == fma.end()) {
			return fma.emplace(symb->scope,
				map<string, symbol*>{ {symb->wd.ss,symb}}).second;
			
		}
		else {
			auto x = fma.find(symb->scope)->second;
			if (x.find(symb->wd.ss) != x.end()) return  false;
			fma[symb->scope][symb->wd.ss] = symb;
			return true;
		}
	}
	symbol* find(string name,symbol * scope) {
		while (true) {
			auto x = fma.find(scope);
			if (x != fma.end()) {
				auto y = x->second.find(name);
				if (y != x->second.end()) return y->second;
			}
			if (scope == 0) break;
			scope = scope->scope;
		}
		return 0;
	}
	int findt(string name,symbol * scope) {
		symbol* it = find(name, scope);
		if (it == 0) return -1;
		else return it->type;
	}
	int findf(string name,symbol * scope) {
		symbol* it = find(name, scope);
		if (it == 0) return -1;
		else return it->form;
	}

};
struct hashSymbol {
	size_t operator()(const symbol& x) const {
		return hash<int>()(x.type) ^ hash<string>()(x.wd.ss)^hash<int>()(x.form);
	}
};

void no_Has();
void hasReturnFun();
void noReturnFun();
void mainFun();
void noSignInt(int& n);
void con(int& n,int &type);
void integer(int& n);
void stepLen();
//
void onsDef(symbol* scope);
void consExplain(symbol* scope);
void varExplain(symbol* scope);
void varDef(symbol* scope);
void varIniDef(symbol* scope);
void varNoIniDef(symbol* scope);
void declareHead(symbol* symb);
void parList(vector<symbol*>& parList, symbol* scope);
void compSen(symbol* scope);
void senList(symbol* scope);
void sentence(symbol* scope);
void repSen(symbol* scope);
void condSen(symbol* scope);
void hasCallSen(symbol* scope);
void noCallSen(symbol* scope);
void valParList(symbol* scope, vector<symbol*> parList);
void assignSen(symbol* scope);
void readSen(symbol* scope);
void writeSen(symbol* scope);
void str();
void situaSen(symbol* scope);
void returnSen(symbol* scope);
void expression(symbol* scope, int& type);
void item(symbol* scope, int& type);
void factor(symbol* scope, int& type);
void caseTable(symbol* scope, int type);
void subCase(symbol* scope, int type);
void defaul(symbol * scope);
void condition(symbol* scope);
void program();
#endif
