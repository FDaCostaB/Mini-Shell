//
// Created by fabie on 24/02/2021.
//

#ifndef TP4_CMDEXECUTION_H
#define TP4_CMDEXECUTION_H

void executeCmd (struct cmdline *l);
void executePipe(struct cmdline *l);
void executePipes(struct cmdline *l);
void redirect(char *file, int old);
void handler(int sig);

#endif //TP4_CMDEXECUTION_H
