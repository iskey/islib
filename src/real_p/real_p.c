#define _GNU_SOURCE
#include <sched.h>
#include "stdio.h"
#include "pthread.h"
#include "unistd.h"
#include "string.h"
#include "assert.h"
#include "time.h"

#define RELEASE
#ifdef RELEASE
#define BLOG_STR_(a,b,c) a#b"\n"
#else
#define BLOG_STR_(a,b,c) a#b c"\n"
#endif

#define LOG_NOW	LOG_DBG
#define LOG_DBG	20
#define LOG_INF	40
#define LOG_ERR	100

#if (LOG_NOW<=LOG_ERR)
    #define _LOG_ERR(format,...) fprintf(stderr, BLOG_STR_("LOG_ERR: ",format,"     ---- FILE:%s, %s():%d."), ##__VA_ARGS__,__FILE__,__func__,__LINE__);
#else
    #define _LOG_ERR(format,...)
#endif
#if (LOG_NOW<=LOG_INF)
    #define _LOG_INF(format,...) fprintf(stderr, BLOG_STR_("LOG_INF: ",format,"     ---- FILE:%s, %s():%d."), ##__VA_ARGS__,__FILE__,__func__,__LINE__);
#else
    #define _LOG_INF(format,...)
#endif
#if (LOG_NOW<=LOG_DBG)
    #define _LOG_DBG(format,...) fprintf(stderr, BLOG_STR_("LOG_DBG: ",format,"     ---- FILE:%s, %s():%d."), ##__VA_ARGS__,__FILE__,__func__,__LINE__);
#else
    #define _LOG_DBG(format,...)
#endif

#define blog(level, format, ...) _##level(format, ##__VA_ARGS__);

/** Maximum cpu number supported. */
#define MAX_CPU 128
/* Current priority */
static int g_priority= 98;
/* Current policy */
static int g_policy= SCHED_RR;
/* Max Load number */
#define LOAD_MAX 10000000
#define LOAD_STEP 100000
/* Current Load number */
static long g_load= 10;
static long g_sleep= 5;
/* This program will running within g_running_count-- every time.*/
static long g_running_count= 1;
/* Thread exit flag */
static int g_thread_exit= 0;
/* Attach cpu id */
static int g_attach_cpu_id= -1;

/*****************************************************************************
* º¯ Êý Ãû: show_thread_priority
* ¹¦ÄÜÃèÊö: Show max/min priority of the thread
* ÊäÈë²ÎÊý: int policy :  -- thread policy, 
*           SCHED_FIFO:
*           SCHED_RR:
*           SCHED_OTHER:
* Êä³ö²ÎÊý: ÎÞ
* ·µ »Ø Öµ: ÎÞ
* ×÷    Õß: xie.yichao
* ´´½¨ÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
* °æ    ±¾: V1.0
* ÐÞ¸Ä¼ÇÂ¼:
*  1.ÐÞ¸ÄÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
*    °æ ±¾ ºÅ: 
*    ÐÞ ¸Ä ÈË: xie.yichao
*    ÐÞ¸ÄÄÚÈÝ: 
*****************************************************************************/
static void show_thread_priority(int policy)
{
	int priority;
	priority= sched_get_priority_max(policy);
	assert(priority!= -1);
	blog(LOG_DBG, "max_priority=%d", priority);
	priority= sched_get_priority_min(policy);
	assert(priority!= -1);
	blog(LOG_DBG, "min_priority=%d", priority);
}

/*****************************************************************************
* º¯ Êý Ãû: get_thread_priority
* ¹¦ÄÜÃèÊö: Get the priority of the thread.
* ÊäÈë²ÎÊý: pthread_attr_t *attr :  -- attribute of the thread.
* Êä³ö²ÎÊý: NULL
* ·µ »Ø Öµ: the priority of the thread.
* ×÷    Õß: xie.yichao
* ´´½¨ÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
* °æ    ±¾: V1.0
* ÐÞ¸Ä¼ÇÂ¼:
*  1.ÐÞ¸ÄÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
*    °æ ±¾ ºÅ: 
*    ÐÞ ¸Ä ÈË: xie.yichao
*    ÐÞ¸ÄÄÚÈÝ: 
*****************************************************************************/
static int get_thread_priority(pthread_attr_t *attr)
{
	struct sched_param param;
	int ret= pthread_attr_getschedparam(attr, &param);
	assert(ret== 0);
	blog(LOG_DBG, "thread:%d priority is %d.", pthread_self(), param.__sched_priority);
	return param.__sched_priority;
}

/*****************************************************************************
* º¯ Êý Ãû: set_thread_priority
* ¹¦ÄÜÃèÊö: Set the priority of the thread
* ÊäÈë²ÎÊý: pthread_attr_t *attr :  -- attribute of the thread.
*           int priority :          -- the priority be set to the thread.
* Êä³ö²ÎÊý: ÎÞ
* ·µ »Ø Öµ: static
* ×÷    Õß: xie.yichao
* ´´½¨ÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
* °æ    ±¾: V1.0
* ÐÞ¸Ä¼ÇÂ¼:
*  1.ÐÞ¸ÄÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
*    °æ ±¾ ºÅ: 
*    ÐÞ ¸Ä ÈË: xie.yichao
*    ÐÞ¸ÄÄÚÈÝ: 
*****************************************************************************/
static int set_thread_priority(pthread_attr_t *attr, int priority)
{
	struct sched_param param;
	int ret= 0;
	ret= pthread_attr_getschedparam(attr, &param);
	assert(ret== 0);
	param.__sched_priority= priority;
	ret= pthread_attr_setschedparam(attr, &param);
	assert(ret== 0);
	return ret;
}


static int set_cur_process_priority(pthread_attr_t *attr, int policy, int priority)
{
	struct sched_param param;
	int ret= 0;
	ret= pthread_attr_getschedparam(attr, &param);
	assert(ret== 0);
	param.__sched_priority= priority;
	ret= sched_setscheduler(0, policy, &param);

	return ret;
}
/*****************************************************************************
* º¯ Êý Ãû: get_thread_policy
* ¹¦ÄÜÃèÊö: Get the policy to the thread with @attr.
* ÊäÈë²ÎÊý: pthread_attr_t *attr :  -- attribute of the thread.
* Êä³ö²ÎÊý: ÎÞ
* ·µ »Ø Öµ: the policy, suche as SCHED_FIFO, SCHED_RR, SCHED_OTHER.
* ×÷    Õß: xie.yichao
* ´´½¨ÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
* °æ    ±¾: V1.0
* ÐÞ¸Ä¼ÇÂ¼:
*  1.ÐÞ¸ÄÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
*    °æ ±¾ ºÅ: 
*    ÐÞ ¸Ä ÈË: xie.yichao
*    ÐÞ¸ÄÄÚÈÝ: 
*****************************************************************************/
static int get_thread_policy(pthread_attr_t *attr)
{
	int policy;
	int ret= 0;
	ret= pthread_attr_getschedpolicy(attr, &policy);
	assert(ret== 0);
	switch(policy)
	{
		case SCHED_FIFO:
			blog(LOG_DBG, "Current policy is SCHED_FIFO.");
			break;
		case SCHED_RR:
			blog(LOG_DBG, "Current policy is SCHED_RR.");
			break;
		case SCHED_OTHER:
			blog(LOG_DBG, "Current policy is SCHED_OTHER.");
			break;
		default:
			blog(LOG_DBG, "Current policy is UNKNOW.");
			break;
	}

	return policy;
}
/*****************************************************************************
* º¯ Êý Ãû: set_thread_policy
* ¹¦ÄÜÃèÊö: Set the @policy to the thread
* ÊäÈë²ÎÊý: pthread_attr_t *attr :  -- attribute of the thread.
*           int policy :            -- the policy of the thread.
* Êä³ö²ÎÊý: ÎÞ
* ·µ »Ø Öµ: 
* ×÷    Õß: xie.yichao
* ´´½¨ÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
* °æ    ±¾: V1.0
* ÐÞ¸Ä¼ÇÂ¼:
*  1.ÐÞ¸ÄÈÕÆÚ: 2015Äê2ÔÂ5ÈÕ
*    °æ ±¾ ºÅ: 
*    ÐÞ ¸Ä ÈË: xie.yichao
*    ÐÞ¸ÄÄÚÈÝ: 
*****************************************************************************/
static void set_thread_policy(pthread_attr_t *attr, int policy)
{
	int ret= pthread_attr_setschedpolicy(attr, policy);
	assert(ret== 0);
}

#define MAX_PRIME 100000
void do_primes()
{
    unsigned long i, num, primes = 0;
    for (num = 1; num <= MAX_PRIME; ++num) {
        for (i = 2; (i <= num) && (num % i != 0); ++i);
        if (i == num)
            ++primes;
    }
    printf("Calculated %d primes.\n", primes);
}

void *process_load(void *arg)
{
	int cpu_id= *(int*)arg;
	cpu_set_t mask;
	blog(LOG_DBG, "load pid:%d, arg is %d.", (int)pthread_self(), cpu_id);
	
	/** Set cpu affinity. */
	CPU_ZERO(&mask);
	CPU_SET(cpu_id, &mask);
	if(pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask)< 0){
		blog(LOG_ERR, "set thread affinity failed.");
	}
	else{
		blog(LOG_INF, "thread %d is running in processor %d.",
				        pthread_self(), cpu_id);
	}

	/** Query policy. */
	pthread_attr_t pid_attr;
	pthread_attr_init(&pid_attr);

    /** Get thread policy and priority. */
	get_thread_policy(&pid_attr);
	//get_thread_priority(&pid_attr);

    /** Set thread policy and priority. */
	set_thread_policy(&pid_attr, g_policy);
	//set_thread_priority(&pid_attr, g_priority);

    /** Get thread policy and priority again to 
      check if we have been set successfully. */
	get_thread_policy(&pid_attr);
	//get_thread_priority(&pid_attr);

	/** Set process priority. */
	set_cur_process_priority(&pid_attr, g_policy, g_priority);

	/** Load cpu. */
#if 0
	unsigned long volatile count= LOAD_MAX;
	unsigned int sleep_t= 0;
	while(!g_thread_exit&& count--)
	{
        if(count< 1){
            count= g_load;
            sleep_t= --sleep_t% g_sleep;
            usleep(sleep_t+ 3);
        }
	};
#endif
	/** Load cpu2. */
	unsigned long volatile count= g_running_count;
	struct timeval{
		long int tv_sec; // 秒数
		long int tv_usec; // 微秒数
	} local_time;

	blog(LOG_DBG, "Will running with count %ld.", count)
	if(count== 0)
	{
		while(1)
			usleep(g_sleep);
	}
	else
	{
		while(count--){
			usleep(g_sleep);
			do_primes();
		}
	}

	blog(LOG_INF, "thread:%d exited.", pthread_self());
}
static void console_help(char* argv[])
{
	blog(LOG_INF, "############## help ##################");
	blog(LOG_INF, "%s [option].", argv[0]);
	blog(LOG_INF, "-s: set usleep time for usleep(). default is %d.", g_sleep);
	blog(LOG_INF, "-t: set running time for exit. default is %d , "0" for never exited.", g_running_count);
	blog(LOG_INF, "-p: schedule policy, SCHED_OTHER:0/SCHED_FIFO:1/SCHED_RR:2.")
	blog(LOG_INF, "-n: schedule priority number, 1<X<140")
	blog(LOG_INF, "-a: attach cpu id, default to attach all.")
	blog(LOG_INF, "Example: %s -s 1 -t 1 -p 2 -n 99.", argv[0]);
	blog(LOG_INF, "If you have no idea, use '%s -a [cpu_id]' or just '%s'", argv[0],argv[0]);
	blog(LOG_INF, "#######################################");
}

#if 0
static void console_help(void)
{
	blog(LOG_INF, "########## console control ############");
	blog(LOG_INF, "Press \"+\" to increase CPU-LOAD.");
	blog(LOG_INF, "Press \"-\" to decrease CPU-LOAD.");
	blog(LOG_INF, "Press \"q\" to exit.");
	blog(LOG_INF, "#######################################");
}

static void console_loop(void)
{
	int m_exit= 0;
	unsigned char ch;
    while(!m_exit)
    {
        ch= getchar();
        switch(ch)
        {
            case '+':
                if(g_load+ LOAD_STEP< LOAD_MAX) g_load+= LOAD_STEP;
                blog(LOG_INF, "Current load is %lu.", g_load);
                break;
            case '-':
                if(g_load- LOAD_STEP> 0) g_load-= LOAD_STEP;
                blog(LOG_INF, "Current load is %lu.", g_load);
                break;
            case '*':
                if(g_sleep+ 1< 10) g_sleep++;
                blog(LOG_INF, "Current load is %lu.", g_sleep);
                break;
            case '/':
                if(g_sleep- 1> 0) g_sleep--;
                blog(LOG_INF, "Current load is %lu.", g_sleep);
                break;
            case 'q':
                g_thread_exit= 1;
                m_exit= 1;
                blog(LOG_INF, "Thread will exit.");
                break;
            case 'p':
            	blog(LOG_INF, "Current load is %d, sleep gap is %d.", g_load, g_sleep);
            	break;
            case '\n':
            	blog(LOG_INF, "Current load is %d, sleep gap is %d.", g_load, g_sleep);
            	system("uptime");
                break;
            default:
                blog(LOG_INF, "Operation not support.");
                break;
        }
    }
}
#endif

static int prase_param(int argc, char* argv[])
{
	int opt;
	while((opt= getopt(argc, argv, ":s:t:p:n:a:"))!= -1)
	{
		switch(opt)
		{
			case 'a':
			{
				int cpu_id;
				cpu_id= atoi(optarg);
				if(cpu_id< 0){
					blog(LOG_ERR, "parameter for "-a" need a valid number");
					goto ERR;
				}
				else{
					g_attach_cpu_id= cpu_id;
					blog(LOG_INF, "CPU[%d] will be attached", g_attach_cpu_id);
				}
				break;
			}
			case 's':
			{
				long sleep_time;
				sleep_time= atol(optarg);
				if(sleep_time< 0){
					blog(LOG_ERR, "sleep time can not be a negative number, sleep time will be set to default.");
					goto ERR;
				}
				else{
					g_sleep= sleep_time;
					blog(LOG_INF, "Sleep time be set to %ld.", g_sleep);
				}
				break;
			}
			case 't':
			{
				long running_time;
				running_time= atol(optarg);
				if(running_time< 0){
					blog(LOG_ERR, "running time number can not be a negative number, running time will be set to default.")
					goto ERR;
				}
				else{
					g_running_count= running_time;
					blog(LOG_INF, "Running time be set to %ld.", g_running_count);
				}
				break;
			}
			case 'p':
			{
				long policy;
				policy= atol(optarg);
				if(policy== 0){
					g_policy= SCHED_OTHER;
					blog(LOG_INF, "Schedule policy be set to SCHED_OTHER.");
				}
				else if(policy== 1)
				{
					g_policy= SCHED_FIFO;
					blog(LOG_INF, "Schedule policy be set to SCHED_FIFO.");
				}
				else if(policy== 2)
				{
					g_policy= SCHED_RR;
					blog(LOG_INF, "Schedule policy be set to SCHED_RR.");				
				}
				else{
					blog(LOG_ERR, "Schedule policy %ld not supported.", policy);
					goto ERR;
				}
				break;
			}
			case 'n':
			{
				long pri;
				pri= atol(optarg);
				if((pri> 0)&& (pri< 140)){
					g_priority= pri;
					blog(LOG_INF, "Schedule priority be set to %ld.", pri);
				}
				else{
					blog(LOG_ERR, "Schedule priority %ld not supported.", pri);
					goto ERR;
				}
				break;
			}
			case ':':
			{
				blog(LOG_ERR, "option -'%c' need a value.", opt);
				goto ERR;
				break;
			}
			case '?':
			{
				blog(LOG_ERR, "unknown option: %c.", optopt);
				goto ERR;
				break;
			}
		}
	}	

	return 0;

ERR:
	return -1;
}

int main(int argc, char* argv[])
{
	int cpu_num;
	int cpu_id[MAX_CPU]= {0};
	struct sched_param param;
	int i;

	if(argc> 1){
		if((0== strcmp("--help",argv[1]))| (0== strcmp("-h", argv[1]))){
			console_help(argv);
			return 0;
		}
	}

	if(0!= prase_param(argc, argv))
		goto ERR;

	/** Query max/min policy. */
	blog(LOG_DBG, "Show priority of SCHED_RR.");
	show_thread_priority(SCHED_RR);
	blog(LOG_DBG, "Show priority of SCHED_FIFO.");
	show_thread_priority(SCHED_FIFO);

	/** Query policy. */
	pthread_attr_t pid_attr;
	pthread_attr_init(&pid_attr);

    /** Get thread policy and priority. */
	get_thread_policy(&pid_attr);
	get_thread_priority(&pid_attr);

    /** Set thread policy and priority. */
	set_thread_policy(&pid_attr, g_policy);
	set_thread_priority(&pid_attr, g_priority);

    /** Get thread policy and priority again to 
      check if we have been set successfully. */
	get_thread_policy(&pid_attr);
	get_thread_priority(&pid_attr);

	/** Set process priority. */
	set_cur_process_priority(&pid_attr, g_policy, g_priority);
	
	/** Get CPU number. */
	cpu_num= sysconf(_SC_NPROCESSORS_ONLN);
	blog(LOG_INF, "system have %d CPU core.", cpu_num);
	if(cpu_num> MAX_CPU){
		blog(LOG_INF, "Number of cpu -- %d is too many for this test programs.\n");
		return -1;
	}

	/* Show proc status.*/
	char cmd_buf[50];
	sprintf(cmd_buf, "cat /proc/%d/stat", getpid());
	printf("Proc status:\n");
	system(cmd_buf);
	printf("\n");

	pthread_t load_pid[MAX_CPU];
	memset(load_pid, 0, sizeof(load_pid));

	/** Create cpu threads. */
	for(i= 0; i< cpu_num; i++){
		
		/* Attach special cpu id */
		if((g_attach_cpu_id!= -1) && (g_attach_cpu_id!= i))
			continue;

	    cpu_id[i]= i;
		if(pthread_create(&load_pid[i], &pid_attr, (void*)process_load, &cpu_id[i])){
			blog(LOG_ERR, "thread create failed.\n");
			goto T_CREATE_ERR;
		}
	}

	//console_loop();

	/** join thread. */
	for(i= 0; i< cpu_num; i++){

		/* Only join the attach cpu id thread. */
		if((g_attach_cpu_id!= -1) && (g_attach_cpu_id!= i))
			continue;

		pthread_join(load_pid[i], NULL);
	}

	blog(LOG_INF, "program finished.");

	return 0;

T_CREATE_ERR:
	for(i; i> 0; i--){
		pthread_join(load_pid[i], NULL);
	}
ERR:
	return -1;
}
