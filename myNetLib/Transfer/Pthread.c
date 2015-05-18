
//#define FIRST_SAMPLE
//#define SECOND_SAMPLE
//#define THIRD_SAMPLE
//#define FOUTH_SAMPLE
#define FIFTH_SAMPLE


#ifdef FIRST_SAMPLE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_message_function( void *ptr );

main()
{
     pthread_t thread1, thread2;
     char *message1 = "Thread 1";
     char *message2 = "Thread 2";
     int  iret1, iret2;

    /* Create independant threads each of which will execute function */

     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);

     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */

     pthread_join( thread1, NULL);
     pthread_join( thread2, NULL); 

     printf("Thread 1 returns: %d\n",iret1);
     printf("Thread 2 returns: %d\n",iret2);
     exit(0);
}

void *print_message_function( void *ptr )
{
     char *message;
     message = (char *) ptr;
     printf("%s \n", message);
}

#endif

#ifdef SECOND_SAMPLE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *functionC();
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int  counter = 0;

main()
{
   int rc1, rc2;
   pthread_t thread1, thread2;

   /* Create independant threads each of which will execute functionC */

   if( (rc1=pthread_create( &thread1, NULL, &functionC, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc1);
   }

   if( (rc2=pthread_create( &thread2, NULL, &functionC, NULL)) )
   {
      printf("Thread creation failed: %d\n", rc2);
   }

   /* Wait till threads are complete before main continues. Unless we  */
   /* wait we run the risk of executing an exit which will terminate   */
   /* the process and all threads before the threads have completed.   */

   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL); 

   exit(0);
}

void *functionC()
{
   pthread_mutex_lock( &mutex1 );
   counter++;
   printf("Counter value: %d\n",counter);
   pthread_mutex_unlock( &mutex1 );
}

#endif

#ifdef THIRD_SAMPLE

#include <stdio.h>
#include <pthread.h>

#define NTHREADS 10
void *thread_function();
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
int  counter = 0;

main()
{
   pthread_t thread_id[NTHREADS];
   int i, j;

   for(i=0; i < NTHREADS; i++)
   {
      pthread_create( &thread_id[i], NULL, &thread_function, NULL );
   }

   for(j=0; j < NTHREADS; j++)
   {
      pthread_join( thread_id[j], NULL); 
   }
  
   /* Now that all threads are complete I can print the final result.     */
   /* Without the join I could be printing a value before all the threads */
   /* have been completed.                                                */

   printf("Final counter value: %d\n", counter);
}

void *thread_function()
{
   printf("Thread number %ld\n", pthread_self());
   pthread_mutex_lock( &mutex1 );
   counter++;
   pthread_mutex_unlock( &mutex1 );
}

#endif

#ifdef FOUTH_SAMPLE
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t count_mutex     = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  condition_cond  = PTHREAD_COND_INITIALIZER;

void *functionCount1();
void *functionCount2();
int  count = 0;
#define COUNT_DONE  10
#define COUNT_HALT1  3
#define COUNT_HALT2  6

main()
{
   pthread_t thread1, thread2;

   pthread_create( &thread1, NULL, &functionCount1, NULL);
   pthread_create( &thread2, NULL, &functionCount2, NULL);
   pthread_join( thread1, NULL);
   pthread_join( thread2, NULL);

   exit(0);
}

void *functionCount1()
{
   for(;;)
   {
      pthread_mutex_lock( &condition_mutex );
      while( count >= COUNT_HALT1 && count <= COUNT_HALT2 )
      {
         pthread_cond_wait( &condition_cond, &condition_mutex );
      }
      pthread_mutex_unlock( &condition_mutex );

      pthread_mutex_lock( &count_mutex );
      count++;
      printf("Counter value functionCount1: %d\n",count);
      pthread_mutex_unlock( &count_mutex );

      if(count >= COUNT_DONE) return(NULL);
    }
}

void *functionCount2()
{
    for(;;)
    {
       pthread_mutex_lock( &condition_mutex );
       if( count < COUNT_HALT1 || count > COUNT_HALT2 )
       {
          pthread_cond_signal( &condition_cond );
       }
       pthread_mutex_unlock( &condition_mutex );

       pthread_mutex_lock( &count_mutex );
       count++;
       printf("Counter value functionCount2: %d\n",count);
       pthread_mutex_unlock( &count_mutex );

       if(count >= COUNT_DONE) return(NULL);
    }

}

#endif

#ifdef FIFTH_SAMPLE
 
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


  void reader_function(void);
  void writer_function(void);
  char make_new_item();
  void consume_item(char);
  
  char buffer;
  int buffer_has_item = 0;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  struct timespec delay;
  int cnt = 0;
 
  main()
  {
     pthread_t reader;
     const pthread_mutexattr_t *pthread_attr_default = NULL;
  
     delay.tv_sec = 2;
     delay.tv_nsec = 0;
  
//   pthread_mutex_init(&mutex, pthread_mutexattr_default);
//     pthread_create( &reader, pthread_attr_default, (void*)&reader_function,
//                    NULL);
     pthread_create( &reader, NULL, (void*)&reader_function, NULL);

     writer_function();

  }

  char make_new_item()
  {
	char c = 'A' + cnt++;
	printf("Make New Item %c\n", c);
	return c;
  }

#ifndef pthread_delay_np
  void pthread_delay_np(struct timespec *t)
  {
	printf("--");
	usleep(t);
  }
#endif

  void consume_item(char s)
  {
	printf("Consume Item %c\n", s);
  }
  
  void writer_function(void)
  {
     while(1)
     {
          pthread_mutex_lock( &mutex );
          if ( buffer_has_item == 0 )
          {
               buffer = make_new_item();
               buffer_has_item = 1;
          }
	  else printf(".");
          pthread_mutex_unlock( &mutex );
          pthread_delay_np( &delay );
     }
  }
  
  void reader_function(void)
  {
     while(1)
     {
          pthread_mutex_lock( &mutex );
          if ( buffer_has_item == 1)
          {
               consume_item( buffer );
               buffer_has_item = 0;
          }
	  else printf("*");
          pthread_mutex_unlock( &mutex );
          pthread_delay_np( &delay );
     }
  }
#endif

