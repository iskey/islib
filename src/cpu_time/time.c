#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>

#define handle_error(msg) \
  do { perror(msg); exit(EXIT_FAILURE);} while (0)

#define handle_error_en(en, msg) \
  do {errno = en; perror(msg); exit(EXIT_FAILURE);} while(0)

static void pclock(char *msg, clockid_t cid)
{
   struct timespec ts;

   printf("%s", msg);
   if (clock_gettime(cid, &ts) == -1)
	   handle_error("clock_gettime");
   printf("%4ld.%03ld\n", ts.tv_sec, ts.tv_nsec / 1000000);
}

static void *thread_process(void *arg)
{
	printf("Subthread staring infinite loop\n");
	for(;;);
}

int main()
{
  pthread_t thread;
  clockid_t cid;
  int i,ret;

  ret = pthread_create(&thread, NULL, thread_process, NULL);
  if (0!= ret)
	handle_error_en(ret, "pthread_create error");

	printf("Main thread sleeping.");
	sleep(1);

	printf("Main thread consuming some CPU time..\n");
	for(i=0; i< 20000000; i++)
		getppid();

	pclock("Process total CPU time", CLOCK_PROCESS_CPUTIME_ID);

	ret = pthread_getcpuclockid(pthread_self(), &cid);
	if (0!= ret)
		handle_error_en(ret, "pthread_getcpuclockid");

	pclock("Main thread CPU time: ", cid);

	ret = pthread_getcpuclockid(thread, &cid);
	if(0!= ret)
		handle_error_en(ret, "pthread_getcpuclockid");

	pclock("Subthread CPU time: ", cid);

	exit(EXIT_SUCCESS);
}
