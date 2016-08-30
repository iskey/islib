/*list.c*/

#include <stdlib.h>
#include <string.h>

#include <list.h>

/*list_init*/
void list_init(List *list, void(*destroy)(void *data)){
	/* Initialize the list */

	list->size = 0;
	list->destroy = destroy;
	list->head = NULL;
	list->tail = NULL;

	return;
}

/*list destroy */
void list_destroy(List *list){
	void *data;

	/*Remove each element*/
	while(list_size(list)> 0){
		if(list_rem_next(list, NULL, (void **)&data)== 0 && list->destroy!= NULL){
		/*Call a user-defined function to free dynamiclly allocated data*/
			list->destroy(data);
		}
	}

	/*No operations are allowed now,but clear the structure.*/
	memset(list, 0, sizeof(List));
	return;
}

/*list_ins_next*/
int list_ins_next(List *list, ListElmt *element, const void *data){
	ListElmt *new_element;

	/*Allocate storage for the new element*/
	if((new_element= (ListElmt*)malloc(sizeof(ListElmt)))== NULL){
		return -1;
	}

	/*Insert the element into the list.*/
	new_element->data= (void *)data;
	if(element == NULL){
		/*Handle inseration at the end of list*/
		if(list_size(list)== 0){
			list->tail = new_element;
		}
		new_element->next = list->head;
		list->head = new_element;
	}
	else{
		/*Handle insertion somewhere other than the head*/
		if(element->next == NULL){
			list->tail = new_element;
		}

		new_element->next = element->next;
		element->next = new_element;
	}

	/*Adjust the size ot the list */
	list->size++;

	return 0;
}

/*list_rem_next */
int list_rem_next(List *list, ListElmt *element, void **data)
{
	ListElmt *old_element;

	if(element== NULL){
		/*Handle removal from the head of the list*/
		*data = list->head->data;
		old_element = list->head;
		list->head= list->head->next;

		if(list_size(list)== 1){
			list->tail = NULL;
		}
	}
	else{
		/*Handle removal from somewhere other than the head */
		if(element->next== NULL){
			return -1;
		}

		*data = element->next->data;
		old_element = element->next;
		element->next = element->next->next;

		if(element->next == NULL){
			list->tail = element;
		}
	}

	/*Free the storage allocated by the abstract data type*/
	free(old_element);

	/*Adjust the size of the list for the removed element.*/
	list->size--;
	return 0;
}

void ele_free(void *data)
{
	free(data);
}
#if 0
int main()
{
	List list;

	list_init(&list, ele_free);

	list_ins_next(&list, NULL, "hello world.");
	list_ins_next(&list, NULL, "hello xiaoxie.");
	list_ins_next(&list, NULL, "you are very good.");

	int i;
	ListElmt *ele = list.head;
 	for(;ele->next!= NULL; ele=ele->next){
        printf("list data is %s.\n", (char *)(list_data(ele)));
	}
	printf("list data is %s.\n", (char *)(list_data(ele)));

	return 0;
}

#endif
