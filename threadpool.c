#include <stdio.h>
#include <stdlib.h>

#include "threadpool.h"

void * threadpool_work(void * arg){
  threadpool * pool = (threadpool *) arg;
  task_t * current;

  while(1) {
    pthread_mutex_lock(addr_mutex_threadpool(pool));
    while(head_threadpool(pool)  == NULL && run_flag_threadpool(pool)){
      pthread_cond_wait(addr_q_not_empty_threadpool(pool), addr_mutex_threadpool(pool));
    }
    if(!run_flag_threadpool(pool)){
      pthread_mutex_unlock(addr_mutex_threadpool(pool));
      break;
    }
    current = head_threadpool(pool);
    x_head_threadpool(pool) = next_task_t(current);

    if (head_threadpool(pool) == NULL) {
      x_tail_threadpool(pool) = NULL;
    }
    pthread_mutex_unlock(addr_mutex_threadpool(pool));

    (current->method) (current->arg);
    free(current);
  }
  return 0;
}

threadpool * threadpool_create(int num_threads) {
  threadpool * pool;
  int i;

  pool = (threadpool *) malloc(sizeof(threadpool));
  if(pool == NULL){
    fprintf(stderr, "Unable to create threadpool\n");
    return NULL;
  }
  x_run_flag_threadpool(pool) = 1;
  x_num_threads_threadpool(pool) = num_threads;
  x_head_threadpool(pool) = NULL;
  x_tail_threadpool(pool) = NULL;
  x_threads_threadpool(pool) = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
  pthread_mutex_init(addr_mutex_threadpool(pool), NULL);
  pthread_cond_init(addr_q_not_empty_threadpool(pool), NULL);

  for(i=0;i < num_threads; i++){
    if (pthread_create(&(pool->threads[i]), NULL, threadpool_work, pool) != 0) {
      fprintf(stderr, "Failed to create thread\n");
      return NULL;
    }
  }
  return pool;
}

void threadpool_dispatch(threadpool *pool, threadpool_dispatch_fn fn, void * arg) {
  task_t *current_task;
  current_task = (task_t *) malloc(sizeof(task_t));
  if(current_task == NULL) {
    fprintf(stderr, "Failed to create task\n");
    return;
  }
  current_task->method = fn;
  current_task->arg = arg;
  x_next_task_t(current_task) = NULL;

  pthread_mutex_lock(addr_mutex_threadpool(pool));

  if (head_threadpool(pool) == NULL) {
    x_head_threadpool(pool) = current_task;
  } else {
    x_next_task_t(tail_threadpool(pool)) = current_task;
  }
  x_tail_threadpool(pool) = current_task;
  
  //THREADPOOL_DISPLAY_TASKS(pool);
  pthread_cond_signal(addr_q_not_empty_threadpool(pool));
  pthread_mutex_unlock(addr_mutex_threadpool(pool));
}

void threadpool_destroy(threadpool * pool, threadpool_each_task_fn func, void * arg){
  task_t * next;
  task_t * tmp;

  assert(pool != NULL);

  pthread_mutex_lock(addr_mutex_threadpool(pool));
  tmp = head_threadpool(pool);
  x_run_flag_threadpool(pool) = 0;

  pthread_cond_broadcast(addr_q_not_empty_threadpool(pool));
  x_head_threadpool(pool) = NULL;
  x_tail_threadpool(pool) = NULL;
  pthread_mutex_unlock(addr_mutex_threadpool(pool));

  for(int i = 0; i < num_threads_threadpool(pool); i++) {
    pthread_join(threads_threadpool(pool)[i], NULL);
  }

  while(tmp != NULL){
    next = next_task_t(tmp);
    if(func != NULL){
      (func)(tmp, arg);
    }
    free(tmp);
    tmp = next;
  }

  pthread_mutex_destroy(addr_mutex_threadpool(pool));
  pthread_cond_destroy(addr_q_not_empty_threadpool(pool));
  free(threads_threadpool(pool));
  free(pool);
}
