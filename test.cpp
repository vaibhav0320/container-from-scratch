#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <sys/wait.h>
#include <sched.h>

int main(int argc, char** argv){
    char** list;
    list = &argv[1];
    int pid;
    pid=fork();
    if(pid==0){
    execve(list[0],list,0);}

    else{
        wait(&pid);
    }
    return 0;
}