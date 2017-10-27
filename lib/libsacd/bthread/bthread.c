#include <signal.h>
#include <pthread.h>
#include <errno.h>
#include "bthread.h"

void pthread_cancel_handler(int signum) {
	pthread_exit(0);
}

void pthread_init(void) {
	struct sigaction sa;
	struct pthread_internal_t * p = (struct pthread_internal_t *)pthread_self();
	
	if(p->attr.flags & PTHREAD_ATTR_FLAG_CANCEL_HANDLER)
		return;
	
	// set thread status as pthread_create should do.
	// ASYNCROUNOUS is not set, see pthread_setcancelstate(3)
	p->attr.flags |= PTHREAD_ATTR_FLAG_CANCEL_HANDLER|PTHREAD_ATTR_FLAG_CANCEL_ENABLE;
	
	sa.sa_handler = pthread_cancel_handler;
	sigemptyset(&(sa.sa_mask));
	sa.sa_flags = 0;
	
	sigaction(SIGRTMIN, &sa, NULL);
}

static void call_exit (void)
{
  pthread_exit (0);
}

int __pthread_do_cancel (struct pthread_internal_t *p)
{
	
	if(p == (struct pthread_internal_t *)pthread_self())
    call_exit ();
  else if(p->attr.flags & PTHREAD_ATTR_FLAG_CANCEL_HANDLER)
    pthread_kill((pthread_t)p, SIGRTMIN);
	else
		pthread_kill((pthread_t)p, SIGTERM);

  return 0;
}

int pthread_cancel (pthread_t t)
{
  int err = 0;
  struct pthread_internal_t *p = (struct pthread_internal_t*) t;
	
	pthread_init();
	
  pthread_mutex_lock (&p->cancel_lock);
  if (p->attr.flags & PTHREAD_ATTR_FLAG_CANCEL_PENDING)
    {
      pthread_mutex_unlock (&p->cancel_lock);
      return 0;
    }
    
  p->attr.flags |= PTHREAD_ATTR_FLAG_CANCEL_PENDING;

  if (!(p->attr.flags & PTHREAD_ATTR_FLAG_CANCEL_ENABLE))
    {
      pthread_mutex_unlock (&p->cancel_lock);
      return 0;
    }

  if (p->attr.flags & PTHREAD_ATTR_FLAG_CANCEL_ASYNCRONOUS) {
		pthread_mutex_unlock (&p->cancel_lock);
    err = __pthread_do_cancel (p);
	} else {
		// DEFERRED CANCEL NOT IMPLEMENTED YET
		pthread_mutex_unlock (&p->cancel_lock);
	}

  return err;
}
