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
symbol* nowsymb;
static string dline = "_";
string opi[27] = { "neg","addu","subu","mul","div","jr","sw","sw","bge","bne","beq","ble",
"bgt","blt","j","xxx","xxx","xx","jal","xx","xx","xx","bne","move","xxx","sll","srl" };
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
#define LIM 7
//修改LIM-->stroe&$gp需要修改
#define MNUM 800
#define GENBUF(s,x) do{\
	s = PRELAB;\
	s += "_buf";\
	if(x==1) s+=":";\
}while(0)
#define GENFLAB(x) (PRELAB+dline+(x))
string reg[2];
void error(Mcode mcode) {
	printf("\nerror at:%s\n", mcode.s.c_str());
}
Mcode fnextC() {
	static int len = mcodlis.getSize() - 1;
	if (pos < len)
		return mcodlis.get(pos++);
	else return mcodlis.get(pos);
}
void outI() {
	gdeci.outI();
	maini.outI();
	funi.outI();
}
void getiop(int op,string &s) {
	if (op == ADDOP) s = "addiu";
	if (op == SUBOP) s = "subiu";
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
}
void LoadReg(insLis* insl,Data data,int pos,int pattern=0) { 
	/*读取地址所在地的数值到reg*/
	string s;
	static string ini[2] = {"$t8","$t9"};
	//注意assignc中有隐式绑定
	if (pattern == 0) reg[pos] = ini[pos];
	if (data.type == CONSADD) {
		SAPP(0, s, "li %s %d", reg[pos].c_str(), data.cont.cons);
		insl->push(s);
	}
	else if (data.type == MVARADD) {
		//特判load
		if (data.cont.varid <= LIM) {
			if (pattern == 0) 
				reg[pos] = "$t" + to_string(data.cont.varid);
			else {
				SAPP(0, s, "move %s $t%d", reg[pos].c_str(),data.cont.varid);
				insl->push(s);
			}
				
		}
		else {
			SAPP(0, s, "lw %s %d($gp)", reg[pos].c_str(), data.cont.varid * 4);
			insl->push(s);
		}
				
	}
	else if (data.type == VARADD) {
		symbol* symb = data.cont.symb;
		if (symb->form == VARF) {
			if (symb->scope == 0) 
				SAPP(0, s, "lw %s %s", reg[pos].c_str(), symb->wd.s.c_str());
			else 
				SAPP(0, s, "lw %s %d($fp)", reg[pos].c_str(),symb->pos);
			insl->push(s);
		}
		else if (symb->form == CONSF) {
			SAPP(0,s,"li %s %d", reg[pos].c_str(), symb->value.var);
			insl->push(s);
		}
		else if (symb->form == ARRAYF) { //数组
			insl->push("error: an array not load");
			//LoadReg(insl, *data.off, reg, des, 1, keep);
			//insl->push("sll " + reg + " " + reg + " 2");
			//if (symb->scope == 0)
			//	SAPP(0, s, "lw %s %s(%s)", reg.c_str(), symb->wd.s.c_str(), reg.c_str());
			//else {
			//	SAPP(0, s, "addiu %s %s %d", reg.c_str(), reg.c_str(), symb->pos);
			//	insl->push(s);
			//	SAPP(0, s, "addu %s %s $fp", reg.c_str(), reg.c_str());
			//	insl->push(s);
			//	SAPP(0, s, "lw %s (%s)", reg.c_str(), reg.c_str(), reg.c_str());
			//}

			//insl->push(s);
		}
	}
	else {
		printf("load cont error!!");
	}
		
	
}
void store(insLis * insl,string reg,Data *des) {
	/*入栈中间函数&&消栈*/
	string s;
	int id=0;
	if (des->type == MVARADD)
		id = des->cont.varid;
	SAPP(0, s, "sw %s %d($gp)",reg.c_str(), id*4);
	if (reg[1] == 't'&&des->type==MVARADD) {
		if (id > LIM) {
			insl->push(s);
		}
			
	}
	else {
		/*printf("store error\n");*/
	}
		
}
void parseExp(insLis* insl,Mcode mcode) {
	/*f
	multc() addc() subc() divc() sllc() srlc()
	*/
	int back = 0;
	string rdes,sop=opi[mcode.op];
	LoadReg(insl, *mcode.srcl, 0);
	if (mcode.srcr->type==CONSADD) {
		reg[1] = to_string(mcode.srcr->cont.varid);
		getiop(mcode.op,sop);
	}
	else 
	LoadReg(insl,*mcode.srcr,1);
	SAPP(0, rdes, "$t%d", mcode.des->cont.varid);
	if(mcode.des->cont.varid>LIM)
		rdes = "$t8";
	insl->push(sop + " "+rdes +" "+reg[0] +" "+reg[1]);
	store(insl,rdes,mcode.des);
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

	nowsymb = mcode.srcl->cont.symb;
	if (nowsymb->wd.ss == "main") return;
	for (int i = 0; i <= nowsymb->mVar; i++) {
		if (i <= LIM) {
			SAPP(0, s, "sw $t%d -%d($sp)", i, 4 * (i+1));
			insl->push(s);
		}
		else {
			SAPP(0, s, "lw $t8 %d($gp)", 4 * i);
			insl->push(s);
			SAPP(0, s, "sw $t8 -%d($sp)", 4 * (i+1));
			insl->push(s);
		}
	}
	if (nowsymb->mVar >= 0)
		insl->push("addiu $sp $sp -" + to_string(4*(1+nowsymb->mVar)));
}
void parseAss(insLis* insl,Mcode mcode) {
	/*
	assignc()
	*/
	string s,rdes;
	LoadReg(insl,*mcode.srcl,0);
	Data* des = mcode.des;
	symbol* symb = des->cont.symb;
	if (symb->form == ARRAYF) {
		LoadReg(insl, *des->off, 1);
		if(symb->scope==0)
			SAPP(0, s, "sw %s %s(%s)", reg[0].c_str(),symb->wd.s.c_str(),reg[1].c_str());
		else {
			s =  "addu $t9 "+reg[1] +" $fp";
			insl->push(s);
			SAPP(0, s, "sw %s %d($t9)", reg[0].c_str(),symb->pos);
		}
		insl->push(s);
	}
	else {
		if (symb->scope == 0)
			SAPP(0, s, "sw %s %s", reg[0].c_str(),symb->wd.s.c_str());
		else
			SAPP(0, s, "sw %s %d($fp)", reg[0].c_str(),symb->pos);
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
		reg[0] = "$a0";
		LoadReg(insl, *mcode.srcl,0,1);
		insl->push("li $v0 1");
	}
	else if(mcode.srcl->isinf.isc){
		reg[0] = "$a0";
		LoadReg(insl, *mcode.srcl,0,1);
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
	string rdes;
	SAPP(0, rdes, "$t%d", mcode.des->cont.varid);
	if (mcode.des->cont.varid > LIM)
		rdes = "$t8";
	LoadReg(insl, *mcode.srcl, 0);
	insl->push("neg " + rdes +" "+reg[0]);
	store(insl,rdes,mcode.des);
}
void parseCase(insLis* insl, Mcode mcode) {
	/*
	casec()
	*/
	LoadReg(insl, *mcode.srcl, 0);
	LoadReg(insl, *mcode.srcr, 1);
	insl->push("bne " + reg[0] + " " + reg[1] + " " + mcode.des->s);
	if (mcode.srcl->type == MVARADD && mcode.srcl->cont.varid > LIM)
		insl->push("addiu $sp $sp 4");

}
void parseJump(insLis* insl,Mcode mcode) {
	/*
	condjc() jumpc()
	*/
	int back = 0;
	string ms;
	if (mcode.op == JOP) 
		insl->push(opi[JOP] + " " + mcode.des->s);
	else {
		LoadReg(insl, *mcode.srcl, 0);
		LoadReg(insl, *mcode.srcr, 1);
		insl->push(opi[mcode.op]+" "+reg[0] +" "+reg[1] + " "+ mcode.des->s);
	}
}
void parseCall(insLis* insl ,Mcode mcode) {
	/*
	callc()
	*/
	insl->push(opi[mcode.op] +" "+ GENFLAB(mcode.srcl->s));
	if (mcode.des != 0) {
		if (mcode.des->cont.varid > LIM) {
			insl->push("sw $v0 "+to_string(mcode.des->cont.varid*4)+"($gp)");
		}
		else {
			string s;
			SAPP(0, s, "move $t%d $v0", mcode.des->cont.varid);
			insl->push(s);
		}

	}
}
void parsePush(insLis* insl,Mcode mcode) {
	/*
	pushc()
	*/
	string ms;
	LoadReg(insl, *mcode.srcl, 0);
	SAPP(0, ms, "addiu $sp $sp %d", -4);
	insl->push(ms);
	SAPP(0, ms, "sw %s ($sp)", reg[0].c_str());
	insl->push(ms);
}
void parseRet(insLis* insl, Mcode mcode) {
	/*
	retc()
	*/
	int sizev = symtab.getVarSize(mcode.srcr->cont.symb);
	int sizep = symtab.getParSize(mcode.srcr->cont.symb);
	int back = 0;
	string s;
	if (mcode.srcl) {
		reg[0] = "$v0";
		LoadReg(insl, *mcode.srcl, 0, 1);
	}
	SAPP(0, s, "lw $ra %d($fp)", sizev + 4);
	insl->push(s);
	SAPP(0, s, "addiu $sp $fp %d", sizev +sizep+ 8);
	insl->push(s);
	if (nowsymb->wd.ss != "main") {
		for (int i = 0; i <= nowsymb->mVar; i++) {
			if (i <= LIM) {
				SAPP(0, s, "lw $t%d -%d($fp)", i, 4 * (i + 1));
				insl->push(s);
			}
			else {
				SAPP(0, s, "lw $t8 -%d($fp)", 4 * (i + 1));
				insl->push(s);
				SAPP(0, s, "sw $t8 %d($gp)", 4 * i);
				insl->push(s);
			}
		}
	}
	SAPP(0, s, "lw $fp %d($fp)", sizev);
	insl->push(s);
	insl->push("jr $ra");
}
void parseLoad(insLis* insl, Mcode mcode) {
	/*
	loadc()
	notice:  0对应于$t8,1对应于$t9
	*/
	int id = mcode.des->cont.varid;
	symbol* symb = mcode.srcl->cont.symb;
	string rdes,s;
	LoadReg(insl, *mcode.srcr, 0);
	if (id <= LIM) SAPP(0, rdes, "$t%d", id);
	else  rdes = "$t9";
	if (symb->scope == 0)
		SAPP(0, s, "lw %s %s(%s)", rdes.c_str(), symb->wd.s.c_str(), reg[0].c_str());
	else {
		s = "addu $t8 " + reg[0] + " $fp";
		insl->push(s);
		SAPP(0, s, "lw %s %d($t8)", rdes.c_str(), symb->pos);
	}
	insl->push(s);
	store(insl, rdes, mcode.des);
}
void parseMove(insLis* insl, Mcode mcode) {
/*
	movec()
*/
	if (*mcode.des == *mcode.srcl) return;
	LoadReg(insl, *mcode.des, 0);
	LoadReg(insl, *mcode.srcl, 1);
	insl->push("move " + reg[0] + " " + reg[1]);
	store(insl, reg[0], mcode.des);
}
void parseLab(insLis* insl, Mcode mcode) {
	/*
	glabc()
	*/
	insl->push(mcode.s+":");
}
void parseEnd(insLis* insl, Mcode mcode) {
	insl->push(ENDLAB+":");
}
void parseCode(insLis* insl, Mcode mcode) {
	/*解析*/
	if (FDEBUG == 1) {
		printf("%s\n", mcode.s.c_str());
		fflush(stdout);
	}
		
	if (mcode.op == ADDOP || mcode.op == SUBOP || mcode.op == MULOP || mcode.op == DIVOP||
	mcode.op==SRLOP || mcode.op == SLLOP) {
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
		parseCase(insl, mcode);
	else if (mcode.op == MOVEOP) {
		parseMove(insl, mcode);
	}
	else if (mcode.op == LOADOP) {
		parseLoad(insl,mcode);
	}
	else error(mcode);
}
void gDeclare(Mcode (*nextC)()) { //生产固定区域
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
void gMain(Mcode (*nextC)()) {
	string ms;
	maini.push(".text");
	SAPP(0, ms, "addiu $sp $sp -%d", MNUM);
	maini.push(ms);
	maini.push("move $gp $sp");
	maini.push(string("la $ra ")+ENDLAB);
	Mcode mcode;
	while (true)
	{
		mcode = nextC();
		parseCode(&maini, mcode);
		if (mcode.op == ENDOP) break;
	}
}
void gFun(Mcode (*nextC)()) {
	funi.push("li $v0 10");
	funi.push("syscall");
	string s;
	Mcode mcode;
	while (true) {
		mcode = nextC();
		if (mcode.op == DECLAREOP && mcode.srcl->cont.symb->wd.ss=="main") {
			break;
		}
		parseCode(&funi, mcode);
	}
}
void genFCode() {
	Mcode(*p)() = fnextC;
	gDeclare(p);
	gFun(p);
	pos--;
	gMain(p);
}