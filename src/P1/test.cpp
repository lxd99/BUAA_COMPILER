#include<bits/stdc++.h>
#define LOWER(x) (((x)>='A'&&(x)<='Z'?(x)-'A'+'a':(x)))
using namespace std;
string s,s1;
int c;
int main(){
    FILE* fp = fopen("in.txt","r");
    for(int i=1;i<=10;i++){
        c = fgetc(fp);
        printf("%d\n",c);
    }
    fseek(fp,-1,SEEK_CUR);
    c = fgetc(fp);
    printf("%d\n",c);
}