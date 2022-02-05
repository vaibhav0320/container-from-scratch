#include <unistd.h>
#include <stdio.h>
#include <cstdlib>
#include <sys/wait.h>
#include <sched.h>

int main(int argc, char** argv){
    char** list;
    list = &argv[1];
    int pid;
    char temp_dir[] = "/tmp/con.XXXXXX";
    if( !mkdtemp(temp_dir) ){
        fprintf(stdout, "=> Temp directory failed! \n");
        return -1;
    }
    return 0;
}