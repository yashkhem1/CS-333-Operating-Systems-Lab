#include "rwlock.h"

void InitalizeReadWriteLock(struct read_write_lock * rw)
{
  //	Write the code for initializing your read-write lock.
	rw->readCount = 0;
	rw->writeCount = 0;
	rw->writePresent = 0;
	rw->mutex = PTHREAD_MUTEX_INITIALIZER;
	rw->read = PTHREAD_COND_INITIALIZER;
	rw->write = PTHREAD_COND_INITIALIZER;
}

void ReaderLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the reader.
	pthread_mutex_lock(&rw->mutex);
	while(rw->writePresent){
		pthread_cond_wait(&rw->read,&rw->mutex);
	}
	rw->readCount++;
	pthread_mutex_unlock(&rw->mutex);	
}

void ReaderUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the reader.
	pthread_mutex_lock(&rw->mutex);
	rw->readCount--;
	if(rw->readCount==0){
		pthread_cond_broadcast(&rw->write);	
	}
	pthread_mutex_unlock(&rw->mutex);
}

void WriterLock(struct read_write_lock * rw)
{
  //	Write the code for aquiring read-write lock by the writer.
	pthread_mutex_lock(&rw->mutex);
	while(rw->readCount > 0 || rw->writePresent)
		pthread_cond_wait(&rw->write,&rw->mutex);
	rw->writePresent=1;
	pthread_mutex_unlock(&rw->mutex);
}

void WriterUnlock(struct read_write_lock * rw)
{
  //	Write the code for releasing read-write lock by the writer.
	pthread_mutex_lock(&rw->mutex);
	rw->writePresent=0;
	pthread_cond_broadcast(&rw->write);
	pthread_cond_broadcast(&rw->read);
	pthread_mutex_unlock(&rw->mutex);
}
