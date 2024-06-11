#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
        /* TODO: put a new process to queue [q] */
        // printf("\t=> q->size: %d\n",q->size);
        if(q->size == MAX_QUEUE_SIZE) 
        {
                // printf("\tTH1: \n");
                exit(1);
        }
        else if (q->size == 0)
        {
                // printf("\tTH2: \n");
                q->proc[0] = proc;
                q->size++;
        }
        else {
                // printf("\tTH3: \n");
                q->proc[q->size++] = proc;
        }
        // printf("\t=> AFTER: q->size: %d\n",q->size);
}

struct pcb_t * dequeue(struct queue_t * q) {
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */
        if(q==NULL || q->size == 0) {
                // printf("\tReturn NULL is dequeue line 36 in queue.c: \n");
                return NULL;
        }
        struct pcb_t *proc = q->proc[0];
        for(int i = 0; i < q->size -1 ; i++)
        {
                q->proc[i] = q->proc[i+1];
        }
        q->proc[q->size - 1] = NULL;
        q->size--;
	return proc;
}
