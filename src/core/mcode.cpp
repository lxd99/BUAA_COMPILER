#define  _CRT_SECURE_NO_WARNINGS
#include "mcode.h"
#define TMAX 7
/*只能用于mcode中的代码*/
#define ISCONST(x) ((x)->type==CONSADD|| \
	((x)->type==VARADD && (x)->cont.symb->form==CONSF))
#define ISZERO(x) (ISCONST(x)&&(x)->cont.cons == 0)
#define ISONE(x) (ISCONST(x)&&(x)->cont.cons == 1)
#define GETCONS(x,num) do{ \
	if((x)->type == CONSADD) num = (x)->cont.cons; \
	else { \
		num = (x)->cont.symb->value.var;\
	}\
}while(0)
int tid = 0;
int dept = 0;
McodeList	 mcodlis;
void outC() {
	mcodlis.outC();
}
int ispow(int x) {
	if (x <= 0) return -1;
	int cnt = 0;
	while (x%2==0) {
		x /= 2;
		cnt++;
	}
	return x == 1 ? cnt : -1;
}
//true code
void negc(Data *srcl,Data *&des) {
	static string op = "-";
	/*<负运算>*/
	Mcode mcode;
	des = new Data;
	int move = 0;
	if (ISCONST(srcl) && OPT >= 1) {
		int l;
		GETCONS(srcl, l);
		des = new Data(CONSADD, (void*)(-l));
	}
	else {
		des = new Data(MVARADD,(void*)(dept++));
		mcode = Mcode(NEGOP, des, srcl);
		mcode.genS(des->getS() + "=" + op + srcl->getS());
		mcodlis.push(mcode);
	}
	/*gRS("neg", des, src);*/
}
void slc(int op,Data* srcl, Data *srcr,Data*& des) {
	/*
	只支持左右移常数!
	*/
	string sop = op == SLLOP ? "sll" : "srl";
	if (ISCONST(srcl) && ISCONST(srcr)) {
		int l, r;
		GETCONS(srcl, l);
		GETCONS(srcr, r);
		des = new Data(CONSADD, (void*)(l << r));
	}
	else {
		des = new Data(MVARADD,(void*)(dept++));
		Mcode mcode = Mcode(op, des, srcl, srcr);
		mcode.genS(des->getS() + " = " +
			srcl->getS() + " " + sop + " " + srcr->getS());
		mcodlis.push(mcode);
	}
}
void addc(Data* srcl, Data* srcr, Data* &des) {
	static string op = "+";
	int move = 0;
	des = new Data;
	Mcode mcode;
	if (ISCONST(srcl) && ISCONST(srcr) && OPT >= 1) {
		int l, r;
		GETCONS(srcl, l);
		GETCONS(srcr, r);
		des = new Data(CONSADD, (void*)(l + r));
	}
	else if (OPT >= 1 && ISZERO(srcl))
		des = srcr;
	else if (OPT >= 1 && ISZERO(srcr))
		des = srcl;
	else {
		des = new Data(MVARADD, (void*)(dept++));
		mcode = Mcode(ADDOP, des, srcl, srcr);
		mcode.genS(des->getS() + "=" + srcl->getS() + op + srcr->getS());
		mcodlis.push(mcode);
	}
	/*gRD("add", des, srcl, srcr);*/
}
void subc(Data* srcl, Data* srcr, Data* &des) {
	static string op = "-";
	int move = 0;
	des = new Data;
	Mcode mcode;
	if (ISCONST(srcl) && ISCONST(srcr) && OPT >= 1) {
		int l, r;
		GETCONS(srcl, l);
		GETCONS(srcr, r);
		des = new Data(CONSADD, (void*)(l - r));
	}
	else if (OPT >= 1 && ISZERO(srcl))
		des = srcr;
	else if (OPT >= 1 && ISZERO(srcr))
		des = srcl;
	else {
		des = new Data(MVARADD, (void*)(dept++));
		mcode = Mcode(SUBOP, des, srcl, srcr);
		mcode.genS(des->getS() + "=" + srcl->getS() + op + srcr->getS());
		mcodlis.push(mcode);
	}

	/*gRD("sbu", des, srcl, srcr);*/
}
void multc(Data* srcl, Data* srcr, Data*& des) {
	static string op = "*";
	int move = 0;
	des = new Data;
	Mcode mcode;
	if (ISCONST(srcl) && ISCONST(srcr) && OPT >= 1) {
		int l, r;
		GETCONS(srcl, l);
		GETCONS(srcr, r);
		des = new Data(CONSADD, (void*)(l * r));
	}
	else if ((ISZERO(srcl)||ISZERO(srcr)) && OPT >= 1) {
		des = new Data(CONSADD, 0);
	}
	else if (ISCONST(srcl) && ispow(srcl->cont.cons)!=-1) {
		int x = ispow(srcl->cont.cons);
		slc(SLLOP, srcr, new Data(CONSADD, (void*)x),des);
	}
	else if (ISCONST(srcr) && ispow(srcr->cont.cons) != -1) {
		int x = ispow(srcr->cont.cons);
		slc(SLLOP, srcl, new Data(CONSADD, (void*)x), des);
	}
	else {
		des = new Data(MVARADD, (void*)(dept++));
		mcode = Mcode(MULOP, des, srcl, srcr);
		mcode.genS(des->getS() + "=" + srcl->getS() + op + srcr->getS());
		mcodlis.push(mcode);
	}
	/*gRD("mulo", des, srcl, srcr);*/
}
void divc(Data* srcl, Data* srcr, Data* & des) {
	static string op = "/";
	int move = 0;
	des = new Data;
	Mcode mcode;
	if (ISCONST(srcl) && ISCONST(srcr) && OPT >= 1) {
		int l, r;
		GETCONS(srcl, l);
		GETCONS(srcr, r);
		des = new Data(CONSADD, (void*)(l / r));
	}
	else {
		des = new Data(MVARADD, (void*)(dept++));
		mcode = Mcode(DIVOP, des, srcl, srcr);
		mcode.genS(des->getS() + "=" + srcl->getS() + op + srcr->getS());
		mcodlis.push(mcode);
	}
	
	/*gRD("div", des, srcl, srcr);*/
}
void readc(Data *des) {
	/*
	 readSen()
	*/
	static string op = "read ";
	Mcode mcode = Mcode(READOP,des,0);
	mcode.genS(op + des->getS());
	mcodlis.push(mcode);
	
}
void printc(Data* srcl) {
	static string op = "print ";
	Mcode mcode = Mcode(PRINTOP, 0, srcl);		
	mcode.genS(op + srcl->getS());
	mcodlis.push(mcode);
	
}
void assignc(Data* srcl, Data*& des) {
	static string op = "=";
	Mcode mcode = Mcode(ASSIGNOP,des,srcl);
	mcode.genS(des->getS() + op + srcl->getS());
	mcodlis.push(mcode);
	
}
void inic(Data* srcl) {
	static string op = "ini ";
	Mcode mcode = Mcode(INIOP,0,srcl);
	mcode.genS(op + srcl->getS());
	mcodlis.push(mcode);
	
}
void pushc(Data* des, Data* srcl) {
	static string op = "push ";
	Mcode mcode = Mcode(PUSHOP,des,srcl);
	mcode.genS(op + srcl->getS()); // s形式只输出一部分
	mcodlis.push(mcode);
	
}
void loadc(Data* srcl, Data* srcr, Data*& des) {
	static string op = "load";
	des = new Data(MVARADD,(void*)(dept++));
	Mcode mcode = Mcode(LOADOP, des, srcl,srcr);
	//只需要srcl即可生成函数
	mcode.genS(op + " " + des->getS() + " from " + srcl->getS());
	mcodlis.push(mcode);
}
void callc(symbol* symb,Data *&des) {
	/*
	des:生成变量
	srcl:处理函数
	*/
	static string op = "call ";
	Data* srcl = new Data(VARADD, symb);
	Mcode mcode;
	if (symb->type != VTYPE) {
		des = new Data(MVARADD, (void*)dept++);
		mcode = Mcode(CALLOP, des, srcl);
		mcode.genS(des->getS()+"= "+op + srcl->getS());
	}	
	else {
		des = 0;
		mcode = Mcode(CALLOP, 0, srcl);
		mcode.genS(op + srcl->getS());
	}
		
	mcodlis.push(mcode);
}
void casec(Data* srcl, Data* srcr, Data *des) {
	Mcode mcode = Mcode(CASEOP,des,srcl,srcr);
	mcode.genS(srcl->getS() + " != " + srcr->getS() + " goto " + des->getS());
	mcodlis.push(mcode);
}
void condjc(int op, Data* srcl, Data* srcr, Data* des) {
	Mcode mcode;
	string sop;
	switch (op)
	{
	case EQL: 
		sop = "!=";
		mcode = Mcode(JNEOP,des,srcl,srcr);
		break;
	case NEQ:
		sop = "==";
		mcode = Mcode(JEQOP,des,srcl,srcr); 
		break;

	case GRE: 
		sop = "<=";
		mcode = Mcode(JLEOP, des, srcl, srcr); 
		break;
	case LEQ:
		sop = ">";
		mcode = Mcode(JGTOP, des, srcl, srcr);
		break;

	case LSS:
		sop = ">=";
		mcode = Mcode(JGEOP, des, srcl, srcr);
		break;
	case GEQ:
		sop = "<";
		mcode = Mcode(JLTOP, des, srcl, srcr);
		break;
	}
	mcode.genS(srcl->getS()+" "+sop+" "+srcr->getS() + " goto "+des->getS());
	mcodlis.push(mcode);
}
void newcondjc(Mcode mcode, string lab) {
	map<int, int> joma = {
	{JNEOP,JEQOP},{JEQOP,JNEOP},
	{JGEOP,JLTOP},{JLTOP,JGEOP},
	{JGTOP,JLEOP},{JLEOP,JGTOP}
	};
	map<int, string> jsma = {
		{JNEOP,"!="},{JEQOP,"=="},
		{JGEOP,">="},{JLTOP,"<"},
		{JGTOP,">"},{JLEOP,"<="}
	};
	mcode.op = joma[mcode.op];
	mcode.des = new Data(LABADD, 0);
	mcode.des->s = lab;
	mcode.genS(mcode.srcl->getS() + " " + jsma[mcode.op] + " " + mcode.srcr->getS() + " goto " + mcode.des->getS());
	mcodlis.push(mcode);
}
void jumpc(Data* des) {
	Mcode mcode = Mcode(JOP, des,0);
	mcode.genS("goto " + des->getS());
	mcodlis.push(mcode);

}
void declarec(Data* srcl) {
	dept = 0;
	Mcode mcode = Mcode(DECLAREOP, 0, srcl);
	mcode.genS("declare " + srcl->getS());
	mcodlis.push(mcode);
}
void retc(Data* srcl,Data *srcr) {
	/*
	srcl代表返回值,srcr代表函数
	*/
	Mcode mcode = Mcode(RETOP, 0, srcl,srcr);
	if (srcl) mcode.genS("ret " + srcl->getS());
	else mcode.genS("ret null");
	mcodlis.push(mcode);
}
void glabc(string s) {
	Mcode mcode;
	mcode.op = LABELOP;
	mcode.genS(s);
	mcodlis.push(mcode);
}

void endc() {
	Mcode mcode;
	mcode.op = ENDOP;
	mcode.genS("**end**");
	mcodlis.push(mcode);
}
