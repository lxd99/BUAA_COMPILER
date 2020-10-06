#include "header.h"
extern vector<word> wordList;
map <string, int> funList;
word wd;
int pos = -1;
int pre = 0;
void error(string s) {
	printf("error:%s\n", s.c_str());
	printf("wd: %s\n", wd.s.c_str());
	exit(0);
}
word next(int mode) {
	static int len = wordList.size() - 1;
	if (pos < len)	return wordList[++pos];
	else return wordList[pos];
}
void out() {
	while (pre < pos)
		check(wordList[pre++], 1);

}
void back() {
	if (pos > 0) pos--;
}
void addSen(string s, int mode) {
	if (mode) {
		out();
		printf("%s\n", s.c_str());
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
			else error(s);
		}
	}
}
void program() {
	/*
	[＜常量说明＞］［＜变量说明＞］{＜有返回值函数定义＞|＜无返回值函数定义＞}＜主函数＞
	*/
	static string s = "<程序>";
	static int pre = pos;
	if (wd.type == CONSTTK)	consExplain();
	word mwd = next(0), mwd1 = next(0); back(); back();
	if (ISTYPE(wd.type) && mwd.type==IDENFR && mwd1.type != LPARENT) varExplain();
	no_Has();
	mainFun();
	addSen(s);
}
void consDef() {
	/*
	 int＜标识符＞＝＜整数＞{,＜标识符＞＝＜整数＞}| char＜标识符＞＝＜字符＞{,＜标识符＞＝＜字符＞}
	 FOLLOW: ;
	*/
	static string s = "<常量定义>";
	if (!ISTYPE(wd.type)) error(s);
	if (wd.type == INTTK) {
		while (true)
		{
			wd = next();
			if (wd.type != IDENFR)  error(s);
			wd = next();
			if (wd.type != ASSIGN) error(s);
			wd = next();
			integer();
			if (wd.type == COMMA) continue;
			else if (wd.type == SEMICN) break;
			else error(s);
		}
	}
	else if (wd.type == CHARTK) {
		while (true) {
			wd = next();
			if (wd.type != IDENFR)  error(s);
			wd = next();
			if (wd.type != ASSIGN) error(s);
			wd = next();
			if (wd.type != CHARCON) error(s);
			wd = next();
			if (wd.type == COMMA) continue;
			else if (wd.type == SEMICN) break;
			else error(s);
		}
	}

	addSen(s);
}
void consExplain() {
	/*
	const＜常量定义＞;{ const＜常量定义＞;}
	FOLLOW: int char  void  FIRST(复合语句)
	*/
	static string s = "<常量说明>";
	if (wd.type != CONSTTK) error(s);
	while (wd.type == CONSTTK) {
		wd = next();
		consDef();
		if (wd.type != SEMICN) error(s);
		wd = next();
		if (wd.type == CONSTTK) continue;
		break;
	}

	addSen(s);
}
void varExplain() {
	/*
	＜变量定义＞;{＜变量定义＞;}
	FOLLOW: "int xx()" "char xx()"  FIRST(语句)
	*/
	static string s = "<变量说明>";
	static vector<int> vec[3] = { {IDENFR,LPARENT} };
	static int type = preDeal(1, 2, vec[0], vec[1], vec[2]);
	while (true) {
		varDef();
		if (wd.type != SEMICN) error(s);
		wd = next();
		if (!ISTYPE(wd.type)) break;
		type = preDeal(1, 2, vec[0], vec[1], vec[2]);
		if (type != 1) continue;
		else break;
	}

	addSen(s);
}
void varDef() {
	/*
	＜变量定义无初始化＞|＜变量定义及初始化＞
	*/
	static string s = "<变量定义>";
	vector<int> vec[3];
	if (preDeal(1, 2, vec[0] = { IDENFR,ASSIGN }, vec[1], vec[2]) == 1 ||
		preDeal(1, 5, vec[0] = { IDENFR,LBRACK,INTCON,RBRACK,ASSIGN }, vec[1], vec[2]) == 1 ||
		preDeal(1, 8, vec[0] = { IDENFR,LBRACK,INTCON,RBRACK,LBRACK,INTCON,RBRACK,ASSIGN }, vec[1], vec[2]) == 1)
		varIniDef();
	else varNoIniDef();
	addSen(s);
}
void varIniDef() {
	/*
	＜类型标识符＞＜标识符＞=＜常量＞|
	＜类型标识符＞＜标识符＞'['＜无符号整数＞']'='{'＜常量＞{,＜常量＞}'}'|
	＜类型标识符＞＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']'='{''{'＜常量＞{,＜常量＞}'}'{, '{'＜常量＞{,＜常量＞}'}'}'}'
	FOLLOW:
	*/
	static string  s = "<变量定义及初始化>";
	if (!ISTYPE(wd.type)) error(s);
	wd = next();
	if (wd.type != IDENFR) error(s);
	wd = next();
	if (wd.type == LBRACK) {
		BRACK;
		if (wd.type == LBRACK) {//[][]=
			BRACK;
			if (wd.type != ASSIGN) error(s);
			wd = next();
			if (wd.type != LBRACE) error(s);
			while (true) {
				wd = next();
				if (wd.type != LBRACE) error(s);
				while (true) {
					wd = next();
					con();
					if (wd.type == COMMA) continue;
					else if (wd.type == RBRACE) break;
					else error(s);
				}
				wd = next();
				if (wd.type == COMMA) continue;
				else if (wd.type == RBRACE) break;
				else error(s);
			}
		}
		else if (wd.type == ASSIGN) { //[]=
			wd = next();
			if (wd.type != LBRACE) error(s);
			while (true) {
				wd = next();
				con();
				if (wd.type == COMMA)	continue;
				else if (wd.type == RBRACE) break;
				else error(s);
			}
		}
		else error(s);
		wd = next();
	}
	else if (wd.type == ASSIGN) {
		wd = next();
		con();
	}
	else error(s);
	addSen(s);
}
void varNoIniDef() {
	/*
	＜类型标识符＞(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']')
	{,(＜标识符＞|＜标识符＞'['＜无符号整数＞']'|＜标识符＞'['＜无符号整数＞']''['＜无符号整数＞']' )}
	FOLLOW:
	*/
	static string s = "<变量定义无初始化>";
	if (!ISTYPE(wd.type)) error(s);
	while (true) {
		wd = next();
		if (wd.type != IDENFR) error(s);
		wd = next();
		if (wd.type == LBRACK) {
			BRACK;
			if (wd.type == LBRACK)
				BRACK;
		}
		if (wd.type == COMMA) continue;
		break;
	}
	addSen(s);
}
void con() {
	/*
	＜整数＞|＜字符＞
	*/
	static string s = "<常量>";
	if (wd.type == PLUS || wd.type == MINU||wd.type==INTCON) integer();
	else wd = next();
	addSen(s);
}
void noSignInt() {
	/*
	＜数字＞｛＜数字＞｝
	*/
	static string s = "<无符号整数>";
	if (wd.type != INTCON) error(s);
	wd = next();
	addSen(s);
}
void integer() {
	/*
	［＋｜－］＜无符号整数＞
	*/
	static string s = "<整数>";
	if (wd.type == PLUS || wd.type == MINU)		wd = next();
	noSignInt();

	addSen(s);
}
void hasReturnFun() {
	/*
	＜声明头部＞'('＜参数表＞')' '{'＜复合语句＞'}'
	FOLLOW: 无返回|主函数
	*/
	static string s = "<有返回值函数定义>";
	declareHead();
	if (wd.type != LPARENT) error(s);

	wd = next();
	parList(); //has empty
	if (wd.type != RPARENT) error(s);

	wd = next();
	if (wd.type != LBRACE) error(s);

	wd = next();
	compSen();
	if (wd.type != RBRACE) error(s);

	wd = next();
	addSen(s);
}
void declareHead() {
	/*
	 int＜标识符＞ |char＜标识符＞
	 FOLLOW： （
	*/
	static string s = "<声明头部>";
	// need error deal
	int type = wd.type == INTTK ? INTFUN : CHARFUN;

	if (!ISTYPE(wd.type)) error(s);
	wd = next();
	if (wd.type != IDENFR) error(s);
	funList.emplace(wd.s, type);
	wd = next();

	addSen(s);
}
void parList() {
	/*
	＜类型标识符＞＜标识符＞{,＜类型标识符＞＜标识符＞}| ＜空＞
	FOLLOW: )
	*/
	static string s = "<参数表>";
	if (ISTYPE(wd.type)) {
		while (true) {
			if (!ISTYPE(wd.type)) error(s);
			wd = next();
			if (wd.type != IDENFR) error(s);
			wd = next();
			if (wd.type == COMMA) {
				wd = next();
				continue;
			}
			else break;
		}
	}

	addSen(s);
}
void compSen() {
	/*
	［＜常量说明＞］［＜变量说明＞］＜语句列＞
	 FOLLOW; }
	 pd error here
	*/
	static string s = "<复合语句>";
	if (ISCONST(wd.type)) consExplain();
	if (ISTYPE(wd.type)) varExplain();
	senList();

	addSen(s);
}
void senList() {
	/*
	{＜语句＞}
	FOLLOW: }
	*/
	static string s = "<语句列>";
	while (ISSEN(wd.type)) {
		sentence();
	}

	addSen(s);
}
void sentence() {
	/*
	 ＜循环语句＞｜＜条件语句＞| ＜有返回值函数调用语句＞;  |＜无返回值函数调用语句＞;｜＜赋值语句＞;｜＜读语句＞;｜＜写语句＞;｜＜情况语句＞｜＜空＞;|＜返回语句＞; | '{'＜语句列＞'}'
	 FOLLOW: } while for if IDENFER scanf printf switch return ;
	 FIRST: while for if IDENFER scanf printf switch return ; {
	*/
	static string s = "<语句>";
	if (wd.type == WHILETK || wd.type == FORTK)
		repSen();
	else if (wd.type == IFTK)
		condSen();
	else if (wd.type == IDENFR) {
		word mwd = next(0); back();
		if (mwd.type == ASSIGN||mwd.type==LBRACK) assignSen();
		else {
			auto it = funList.find(wd.s);
			if (it == funList.end()) error(s);
			else if (HASRETURN(it->second)) hasCallSen();
			else noCallSen();
		}	
		if (wd.type != SEMICN) error(s);
		wd = next();
	}
	else if (wd.type == SCANFTK) {
		readSen();
		if (wd.type != SEMICN) error(s);
		wd = next();
	}

	else if (wd.type == PRINTFTK) {
		writeSen();
		if (wd.type != SEMICN) error(s);
		wd = next();

	}
	else if (wd.type == SWITCHTK)
		situaSen();
	else if (wd.type == SEMICN)
		wd = next();
	else if (wd.type == RETURNTK) {
		returnSen();
		if (wd.type != SEMICN) error(s);
		wd = next();
	}
	else if (wd.type == LBRACE)
	{
		wd = next();
		senList();
		if (wd.type != RBRACE) error(s);
		wd = next();
	}

	addSen(s);
}
void repSen() {
	/*
	while '('＜条件＞')'＜语句＞| for'('＜标识符＞＝＜表达式＞;＜条件＞;＜标识符＞＝＜标识符＞(+|-)＜步长＞')'＜语句＞
	*/
	static string s = "<循环语句>";
	if (wd.type == WHILETK) {
		wd = next();
		if (wd.type != LPARENT) error(s);
		wd = next();
		condition();
		if (wd.type != RPARENT) error(s);
		wd = next();
		sentence();
	}
	else if (wd.type == FORTK) {
		wd = next();
		if (wd.type != LPARENT) error(s);
		wd = next();
		if (wd.type != IDENFR) error(s);
		wd = next();
		if (wd.type != ASSIGN) error(s);
		wd = next();
		expression();
		if (wd.type != SEMICN) error(s);

		wd = next();
		condition();
		if (wd.type != SEMICN) error(s);

		wd = next();
		if (wd.type != IDENFR) error(s);
		wd = next();
		if (wd.type != ASSIGN) error(s);
		wd = next();
		if (wd.type != IDENFR) error(s);
		wd = next();
		if (wd.type != PLUS && wd.type != MINU) error(s);
		wd = next();
		stepLen();

		if (wd.type != RPARENT) error(s);
		wd = next();
		sentence();
	}
	else error(s);

	addSen(s);
}
void condSen() {
	/*
	if '('＜条件＞')'＜语句＞［else＜语句＞］
	*/
	static string s = "<条件语句>";
	if (wd.type != IFTK) error(s);
	wd = next();
	if (wd.type != LPARENT) error(s);
	wd = next();
	condition();
	if (wd.type != RPARENT) error(s);
	wd = next();
	sentence();
	if (wd.type == ELSETK) {
		wd = next();
		sentence();
	}

	addSen(s);
}
void hasCallSen() {
	/*
	＜标识符＞'('＜值参数表＞')'
	*/
	static string s = "<有返回值函数调用语句>";
	if (wd.type != IDENFR) error(s);
	wd = next();
	if (wd.type != LPARENT) error(s);
	wd = next();
	valParList();
	if (wd.type != RPARENT) error(s);
	wd = next();

	addSen(s);
}
void noCallSen() {
	/*
	 ＜标识符＞'('＜值参数表＞')'
	*/
	static string s = "<无返回值函数调用语句>";
	if (wd.type != IDENFR) error(s);
	wd = next();
	if (wd.type != LPARENT) error(s);
	wd = next();
	valParList();
	if (wd.type != RPARENT) error(s);
	wd = next();

	addSen(s);
}
void valParList() {
	/*
	 ＜表达式＞{,＜表达式＞}｜＜空＞
	 FOLLOW:)
	*/
	static string s = "<值参数表>";
	if (wd.type == RPARENT) ;
	else {
		while (true) {
			expression();
			if (wd.type == COMMA) {
				wd = next();
				continue;
			}
			else break;
		}
	}
	addSen(s);
}
void assignSen() {
	/*
	＜标识符＞＝＜表达式＞|＜标识符＞'['＜表达式＞']'=＜表达式＞|＜标识符＞'['＜表达式＞']''['＜表达式＞']' =＜表达式＞
	*/
	static string s = "<赋值语句>";
	if (wd.type != IDENFR) error(s);
	wd = next();
	if (wd.type == LBRACK) {
		wd = next();
		expression();
		if (wd.type != RBRACK) error(s);
		wd = next();
		if (wd.type == ASSIGN) wd = next();
		
		else if (wd.type == LBRACK) {
			wd = next();
			expression();
			if (wd.type != RBRACK) error(s);
			wd = next();
			if (wd.type != ASSIGN) error(s);
			wd = next();
		}
		else error(s);
	}
	else if (wd.type == ASSIGN)
		wd = next();
	else error(s);
	expression();
	addSen(s);
}
void readSen() {
	/*
	scanf '('＜标识符＞')'
	*/
	static string s = "<读语句>";
	if (wd.type != SCANFTK) error(s);
	wd = next();
	if (wd.type != LPARENT) error(s);
	wd = next();
	if (wd.type != IDENFR) error(s);
	wd = next();
	if (wd.type != RPARENT) error(s);
	wd = next();
	addSen(s);
}
void writeSen() {
	/*
	printf '(' ＜字符串＞,＜表达式＞ ')'| printf '('＜字符串＞ ')'| printf '('＜表达式＞')'
	*/
	static string s = "<写语句>";
	if (wd.type != PRINTFTK) error(s);
	wd = next();
	if (wd.type != LPARENT) error(s);
	wd = next();
	if (wd.type == STRCON) {
		str();
		if (wd.type == COMMA) {
			wd = next();
			expression();
		}
	}
	else expression();

	if (wd.type != RPARENT) error(s);
	wd = next();

	addSen(s);
}
void str() {
	/*
	 "｛十进制编码为32,33,35-126的ASCII字符｝"
	*/
	static string s = "<字符串>";
	if (wd.type != STRCON) error(s);
	wd = next();

	addSen(s);
}
void situaSen() {
	/*
	switch ‘(’＜表达式＞‘)’ ‘{’＜情况表＞＜缺省＞‘}’
	*/
	static string s = "<情况语句>";
	if (wd.type != SWITCHTK) error(s);
	wd = next();
	if (wd.type != LPARENT) error(s);
	wd = next();
	expression();
	if (wd.type != RPARENT) error(s);
	wd = next();
	if (wd.type != LBRACE) error(s);
	wd = next();
	caseTable();
	defaul();
	if (wd.type != RBRACE) error(s);
	wd = next();
	addSen(s);
}
void returnSen() {
	/*
	 return['('＜表达式＞')']
	 FOLLOW: ;
	*/
	static string s = "<返回语句>";
	if (wd.type != RETURNTK) error(s);
	wd = next();
	if (wd.type == LPARENT) {
		wd = next();
		expression();
		if (wd.type != RPARENT) error(s);
		wd = next();
	}

	addSen(s);
}

void noReturnFun() {
	/*
	void＜标识符＞'('＜参数表＞')''{'＜复合语句＞'}'
	*/
	static string s = "<无返回值函数定义>";
	if (wd.type != VOIDTK) error(s);
	wd = next();
	if (wd.type != IDENFR) error(s);
	funList.emplace(wd.s, VOIDFUN);
	wd = next();
	if (wd.type != LPARENT) error(s);
	wd = next();
	parList();
	if (wd.type != RPARENT) error(s);
	wd = next();
	if (wd.type != LBRACE) error(s);
	wd = next();
	compSen();
	if (wd.type != RBRACE) error(s);
	wd = next();

	addSen(s);
}
void expression() {
	/*
	[＋｜－]＜项＞{＜加法运算符＞＜项＞}
	FOLLOW: ] )  ;
	*/
	static string s = "<表达式>";
	if (wd.type == PLUS || wd.type == MINU) wd = next();
	item();
	while (wd.type == PLUS || wd.type == MINU) {
		wd = next();
		item();
	}

	addSen(s);
}
void item() {
	/*
	＜因子＞{＜乘法运算符＞＜因子＞}
	FOLLOW: ; ] ) + -
	*/
	static string s = "<项>";
	factor();
	while (wd.type == MULT || wd.type == DIV) {
		wd = next();
		factor();
	}

	addSen(s);
}
void factor() {
	/*
	＜标识符＞｜＜标识符＞'['＜表达式＞']'|＜标识符＞'['＜表达式＞']''['＜表达式＞']'|'('＜表达式＞')'｜＜整数＞|＜字符＞｜＜有返回值函数调用语句＞
	*/
	static string s = "<因子>";
	if (wd.type == IDENFR) {
		word mwd = next(0); back();
		if (mwd.type == LBRACK) {
			wd = next();
			EBRACK;
			if (wd.type == LBRACK) {
				EBRACK;
			}
		}
		else if (mwd.type == LPARENT) {
			auto it = funList.find(wd.s);
			if (it == funList.end() || !HASRETURN(it->second)) error(s);
			hasCallSen();
		}
		else wd = next();
	}
	else if (wd.type == LPARENT) {
		wd = next();
		expression();
		if (wd.type != RPARENT) error(s);
		wd = next();

	}
	else if (wd.type == MINU || wd.type == PLUS || wd.type == INTCON)
		integer();
	else if (wd.type == CHARCON)
		wd = next();
	else error(s);
	addSen(s);
}
void caseTable() {
	/*
	＜情况子语句＞{＜情况子语句＞}
	FOLLOW: default
	*/
	static string s = "<情况表>";
	subCase();
	while (wd.type == CASETK) {
		subCase();
	}

	addSen(s);
}
void subCase() {
	/*
	case＜常量＞：＜语句＞
	*/
	static string s = "<情况子语句>";
	if (wd.type != CASETK) error(s);
	wd = next();
	con();
	if (wd.type != COLON) error(s);
	wd = next();
	sentence();
	addSen(s);
}
void defaul() {
	/*
	default :＜语句＞
	*/
	static string s = "<缺省>";
	if (wd.type != DEFAULTTK) error(s);
	wd = next();
	if (wd.type != COLON) error(s);
	wd = next();
	sentence();

	addSen(s);
}
void stepLen() {
	/*
	＜无符号整数＞
	*/
	static string s = "<步长>";
	noSignInt();
	addSen(s);
}
void condition() {
	/*
	＜表达式＞＜关系运算符＞＜表达式＞
	*/
	static string s = "<条件>";
	expression();
	if (!ISRELATEOP(wd.type)) error(s);
	wd = next();
	expression();

	addSen(s);
}
void mainFun() {
	/*
	void main‘(’‘)’ ‘{’＜复合语句＞‘}’
	*/
	static string s = "<主函数>";
	if (wd.type != VOIDTK) error(s);
	wd = next();
	if (wd.type != MAINTK) error(s);
	wd = next();
	if (wd.type != LPARENT) error(s);
	wd = next();
	if (wd.type != RPARENT) error(s);
	wd = next();
	if (wd.type != LBRACE) error(s);
	wd = next();
	compSen();
	if (wd.type != RBRACE) error(s);
	wd = next();
	addSen(s);
}
