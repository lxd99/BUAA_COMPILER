#include "header.h"
vector <word> wordList;
int line = 1;
int no = 1;
string id[39] = { "IDENFR","INTCON","CHARCON","STRCON","CONSTTK","INTTK","CHARTK","VOIDTK","MAINTK","IFTK",
"ELSETK","SWITCHTK","CASETK","DEFAULTTK","WHILETK","FORTK","SCANFTK","PRINTFTK","RETURNTK","PLUS",
"MINU","MULT","DIV","LSS","LEQ","GRE","GEQ","EQL","NEQ","COLON",
"ASSIGN","SEMICN","COMMA","LPARENT","RPARENT","LBRACK","RBRACK","LBRACE","RBRACE" };
void error(int num,string s) {
	printf("fuck! %s\n", s.c_str());
}
void check(word wd,int mode) {
	if (mode && wd.type >= 0) {
		printf("%s %s\n", id[wd.type].c_str(), wd.s.c_str());
	}
}
void addWord(int num,string s,int mode) {
	if (num>=0&&mode) {
		printf("%s %s\n", id[num].c_str(), s.c_str());
	}
	wordList.push_back(word(num, s,line,no));
	no++;
}
void getsym(FILE* fp) {
	int c = 0;
	string ms;
	
	while (true) {
		ms.clear();
		c = fgetc(fp);
		while (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
			if (c == '\n') line++, no = 1;
			c = fgetc(fp);
		}
		if (c == -1) break;
		ms.push_back(c);
		//@signle
		if (c == '(') {
			addWord(LPARENT, ms);
		}
		else if (c == ')') {
			addWord(RPARENT, ms);
		}
		else if (c == '[') {
			addWord(LBRACK, ms);
		}
		else if (c == ']') {
			addWord(RBRACK, ms);
		}
		else if (c == '{') {
			addWord(LBRACE, ms);
		}
		else if (c == '}') {
			addWord(RBRACE, ms);
		}
		else if (c == '+') {
			addWord(PLUS, ms);
		}
		else if (c == '-') {
			addWord(MINU, ms);
		}
		else if (c == '*') {
			addWord(MULT, ms);
		}
		else if (c == '/') {
			addWord(DIV, ms);
		}
		else if (c == ',') {
			addWord(COMMA, ms);
		}
		else if (c == ':') {
			addWord(COLON, ms);
		}
		else if (c == ';') {
			addWord(SEMICN, ms);
		}
		//@doubel+saas
		else if (c == '!') {
			c = fgetc(fp);
			ms.push_back(c);
			if (c != '=') {
				error(ERROR, ms);
			}		
			else {
				addWord(NEQ, ms);
			}
		}
		else if (c == '=') { // ==\=
			c = fgetc(fp);
			if (c == '=') {
				ms.push_back(c);
				addWord(EQL, ms);
			}
			else {
				fseek(fp, -1, SEEK_CUR);
				addWord(ASSIGN, ms);
			}
		}
		else if (c == '<') { //<\<=
			c = fgetc(fp);
			if (c == '=') {
				ms.push_back(c);
				addWord(LEQ, ms);
			}
			else {
				fseek(fp, -1, SEEK_CUR);
				addWord(LSS, ms);
			}
		}
		else if (c == '>') { // >\>= 
			c = fgetc(fp);
			if (c == '=') {
				ms.push_back(c);
				addWord(GEQ, ms);
			}
			else {
				fseek(fp, -1, SEEK_CUR);
				addWord(GRE, ms);
			}
		}
		else if (c == '\"') {
			int iserror = 0;
			ms.pop_back();
			c = fgetc(fp);
			while (c!='\"') {
				ms.push_back(c);
				if (!ISSTR(c)) iserror = 1;
				c = fgetc(fp);
			}
			
			if (ms.size() == 0 || iserror) addWord(ERROR, ms);
			else addWord(STRCON, ms);

		}
		else if (c == '\'') {
			int iserror = 0;
			ms.pop_back();
			c = fgetc(fp);
			while (c != '\'') {
				ms.push_back(c);
				if (!ISCHAR(c)) iserror = 1;
				c = fgetc(fp);
			}
			if (ms.size() == 0 || iserror) addWord(ERROR, ms);
			else addWord(CHARCON, ms);
		}
		//@multi
		else if (ISDIGIT(c)) {
			c = fgetc(fp);
			while (ISDIGIT(c)) {
				ms.push_back(c);
				c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			addWord(INTCON, ms);
		}
		else if (ISALPHA(c)) { //15 special && 1 common
			c = fgetc(fp);
			while (ISALPHA(c) || ISDIGIT(c)) {
				ms.push_back(c);
				c = fgetc(fp);
			}
			string mms = ms;
			for (auto it = mms.begin(); it != mms.end(); it++) {
				*it = LOWER((*it));
			}
			if (mms.compare("const") == 0) {
				addWord(CONSTTK, ms);
			}
			else if (mms.compare("int") == 0) {
				addWord(INTTK, ms);
			}
			else if (mms.compare("char") == 0) {
				addWord(CHARTK, ms);
			}
			else if (mms.compare("void") == 0) {
				addWord(VOIDTK, ms);
			}
			else if (mms.compare("main") == 0) {
				addWord(MAINTK, ms);
			}
			else if (mms.compare("if") == 0) {
				addWord(IFTK, ms);
			}
			else if (mms.compare("else") == 0) {
				addWord(ELSETK, ms);
			}
			else if (mms.compare("switch") == 0) {
				addWord(SWITCHTK, ms);
			}
			else if (mms.compare("case") == 0) {
				addWord(CASETK, ms);
			}
			else if (mms.compare("default") == 0) {
				addWord(DEFAULTTK, ms);
			}
			else if (mms.compare("while") == 0) {
				addWord(WHILETK, ms);
			}
			else if (mms.compare("for") == 0) {
				addWord(FORTK, ms);
			}
			else if (mms.compare("scanf") == 0) {
				addWord(SCANFTK, ms);
			}
			else if (mms.compare("printf") == 0) {
				addWord(PRINTFTK, ms);
			}
			else if (mms.compare("return") == 0) {
				addWord(RETURNTK, ms);
			}
			else {
				addWord(IDENFR, ms);
			}
			if (c != -1) fseek(fp, -1, SEEK_CUR); //eof pd
		}
		else {
			error(ERROR,ms);
		}
	}
	addWord(EOF,"");
}
