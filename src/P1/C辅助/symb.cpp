#include<bits/stdc++.h>
using namespace std;
int i = 0,deal=0;
char s[150];
vector<string> vec;
int main(){
    if(deal==0){
        freopen("in.txt","r",stdin);
        freopen("out.txt","w",stdout);
        while(scanf("%s",s)!=-1){
            vec.emplace_back(s);
            printf("#define %s %d\n",s,i++);
        }
        printf("string id[%d]={",i);
        int cnt = 0;
        for(auto i : vec){
            cnt++;
            printf("\"%s\",",i.c_str());
            if(cnt%10==0) printf("\n");
        }
        printf("};\n");
    }

    if(deal==1){
        freopen("format.txt","r",stdin);
        freopen("stdformat.txt","w",stdout);
        for(int i=1;i<=8;i++) scanf("%s",s);
        for(int i=1;i<=9;i++){
            for(int j=1;j<=8;j++){
                scanf(" %s",s);
                 printf("%-15s",s);
            }
            printf("\n");
        }
        for(int i=1;i<=6;i++){
            scanf("%s",s);
            printf("%-15s",s);
        }
        printf("\n");
    }
}