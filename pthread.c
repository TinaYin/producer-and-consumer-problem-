# producer-and-consumer-problem-

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/types.h>
#define MAX 40     //maxium numbers to producer

//define the global variable
int buffer = 3;
pthread_mutex_t mutex;
pthread_cond_t condc, condp;


/*linked list is referenced from web: http://www.geeksforgeeks.org/doubly-linked-list/ */

//create a struct of Node
struct Node{
   int data;
   struct Node *next; //pointer to next node 
   struct Node *prev; //pointer to previous node
};

//create a global pointer to head
struct Node* head = NULL;

/*insect a node in front of the linked list */
void add(struct Node** head_ref, int new_data){
	
	//allocate node
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
        
	struct Node *last = *head_ref;


        //put data for new_node
        new_node->data = new_data;

	//set the next of new_node equal to null since it goes to the end of list
	new_node->next = NULL;

	//if the linked list is empty, then make the new list as head
	if(*head_ref==NULL){
		new_node->prev = NULL;
		*head_ref = new_node;
		return;
	}
	
	//else traverse the list till reach the last node	
	while(last->next!=NULL){
		last = last->next;
	}
	
	//change the next of last node
	last->next = new_node;

	//set last node to previous of new node
	new_node->prev = last;
	
}

//delete the node from the head of list
void delete(struct Node **head_ref, struct Node *del){
	
	//base case
	if(*head_ref == NULL || del == NULL){
		return;
	}
	
	//check if node to be deleted is head node
	if(*head_ref == del){
		*head_ref = del->next;  //delete the head node
	}

	free(del);
	return;

}

//tranverse the list and print the items in the doubly linked list
void printList(struct Node *node){
	struct Node *last;

	//iterater the linked list, and print each items
	while(node != NULL){
		printf(" %d ", node->data);
		last = node;
		node = node->next;
	}
	printf("\n");
}

//iterate the list and return the size of the buffer 
int getBufferSize(struct Node *node){
	int size = 0;
	struct Node *last;
	while(node != NULL){
		last = node;
		node = node->next;
		size = size + 1;
	}
	return size;
}

//producer1
void *producer1(void *ptr){   //produce data
	int r;
	
	while(1){
		pthread_mutex_lock(&mutex);  //give excluesive access to the buffer
		if(buffer>=MAX){ //if buffer is full, print overflow
			printf("---producer1---\n");
			printf("  Overflow\n");
			printf("\n");
			pthread_cond_wait(&condp, &mutex);
		}
	
		else if(buffer<MAX){  //else if the buffer is not full
			r = rand()%40;
			if((r%2)!=0){ //check the random value is odd 
				printf("---producer1---\n");
				printf("  before: ");
				printList(head);    //print the buffer before producer1 produce a value
			    	add(&head, r);
			    	buffer = buffer + 1;
			    	printf("  produce: %d \n", r); //print the value produced
				printf("  after: ");
			    	printList(head);  //produce the buffer after producer1 produce a value
				printf("\n");		 
			}
		}
		pthread_cond_signal(&condc); //wake up consumer
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	
	pthread_exit(0);	
}

//producer2
void *producer2(void *ptr){
	int r;
	while(1){
		pthread_mutex_lock(&mutex); //give exclusive access to the buffer 
		if(buffer>=MAX){ //if the buffer is full
			printf("---producer2---\n");
			printf("  Overflow\n");
			printf("\n");
			pthread_cond_wait(&condp, &mutex);
		}
		if(buffer<MAX){ //if the buffer is not full
			r = rand()%40;  //generate a random value
			if(r%2==0){ //if the value is even
				printf("---producer2---\n");
				printf("  before: ");
				printList(head);  //print the buffer before producer2 produce a value
				add(&head, r);
				buffer = buffer + 1;
				printf("  produce: %d \n", r);
				printf("  after: ");  //print the buffer after producer2 produce a value
				printList(head);
				printf("\n");
			}
		}
		pthread_cond_signal(&condc);  //wake up consumer
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}
	pthread_exit(0);
}

//consumer1
void *consumer1(void *ptr){
	while(1){
		pthread_mutex_lock(&mutex);  //give excluesive access to the buffer 
		if(buffer<=0){ //if the buffer is empty, print underflow
			printf("---consumer1---\n");
			printf("  Underflow\n");  
			printf("\n");
			pthread_cond_wait(&condc, &mutex);
		}
		if(buffer>0){  //else if the buffer is not empty
			int n = head->data; //get the value of head node
			if(n%2!=0){ //if the value is odd, delete the head node
				printf("---consumer1---\n");
				printf("  before: ");
				printList(head); //print the beffer before consumer1 consumed 
				delete(&head, head);  //delete the head node of the buffer 
				buffer = buffer -1;
				printf("  after: ");
				printList(head);   //print the buffer after consumer1 consumed 
				printf("\n");
			}else{  //else if the value is even, wait
				printf("---consumer1---\n");
				printf("  even value, has to wait... \n");
				printf("\n");
			}
		}
			pthread_cond_signal(&condp); //wake up the producer
			pthread_mutex_unlock(&mutex); //release access to buffer
			sleep(1);	
	}
	pthread_exit(0);
}

//consumer2
void *consumer2(void *ptr){
	while(1){
		pthread_mutex_lock(&mutex);  //give excluesive access to the buffer 
		if(buffer<=0){  //check if the buffer is empty
			printf("---consumer2---\n");
			printf("  Underflow\n");
			printf("\n");
			pthread_cond_wait(&condc, &mutex);
		}
		if(buffer>0){ //if the buffer is not empty
			printf("---consumer2---\n");
			int n = head->data;
			if(n%2==0){ //check if the value is even
				printf("  before: ");
				printList(head);  //print the buffer before consumer2 consumed
				delete(&head, head); //delete the head node of the buffer
				buffer = buffer -1;
				printf("  after: ");
				printList(head); //print the buffer after consumer2 consumed 
				printf("\n");
			}else{
				printf("  odd value, has to wait... \n");
				printf("\n");
			}
		}

			pthread_cond_signal(&condp); //wake up the producer
			pthread_mutex_unlock(&mutex);
			sleep(1);		
	}
	pthread_exit(0);
}

int main(){
	
    	/*initialize the doubly linked list with three notes */
        add(&head, 9);
        add(&head, 10);
	    add(&head,11);
    
	//print the initialized list
	printf("--initialized linked list-- \n");
	printList(head);
	printf("\n");

	pthread_t pro, con;
	pthread_mutex_init(&mutex, 0);  
	pthread_cond_init(&condc, 0);
	pthread_cond_init(&condp, 0);
	
        /*create four threads */
	pthread_create(&pro, 0, producer1, 0);
	pthread_create(&con, 0, consumer1, 0);
	pthread_create(&pro, 0, producer2, 0);
	pthread_create(&con, 0, consumer2, 0);

	pthread_join(pro, 0);
	pthread_join(con, 0);
	pthread_cond_destroy(&condc);
	pthread_cond_destroy(&condp);
	pthread_mutex_destroy(&mutex);
	return 0;

}
