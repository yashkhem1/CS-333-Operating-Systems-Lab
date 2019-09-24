#include "types.h"
#include "stat.h"
#include "user.h"

int main()
{
	int ret;
	int i;
	ucounter_init();
	// int flag=1;

	ret = fork();
	if(ret == 0)
	{
	  for(i=0; i < 10; i++) {
	  	uspinlock_acquire(0);
	    printf(1, "I am child\n");
	    ucounter_set(0,1);
	    // ucv_wakeup(1236);

	    ucv_wakeup(1235);
	    ucv_sleep(1234,0);
	    uspinlock_release(0);
	  }
	  ucv_wakeup(1235);
          exit();
	}
	else
	{
		// uspinlock_acquire(0);
		// ucv_sleep(1236,0);
		// uspinlock_release(0);
		// sleep(1);
	  while(ucounter_get(0)!=1);
	  for(i=0; i < 10; i++) {
	  	uspinlock_acquire(0);
	    printf(1, "I am parent\n");
	    ucv_wakeup(1234);
	    ucv_sleep(1235,0);
	    uspinlock_release(0);
	  }
	  ucv_wakeup(1234);
	  wait();
          exit();
	}
}

