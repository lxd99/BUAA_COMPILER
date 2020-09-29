#include <bits/stdc++.h>
using namespace  std;
#define IDENFR 0
#define INTCON 1
#define CHARCON 2
#define STRCON 3
#define CONSTTK 4
#define INTTK 5
#define CHARTK 6
#define VOIDTK 7
#define MAINTK 8
#define IFTK 9
#define ELSETK 10
#define SWITCHTK 11
#define CASETK 12
#define DEFAULTTK 13
#define WHILETK 14
#define FORTK 15
#define SCANFTK 16
#define PRINTFTK 17
#define RETURNTK 18
#define PLUS 19
#define MINU 20
#define MULT 21
#define DIV 22
#define LSS 23
#define LEQ 24
#define GRE 25
#define GEQ 26
#define EQL 27
#define NEQ 28
#define COLON 29
#define ASSIGN 30
#define SEMICN 31
#define COMMA 32
#define LPARENT 33
#define RPARENT 34
#define LBRACK 35
#define RBRACK 36
#define LBRACE 37
#define RBRACE 38
string id[39]={"IDENFR","INTCON","CHARCON","STRCON","CONSTTK","INTTK","CHARTK","VOIDTK","MAINTK","IFTK",
"ELSETK","SWITCHTK","CASETK","DEFAULTTK","WHILETK","FORTK","SCANFTK","PRINTFTK","RETURNTK","PLUS",
"MINU","MULT","DIV","LSS","LEQ","GRE","GEQ","EQL","NEQ","COLON",
"ASSIGN","SEMICN","COMMA","LPARENT","RPARENT","LBRACK","RBRACK","LBRACE","RBRACE"};
#define ISALPHA(x) ((x)>='A'&&(x)<='Z'||x>='a'&&x<='z'||x=='_')
#define ISDIGIT(x) ((x)>='0'&&x<='9')
#define ISOP(x) ((x)=='+'||(x)=='-'||(x)=='*'||(x)=='/')
#define ISCHAR(x) (ISALPHA(x)||ISDIGIT(x)||ISOP(x))
#define ISSTR(x) ((x)>=35&&(x)<=126||(x)==32||(x)==33)
#define LOWER(x) (((x)>='A'&&(x)<='Z'?(x)-'A'+'a':(x)))
char c;
void error(int num,string s = ""){
   printf("fuck! %s\n",s.c_str());
}
void add(int num,string s,int pt=1){
   if(pt){
      printf("%s %s\n",id[num].c_str(),s.c_str());
   }
   
}
void getsym(FILE* fp){
   int c=0;
   string ms;
   while(true){
      ms.clear();
      c = fgetc(fp);
      while(c==' '||c=='\n'||c=='\t'||c=='\r'){
         c = fgetc(fp);
      }
      if(c==-1) break;
      //@signle
      if(c=='('){
         ms.push_back(c);
         add(LPARENT,ms);
      }else if(c==')'){
         ms.push_back(c);
         add(RPARENT,ms);
      }else if(c=='['){
         ms.push_back('[');
         add(LBRACK,ms);
      }else if(c==']'){
         ms.push_back(']');
         add(RBRACK,ms);
      }
      else if(c=='{'){
         ms.push_back(c);
         add(LBRACE,ms);
      }else if(c=='}'){
         ms.push_back(c);
         add(RBRACE,ms);
      }else if(c=='+'){
         ms.push_back(c);
         add(PLUS,ms);
      }else if(c=='-'){
         ms.push_back(c);
         add(MINU,ms);
      }else if(c=='*'){
         ms.push_back(c);
         add(MULT,ms);
      }else if(c=='/'){
         ms.push_back(c);
         add(DIV,ms);
      }else if(c==','){
         ms.push_back(c);
         add(COMMA,ms);
      }else if(c==':'){
         ms.push_back(c);
         add(COLON,ms);
      }else if(c==';'){
         ms.push_back(c);
         add(SEMICN,ms);
      }
      //@doubel+
      else if(c=='!'){
         c = fgetc(fp);
         if(c!='=') error(0);
         else{
            ms.append("!=");
            add(NEQ,ms);
         }
      }
      else if(c=='='){ // ==\=
         ms.push_back(c);
         c = fgetc(fp);
         if(c=='='){
            ms.push_back(c);
            add(EQL,ms);
         }else{
            fseek(fp,-1,SEEK_CUR);
            add(ASSIGN,ms);
         }
      }
      else if(c=='<'){ //<\<=
         ms.push_back(c);
         c = fgetc(fp);
         if(c=='='){
            ms.push_back(c);
            add(LEQ,ms);
         }else{
            fseek(fp,-1,SEEK_CUR);
            add(LSS,ms);
         }
      }
      else if(c=='>'){ // >\>= 
         ms.push_back(c);
         c = fgetc(fp);
         if(c=='='){
            ms.push_back(c);
            add(GEQ,ms);
         }else{
            fseek(fp,-1,SEEK_CUR);
            add(GRE,ms);
         }
      }
      else if(c=='\"'){
         c = fgetc(fp);
         while(ISSTR(c)){
            ms.push_back(c);
            c = fgetc(fp);
         }

         if(c!='\"') error(0);
         else add(STRCON,ms);
         
      }else if(c=='\''){
         char mc1=fgetc(fp),mc2=fgetc(fp);
         if(mc2!='\''||!ISCHAR(mc1)){
            error(0);
         }else{
            ms.push_back(mc1);
            add(CHARCON,ms);
         }
      }
      //@multi
      else if(ISDIGIT(c)){ 
         ms.push_back(c);
         c= fgetc(fp);
         while(ISDIGIT(c)){
            ms.push_back(c);
            c = fgetc(fp);
         }
         fseek(fp,-1,SEEK_CUR);
         add(INTCON,ms);
      }
      else if(ISALPHA(c)){ //15 special && 1 common
         ms.push_back(c);
         c = fgetc(fp);
         while(ISALPHA(c)||ISDIGIT(c)){
            ms.push_back(c);
            c= fgetc(fp);
         }
         string mms = ms;
         for(auto it=mms.begin();it!=mms.end();it++){
            *it = LOWER((*it));
         }
         if(mms.compare("const")==0){
            add(CONSTTK,ms);
         }else if(mms.compare("int")==0){
            add(INTTK,ms);
         }else if(mms.compare("char")==0){
            add(CHARTK,ms);
         }else if(mms.compare("void")==0){
            add(VOIDTK,ms);
         }else if(mms.compare("main")==0){
            add(MAINTK,ms);
         }else if(mms.compare("if")==0){
            add(IFTK,ms);
         }else if(mms.compare("else")==0){
            add(ELSETK,ms);
         }else if(mms.compare("switch")==0){
            add(SWITCHTK,ms);
         }else if(mms.compare("case")==0){
            add(CASETK,ms);
         }else if(mms.compare("default")==0){
            add(DEFAULTTK,ms);
         }else if(mms.compare("while")==0){
            add(WHILETK,ms);
         }else if(mms.compare("for")==0){
            add(FORTK,ms);
         }else if(mms.compare("scanf")==0){
            add(SCANFTK,ms);
         }else if(mms.compare("printf")==0){
            add(PRINTFTK,ms);
         }else if(mms.compare("return")==0){
            add(RETURNTK,ms);
         }
         else{
            add(IDENFR,ms);
         }
         if(c!=-1) fseek(fp,-1,SEEK_CUR); //eof pd
      }else{
         error(0);
      }

   }
}
int main ()
{
   // FILE* fp  = fopen("../P0/testFile/testfile4.txt","r");
   FILE* fp  = fopen("testfile.txt","r");
   freopen("output.txt","w",stdout);
   getsym(fp);
}