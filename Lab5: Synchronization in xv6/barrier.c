/*----------xv6 sync lab----------*/
#include "types.h"
#include "x86.h"
#include "spinlock.h"
#include "defs.h"
#include "barrier.h"

//define any variables needed here
int barrier_count;
struct spinlock barrier_lock;

int
barrier_init(int n)
{
  //to be done
    barrier_count=n;
    initlock(&barrier_lock,"blk");

  return 0;
}

int
barrier_check(void)
{
  //to be done
    acquire(&barrier_lock);
    barrier_count--;
    if(barrier_count==0){
        wakeup(&barrier_count);
    }

    else{
        sleep(&barrier_count,&barrier_lock);
    }
    release(&barrier_lock);
  return 0;
}

/*----------xv6 sync lock end----------*/
