#include<bits/stdc++.h>
using namespace std;
char s1[50],s2[50];
int len;
vector <string> vec;
int main(){
	freopen("ins_in.txt","r",stdin);
	freopen("ins_out.txt","w",stdout);
	while(scanf("%s %s",s1,s2)==2){
		printf("#define %s %d\n",s1,len);
		vec.push_back(s2);
		len++;
	}
	printf("string opi[%d]={",len);
	for(int j=0;j<vec.size()-1;j++){
		string i =  vec[j];
		printf("\"%s\",",i.c_str());
	}
	printf("\"%s\"",vec[vec.size()-1].c_str());
	printf("};\n");
} 
