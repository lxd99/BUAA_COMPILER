#define  _CRT_SECURE_NO_WARNINGS
#include "gram.h"
#include "symb.h"
#include "mcode.h"
//#define debug
extern int dept;
extern vector<word> wordList;
#define errors(num) { backto(num); status = -1; return 1;}
static int pos = -1;
int pre = 0;
int stablen = 0;
int ifid = 0,repid = 0,swid=0;
static char sbuf[150];
word wd;
symbtable symtab;
map<string, int> strtab;
word next(int mode) {
	static int len = wordList.size() - 1;
	if (pos < len) return wordList[++pos];

	else return wordList[pos];
}
void out() {
	while (pre < pos)
		check(wordList[pre++], 1);

}
static void back() {
	if (pos > 0) pos--;
}
void error(word mwd, string s = "", int mode = 'a', int isout = WASYMB) {
	if (mode == SSEMICN || mode == SRPARENT || mode == SRBRACK) mwd = wordList[pos - 1];
#ifdef debug	
	if (isout) printf("%d %d %s %c\n", mwd.line,wd.no,s.c_str(),mode);
#endif
#ifndef debug
	if (isout) printf("%d %c\n", mwd.line, mode);
#endif // !debug

	if (mode == SRPARENT || mode == SSEMICN || mode == SRBRACK) {
		back();
		back();
		next();
	}
}
#define GETPARPOS(x) do {\
	for(auto i  = (x).rbegin();i!=(x).rend();++i){\
		(*i)->pos = size;\
		size += (*i)->getSize();\
	}\
}while(0)
void addSen(string s, int mode) {
	if (mode) {
		out();
		printf("%s\n", s.c_str());
		fflush(stdout);
		//cout << s << endl;
	}
}
int preDeal(int num, int len, vector<int> a, vector<int> b, vector<int> c) {
	static vector<int> mid[3];
	int flag = 0;
	mid[0] = a;
	mid[1] = b;
	mid[2] = c;

	for (int i = 0; i < len; i++) {
		word mwd = next(0);
		for (int j = 0; j < num; j++) {
			if (mwd.type != mid[j][i]) flag |= (1 << j);
		}
	}
	for (int i = 0; i < len; i++) back();
	for (int i = 0; i < num; i++) {
		if ((flag & (1 << i)) == 0) return i + 1;
	}
	return num + 1;
}

void no_Has() {
	static string s = "no_Has";
	while (true) {
		if (ISTYPE(wd.type)) 
			hasReturnFun();
		
		else if (wd.type == VOIDTK) {
			word mwd = next(0); back();
			if (mwd.type == MAINTK) break;
			else if (mwd.type == IDENFR) noReturnFun();
			else error(wd,s);
		}
	}
}
void program() {
	/*
	[＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
	*/
	static string s = "<程序>";
	static int pre = pos;
	int size = 0;
	if (wd.type == CONSTTK)	consExplain(0);
	word mwd = next(0), mwd1 = next(0); back(); back();
	if (ISTYPE(wd.type) && mwd.type==IDENFR && mwd1.type != LPARENT) varExplain((symbol*)0,size);
	no_Has();
	mainFun();
	endc();
	addSen(s);
}
void consDef(symbol * scope) {
	/*
	 int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}| char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
	 FOLLOW: ;
	*/
	static string s = "<常量定义>";
	symbol *symb = new symbol;
	int n,type=-1; //不会用到type判断
	symb->scope = scope;
	if (!ISTYPE(wd.type)) error(wd,s);
	type=symb->type = GETTYPE(wd.type);
	symb->form = CONSF;
	if (wd.type == INTTK) {
		while (true)
		{
			wd = next();
			if (wd.type != IDENFR)  error(wd,s);
			symb->wd = wd;
			wd = next();
			if (wd.type != ASSIGN) error(wd,s);
			wd = next();
			
			if (wd.type == INTCON || wd.type == PLUS || wd.type == MINU) integer(n);
			else {
				error(symb->wd, s, CONTYPE);
				wd = next();
			}

			symb->value.var = n;
			if (!symtab.insert(symb)) error(symb->wd, s, DUPNAME);
			symb = new symbol;
			symb->scope = scope;
			symb->form = CONSF;
			symb->type = type;

			if (wd.type == COMMA) continue;
			else break;
		}
	}
	else if (wd.type == CHARTK) {
		while (true) {
			wd = next();
			if (wd.type != IDENFR)  error(wd,s);
			symb->wd = wd;
			wd = next();
			if (wd.type != ASSIGN) error(wd,s);
			
			wd = next();
			if (wd.type ==ERROR) error(symb->wd, s, WASYMB);
			else if (wd.type != CHARCON) error(symb->wd,s, CONTYPE);
			else symb->value.var = wd.s[0]; //不出错才行

			wd = next();

			if (!symtab.insert(symb)) error(symb->wd, s, DUPNAME);
			symb = new symbol;
			symb->scope = scope;
			symb->form = CONSF;
			symb->type = type;

			if (wd.type == COMMA) continue;
			else break;
		}
	}

	addSen(s);
}
void consExplain(symbol * scope) {
	/*
	const＜常量定义＞;{ const＜常量定义＞;}
	FOLLOW: int char  void  FIRST(复合语句)
	*/
	static string s = "<常量说明>";
	if (wd.type != CONSTTK) error(wd,s);
	while (wd.type == CONSTTK) {
		wd = next();
		consDef(scope);
		if (wd.type != SEMICN) error(wd,s,SSEMICN);
		wd = next();
		if (wd.type == CONSTTK) continue;
		break;
	}
	addSen(s);
}
void varExplain(symbol *scope,int& size) {
	/*
	＜变量定义＞;{＜变量定义＞;}
	FOLLOW: "int xx()" "char xx()"  FIRST(语句)
	*/
	static string s = "<变量说明>";
	static vector<int> vec[3] = { {IDENFR,LPARENT} };
	static int type = preDeal(1, 2, vec[0], vec[1], vec[2]);
	int status = 0;
	while (true) {
		varDef(scope,size);
		if (wd.type != SEMICN) error(wd,s,SSEMICN);
		wd = next();
		if (!ISTYPE(wd.type)) break;
		type = preDeal(1, 2, vec[0], vec[1], vec[2]);
		if (type != 1) continue;
		else break;
	}

	addSen(s);
}
void varDef(symbol * scope,int &size) {
	/*
	＜变量定义无初始化＞|＜变量定义及初始化＞
	*/
	static string s = "<变量定义>";
	vector<int> vec[3];
	int status = 0;
	if (preDeal(1, 2, vec[0] = { IDENFR,ASSIGN }, vec[1], vec[2]) == 1 ||
		preDeal(1, 5, vec[0] = { IDENFR,LBRACK,INTCON,RBRACK,ASSIGN }, vec[1], vec[2]) == 1 ||
		preDeal(1,4,vec[0]={IDENFR,LBRACK,INTCON,ASSIGN},vec[1],vec[2])==1||
		preDeal(1, 8, vec[0] = { IDENFR,LBRACK,INTCON,RBRACK,LBRACK,INTCON,RBRACK,ASSIGN }, vec[1], vec[2]) == 1||
		preDeal(1, 7, vec[0] = {IDENFR, LBRACK, INTCON, LBRACK, INTCON, RBRACK, ASSIGN},vec[1],vec[2]) == 1||
		preDeal(1, 7, vec[0] = {IDENFR, LBRACK, INTCON, RBRACK, LBRACK, INTCON, ASSIGN},vec[1],vec[2]) == 1||
		preDeal(1, 6, vec[0] = {IDENFR, LBRACK, INTCON, LBRACK, INTCON, ASSIGN}, vec[1], vec[2]) == 1
		)
		varIniDef(scope,size);
	else varNoIniDef(scope,size);
	addSen(s);
}
void varIniDef(symbol * scope,int &size) {
	/*
	＜类型标识符＞＜标识符＞=＜常量＞|
	＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'|
	＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
	FOLLOW:
	*/
	static string  s = "<变量定义及初始化>";
	symbol* symb = new symbol; int n,type = -1; Value value;
	if (!ISTYPE(wd.type)) error(wd,s);
	symb->type = GETTYPE(wd.type);
	wd = next();
	if (wd.type != IDENFR) error(wd,s);
	symb->wd = wd;
	wd = next();
	if (wd.type == LBRACK) {
		BRACK(n);
		symb->dim.push_back(n);
		if (wd.type == LBRACK) {//[][]=
			BRACK(n);

			symb->dim.push_back(n);
			DARRAY(symb->dim[0], symb->dim[1], value.da);
			symb->form = ARRAYF;

			if (wd.type != ASSIGN) error(wd,s);
			wd = next();
			if (wd.type != LBRACE) error(wd,s);
			int cnt0 = 0, cnt1 = 0,iserror = 0;
			while (true) {
				wd = next();
				if (wd.type != LBRACE) error(wd,s);
				while (true) {
					wd = next();
					con(n,type);

					if (type != symb->type) error(wd,s,CONTYPE);
					if (cnt0 < symb->dim[0] && cnt1 < symb->dim[1]) value.da[cnt0][cnt1] = n;
					cnt1++;
					if (wd.type == COMMA) continue;
					else if (wd.type == RBRACE)	break;
					else error(wd,s);
				}
				if (cnt1 != symb->dim[1]) iserror = 1;
				cnt0++; cnt1 = 0;
				wd = next();
				if (wd.type == COMMA) continue;
				else if (wd.type == RBRACE) break;
				else error(wd,s);
			}
			if (iserror || cnt0 != symb->dim[0]) error(wd,s, ARRYNUM);
		}
		else if (wd.type == ASSIGN) { //[]=
			int cnt = 0;
			value.sa = new int[symb->dim[0]];
			symb->form = ARRAYF;

			wd = next();
			if (wd.type != LBRACE) error(wd,s);
			while (true) {
				wd = next();
				con(n,type);
				if (type != symb->type) error(wd, s, CONTYPE);
				if (cnt < symb->dim[0]) value.sa[cnt] = n;
				cnt++;
				if (wd.type == COMMA)	continue;
				else if (wd.type == RBRACE) break;
				else error(wd,s);
			}
			if (cnt != symb->dim[0]) error(wd, s, ARRYNUM);
		}
		else error(wd,s);
		wd = next();
	}
	else if (wd.type == ASSIGN) {
		symb->form = VARF;
		wd = next();
		con(n,type);
		if (type != symb->type) error(wd,s,CONTYPE);
		value.var = n;
	}
	else error(wd,s);
	symb->value = value;
	symb->scope = scope;
	symb->isIni = true;
	symb->pos = size;
	size += symb->getSize();
	if (!symtab.insert(symb)) error(symb->wd,s,DUPNAME);
	inic(new Data(VARADD, symb));

	addSen(s);
}
void varNoIniDef(symbol *scope,int &size) {
	/*
	＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']')
	{,(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
	FOLLOW:
	*/
	static string s = "<变量定义无初始化>";
	int n,type;
	symbol* symb = new symbol;
	symb->scope = scope;
	if (!ISTYPE(wd.type)) error(wd,s);
	type = symb->type = GETTYPE(wd.type);
	while (true) {
		wd = next();
		if (wd.type != IDENFR) error(wd,s);
		symb->wd = wd;
		wd = next();
		if (wd.type == LBRACK) {
			BRACK(n);
			symb->dim.push_back(n);
			if (wd.type == LBRACK) {
				BRACK(n);
				symb->dim.push_back(n);
			}
		}
		if (symb->dim.size() > 0) symb->form = ARRAYF;
		else symb->form = VARF;
		symb->pos = size;
		if (!symtab.insert(symb)) error(symb->wd, s, DUPNAME);
		if (scope == 0)  inic(new Data(VARADD, symb));
		size += symb->getSize();
		symb = new symbol;
		symb->scope = scope;
		symb->type = type;
		if (wd.type == COMMA) continue;
		break;
	}
	addSen(s);
}
void con(int &n,int &type) {
	/*
	＜整数＞|＜字符＞
	*/
	static string s = "<常量>";
	if (wd.type == PLUS || wd.type == MINU || wd.type == INTCON) {
		integer(n);
		type = ITYPE;
	}
	else {
		if (wd.type == ERROR) error(wd, s, WASYMB);
		n = wd.s[0];
		wd = next();
		type = CTYPE;
	}
	addSen(s);
}
void noSignInt(int &n) {
	/*
	＜数字＞｛＜数字＞｝
	*/
	static string s = "<无符号整数>";
	n = 0;
	if (wd.type != INTCON) error(wd,s);
	STRTINT(wd.s, n);
	wd = next();
	addSen(s);
}
void integer(int &n) {
	/*
	［＋｜－］＜无符号整数＞
	*/
	static string s = "<整数>";
	n = 0; int  f = 1;
	if (wd.type == PLUS || wd.type == MINU) {
		f = wd.type == PLUS ? 1 : -1;
		wd = next();
	}
	noSignInt(n);
	n = n * f;
	addSen(s);
}
void hasReturnFun() {
	/*
	＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
	FOLLOW: 无返回|主函数
	*/
	static string s = "<有返回值函数定义>";
	int size = 0;
	symbol *symb = new symbol;
	symb->scope = 0; symb->form = FUNF;
	declareHead(symb);
	Data* srcl = new Data(VARADD, symb);
	if (wd.type != LPARENT) error(wd,s);

	wd = next();
	parList(symb->parList,symb); //has empty
	if (wd.type != RPARENT) error(wd,s,SRPARENT);
	if (!symtab.insert(symb)) error(symb->wd, s, DUPNAME);

	wd = next();
	if (wd.type != LBRACE) error(wd,s);

	declarec(srcl);
	wd = next();
	compSen(symb,size);
	if (wd.type != RBRACE) error(wd,s);

	if (symb->retcnt == 0) error(wd,s,HASNORET);
	wd = next();
	addSen(s);
	size += 8;
	GETPARPOS(symb->parList);
	retc(new Data(CONSADD,0), new Data(VARADD, symb));
}
void declareHead(symbol *symb) {
	/*
	 int＜标识符＞ |char＜标识符＞
	 FOLLOW： （
	*/
	static string s = "<声明头部>";
	// need error deal
	symb->type = GETTYPE(wd.type);
	if (!ISTYPE(wd.type)) error(wd,s);
	wd = next();
	if (wd.type != IDENFR) error(wd,s);
	symb->wd = wd;
	wd = next();

	addSen(s);
}
void parList(vector<symbol*> &parList,symbol * scope) {
	/*
	＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
	FOLLOW: )
	*/
	static string s = "<参数表>";
	symbol* symb = new symbol;
	symb->scope = scope;
	if (ISTYPE(wd.type)) {
		while (true) {
			if (!ISTYPE(wd.type)) error(wd,s);
			symb->type = GETTYPE(wd.type);
			symb->form = VARF;
			wd = next();
			if (wd.type != IDENFR) error(wd,s);
			symb->wd = wd;
			wd = next();
			if (!symtab.insert(symb)) error(symb->wd,s,DUPNAME);
			parList.push_back(symb);
			symb = new symbol;
			symb->scope = scope;
			if (wd.type == COMMA) {
				wd = next();
				continue;
			}
			else break;
		}
	}

	addSen(s);
}
void compSen(symbol * scope,int& size) {
	/*
	［＜常量说明＞］［＜变量说明＞］＜语句列＞
	 FOLLOW; }
	 pd error here
	*/
	static string s = "<复合语句>";
	if (ISCONST(wd.type)) consExplain(scope);
	if (ISTYPE(wd.type)) varExplain(scope,size);
	senList(scope);

	addSen(s);
}
void senList(symbol * scope) {
	/*
	{＜语句＞}
	FOLLOW: }
	*/
	static string s = "<语句列>";
	while (ISSEN(wd.type)) {
		sentence(scope);
	}

	addSen(s);
}
void sentence(symbol * scope) {
	/*
	 ＜循环语句＞｜＜条件语句＞| ＜有返回值函数调用语句＞;  |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜0况语句＞｜＜空＞;|＜返回语句＞; | '{'＜语句列＞'}'
	 FOLLOW: } while for if IDENFER scanf printf switch return ;
	 FIRST: while for if IDENFER scanf printf switch return ; {
	*/
	static string s = "<语句>";
	if (wd.type == WHILETK || wd.type == FORTK)
		repSen(scope);
	else if (wd.type == IFTK)
		condSen(scope);
	else if (wd.type == IDENFR) {
		word mwd = next(0); back();
		if (mwd.type == ASSIGN||mwd.type==LBRACK) assignSen(scope);
		else {
			Data* srcl;
			auto it = symtab.findt(wd.ss, scope);
			if (it<0) error(wd,s,NONAME);
			if (it==ITYPE||it==CTYPE) hasCallSen(scope,srcl);
			else noCallSen(scope,srcl);
		}	
		if (wd.type != SEMICN) error(wd,s,SSEMICN);
		wd = next();
	}
	else if (wd.type == SCANFTK) {
		readSen(scope);
		if (wd.type != SEMICN) error(wd,s,SSEMICN);
		wd = next();
	}

	else if (wd.type == PRINTFTK) {
		writeSen(scope);
		if (wd.type != SEMICN) error(wd,s,SSEMICN);
		wd = next();

	}
	else if (wd.type == SWITCHTK)
		situaSen(scope);
	else if (wd.type == RETURNTK) {
		returnSen(scope);
		if (wd.type != SEMICN) error(wd,s,SSEMICN);
		wd = next();
	}
	else if (wd.type == LBRACE)
	{
		wd = next();
		senList(scope);
		if (wd.type != RBRACE) error(wd,s);
		wd = next();
	}
	else {
		if (wd.type != SEMICN) error(wd, s, SSEMICN);
		wd = next();
	}

	addSen(s);
}
void repSen(symbol * scope) {
	/*
	while '('＜条件＞')'＜语句＞| for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
	*/
	static string s = "<循环语句>";
	string lab1,lab2;
	int type = -1,mid = repid++;
	Data *srcl = new Data(), *srcr = new Data();
	SAPP(0, lab1, "%s_%d_rep1", PRELAB, mid);
	if (wd.type == WHILETK) {
		glabc(lab1);
		wd = next();
		if (wd.type != LPARENT) error(wd,s);
		wd = next();
		SAPP(0, lab2, "%s_%d_rep2", PRELAB, mid);
		condition(scope,lab2);
		if (wd.type != RPARENT) error(wd,s,SRPARENT);
		wd = next();
		sentence(scope);
	}
	else if (wd.type == FORTK) {
		wd = next();
		if (wd.type != LPARENT) error(wd,s);
		wd = next();
		CHECKWD(type);//IDENFR
		srcl = new Data(VARADD, symtab.find(wd.s,scope));

		wd = next();
		if (wd.type != ASSIGN) error(wd,s);
		wd = next();
		expression(scope,type,srcr);
		
		assignc(srcr, srcl);
		glabc(lab1);

		if (wd.type != SEMICN) error(wd,s,SSEMICN);
		wd = next();
		SAPP(0, lab2, "%s_%d_rep_2", PRELAB,mid);
		condition(scope,lab2);
		if (wd.type != SEMICN) error(wd,s,SSEMICN);

		wd = next();
		CHECKWD(type); //IDENFER
		wd = next();
		if (wd.type != ASSIGN) error(wd,s);
		wd = next();
		CHECKWD(type); //IDENFER
		wd = next();
		int op = wd.type == PLUS ? 1 : 0;
		if (wd.type != PLUS && wd.type != MINU) error(wd,s);
		wd = next();
		stepLen(srcr);

		if (wd.type != RPARENT) error(wd,s,SRPARENT);
		wd = next();
		sentence(scope);

		if (op) addc(srcl, srcr, srcr);
		else subc(srcl, srcr, srcr);
		assignc(srcr, srcl);
	}
	else error(wd,s);

	srcl = new Data(LABADD, 0);
	srcl->s = lab1;
	jumpc(srcl);
	glabc(lab2);
	addSen(s);
}
void condSen(symbol * scope) {
	/*
	if '('＜条件＞')'＜语句＞［else＜语句＞］
	*/
	static string s = "<条件语句>";
	int mid = ifid++;
	string if1,if2;
	SAPP(0, if1, "%s_%d_if1",PRELAB,mid);
	SAPP(0, if2, "%s_%d_if2", PRELAB, mid);
	if (wd.type != IFTK) error(wd,s);
	wd = next();
	if (wd.type != LPARENT) error(wd,s);
	wd = next();
	condition(scope,if1);
	if (wd.type != RPARENT) error(wd,s,SRPARENT);
	wd = next();
	sentence(scope);

	if (wd.type == ELSETK) {
		Data* srcl = new Data(LABADD, 0);
		srcl->s = if2;

		jumpc(srcl);
		glabc(if1);
		
		wd = next();
		sentence(scope);

		glabc(if2);
	}
	else glabc(if1);
	
	addSen(s);
}
void hasCallSen(symbol *scope,Data *&des) {
	/*
	＜标识符＞'('＜值参数表＞')'
	*/
	static string s = "<有返回值函数调用语句>";
	int type = -1;
	CHECKWD(type); //IDENFER
	symbol * fun = symtab.find(wd.ss,scope);
	wd = next();
	if (wd.type != LPARENT) error(wd,s);
	wd = next();
	valParList(scope,fun->parList);
	callc(fun, des);
	if (wd.type != RPARENT) error(wd,s,SRPARENT);
	wd = next();
	addSen(s);
}
void noCallSen(symbol *scope,Data *&des) {
	/*
	 ＜标识符＞'('＜值参数表＞')'
	*/
	static string s = "<无返回值函数调用语句>";
	int type = -1;
	CHECKWD(type);
	symbol *fun = symtab.find(wd.ss, scope);
	wd = next();
	if (wd.type != LPARENT) error(wd,s);
	wd = next();
	valParList(scope,fun->parList);
	callc(fun, des);
	if (wd.type != RPARENT) error(wd,s,SRPARENT);
	wd = next();

	addSen(s);
}
void valParList(symbol * scope,vector<symbol*> parList) {
	/*
	 ＜表达式＞{,＜表达式＞}｜＜空＞
	 FOLLOW:)
	 MISSFOLLOW: ;
	*/
	static string s = "<值参数表>";
	int len = parList.size(), no = 0, iserror = 0,type;
	Data* srcl = new Data();
	while (ISEXPB(wd.type)){
		word mwd = wd;
		expression(scope,type,srcl);
		if (no < len && type != parList[no]->type) error(mwd,s,PARTYPE); //恶意换行
		pushc(new Data(VARADD, parList[no]), srcl);
		no++;
		if (wd.type != COMMA) break;
		wd = next();
	}
	if (no != len) error(wd, s, PARNUM);
	addSen(s);
}
void assignSen(symbol * scope) {
	/*
	＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞|＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
	*/
	static string s = "<赋值语句>";
	int type=-1;
	Data* srcl = new Data(), *exp = new Data();
	symbol* symb;
	CHECKWD(type);
	symb = symtab.find(wd.ss, scope);
	if (symb==0||symb->form == CONSF) error(wd, s, MODCONS);
	wd = next();
	if (wd.type == LBRACK) {
		wd = next();
		expression(scope,type,srcl);
		if (wd.type != RBRACK) error(wd,s,SRBRACK);
		wd = next();
		if (wd.type == ASSIGN) {
			if(!ISTID(srcl))
				addc(srcl, new Data(CONSADD, (void*)0), srcl);
			srcl= new Data(VARADD, symb,srcl);
			wd = next();
		}
		else if (wd.type == LBRACK) {
			wd = next();
			multc(srcl, new Data(CONSADD, (void*)symb->dim[1]), srcl);
			expression(scope,type,exp);
			addc(srcl, exp, srcl);
			srcl= new Data(VARADD, symb, srcl);
			if (wd.type != RBRACK) error(wd,s,SRBRACK);
			wd = next();
			if (wd.type != ASSIGN) error(wd,s);
			wd = next();
		}
		else error(wd,s);
	}
	else if (wd.type == ASSIGN) {
		wd = next();
		srcl = new Data(VARADD, symb);
	}
	else error(wd,s);
	expression(scope,type,exp);
	assignc(exp, srcl);
	addSen(s);
}
void readSen(symbol * scope) {
	/*
	scanf '('＜标识符＞')'
	*/
	static string s = "<读语句>";
	int type = -1;
	Data* srcl;
	if (wd.type != SCANFTK) error(wd,s);
	wd = next();
	if (wd.type != LPARENT) error(wd,s);
	wd = next();
	CHECKWD(type);
	symbol* symb = symtab.find(wd.ss, scope);
	srcl = new Data(VARADD, symb);
	if (symb !=0&&symb->form == CONSF) error(wd,s,MODCONS);
	wd = next();
	if (wd.type != RPARENT) error(wd,s,SRPARENT);
	wd = next();
	readc(srcl);
	addSen(s);
}
void writeSen(symbol * scope) {
	/*
	printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')'
	*/
	static string s = "<写语句>";
	int type = -1;
	Data* srcl = new Data;
	if (wd.type != PRINTFTK) error(wd,s);
	wd = next();
	if (wd.type != LPARENT) error(wd,s);
	wd = next();
	if (!ISEXPB(wd.type)) {
		string ms = wd.s;
		str();
		srcl= new Data(STRADD, (void*)strtab.find(ms)->second);
		srcl->isinf.ise = wd.type==COMMA?0:1;
		printc(srcl);
		if (wd.type == COMMA) {
			wd = next();
			expression(scope,type,srcl);
			printc(srcl);
		}
	}
	else {
		expression(scope, type, srcl);
		printc(srcl);
	}

	if (wd.type != RPARENT) error(wd,s,SRPARENT);
	wd = next();

	addSen(s);
}
void str() {
	/*
	 "｛十进制编码为32,33,35-126的ASCII字符｝"
	*/
	static string s = "<字符串>";
	if (strtab.emplace(wd.s, stablen).second) {
		stablen += 1;
	}
	if (wd.type != STRCON) error(wd,s,WASYMB);
	wd = next();

	addSen(s);
}
void situaSen(symbol * scope) {
	/*
	switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
	*/
	static string s = "<情况语句>";
	int type = -1,mid = swid++;
	Data* srcl = new Data(),*srcr;
	
	string ms;
	SAPP(0, ms, "%s_%d_switch_end", PRELAB, mid);
	srcr = new Data(LABELOP, 0); srcr->s = ms;

	if (wd.type != SWITCHTK) error(wd,s);
	wd = next();
	if (wd.type != LPARENT) error(wd,s);
	wd = next();
	expression(scope,type,srcl);
	if (wd.type != RPARENT) error(wd,s,SRPARENT);
	wd = next();
	if (wd.type != LBRACE) error(wd,s);
	wd = next();
	caseTable(scope,type,srcl,srcr,mid);


	if (wd.type != DEFAULTTK) error(wd,s,SDEFAULT);
	else defaul(scope);
	
	if (ISTID(srcl)) dept -= 1;
	glabc(srcr->s);
	if (wd.type != RBRACE) error(wd,s);
	wd = next();
	addSen(s);
}
void returnSen(symbol * scope) {
	/*
	 return['('＜表达式＞')']
	 FOLLOW: ;
	*/
	static string s = "<返回语句>";
	int type = -1;
	scope->retcnt++;
	Data* srcl = 0;
	if (wd.type != RETURNTK) error(wd,s);
	wd = next();
	if (wd.type == LPARENT) {
		wd = next();
		if (ISEXPB(wd.type)) expression(scope, type,srcl); //hasProblem
		if (type != scope->type) {
			if (scope->type == VTYPE) error(wd, s, NOHASRET);
			else error(wd, s, HASNORET);
		}
		if (wd.type != RPARENT) error(wd, s, SRPARENT);
		wd = next();
	}
	else if (scope->type != VTYPE) error(wd, s, HASNORET);
	retc(srcl,new Data(VARADD,scope));
	addSen(s);
}

void noReturnFun() {
	/*
	void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
	*/
	static string s = "<无返回值函数定义>";
	int type = -1,size=0;
	symbol *symb = new symbol;
	symb->scope = 0; 
	symb->form = FUNF; 
	symb->type = VTYPE;
	Data* srcl = new Data(VARADD,symb);
	if (wd.type != VOIDTK) error(wd,s);
	wd = next();
	if (wd.type != IDENFR) error(wd, wd.s);
	symb->wd = wd;
	wd = next();
	if (wd.type != LPARENT) error(wd,s);
	wd = next();
	parList(symb->parList,symb);
	if (wd.type != RPARENT) error(wd,s,SRPARENT);
	wd = next();
	if (wd.type != LBRACE) error(wd,s);
	wd = next();

	declarec(srcl);

	if (!symtab.insert(symb)) error(symb->wd, s, DUPNAME);
	compSen(symb,size);
	if (wd.type != RBRACE) error(wd,s);
	wd = next();
	addSen(s);
	size += 8;
	GETPARPOS(symb->parList);
	retc(0, new Data(VARADD, symb));
}
void expression(symbol * scope,int& type,Data *&des) {
	/*
	[＋｜－]＜项＞{＜加法运算符＞＜项＞}
	FOLLOW: ] )  ; == >= <= !=
	*/
	static string s = "<表达式>";
	int mtype = -1,f=1; type = -1;
	Data *srcl,*srcr;
	srcl = new Data;
	srcr = new Data;
	if (wd.type == PLUS || wd.type == MINU) {
		if (wd.type == MINU) f = -1;
		wd = next();
		type = ITYPE;
	}
	item(scope,mtype,srcl);
	if (f == -1) negc(srcl,srcl);
	if (mtype == ITYPE) type = ITYPE;
	while (wd.type == PLUS || wd.type == MINU) {
		f = wd.type == PLUS ? 1 : -1;
		wd = next();
		item(scope,mtype,srcr);
		type = ITYPE;
		if (f == 1) addc(srcl, srcr, srcl);
		else subc(srcl, srcr, srcl);
	}
	if (type != ITYPE) type = CTYPE;
	des = srcl;
	addSen(s);
}
void item(symbol* scope,int &type,Data *&des) {
	/*
	＜因子＞{＜乘法运算符＞＜因子＞}
	FOLLOW: ; ] ) + -
	*/
	static string s = "<项>";
	int mtype = -1,f=0;
	Data *srcl, *srcr;
	srcl = new Data;
	srcr = new Data;
	factor(scope,mtype,srcl);
	if (mtype == ITYPE) type = ITYPE;
	while (wd.type == MULT || wd.type == DIV) {
		f = wd.type == MULT ? 1 : -1;
		wd = next();
		factor(scope,mtype,srcr);
		type = ITYPE;
		if (f == 1) multc(srcl, srcr, srcl);
		else divc(srcl, srcr, srcl);
	}
	if (type != ITYPE) type = CTYPE;
	des = srcl;
	addSen(s);
}
void factor(symbol * scope,int &type,Data *&des) {
	/*
	＜标识符＞｜＜标识符＞'['＜表达式＞']'|＜标识符＞'['＜表达式＞']''['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
	*/
	static string s = "<因子>";
	int mtype = -1;
	Data *srcl, *srcr;
	symbol* symb;  // local use
	srcl = new Data;
	srcr = new Data;
	if (wd.type == IDENFR) {
		CHECKWD(mtype);//IDENFER
		symb = symtab.find(wd.ss, scope);
		if (mtype == ITYPE) type = ITYPE;
		word mwd = next(0); back();
		if (mwd.type == LBRACK) {
			wd = next();
			EBRACK(scope, mtype,srcl);
			if (wd.type == LBRACK) {
				multc(srcl, new Data(CONSADD, (void*)symb->dim[1]), srcl);
				EBRACK(scope, mtype, srcr);
				addc(srcl, srcr, srcl);
			}
			else if(!ISTID(srcl))
				addc(srcl, new Data(CONSADD,(void*) 0),srcl);
			srcl = new Data(VARADD, symb, srcl);
		}
		else if (mwd.type == LPARENT) {
			symb =symtab.find(wd.ss,scope);
			if (symb->type==VTYPE) error(wd, s);
			type = symb->type;
			hasCallSen(scope,srcl);
		}
		else {
			symb = symtab.find(wd.ss, scope);
			if (symb->type == ITYPE) type = symb->type;
			wd = next();
			srcl = new Data(VARADD, symb);
		}
		if (symb->type == CTYPE) srcl->isinf.isc = 1;
	}
	else if (wd.type == LPARENT) {
		type = ITYPE;
		wd = next();
		expression(scope,mtype,srcl);
		if (wd.type != RPARENT) error(wd, s, SRPARENT);
		srcl->isinf.isc = 0;
		wd = next();

	}
	else if (wd.type == MINU || wd.type == PLUS || wd.type == INTCON) {
		srcl = new Data(CONSADD,0);
		integer(srcl->cont.cons);
		type = ITYPE;
	}
	else if (wd.type == CHARCON) {
		srcl= new Data(CONSADD, (void *)(wd.s[0]));
		srcl->isinf.isc = 1;
		wd = next();
	}
	else {
		error(wd, s,WASYMB);
		wd = next();
	}
	if (type != ITYPE) type = CTYPE;
	des = srcl;
	addSen(s);
}
void caseTable(symbol * scope,int type,Data *exp,Data *end,int mid) {
	/*
	＜情况子语句＞{＜情况子语句＞}
	FOLLOW: default
	srcl-->需要比较,srcr-->end地址,des-->下一个地址
	*/
	static string s = "<情况表>";
	string ms;
	int id = 0;
	
	SAPP(0, ms, "%s_%d_switch%d", PRELAB, mid, id++);
	Data* nex = new Data(LABADD,0);
	nex->s = ms;

	subCase(scope,type,exp,nex,mid);
	while (wd.type == CASETK) {
		jumpc(end);
		glabc(ms);

		SAPP(0, ms, "%s_%d_switch%d", PRELAB, mid, id++);
		nex = new Data(LABADD, 0);
		nex->s = ms;

		subCase(scope,type,exp,nex,mid);
	}
	jumpc(end);
	glabc(ms);

	addSen(s);
}
void subCase(symbol *scope,int type,Data *exp,Data *nex,int mid) {
	/*
	case＜常量＞：＜语句＞
	*/
	static string s = "<情况子语句>";
	int mtype=-1,n=0;
	Data* srcl;
	if (wd.type != CASETK) error(wd,s);
	wd = next();
	con(n,mtype);
	srcl = new Data(CONSADD, (void*)n);
	casec(exp, srcl, nex);
	if (mtype != type) error(wd, s,CONTYPE);
	if (wd.type != COLON) error(wd,s);
	wd = next();
	sentence(scope);
	addSen(s);
}
void defaul(symbol * scope) {
	/*
	default :＜语句＞
	*/
	static string s = "<缺省>";
	if (wd.type != DEFAULTTK) error(wd,s);
	wd = next();
	if (wd.type != COLON) error(wd,s);
	wd = next();
	sentence(scope);

	addSen(s);
}
void stepLen(Data * &des) {
	/*
	＜无符号整数＞
	*/
	static string s = "<步长>";
	int n = 0;
	noSignInt(n);

	Data* srcl = new Data(CONSADD, (void*)n);
	des = srcl;
	addSen(s);
}
void condition(symbol * scope,string ms) {
	/*
	＜表达式＞＜关系运算符＞＜表达式＞
	*/
	static string s = "<条件>";
	Data* srcl,*srcr,*des;
	int type1=-1,type2=-1,op = -1;
	expression(scope,type1,srcl);

	if (!ISRELATEOP(wd.type)) error(wd,s);
	op = wd.type;
	wd = next();
	expression(scope,type2,srcr);
	des = new Data(LABADD, 0);
	des->s = ms;
	condjc(op,srcl,srcr,des);
	if (type1 != ITYPE||type2 !=ITYPE) error(wd,s,IFTYPE);
	addSen(s);
}
void mainFun() {
	/*
	void main‘(’‘)’ ‘{’＜复合语句＞‘}’
	*/
	static string s = "<主函数>";
	int size = 0;
	symbol *symb = new symbol;
	symb->form = FUNF; 
	symb->scope = 0;

	if (wd.type != VOIDTK) error(wd,s);
	symb->type = GETTYPE(wd.type);
	wd = next();
	if (wd.type != MAINTK) error(wd,s);
	symb->wd = wd;
	if (!symtab.insert(symb)) error(symb->wd,s);
	wd = next();
	if (wd.type != LPARENT) error(wd,s);
	wd = next();
	if (wd.type != RPARENT) error(wd,s,SRPARENT);
	wd = next();
	if (wd.type != LBRACE) error(wd,s);
	wd = next();

	declarec(new Data(VARADD, symb));
	compSen(symb,size);
	if (wd.type != RBRACE) error(wd,s);
	wd = next();
	addSen(s);
}
