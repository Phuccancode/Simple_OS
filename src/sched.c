
#include "queue.h"
#include "sched.h"
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
static struct queue_t ready_queue;
static struct queue_t run_queue;
static pthread_mutex_t queue_lock;

#ifdef MLQ_SCHED
static struct queue_t mlq_ready_queue[MAX_PRIO];
static int slot[MAX_PRIO];
void decrease_slot(int index)
{
	if(index < 0 || index >= MAX_PRIO){exit(1);}
	slot[index]--;
}
#endif

int queue_empty(void) {
#ifdef MLQ_SCHED
	unsigned long prio;
	for (prio = 0; prio < MAX_PRIO; prio++)
		if(!empty(&mlq_ready_queue[prio])) 
			return -1;
#endif
	return (empty(&ready_queue) && empty(&run_queue));
}

void init_scheduler(void) {
#ifdef MLQ_SCHED
	int i ;
	for (i = 0; i < MAX_PRIO; i ++)
	{
		mlq_ready_queue[i].size = 0;
		slot[i] = MAX_PRIO - i;
	}
#endif
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}

#ifdef MLQ_SCHED
void heal_slot(void)
{
	for ( int j = 0;  j < MAX_PRIO; j++)
	{
		slot[j] = MAX_PRIO - j;
	}
}
struct pcb_t* get_prior_of_next_proc(void)
{
	struct pcb_t * proc = NULL;
	struct queue_t * q;
	int i;
	for(i = 0;  i < MAX_PRIO ; i++)
	{
		q = &mlq_ready_queue[i];
		if(!empty(q) && slot[i] > 0)
		{
			// proc = dequeue(&mlq_ready_queue[i]);
			if(q != NULL)
			{
				proc = q->proc[0];
				return proc;
			}
			break;
		}
	}
	return proc;
}
/* 
 *  Stateful design for routine calling
 *  based on the priority and our MLQ policy
 *  We implement stateful here using transition technique
 *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
 */
struct pcb_t * get_mlq_proc(void) {	
	struct pcb_t * proc = NULL;
	/*TODO: get a process from PRIORITY [ready_queue].
	 * Remember to use lock to protect the queue.
	 */
	pthread_mutex_lock(&queue_lock);
	int i;
	// printf("\tMAX_PRIO#88 schec.c = %d\n", MAX_PRIO);
	for(i = 0;  i < MAX_PRIO ; i++)
	{
		if(!empty(&mlq_ready_queue[i]) && slot[i] > 0)
		{
			// printf("\tget pcb_t in queue[%d]: \n", i);
			proc = dequeue(&mlq_ready_queue[i]);
			slot[i]--;
			if ( i == (MAX_PRIO - 1))
			{
				// printf("\t -> restore slot line 97 in sched.c\n");
				for ( int j = 0;  j < MAX_PRIO; j++)
				{
					slot[j] = MAX_PRIO - j;
				}
			}
			break;
		}
	}
	if ( i == MAX_PRIO )
	{
		// printf("\t -> restore slot line 108 in sched.c\n");
		for ( int j = 0;  j < MAX_PRIO; j++)
		{
			slot[j] = MAX_PRIO - j;
		}
	}
	pthread_mutex_unlock(&queue_lock);
	return proc;
}
int check_slot_empty(int index)
{
	if(index < 0 || index >= MAX_PRIO)
	{
		return -1;
	}
	if(slot[index] == 0)
	{
		return 1;
	}
	else return 0;
}
int check_empty_queue_at_index(int index)
{
	if(index < 0 || index >= MAX_PRIO)
	{
		return -1;
	}
	if(mlq_ready_queue[index].size == 0){
		return 1;
	}
	else return 0;
}
void put_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	// printf("add proc into %d queue\n",proc->prio);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_mlq_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);	
}

struct pcb_t * get_proc(void) {
	return get_mlq_proc();
}

void put_proc(struct pcb_t * proc) {
	return put_mlq_proc(proc);
}

void add_proc(struct pcb_t * proc) {
	return add_mlq_proc(proc);
}
#else
struct pcb_t * get_proc(void) {
	struct pcb_t * proc = NULL;
	/*TODO: get a process from [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */
	return proc;
}

void put_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&run_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_proc(struct pcb_t * proc) {
	pthread_mutex_lock(&queue_lock);
	enqueue(&ready_queue, proc);
	pthread_mutex_unlock(&queue_lock);	
}
#endif


