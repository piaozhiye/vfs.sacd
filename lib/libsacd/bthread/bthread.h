#ifndef _BTHREAD_H_
#define _BTHREAD_H_

#include <pthread.h>

#define PTHREAD_CANCEL_ENABLE    	 0x00000010
#define PTHREAD_CANCEL_DISABLE   	 0x00000000

#define PTHREAD_CANCEL_ASYNCHRONOUS 0x00000020
#define PTHREAD_CANCEL_DEFERRED     0x00000000

#define PTHREAD_CANCELED ((void *) -1)

struct pthread_internal_t {
  struct pthread_internal_t* next;
  struct pthread_internal_t* prev;

  pid_t tid;

  void** tls;

  volatile pthread_attr_t attr;

  __pthread_cleanup_t* cleanup_stack;

  void* (*start_routine)(void*);
  void* start_routine_arg;
  void* return_value;

  void* alternate_signal_stack;

  /*
   * The dynamic linker implements dlerror(3), which makes it hard for us to implement this
   * per-thread buffer by simply using malloc(3) and free(3).
   */
#define __BIONIC_DLERROR_BUFFER_SIZE 508
  char dlerror_buffer[__BIONIC_DLERROR_BUFFER_SIZE];
	
	//  ugly hack: use last 4 bytes of dlerror_buffer as cancel_lock
	pthread_mutex_t cancel_lock; 
};

/* Has the thread a cancellation request? */
#define PTHREAD_ATTR_FLAG_CANCEL_PENDING 0x00000008

/* Has the thread enabled cancellation? */
#define PTHREAD_ATTR_FLAG_CANCEL_ENABLE 0x00000010

/* Has the thread asyncronous cancellation? */
#define PTHREAD_ATTR_FLAG_CANCEL_ASYNCRONOUS 0x00000020

/* Has the thread a cancellation handler? */
#define PTHREAD_ATTR_FLAG_CANCEL_HANDLER 0x00000040

struct pthread_internal_t *__pthread_getid ( pthread_t );

int __pthread_do_cancel (struct pthread_internal_t *);

void pthread_init(void);
int pthread_cancel(pthread_t t);
#endif
