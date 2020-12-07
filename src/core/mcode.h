#include	"symb.h"
#ifndef MCODE
#define MCODE 
#define DADDR 0x10000000
#define TADDR 0x400000
#define KADDR 0x80000000
#define MDEBUG 0

#define MVARADD 0
#define VARADD 1
#define CONSADD 2
#define STRADD 3
#define LABADD 4
#define FUNADD 5


#define NEGOP 0
#define ADDOP 1
#define SUBOP 2
#define MULOP 3
#define DIVOP 4
#define RETOP 5
#define PUSHOP 6
#define ASSIGNOP 7
#define JGEOP 8
#define JNEOP 9
#define JEQOP 10
#define JLEOP 11
#define JGTOP 12
#define JLTOP 13
#define JOP 14
#define READOP 15
#define PRINTOP 16
#define INIOP 17
#define CALLOP 18
#define LABELOP 19
#define ENDOP 20
#define DECLAREOP 21

#define PRELAB  "dxlduckingss"
#define MODKVAR(des,num)		\
	{	(des)->type = MVARADD;\
		(des)->cont.varid = dept + (num); \
		dept += (num) + 1; \
	}while(0)

#define TRANS(x) #x
#define SAPP(sta,s,format, ...) \
do { \
  if(sta==0) s ="";\
  sprintf(sbuf,format,##__VA_ARGS__); \
  s += sbuf; \
} while(0)
union Content
{
	void* abst;
	symbol* symb;
	int varid;
	int sid;
	int cons;
};
struct isInf {
	int isc;
	int ise;
	isInf()
	{
		isc = 0;
		ise = 1;
	}
};
struct Data
{
	int type;
	struct  isInf isinf;
	Content cont;
	struct Data* off;
	string s;
	Data() {
		type = INIT;
		off = 0;
	}
	Data(int mtype, void* mcont,void* moff=0) {
		type = mtype;
		cont.abst = mcont;
		off = (Data*)moff;
	}
	string getS() {
		if (s != "") return s;
		char sbuf[50];
		if (type == MVARADD) {
			sprintf(sbuf, "t%d", cont.varid);
			s = sbuf;
		}
		else if (type == CONSADD) {
			sprintf(sbuf, "%d", cont.cons);
			s = sbuf;
		}
		else if (type == VARADD) {
			symbol* symb = cont.symb;
			s = symb->wd.s;
			if (symb->form == ARRAYF) {
				if (off == 0) s += "[]";
				else s += "[" + off->getS() + "]";
			}
			else if (symb->form == FUNF) ;
		}
		else if (type == STRADD) {
			s = PRELAB;
			sprintf(sbuf, "%d", cont.sid);
			s += sbuf;
		}
		else if (type == LABADD);
		else s = "????";
		return s;
	}
};
struct Mcode {
	Data *des,*srcl,*srcr;
	int op;
	string s;
	Mcode() { op = INIT; }
	Mcode(int mop,Data* mdes, Data* msrcl) {
		op = mop;
		des = mdes;
		srcl = msrcl;
	}
	Mcode(int mop,Data *mdes, Data *msrcl, Data* msrcr) {
		op = mop;
		des = mdes;
		srcl = msrcl;
		srcr = msrcr;
	}
	void genS(string ms) {
		s = ms;
	}
};
struct McodeList {
	vector<Mcode> cList;
	map<string, int> sma;
	void push(Mcode data) {
		cList.push_back(data);
		if (data.op == LABELOP) {
			if (!sma.emplace(data.s, cList.size() - 1).second) {
				printf("duplicate label! %s",data.s.c_str());
			}
		}
		if (MDEBUG == 1) printf("%s\n",data.s.c_str());
	}
	int getSize() {
		return cList.size();
	}
	Mcode get(int id) {
		return cList[id];
	}
	void outC() {
		for (auto i : cList)
			printf("%s\n", i.s.c_str());
	}

};
void negc(Data* srcl,Data* &des);
void addc(Data* srcl, Data* srcr, Data* &des);
void subc(Data* srcl, Data* srcr, Data* &des);
void multc(Data* srcl, Data* srcr, Data* &des);
void divc(Data* srcl, Data* srcr, Data* &des);
void arrc(Data* srcl, Data* &des);
void assignc(Data* srcl,Data*& des);
void printc(Data* srcl);
void readc(Data* des);
void glabc(string s);
void condjc(int op,Data* srcl, Data* srcr,Data* des);
void pushc(Data* des,Data *srcl);
void inic(Data* srcl);
void callc(symbol* symb, Data* &des);
void jumpc(Data *des);
void declarec(Data *srcl);
void endc();
void retc(Data *srcl,Data *srcr);
void outC();
#endif
