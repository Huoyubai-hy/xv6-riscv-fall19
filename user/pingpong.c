#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[])
{
    int parent_fd[2],child_fd[2];
    if(pipe(parent_fd)<0)
    {
        fprintf(2,"parent pipe error!\n");
        exit();
    }
    if(pipe(child_fd)<0)
    {
        fprintf(2,"child pipe error!\n");
        exit();
    }
    char ping[4] = "ping";
    char pong[4] = "pong";
    char a[4];
    char b[4];
    
    write(parent_fd[1],&ping,sizeof(ping));

    if(fork() == 0)
    {
        read(parent_fd[0],&a,sizeof(a));
        printf("%d: received %s\n",getpid(),a);

        write(child_fd[1],&pong,sizeof(pong));
        exit();
    }

    read(child_fd[0],&b,sizeof(b));
    printf("%d: received %s\n",getpid(),b);
    wait();
    exit();
}
