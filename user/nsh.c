#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#define MAX 1024

char cmd[MAX];
char *left,*right;

/*删除命令中的第一个空格和最后一个空格，不能省略！*/
char *delete_blank(char *h)
{
    char *t = h;
    while (*t)
        t++;
    while (*h == ' ')
        *(h++) = '\0';
    while (*(--t) == ' ')
        ;
    *(t + 1) = '\0';
    return h;
}

/*提取管道右部分命令*/
char* extract(char *cmd){
    for(;*cmd!='\0'&& *cmd!='|';cmd++);
    if(*cmd == '\0')
        return 0;
    else
    {
        *cmd = '\0';
        return cmd+1;
    }
}
/*重定向*/
void redirect(int m,int p[]){
    close(m);
    dup(p[m]);
    close(p[0]);
    close(p[1]);
}
/*处理命令*/
void runcmd(char* cmd){
    //使用word存储一个个单词，包括重定向符号
    char word[20][20];

    char* point[20];
    //记录命令中的单词数，含重定向符号
    int num = 0;
    char *temp = word[num];
    //分别记录输入重定向符号、输出重定向符号的下标
    int input = 0;
    int output = 0;

    cmd = delete_blank(cmd);

    //给word中每一个单词对应一个指针
    for(int i = 0;i<20;i++){
        point[i] = word[i];
    }
    //将命令拆解，存入word
    for(char *t = cmd;*t;t++){
        //以空格作为分界
        if(*t ==' '){
            *temp = '\0';
            num++;
            temp = word[num];
        }else{
            if(*t=='<'){
                input = num+1;
            }else if(*t=='>'){
                output = num+1;
            }
            *temp = *t;
            temp++;
        }
    }
    *temp = '\0';
    num++;
    //末尾加0表示结束
    point[num] = 0;

    //point2是对point的处理，不含重定向符号
    char* point2[20];
    //num2表示命令中除去重定向符号剩下的单词数
    int num2 = 0;

    for(int i = 0;i<num;i++){
        //跳过输入重定向符号以及后面的一个单词
        if(i==input-1) 
            i+=2;
        if(i==output-1)
            i+=2;
        point2[num2] = point[i];
        num2++;
    }
    point2[num2] = 0;

    //输入重定向
    if(input){
        close(0);
        if(open(word[input],O_RDONLY)<0){
            fprintf(2, "open %s failed\n", word[input]);
            exit(-1);
        }
    }
    //输出重定向
    if(output){
        close(1);
        if(open(word[output],O_WRONLY | O_CREATE)<0){
            fprintf(2, "open %s failed\n", word[output]);
            exit(-1);
        }
    }
    if(fork()==0){
        if(point2[0]==0)
            exit(-1);
        //执行命令
        exec(point2[0],point2);
        fprintf(2, "exec %s failed\n", point2[0]);
        exit(-1);
    }
    wait(0);
}
/*执行管道命令，输入输出重定向*/
void run(){
    if(left){
        int p[2];
        pipe(p);
        if(fork()==0){
            if(right)
                redirect(1,p);
            runcmd(left);
        }
        /*再创建一个子进程，注意是else if，不能写成if*/
        else if(fork()==0){
            if(right){
                redirect(0,p);
                runcmd(right);
            }
        }
        close(p[0]);
        close(p[1]);
        wait(0);
        wait(0);
        exit(0);
    }
}

int
main(){
    while(1){
        printf("@");
        memset(cmd,0,MAX);
        gets(cmd,MAX);
        /*处理命令为空的情况，不能省略！*/
        if(cmd[0] == 0){
            exit(0);
        }
        /*将回车换成'\0'*/        
        *strchr(cmd,'\n') = '\0';
        if(fork()==0){
            left = cmd;
            right = extract(cmd);
            run();
        }
        wait(0);
    }
    exit(0);
}