#include<string>
#include<vector>
#include "gram.h"
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
#define EBRACK(scope,ctype,src)	\
	do{ \
		wd = next(); \
		expression(scope,ctype,src); \
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
struct symbol{
	word wd;
	int type;
	int form;
	int retcnt;
	int pos;
	int mVar=-1;
	bool isIni;
	//int layer;
	vector<int> dim;
	vector <symbol*>  parList;
	symbol* scope;
	union Value value;
	symbol() { type = INIT; scope = 0;  retcnt = 0; isIni = false; }
	bool operator ==(const symbol& x) const {
		return wd.ss == x.wd.ss && scope == x.scope;
	}
	bool operator <(const symbol& x) const {
		return wd < x.wd;
	}
	int* arr = 0;
	//for code_generate
	int getSize() {
		int base = -1,sum=1;
		if(type == CTYPE||type==ITYPE) base = 4;
		for (auto i : dim)	sum *= i;
		return sum* base;
	}
	
	int* getArray() {
		if (arr == 0 && form==ARRAYF && isIni) {
			if (dim.size() == 1) arr = value.sa;
			else {
				int mlen = getSize();
				arr = new int[mlen/4];
				for (int i = 0; i < dim[0]; i++) 
					for (int j = 0; j < dim[1]; j++) 
						arr[i * dim[1] + j] = value.da[i][j];
			}
		}
		return arr;
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
	vector<symbol*> getVars(symbol* symb) {
		vector<symbol*> vec;
		auto it = fma.find(symb);
		if (it == fma.end()) return vec;
		auto vma = it->second;
		for (auto i : vma) 
			vec.push_back(i.second);
		return vec;

	}
	int getVarSize(symbol* symb) {
		int size = 0;
		vector <symbol*> vec = getVars(symb);
		for (auto i : vec) {
			if(i->form !=CONSF)
				size += i->getSize();
		}
			
		return size - getParSize(symb);
	}
	int getParSize(symbol* symb) {
		int size = 0;
		for (auto i : symb->parList)
			size += i->getSize();
		return size;
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
void con(int& n, int& type);
void integer(int& n);
void stepLen(struct Data*& des);
//
void consDef(symbol* scope);
void consExplain(symbol* scope);
void varExplain(symbol* scope,int &size);
void varDef(symbol* scope,int &size);
void varIniDef(symbol* scope,int &size);
void varNoIniDef(symbol* scope,int &size);
void declareHead(symbol* symb);
void parList(vector<symbol*>& parList, symbol* scope);
void compSen(symbol* scope,int &size);
void senList(symbol* scope);
void sentence(symbol* scope);
void repSen(symbol* scope);
void condSen(symbol* scope);
void hasCallSen(symbol* scope,struct Data* &des);
void noCallSen(symbol* scope,struct Data * &des);
void valParList(symbol* scope, vector<symbol*> parList);
void assignSen(symbol* scope);
void readSen(symbol* scope);
void writeSen(symbol* scope);
void str();
void situaSen(symbol* scope);
void returnSen(symbol* scope);
void expression(symbol* scope, int& type, struct Data* &des);
void item(symbol* scope, int& type, struct Data* &des);
void factor(symbol* scope, int& type, struct Data* &des);
void caseTable(symbol* scope, int type, Data* exp, Data* end, int mid);
void subCase(symbol* scope, int type, Data* exp, Data* nex, int mid);
void defaul(symbol* scope);
void condition(symbol* scope, string ms);
#endif
