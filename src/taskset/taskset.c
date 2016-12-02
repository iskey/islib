#define _GNU_SOURCE

#include <sched.h>
#include <getopt.h>
#include <ctype.h>
#include "stdio.h"
#include "string.h"
#include <stdlib.h>

#define TASKSET_PRINTF_MASK "0x%x"

int main(int argc, char **argv)
{
	cpu_set_t mask;
	pid_t pid = 0;
	int i;
	int cpu_id;

	if ((argc < 3) || (!*argv)){
        printf("\nUsage: %s [pid] [cpu_id ...]\n\n", argv[0]);
        goto ERR;
	}
	
	pid = atoi(argv[1]);

	if (sched_getaffinity(pid, sizeof(mask), &mask) < 0){
		printf("can't %cet pid %d's affinity\n", 'g', pid);
		goto ERR;
	}
	printf("pid %d's affinity mask: "TASKSET_PRINTF_MASK"\n",
			pid, mask);

	CPU_ZERO(&mask);
	for (i= 1; i< argc; i++)
	{
	    cpu_id = atoi(argv[i]);
	    CPU_SET(cpu_id, &mask);
	}
	printf("pid %d's new affinity mask: "TASKSET_PRINTF_MASK"\n", pid, mask);

	/* Set pid's or our own (pid==0) affinity */
	if (sched_setaffinity(pid, sizeof(mask), &mask))
		printf("can't %cet pid %d's affinity\n", 's', pid);

ERR:
    return -1;
}
