#include <stdio.h>
#include <dlfcn.h>

typedef void (*foo_print_func)(char *a);
typedef void (*bar_print_func)(char *a, char *b);

int main()
{
	void *foo_handle;
	void *bar_handle;
	foo_print_func foo_print;
	bar_print_func bar_print;

	foo_handle = dlopen("/home/iskey/work/github/islib/build/libs/libfoo_so.so", RTLD_LAZY);
	if (! foo_handle) {
		printf("%s,%d, NULL == foo_handle\n", __FUNCTION__, __LINE__);
		return -1;
	 }

	bar_handle = dlopen("/home/iskey/work/github/islib/build/libs/libbar_so.so", RTLD_LAZY);
	if (! bar_handle) {
		printf("%s,%d, NULL == bar_handle\n", __FUNCTION__, __LINE__);
		return -1;
	 }

	foo_print = dlsym(foo_handle, "my_print");
	if (!foo_print) {
		printf("%s,%d, NULL == foo_print\n", __FUNCTION__, __LINE__);
		return -1;
	}

	bar_print = dlsym(bar_handle, "my_print");
	if (!bar_print) {
		printf("%s,%d, NULL == bar_print\n", __FUNCTION__, __LINE__);
		return -1;
	}

	foo_print("Hello.");
	bar_print("Hello", "World.");

	dlclose(foo_handle);
	dlclose(bar_handle);
}
