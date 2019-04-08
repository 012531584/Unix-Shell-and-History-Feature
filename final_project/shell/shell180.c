#include <stdio.h>
#include <unistd.h>     // getpid(), getcwd()... unix shell
#include <sys/types.h>  // type definitions, e.g., pid_t
#include <sys/wait.h>   // wait()
#include <signal.h>     // signal name constants and kill()
#include <stdlib.h>
#include <string.h>
//#include <iostream>
//#include <vector>
//#include <string>
#define HISTORY_COUNT 20
#define MAXLINE 80
#ifdef DEBUG
#define DEBUG_OUTPUT(stmt) \
if (1) {stmt;}
#else
#define DEBUG_OUTPUT(stmt) \
if (0) {stmt;}
#endif


int history(char* hist[], int current)
{
    int i = current;
    int total_hist = 0;
    for (int j = 0; j < HISTORY_COUNT;j++) {
        if(hist[j])
            total_hist++;
    }
    int hist_num = total_hist;
    
    do{
        if (hist[i]) {
            printf("%4d %s\n", hist_num, hist[i]);
            hist_num--;
        }
        i = (i + HISTORY_COUNT -1) % HISTORY_COUNT;
    }while(i!= current);
    
    return 0;
}

void clear_history(char* hist[])
{
    for(int i = 0; i < HISTORY_COUNT; i++){
        free(hist[i]);
        hist[i] = NULL;
    }
}


int retriev_history(char* hist[], int current, char* line){
    DEBUG_OUTPUT(printf("retriev ~~~~~\n"));
    if (strncmp(line,"!!",strlen("!!")) == 0) {
        int mru = (current - 2 + HISTORY_COUNT) % HISTORY_COUNT;
        DEBUG_OUTPUT(printf("most recent use %d: \n", mru));
        if(hist[mru]) {
            printf("./%s\n", hist[mru]);
            DEBUG_OUTPUT(printf(" retriev history %s\n", hist[mru]));
            return mru;
        }
        else {
            printf("No commands in history.\n");
        }
    }
    else {
        int len = strlen(line);
        char tmp[len];
        strncpy(tmp,line+1,len-1);
        tmp[len-1] = '\0';
        int num = atoi(tmp);
        int i = current;
        int hist_num = 0;
        do {
            if(hist[i])
                hist_num++;
            if(hist_num == num) break;
            i = (i+1)%HISTORY_COUNT;
        } while(i != current);
        
        if (i == current)
            printf("No such command in history.\n");
        else {
            printf("./%s\n", hist[i]);
            DEBUG_OUTPUT(printf(" retriev history %s\n", hist[i]));
            return i;
        }
    }
    
    return -1;
}

int sh_execute(char* const argv[], int argc)
{
    pid_t pid;
    int status;
    
    if ((pid=fork()) < 0)
    {
        perror( "Internal error: cannot fork." );
        return -1;
    }
    else if (pid == 0)                  //child
    {
        if (execvp (argv[0], argv)<0){       //file name in argv[0]
            perror("Invalid command.");
            exit(1);
        }
    }
    else                                //parent
    {
        if (strcmp(argv[argc-1], "&") == 0)       //waiting relationship
            return 1;                              // running concurrently
        
        else
        {
            do {
                waitpid(pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
    return 1;
}

int add_to_history(char *hist[], int current, char* input){
    if (input[0] == '!')
        return 0;
    free(hist[current]);
    hist[current] = strdup(input);
    return 1;
}


int run_built_in_func(char* argv[], char* hist[], int current){
    const char* builtin_cmd[] = {"exit","cd","history", "clear_history"};
    const int num_builtin = sizeof(builtin_cmd)/sizeof(const char*);
    DEBUG_OUTPUT(printf("num_builtin = %d\n", num_builtin));
    int handler = 0;
    for (int i = 0; i < num_builtin; i++) {
        //DEBUG_OUTPUT(printf("argv[0] = %s\n", argv[0]));
        //DEBUG_OUTPUT(printf("%s\n", builtin_cmd[i]));
        if(strncmp(argv[0], builtin_cmd[i], strlen(builtin_cmd[i])) == 0){
            handler = i+1;
            break;
        }
    }
    DEBUG_OUTPUT(printf("handler = %d\n", handler));
    switch(handler) {
        case 1: {
            exit(0);
            DEBUG_OUTPUT(printf("ghost!!!\n"));
            break;
        }
        case 2: {
            chdir (argv[1]);
            return 1;
        }
        case 3: {
            history(hist, current);
            return 1;
        }
        case 4: {
            clear_history(hist);
            return 1;
        }
        default:
            break;
    }
    return 0;
}
/*
 int retriev_history(char* hist[], int current, char* line){
 DEBUG_OUTPUT(printf("retriev ~~~~~\n"));
 if (strncmp(line,"!!",strlen("!!")) == 0) {
 int mru = (current - 1 + HISTORY_COUNT) % HISTORY_COUNT;
 DEBUG_OUTPUT(printf("most recent use: %d\n", mru));
 if(hist[mru]) {
 DEBUG_OUTPUT(printf(" retriev history %d\n", mru));
 return mru;
 }
 else {
 printf("No commands in history.");
 }
 }
 else {
 int len = strlen(input);
 char tmp[len];
 strncpy(tmp,input+1,len-1);
 tmp[len-1] = '\0';
 int num = atoi(tmp);
 int i = current;
 int hist_num = 0;
 do {
 if(hist[i])
 hist_num++;
 if(hist_num == num) break;
 i = (i+1)%HISTORY_COUNT;
 } while(i != current);
 DEBUG_OUTPUT(printf(" retriev history %d\n", i));
 return i;
 }
 
 return -1;
 }
 */

int main(){
    int ret;
    char* hist[HISTORY_COUNT];
    int i = 0;
    int current = 0;
    
    for (i = 0; i < HISTORY_COUNT; i++)
        hist[i] = NULL;
    
    while (1)
    {
        //cout << "osh> " ;
        printf("osh> ");
        char line[MAXLINE];
        if (fgets(line, MAXLINE, stdin) == 0) {
            // failed to read a line
            perror("cannot read command!");
        }
        //cin.getline(input, MAXLINE);
        if(add_to_history(hist,current,line))
        current = (current + 1) % HISTORY_COUNT;
        
        DEBUG_OUTPUT(printf("read line = %s\n", line));
        
        char input[MAXLINE];
        if (strncmp(line, "!", strlen("!")) == 0) {
            int success;
            success = retriev_history(hist,current,line);
            if(success == -1) continue;
            else strcpy(input,hist[success]);
        }
        else
            strcpy(input,line);
        
        char* argv[MAXLINE/2 + 1];
        char* cmd = strtok(input, " " );
        char* tmp = cmd;
        int argc = 0;
        while ( tmp != NULL )
        {
            argv[argc] = tmp;
            tmp = strtok( NULL, " " );
            argc++;
        }
        
        argv[argc-1] = strtok(argv[argc-1],"\n");
        argv[argc] = NULL;
        DEBUG_OUTPUT(printf("argc = %d\n", argc));
        
        //      if (input[0] == '!')
        //         retriev_history(hist,current,input);
        //      else {
        int status = run_built_in_func(argv,hist,current);
        if(!status)
            sh_execute(argv, argc);
        DEBUG_OUTPUT(printf("nice fish ~~~~~\n"));
        //      }
    }
    
    return 0;
}

