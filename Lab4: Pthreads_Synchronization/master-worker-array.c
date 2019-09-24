#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int item_to_produce;
int item_to_consume;
int items_consumed;
int total_items, max_buf_size, num_workers;
int* buffer;
int currIndex;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// declare any global data structures, variables, etc that are required
// e.g buffer to store items, pthread variables, etc

void print_produced(int num) {

  printf("Produced %d\n", num);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);

}


/* produce items and place in buffer (array or linked list)
 * add more code here to add items to the buffer (these items will be consumed
 * by worker threads)
 * use locks and condvars suitably
 */
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
    {

      if(item_to_produce >= total_items) break;

      pthread_mutex_lock(&lock);
      while(currIndex >= max_buf_size){
        pthread_cond_wait(&empty, &lock);
      }
      buffer[currIndex] = item_to_produce;
      currIndex++;

      print_produced(item_to_produce);
      item_to_produce++;
      pthread_cond_broadcast(&full);
      pthread_mutex_unlock(&lock);
    }
  return 0; 
}

void *consume_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
    {
      if(items_consumed>=total_items) break;

      pthread_mutex_lock(&lock);

      while(currIndex == 0){
        if(items_consumed >= total_items) break; 
        pthread_cond_wait(&full, &lock);
      }

      // pthread_mutex_lock(&lock);

      if(items_consumed >= total_items){
        // printf("Idhar");
        pthread_mutex_unlock(&lock);
        break;
      }


      item_to_consume = buffer[currIndex-1];
      currIndex--;
      items_consumed++;
      print_consumed(item_to_consume,thread_id);
      // printf("Item to Produce %d\n", item_to_produce);

      // printf("currIndex %d\n",currIndex);
      pthread_cond_signal(&empty);
      pthread_mutex_unlock(&lock);
    }
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item

int main(int argc, char *argv[])
{

  int master_thread_id = 0;
  pthread_t master_thread;
  

  if (argc < 4) {
    printf("./master-worker #total_items #max_buf_size #num_workers e.g. ./exe 10000 1000 4\n");
    exit(1);
  }
  else {
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }

  item_to_produce = 0;
  item_to_consume = 0;
  items_consumed = 0;
  buffer =(int*) malloc(max_buf_size*sizeof(int));
  currIndex = 0;

  // Initlization code for any data structures, variables, etc


  //create master producer thread
  pthread_create(&master_thread, NULL, generate_requests_loop, (void *)&master_thread_id);

  //create worker consumer threads
  pthread_t workers[num_workers];
  int worker_thread_id=0;
  for(int i = 0; i <num_workers;i++){
      pthread_create(&workers[i], NULL, consume_requests_loop, (void *)&worker_thread_id);
      worker_thread_id++;
  }


  //wait for all threads to complete
  pthread_join(master_thread, NULL);
  printf("master joined\n");
  for(int i =0; i <num_workers; i++){
    pthread_join(workers[i],NULL);
    printf("worker %d joined\n",i);
  }

  //deallocate and free up any memory you allocated

  return 0;
}
