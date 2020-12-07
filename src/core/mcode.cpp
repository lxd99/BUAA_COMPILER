#define  _CRT_SECURE_NO_WARNINGS
#include "mcode.h"
#define TMAX 7
/*只能用于mcode中的代码*/
#define GETDMOVE do \
{ \
	if((srcl)->type != MVARADD &&(srcr)->type != MVARADD)\
		move = 1;\
	else if((srcl)->type==MVARADD && (srcr)->type==MVARADD)\
		move = -1;\
	else move = 0;\
}while(0)
int tid = 0;
int dept = 0;
McodeList	 mcodlis;
void outC() {
	mcodlis.outC();
}
//true code
void negc(Data *srcl,Data *&des) {
	static string op = "-";
	/*<负运算>*/
	Data* mdes = new Data;
	Mcode mcode;
	int move = 0;
	if (dept == 0) move = 1;
	MODKVAR(mdes, move - 1);

	mcode = Mcode(NEGOP, mdes, srcl);
	mcode.genS(mdes->getS() + "=" + op + srcl->getS());
	mcodlis.push(mcode);
	des = mdes;
	/*gRS("neg", des, src);*/
}
void addc(Data* srcl, Data* srcr, Data* &des) {
	static string op = "+";
	int move = 0;
	Data* mdes = new Data;
	Mcode mcode;
	GETDMOVE;
	MODKVAR(mdes, move - 1);

	mcode = Mcode(ADDOP, mdes,srcl, srcr);
	mcode.genS(mdes->getS() + "=" + srcl->getS() + op + srcr->getS());
	mcodlis.push(mcode);
	
	des = mdes;
	
	/*gRD("add", des, srcl, srcr);*/
}
void subc(Data* srcl, Data* srcr, Data* &des) {
	static string op = "-";
	int move = 0;
	Data* mdes = new Data;
	Mcode mcode;
	GETDMOVE;
	MODKVAR(mdes, move - 1);

	mcode = Mcode(SUBOP, mdes,srcl, srcr);
	mcode.genS(mdes->getS() + "=" + srcl->getS() + op + srcr->getS());
	mcodlis.push(mcode);
	des = mdes;
	/*gRD("sbu", des, srcl, srcr);*/
}
void multc(Data* srcl, Data* srcr, Data*& des) {
	static string op = "*";
	int move = 0;
	Data* mdes = new Data;
	Mcode mcode;
	GETDMOVE;
	MODKVAR(mdes, move - 1);

	mcode = Mcode(MULOP, mdes,srcl, srcr);
	mcode.genS(mdes->getS() + "=" + srcl->getS() + op + srcr->getS());
	mcodlis.push(mcode);
	des = mdes;
	/*gRD("mulo", des, srcl, srcr);*/
}
void divc(Data* srcl, Data* srcr, Data* & des) {
	static string op = "/";
	int move = 0;
	Data* mdes = new Data;
	Mcode mcode;
	GETDMOVE;
	MODKVAR(mdes, move - 1);

	mcode = Mcode(DIVOP, mdes,srcl, srcr);
	mcode.genS(mdes->getS() + "=" + srcl->getS() + op + srcr->getS());
	mcodlis.push(mcode);
	des = mdes;
	
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
	if (des->type == MVARADD) 
		dept -= dept > 0 ? 1 : 0;
	Mcode mcode = Mcode(PUSHOP,des,srcl);
	mcode.genS(op + srcl->getS()); // s形式只输出一部分
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
void jumpc(Data* des) {
	Mcode mcode = Mcode(JOP, des,0);
	mcode.genS("goto " + des->getS());
	mcodlis.push(mcode);

}
void declarec(Data* srcl) {
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
