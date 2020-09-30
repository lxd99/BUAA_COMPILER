#include "header.h"
vector <word> wordList;
string id[39] = { "IDENFR","INTCON","CHARCON","STRCON","CONSTTK","INTTK","CHARTK","VOIDTK","MAINTK","IFTK",
"ELSETK","SWITCHTK","CASETK","DEFAULTTK","WHILETK","FORTK","SCANFTK","PRINTFTK","RETURNTK","PLUS",
"MINU","MULT","DIV","LSS","LEQ","GRE","GEQ","EQL","NEQ","COLON",
"ASSIGN","SEMICN","COMMA","LPARENT","RPARENT","LBRACK","RBRACK","LBRACE","RBRACE" };
void error(int num,string s) {
	printf("fuck! %s\n", s.c_str());
}
void add(int num, string s, int pt) {
	if (pt) {
		printf("%s %s\n", id[num].c_str(), s.c_str());
	}
}
void getsym(FILE* fp) {
	int c = 0;
	string ms;
	while (true) {
		ms.clear();
		c = fgetc(fp);
		while (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
			c = fgetc(fp);
		}
		if (c == -1) break;
		//@signle
		if (c == '(') {
			ms.push_back(c);
			add(LPARENT, ms);
		}
		else if (c == ')') {
			ms.push_back(c);
			add(RPARENT, ms);
		}
		else if (c == '[') {
			ms.push_back('[');
			add(LBRACK, ms);
		}
		else if (c == ']') {
			ms.push_back(']');
			add(RBRACK, ms);
		}
		else if (c == '{') {
			ms.push_back(c);
			add(LBRACE, ms);
		}
		else if (c == '}') {
			ms.push_back(c);
			add(RBRACE, ms);
		}
		else if (c == '+') {
			ms.push_back(c);
			add(PLUS, ms);
		}
		else if (c == '-') {
			ms.push_back(c);
			add(MINU, ms);
		}
		else if (c == '*') {
			ms.push_back(c);
			add(MULT, ms);
		}
		else if (c == '/') {
			ms.push_back(c);
			add(DIV, ms);
		}
		else if (c == ',') {
			ms.push_back(c);
			add(COMMA, ms);
		}
		else if (c == ':') {
			ms.push_back(c);
			add(COLON, ms);
		}
		else if (c == ';') {
			ms.push_back(c);
			add(SEMICN, ms);
		}
		//@doubel+saas
		else if (c == '!') {
			c = fgetc(fp);
			if (c != '=') error(0);
			else {
				ms.append("!=");
				add(NEQ, ms);
			}
		}
		else if (c == '=') { // ==\=
			ms.push_back(c);
			c = fgetc(fp);
			if (c == '=') {
				ms.push_back(c);
				add(EQL, ms);
			}
			else {
				fseek(fp, -1, SEEK_CUR);
				add(ASSIGN, ms);
			}
		}
		else if (c == '<') { //<\<=
			ms.push_back(c);
			c = fgetc(fp);
			if (c == '=') {
				ms.push_back(c);
				add(LEQ, ms);
			}
			else {
				fseek(fp, -1, SEEK_CUR);
				add(LSS, ms);
			}
		}
		else if (c == '>') { // >\>= 
			ms.push_back(c);
			c = fgetc(fp);
			if (c == '=') {
				ms.push_back(c);
				add(GEQ, ms);
			}
			else {
				fseek(fp, -1, SEEK_CUR);
				add(GRE, ms);
			}
		}
		else if (c == '\"') {
			c = fgetc(fp);
			while (ISSTR(c)) {
				ms.push_back(c);
				c = fgetc(fp);
			}

			if (c != '\"') error(0);
			else add(STRCON, ms);

		}
		else if (c == '\'') {
			char mc1 = fgetc(fp), mc2 = fgetc(fp);
			if (mc2 != '\'' || !ISCHAR(mc1)) {
				error(0);
			}
			else {
				ms.push_back(mc1);
				add(CHARCON, ms);
			}
		}
		//@multi
		else if (ISDIGIT(c)) {
			ms.push_back(c);
			c = fgetc(fp);
			while (ISDIGIT(c)) {
				ms.push_back(c);
				c = fgetc(fp);
			}
			fseek(fp, -1, SEEK_CUR);
			add(INTCON, ms);
		}
		else if (ISALPHA(c)) { //15 special && 1 common
			ms.push_back(c);
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
				add(CONSTTK, ms);
			}
			else if (mms.compare("int") == 0) {
				add(INTTK, ms);
			}
			else if (mms.compare("char") == 0) {
				add(CHARTK, ms);
			}
			else if (mms.compare("void") == 0) {
				add(VOIDTK, ms);
			}
			else if (mms.compare("main") == 0) {
				add(MAINTK, ms);
			}
			else if (mms.compare("if") == 0) {
				add(IFTK, ms);
			}
			else if (mms.compare("else") == 0) {
				add(ELSETK, ms);
			}
			else if (mms.compare("switch") == 0) {
				add(SWITCHTK, ms);
			}
			else if (mms.compare("case") == 0) {
				add(CASETK, ms);
			}
			else if (mms.compare("default") == 0) {
				add(DEFAULTTK, ms);
			}
			else if (mms.compare("while") == 0) {
				add(WHILETK, ms);
			}
			else if (mms.compare("for") == 0) {
				add(FORTK, ms);
			}
			else if (mms.compare("scanf") == 0) {
				add(SCANFTK, ms);
			}
			else if (mms.compare("printf") == 0) {
				add(PRINTFTK, ms);
			}
			else if (mms.compare("return") == 0) {
				add(RETURNTK, ms);
			}
			else {
				add(IDENFR, ms);
			}
			if (c != -1) fseek(fp, -1, SEEK_CUR); //eof pd
		}
		else {
			error(0);
		}

	}
}
