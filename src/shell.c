/*
 * Copyright (C) 2002, Simon Nieuviarts
 */

#include <stdio.h>
#include <stdlib.h>
#include "readcmd.h"
#include "csapp.h"
#include "cmdexecution.h"

int main()
{
    Signal(SIGCHLD, handler);
	while (1) {
		struct cmdline *l;
		int i, j;
		char **vars= (char **) malloc(1*sizeof(char *));
		vars[0]=NULL;

		printf("mini-shell> ");
		l = readcmd();

		/* If input stream closed, normal termination */
		if (!l) {
			printf("exit\n");
			exit(0);
		}

		if (l->err) {
			/* Syntax error, read another command */
			printf("error: %s\n", l->err);
			continue;
		}

		if (l->in) printf("in: %s\n", l->in);
		if (l->out) printf("out: %s\n", l->out);

		/* Display each command of the pipe */
		for (i=0; l->seq[i]!=0; i++) {
			char **cmd = l->seq[i];
			printf("seq[%d]: ", i);
			for (j=0; cmd[j]!=0; j++) {
				printf("%s ", cmd[j]);
			}
			printf("\n");
		}

		if(l->seq[0] == NULL){
            fprintf(stderr,"Error : No command found\n");
            continue;
		}
		//Quit if asked
        if( (!strcmp(l->seq[0][0],"quit") || !strcmp(l->seq[0][0],"q")) && l->seq[1] == NULL && l->seq[0][1] == NULL ) {
            printf("quit\n");
            exit(0);
        }

        //execution of a single pipe
        executePipes(l);

	}
}
