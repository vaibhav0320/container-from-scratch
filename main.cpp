//#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <sys/wait.h>
#include <sched.h>

int child (void* );

#define STACK_SIZE (1024*1024)

struct child_config{

    int argc;
    char* hostname;
};

int main(){
    int err = 0;
    //std::cout<<"HELO";

    char* container_name;
    char* image_name;
    struct child_config config = {0}; 
    int flags, child_pid;
    char *stack = 0;
    if( !(stack = (char *)malloc(STACK_SIZE))){
        fprintf(stderr, "=> malloc allocation failed\n");
        goto error;
    }

       

    flags = CLONE_NEWNS
		| CLONE_NEWCGROUP
		| CLONE_NEWPID
		| CLONE_NEWIPC
		| CLONE_NEWNET
		| CLONE_NEWUTS;

    child_pid = clone(child, stack+STACK_SIZE, flags | SIGCHLD, &config); // STACK GROWS DOWNWORDS
    
    if( child_pid == -1 ){
        perror("sfd:");
        fprintf(stderr, "=> clone failed\n");
        err = 1;
        goto clear_resource;
    }

    clear_resource:
        free(stack);
    
    error:
        err = 1;
    return 0;   
}


int child( void* arg){
    
    return 0;
}


