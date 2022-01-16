//#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <sys/wait.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>

int child (void* );

#define STACK_SIZE (1024*1024)

struct child_config{

    int argc;
    char* hostname;
    char** argv;
};

int main(int argc, char** argv){
    int err = 0;
    //std::cout<<"HELO";
    int child_status=0;
    char* container_name;
    char* image_name;
    struct child_config config; 
    config.argc = argc - 1;
    config.hostname = (char*)"uwu";
    config.argv = &argv[argc - config.argc];

    int flags, child_pid;
    char *stack = 0;
    if( !(stack = (char *)malloc(STACK_SIZE))){
        fprintf(stderr, "=> malloc allocation failed\n");
        goto error;
    }



    // setting flags for clone    

    flags = CLONE_NEWNS
		| CLONE_NEWCGROUP
		| CLONE_NEWPID
		| CLONE_NEWIPC
		| CLONE_NEWNET
		| CLONE_NEWUTS;

    child_pid = clone(child, stack+STACK_SIZE, flags | SIGCHLD, &config); // STACK GROWS DOWNWORDS
    

    //clone done if something break only then code should reach here...

    if( child_pid == -1 ){
        perror("sfd:");
        fprintf(stderr, "=> clone failed\n");
        err = 1;
        goto clear_resource;
    }

   
    waitpid(child_pid, &child_status,0);
    err |= WEXITSTATUS(child_status);
    
    clear_resource:
        free(stack);
    
    error:
        err = 1;
    return 0;   
}


int child( void* arg){
     printf("SUCESS\n");
    
    struct child_config *config = (child_config*)arg;
    printf("%s\n",config->argv[0]);
    sleep(2);
    if( sethostname(config->hostname,strlen(config->hostname))
    
    ){
        fprintf(stderr, "=> sethostname failed %m.\n");
        return -1;
    }

    if(execve(config->argv[0],config->argv,0)){
        fprintf(stderr, "=> exec failed %m.\n");
        return -1;
    }
   


    return 0;
}


