#include "kernel/types.h"
#include "user/user.h"

void primes(int fd)
{
    int devide,temp;
    int p[2];
    if(read(fd,&devide,sizeof(int))<=0)
        exit();
    printf("prime %d\n",devide);

    if(pipe(p)<0)
    {
        fprintf(2,"pipe error!\n");
        exit();
    }

    if(fork()==0)
    {
        close(p[1]);
        primes(p[0]);
    }
    else
    {
        while(read(fd,&temp,sizeof(int))>0)
        {
            if(temp % devide != 0)
            {
                close(p[0]);
                write(p[1],&temp,sizeof(temp));
            }
        }
        close(p[1]);
        close(fd);
        wait();
        exit();
    }
    
}

int main(int argc, char* argv[])
{
    int p[2];
    if(pipe(p)<0)
    {
        fprintf(2,"pipe error!\n");
        exit();
    }
    if(fork()==0)
    {
        close(p[1]);
        primes(p[0]);
    }
    else
    {
        close(p[0]);
        for(int i=2;i<=35;i++)
        {
            if(write(p[1],&i,sizeof(i))!=4)
            {
                fprintf(2,"write error!\n");
                exit();
            }
        }
        close(p[1]);
        wait();
        exit();
    }
    return 0;
}
        
    
    