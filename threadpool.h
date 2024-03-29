#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include <assert.h>

typedef struct task_s {
  void (* method) (void *);
  void * arg;
  struct task_s * next;
} task_t;

typedef struct threadpool_s {

  pthread_mutex_t mutex;
  pthread_cond_t q_not_empty;
  pthread_t * threads;

  int num_threads;
  int run_flag;

  task_t *t_head;
  task_t *t_tail;

} threadpool;

#ifdef DEBUG
#define deref_threadpool(_n) (assert((_n)!=0), (_n))
#define deref_task_t(_n) (assert((_n)!=0), (_n))
#else
#define deref_threadpool(_n) (_n)
#define deref_task_t(_n) (_n)
#endif

#define x_head_threadpool(_n) (deref_threadpool(_n)->t_head)
#define x_tail_threadpool(_n) (deref_threadpool(_n)->t_tail)
#define x_threads_threadpool(_n) (deref_threadpool(_n)->threads)
#define x_num_threads_threadpool(_n) (deref_threadpool(_n)->num_threads)
#define x_run_flag_threadpool(_n) (deref_threadpool(_n)->run_flag)
#define head_threadpool(_n) ((void)0, x_head_threadpool(_n))
#define tail_threadpool(_n) ((void)0, x_tail_threadpool(_n))
#define threads_threadpool(_n) ((void)0, x_threads_threadpool(_n))
#define num_threads_threadpool(_n) ((void)0, x_num_threads_threadpool(_n))
#define run_flag_threadpool(_n) ((void)0, x_run_flag_threadpool(_n))

#define x_next_task_t(_n) (deref_task_t(_n)->next)
#define next_task_t(_n) ((void)0, x_next_task_t(_n))

#ifdef DEBUG
#define addr_mutex_threadpool(_n) ((void)0, (assert((_n)!=0),  &((_n)->mutex)))
#define addr_q_not_empty_threadpool(_n) ((void)0, (assert((_n)!=0),  &((_n)->q_not_empty)))
#else
#define addr_mutex_threadpool(_n) ((void)0, &((_n)->mutex))
#define addr_q_not_empty_threadpool(_n) ((void)0, &((_n)->q_not_empty))
#endif

//ugly tmp macro
#ifdef DEBUG
#define THREADPOOL_DISPLAY_TASKS(_n) \
  do { \
  printf("task list:\n"); \
  task_t * tmp = head_threadpool(_n); \
  int task_count = 0; \
  while(tmp != NULL){ \
    task_count++; \
    printf("task %p\n", tmp); \
    tmp = next_task_t(tmp); \
  } \
  printf("task count: %d\n", task_count); \
  } while(0) 
#else
#define THREADPOOL_DISPLAY_TASKS(_n) do {} while(0)
#endif

typedef void (*threadpool_dispatch_fn) (void *);
typedef void (*threadpool_each_task_fn) (task_t *task, void * arg);

void * threadpool_work(void * arg);
threadpool * threadpool_create(int num_threads);
void threadpool_destroy(threadpool * pool, threadpool_each_task_fn func, void * arg);
void threadpool_dispatch(threadpool *pool, threadpool_dispatch_fn fn, void * arg); 

#endif
