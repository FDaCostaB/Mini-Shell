//
// Created by fabie on 24/02/2021.
//
#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include "cmdexecution.h"

#define NMAX 10

void executeCmd (struct cmdline *l){
    int status;
    int pid = Fork();
    if(pid==0) {
        redirect(l->in, 0);
        redirect(l->out, 1);
        Execvp(l->seq[0][0], l->seq[0]);
    }else {
        Wait(&status);
    }
}

void redirect(char *file, int old){
    if (file) {
        int fd;
        if(old) fd = Open(file, O_WRONLY | O_CREAT, S_IRWXU);
        else fd = Open(file,O_RDONLY, S_IRWXU);
        if (fd < 0) {
            printf("%s : Permission denied.\n", file);
            exit(-1);
        }
        Dup2(fd, old);
    }
}
void executePipe(struct cmdline *l){
    int status;

    pid_t childpid1,childpid2;
    int fd[2];
    pipe(fd);

    childpid1 = Fork();
    if (childpid1 == 0) { /* fils */
        // Je ferme la lecture
        Close(fd[0]);
        // J'écris dans le pipe
        Dup2(fd[1],1);
        redirect(l->in, 0);
        Execvp(l->seq[0][0],l->seq[0]);
    } else { /* pere */
        Waitpid(childpid1,&status,0);
        childpid2 = Fork();
        if (childpid2 == 0) {
            //Je ferme l'écriture puis je lis quand  le pere a fini d'écrire.
            Close(fd[1]);
            Dup2(fd[0],0);
            redirect(l->out, 1);
            Execvp(l->seq[1][0],l->seq[1]);
        } else {
            Close(fd[0]);
            Close(fd[1]);
            Waitpid(childpid2,&status,0);
        }
    }
}

void executePipes(struct cmdline *l){
    int status;
    pid_t childpid[NMAX];
    int fd[NMAX-1][2];
    int i = 1;

    if(!l->bg) Signal(SIGCHLD, SIG_DFL);
    //decompte des commandes
    while(l->seq[i] != NULL && i < NMAX){
        pipe(fd[i-1]);
        i++;
    }

    if(i>=NMAX) {
        unix_error("Execution error : too many pipes");
    }


    for(int j = 0; j < i; j++){
        childpid[j] = Fork();
        if(childpid[j]==0){
            for(int k = 0; k < i-1; k++){ // NMAX - 1 = 9
                if (k != j && k != j-1){  // cmd 0 - cmd 1 - cmd 2 - cmd 3
                    Close(fd[k][0]);
                    Close(fd[k][1]);
                }
            }
            if(j-1>=0){
                //Je ferme l'écriture puis je lis quand  le grand frere a fini d'écrire.
                Close(fd[j-1][1]);
                Dup2(fd[j-1][0],0);
            } else {
                redirect(l->in,0);
            }
            if(j < i-1){
                // Je ferme la lecture
                Close(fd[j][0]);
                //J'écris dans le pipe
                Dup2(fd[j][1],1);
            } else {
                redirect(l->out,1);
            }
            Execvp(l->seq[j][0],l->seq[j]);
            break;
        }
    }
    for(int k = 0; k < i-1; k++){
        Close(fd[k][0]);
        Close(fd[k][1]);
    }
    if(!l->bg){
        for(int j = 0; j < i; j++){
            Waitpid(childpid[j],&status,0);
        }
        Signal(SIGCHLD, handler);
    }
}

void handler(int sig)
{
    pid_t pid;

    while( (pid = waitpid(-1, NULL, WNOHANG|WUNTRACED)) >= 0);
        //printf("Handler reaped child %d\n", (int)pid);
    return;
}