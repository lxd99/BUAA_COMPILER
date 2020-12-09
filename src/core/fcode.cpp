#define  _CRT_SECURE_NO_WARNINGS
#include "symb.h"
#include "mcode.h"
#include "fcode.h"
extern McodeList mcodlis;
extern symbtable symtab;
insLis gdeci,maini,funi;
extern map<string, int> strtab;
static char sbuf[150];
static int pos = 0;
static int waste= 0;
static int tid=0;
symbol* nowsymb;
static string dline = "_";
string opi[23] = { "neg","add","sub","mul","div","jr","sw","sw","bge","bne","beq",
"ble","bgt","blt","j","xxx","xxx","xx","jal","xx","xx","xx","bne" };


#define ISARR(x) ((x)->type==VARADD&&(x)->cont.symb->form == ARRAYF)
#define CHECKN(mcode) ((mcode).srcr->type==MVARADD||\
			(ISARR((mcode).srcr)&& (mcode).srcr->off->type == MVARADD ))
#define ISJUMP(x) ((x)==JOP||(x)==JGTOP||(x)==JGEOP||(x)==JLEOP||(x)==JLTOP||(x)==JNEOP||(x)==JEQOP)
#define ENDLAB (PRELAB + dline + "end" + dline + PRELAB)
#define NLINE do{ \
	insl->push("li $a0 10");\
	insl->push("li $v0 11");\
	insl->push("syscall");\
}while(0)
#define LIM 6
#define GENBUF(s,x) do{\
	s = PRELAB;\
	s += "_buf";\
	if(x==1) s+=":";\
}while(0)
#define GENFLAB(x) (PRELAB+dline+(x))
void error(Mcode mcode) {
	printf("\nerror at:%s\n", mcode.s.c_str());
}
Mcode nextC() {
	static int len = mcodlis.getSize() - 1;
	if (pos < len)
		return mcodlis.get(pos++);
	else return mcodlis.get(pos);
}
static Mcode back() {
	if (pos > 0) return mcodlis.get(--pos);
	else return mcodlis.get(pos);
}
void outI() {
	gdeci.outI();
	maini.outI();
	funi.outI();
}
void enterFun(insLis * insl ,int size) {
	/*进入函数中间函数*/
	string s;
	SAPP(0, s, "addiu $sp $sp -%d", size + 8);
	insl->push(s);
	SAPP(0, s, "sw $ra %d($sp)", size + 4);
	insl->push(s);
	SAPP(0, s, "sw $fp %d($sp)", size );
	insl->push(s);
	insl->push("move $fp $sp");
	for (int i = 0; i <= tid; i++) {
		SAPP(0,s,"$sw $t%d -%d($sp)", tid,4*(tid+1));
		insl->push(s);
	}
	SAPP(0, s, "addi $sp $sp -%d", 4 * (tid + 1));
	insl->push(s);
}
void LoadReg(insLis* insl,Data data,string des,string &reg,int pattern=0,int keep=0) { 
	/*读取地址所在地的数值到reg*/
	string s;
	reg = des;
	if (data.type == CONSADD) {
		SAPP(0, s, "li %s %d", des.c_str(), data.cont.cons);
		insl->push(s);
	}
	else if (data.type == MVARADD) {
		if (data.cont.varid <= LIM) {
			if (pattern == 0) //0型pattern
				SAPP(0, reg, "t_%d", data.cont.varid);
			else {
				SAPP(0, s, "move %s t_%d", des.c_str(),data.cont.varid);
				insl->push(s);
			}
				
		}
		else {
			SAPP(0, s, "lw %s ($sp)", des.c_str());
			insl->push(s);
			if(!keep)
				insl->push("addiu $sp $sp 4");
		}

	}
	else if (data.type == VARADD) {
		symbol* symb = data.cont.symb;
		if (symb->form == VARF) {
			if (symb->scope == 0) 
				SAPP(0, s, "lw %s %s", des.c_str(), symb->wd.s.c_str());
			else 
				SAPP(0, s, "lw %s %d($fp)", des.c_str(),symb->pos);
			insl->push(s);
		}
		else if (symb->form == CONSF) {
			SAPP(0,s,"li %s %d", des.c_str(), symb->value.var);
			insl->push(s);
		}
		else if (symb->form == ARRAYF) { //数组
			LoadReg(insl, *data.off, des,des,pattern,keep);
			insl->push("sll "+des+ " "+ des +" 2");
			if (symb->scope == 0)
				SAPP(0, s, "lw %s %s(%s)", des.c_str(), symb->wd.s.c_str(), des.c_str());
			else {
				SAPP(0,s,"addiu %s %s %d", des.c_str(), des.c_str(), symb->pos);
				insl->push(s);
				SAPP(0, s, "addu %s %s $fp", des.c_str(), des.c_str());
				insl->push(s);
				SAPP(0, s, "lw %s (%s)",des.c_str(),des.c_str(),des.c_str());
			}
				
			insl->push(s);
		}
	}
	else
		printf("load cont error!!");
	
}
void store(insLis * insl,string data) {
	/*入栈中间函数&&消栈*/
	string s;
	if (data[0] == 't');
	else if(data=="$v0"){
		insl->push("addi $sp $sp -4");
		insl->push("sw " + data + " 0($sp)");
	}
	else 
		printf("store error\n");
}
void parseExp(insLis* insl,Mcode mcode) {
	/*
	multc() addc() subc() divc()
	*/
	int back = 0;
	string reg1, reg2,rdes;
	LoadReg(insl,*mcode.srcr,"$t7",reg2);
	LoadReg(insl, *mcode.srcl,"$v0",reg1);
	if (mcode.des->cont.varid <= LIM)
		rdes = string("$t") + TRANS(mcode.des->cont.varid);
	else
		rdes = "$v0";
	insl->push(opi[mcode.op] + rdes +" "+reg1 +" "+reg2);
	store(insl,rdes);
}
void parseDeclare(insLis * insl,Mcode mcode) {
	/*
	declarec()
	*/
	int size = symtab.getVarSize(nowsymb);
	insl->push(GENFLAB(nowsymb->wd.s) + ":");
	string s;
	SAPP(0, s, "addiu $sp $sp -%d", size + 8);
	insl->push(s);
	SAPP(0, s, "sw $ra %d($sp)", size + 4);
	insl->push(s);
	SAPP(0, s, "sw $fp %d($sp)", size);
	insl->push(s);

	insl->push("move $fp $sp");
	tid = 0;
}
void parseAss(insLis* insl,Mcode mcode) {
	/*
	assignc()
	*/
	string s,reg1,reg2,rdes;
	LoadReg(insl,*mcode.srcl,"$t7",reg1);
	Data* des = mcode.des;
	symbol* symb = des->cont.symb;
	if (symb->form == ARRAYF) {
		LoadReg(insl, *des->off, "$v0",reg2);
		insl->push("sll" + reg2+" "+reg2 +" 2");

		if(symb->scope==0)
			SAPP(0, s, "sw %s %s(%s)", reg1.c_str(),symb->wd.s.c_str(),reg2.c_str());
		else {
			s =  "add "+ reg2+ " "+reg2 +" $fp";
			insl->push(s);
			SAPP(0, s, "sw %s %d(%s)",symb->pos,reg1.c_str(),reg2.c_str());
		}
		insl->push(s);
	}
	else {
		if (symb->scope == 0)
			SAPP(0, s, "sw %s %s", reg1.c_str(),symb->wd.s.c_str());
		else
			SAPP(0, s, "sw %s %d($fp)", reg1.c_str(),symb->pos);
		insl->push(s);
	}
}
void parseRead(insLis* insl,Mcode mcode) {
	/*
	readc()
	*/
	string s;
	symbol* symb = mcode.des->cont.symb;
	if (mcode.des->type == VARADD) {

		if (symb->type == CTYPE) {
			insl->push("li $v0 8");
			insl->push("li $a1 3");
			GENBUF(s,0);
			insl->push("la $a0 " + s);
			insl->push("syscall");
			insl->push("lb $v0 " + s);
		}
		else if (symb->type == ITYPE) {
			insl->push("li $v0 5");
			insl->push("syscall");
		}
		if (symb->scope == 0)
			SAPP(0, s, "sw $v0 %s", symb->wd.s.c_str());
		else
			SAPP(0, s, "sw $v0 %d($fp)", symb->pos);
		insl->push(s);
	}
	else printf("errorRead");
}
void parsePrint(insLis* insl, Mcode mcode) {
	/*
	printc()
	//need_back_up
	*/
	string s,reg1;
	if (mcode.srcl->type == STRADD) { //str地址
		s = "la $a0 ";
		s += PRELAB;
		SAPP(1,s,"_%d",mcode.srcl->cont.sid);
		insl->push(s);
		insl->push("li $v0 4");
	}
	else if(mcode.srcl->isinf.isc==0){
		LoadReg(insl, *mcode.srcl, "$a0", reg1,1);
		insl->push("li $v0 1");
	}
	else if(mcode.srcl->isinf.isc){
		LoadReg(insl, *mcode.srcl, "$a0", reg1,1);
		insl->push("li $v0 11");
	}
	else printf("errorPrint");
	insl->push("syscall");
	if(mcode.srcl->isinf.ise)
		NLINE;
}
void parseIni(insLis* insl,Mcode mcode) {
	/*
	inic() && 局部初始化变量
	*/
	symbol* symb = mcode.srcl->cont.symb;
	string s;
	if (symb->isIni) {
		if (symb->form == VARF) {
			SAPP(0, s, "li $v0 %d", symb->value.var);
			insl->push(s);
			SAPP(0, s, "sw $v0 %d($fp)",symb->pos);
			insl->push(s);
		}
		else if(symb->form==ARRAYF){
			int* arry = symb->getArray();
			int size = symb->getSize();
			for (int i = 0; i < size; i += 4) {
				SAPP(0, s, "li $v0 %d", arry[i/4]);
				insl->push(s);
				SAPP(0, s, "sw $v0 %d($fp)", i + symb->pos);
				insl->push(s);
			}
		}
	}
}
void parseNeg(insLis* insl, Mcode mcode) {
	/*
	negc()
	*/
	int back = 0;
	string reg,rdes;
	if (mcode.des->cont.varid <= LIM)
		rdes = string("$t") + TRANS(mcode.des->cont.varid);
	else
		rdes = "$v0";
	LoadReg(insl, *mcode.srcl, "$v0",reg);
	insl->push("neg " + rdes +" "+reg);
	store(insl,rdes);
}
void parseCase(insLis* insl, Mcode mcode) {
	string reg1, reg2, rdes;
	LoadReg(insl, *mcode.srcr, "$v0", reg2, 0, 1);
	LoadReg(insl, *mcode.srcl, "$t7", reg1);
	insl->push("bne " + reg1 + " " + reg2 + " " + mcode.des->s);

}
void parseJump(insLis* insl,Mcode mcode) {
	/*
	condjc() jumpc()
	*/
	int back = 0;
	string ms,reg1,reg2;
	if (mcode.op == JOP) 
		insl->push(opi[JOP] + " " + mcode.des->s);
	else {
		LoadReg(insl, *mcode.srcr, "$v0",reg2);
		LoadReg(insl, *mcode.srcl, "$t7",reg1);
		insl->push(opi[mcode.op]+" "+reg1 +" "+reg2 + " "+ mcode.des->s);
	}
}
void parseCall(insLis* insl ,Mcode mcode) {
	/*
	callc()
	*/
	insl->push(opi[mcode.op] +" "+ GENFLAB(mcode.srcl->s));
	if (mcode.des != 0) {
		if (mcode.des->cont.varid > LIM) {
			insl->push("addiu $sp $sp -4");
			insl->push("sw $v0 ($sp)");
		}
		else {
			string s;
			SAPP(0, s, "move $t_%d $v0", mcode.des->cont.varid);
		}

	}
}
void parsePush(insLis* insl,Mcode mcode) {
	/*
	pushc()
	*/
	string ms,reg1;
	if (mcode.srcl->type != MVARADD) {
		LoadReg(insl, *mcode.srcl, "$v0", reg1,1);
		SAPP(0, ms, "addiu $sp $sp %d", -4);
		insl->push(ms);
		insl->push("sw $v0 ($sp)");
	}
	else {
		insl->push("addiu $sp $sp -4");
		if (mcode.srcl->cont.varid <= LIM) {
			SAPP(0, ms, "sw $t_%d ($sp)", mcode.srcl->cont.varid);
		}
	}
}
void parseRet(insLis* insl, Mcode mcode) {
	/*
	retc()
	*/
	int sizev = symtab.getVarSize(mcode.srcr->cont.symb);
	int sizep = symtab.getParSize(mcode.srcr->cont.symb);
	int back = 0;
	string ms,reg;
	if (mcode.srcl) 
		LoadReg(insl, *mcode.srcl, "$v0", reg,1);
	
	for (int i = 0; i <=LIM; i++) {
		SAPP(0, ms, "lw $t%d -%d($fp)", i, (i + 1) * 4);
		insl->push(ms);
	}	
	SAPP(0, ms, "lw $ra %d($fp)", sizev + 4);
	insl->push(ms);
	SAPP(0, ms, "add $sp $fp %d", sizev +sizep+ 8);
	insl->push(ms);
	SAPP(0, ms, "lw $fp %d($fp)",sizev);
	insl->push(ms);
	insl->push("jr $ra");
}
void parseLab(insLis* insl, Mcode mcode) {
	/*
	glabc()
	*/
	if (mcode.move != 0) {
		string s;
		SAPP(0, s, "addiu $sp $sp %d", mcode.move*4);
		insl->push(s);
	}
	insl->push(mcode.s+":");
}
void parseEnd(insLis* insl, Mcode mcode) {
	insl->push(ENDLAB+":");
}
void parseCode(insLis* insl, Mcode mcode) {
	/*解析*/
	if (FDEBUG == 1)
		printf("%s\n",mcode	.s.c_str());
	if (mcode.op == ADDOP || mcode.op == SUBOP || mcode.op == MULOP || mcode.op == DIVOP) {
		parseExp(insl, mcode);
	}
	else if (mcode.op == NEGOP) {
		parseNeg(insl, mcode);
	}
	else if (mcode.op == ASSIGNOP) {
		parseAss(insl, mcode);
	}
	else if (mcode.op == CALLOP) {
		parseCall(insl, mcode);
	}
	else if (mcode.op == RETOP) {
		parseRet(insl, mcode);
	}
	else if (ISJUMP(mcode.op)) {
		parseJump(insl, mcode);
	}
	else if (mcode.op == PRINTOP) {
		parsePrint(insl, mcode);
	}
	else if (mcode.op == READOP) {
		parseRead(insl, mcode);
	}
	else if (mcode.op == LABELOP) {
		parseLab(insl, mcode);
	}
	else if (mcode.op == INIOP) {
		if (mcode.srcl->cont.symb->scope != 0)
			parseIni(insl, mcode);
	}
	else if (mcode.op == DECLAREOP) {
		nowsymb = mcode.srcl->cont.symb;
		parseDeclare(insl, mcode);
	}
	else if (mcode.op == ENDOP) {
		parseEnd(insl, mcode);
	}
	else if (mcode.op == PUSHOP)
		parsePush(insl, mcode);
	else if (mcode.op == CASEOP)
		parseCall(insl, mcode);
	else error(mcode);
}
void gDeclare() { //生产固定区域
	/*
	inic() 
	*/
	vector<symbol*> vec = symtab.getVars(0);
	gdeci.push(".data");
	string s;
	for (auto i : vec) { //全局变量
		s = "";
		if (i->form == VARF ) {
			if (i->isIni)
				SAPP(0, s, "%s: .word %d", i->wd.s.c_str(), i->value.var);
			else
				SAPP(0, s, "%s: .space 4", i->wd.s.c_str());
		}
		else if(i->form ==ARRAYF){

			SAPP(0, s, "%s: ", i->wd.s.c_str());
			auto arr = i->getArray();
			int size = i->getSize();
			if (i->isIni) {
				SAPP(1, s, ".word ");
				for (int j = 0; j * 4 < size; j++)
					SAPP(1, s, "%d ", arr[j]);
			}
			else
				SAPP(1, s, ".space %d", size);

		}
		if(s!="") gdeci.push(s);
	}
	gdeci.push(".asciiz");
	for (auto i : strtab) {
		s = PRELAB;
		SAPP(1, s, "_%d: ", i.second);
		s.push_back('"');
		for (auto j : i.first) {
			if (j == '\\') s.push_back('\\');
			s.push_back(j);
		}
		s.push_back('"');
		gdeci.push(s);
	}
	
	GENBUF(s,1);
	gdeci.push(s);

}
void gMain() {
	maini.push(".text");
	maini.push(string("la $ra ")+ENDLAB);
	Mcode mcode;
	while (true)
	{
		mcode = nextC();
		parseCode(&maini, mcode);
		if (mcode.op == ENDOP) break;
	}
}
void gFun() {
	funi.push("li $v0 10");
	funi.push("syscall");
	string s;
	Mcode mcode;
	while (true) {
		mcode = nextC();
		if (mcode.op == DECLAREOP && mcode.srcl->cont.symb->wd.ss=="main") {
			back();
			break;
		}
		parseCode(&funi, mcode);
	}
}
void genFCode() {
	gDeclare();
	gFun();
	gMain();
}