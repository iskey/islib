/*stack.c*/
#include <stdlib.h>

#include "list.h"
#include "stack.h"

/*stack push */
int stack_push(Stack *stack, const void *data)
{
	/*Push the data onto the top of the stack*/
	return list_ins_next(stack, NULL, data);
}

/*stack_pop*/
int stack_pop(Stack *stack, void **data)
{
	/*Pop the data off the stack*/
	return list_rem_next(stack, NULL, data);
}

#if 1
int main()
{
	Stack stack;
	void *data;

	stack_push(&stack, "hello world.");
	stack_push(&stack, "hello xiaoxie.");
	stack_push(&stack, "hello iskey.");

    int i;
    for(i=0; i<stack_size(&stack); i++){
		stack_pop(&stack, &data);
		printf("stack frame %d is [%s]\n", i, data);
    }
    stack_pop(&stack, &data);
    printf("stack frame %d is [%s]\n", i, data);
}
#endif
