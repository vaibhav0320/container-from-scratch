//#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <sys/wait.h>
#include <sched.h>
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <libgen.h>

struct child_config{

    int argc;
    char* hostname;
    char** argv;
    char* mount_dir;
};

int child (void* );
int pivot_root(char *, char* );
int mounts(child_config* );

#define STACK_SIZE (1024*1024)



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
    
    //Sample linux image
    config.mount_dir = (char*)"/home/vaibhav/Containers/linux/";    

    printf("%d\n",argc);
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

    if( sethostname(config->hostname,strlen(config->hostname))
        
    ){
        fprintf(stderr, "=> sethostname failed %m.\n");
        return -1;
    }
    mounts(config);

    fflush(NULL);
    if(execve(config->argv[0],config->argv,0) ){
        fprintf(stderr, "=> exec failed %m.\n");
        return -1;
    }
   
    return 0;
}

int pivot_root(char *new_root, char* old_root){

   return syscall(SYS_pivot_root,new_root, old_root);
}

int mounts(child_config* config){
    // Remounting the root else pivot_root won't work.
    printf("MOUNTING...\n");
    if( mount(NULL, "/", NULL, MS_PRIVATE | MS_REC, NULL) == -1){

        fprintf(stderr,"=> remounting failed %m.");
        return -1;
    }

    char temp_dir[] = "/tmp/con.XXXXXX";
    if( !mkdtemp(temp_dir) ){
        fprintf(stderr, "=> Temp directory failed! \n");
        return -1;
    }
    // Mounting the user arg at temp. BIND is used else pivot_root won't work.

    if ( mount( config->mount_dir, temp_dir, NULL, MS_PRIVATE | MS_BIND, NULL) ){
        fprintf(stderr,"=> Bind mount failed !\n");
        return -1;
    }

    char inner_temp_dir[] = "/tmp/con.XXXXXX/oldroot.XXXXXX";
    // we want same name con.XXXXXX so copy same thing from string
    memcpy(inner_temp_dir,temp_dir,sizeof(temp_dir)-1);

    if( !mkdtemp(inner_temp_dir) ){
        fprintf(stderr, "=> Temp directory failed! \n");
        return -1;
    }

    if ( pivot_root(temp_dir,inner_temp_dir) ){
        fprintf(stderr, "=> pivot root error!\n");
        return -1;
    }

    char* old_root_dir = basename(inner_temp_dir);
    char old_root[sizeof(inner_temp_dir)+1] = {"/"};
    strcpy(&old_root[1],old_root_dir);

    if(chdir("/")){
        fprintf(stderr,"=> chdir failed\n");
        return -1;
    }

    if( umount2(old_root, MNT_DETACH) ){
        fprintf(stderr, "=> Unmount failed %m\n");
        return -1;
    }

    if( rmdir(old_root) ){
        fprintf(stderr,"=> Oldroot Remove failed\n");
        return -1;
    }

    fprintf(stdout,"=> Mount done....\n");
    return 0;

}





