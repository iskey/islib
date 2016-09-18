#include "map.h"
#include "stdio.h"

int main()
{
	map_int_t m;
	map_init(&m);

	map_set(&m, "testkey", 123);

	int *val = map_get(&m, "testkey");

	printf("value is %d.\n", *val);
}
