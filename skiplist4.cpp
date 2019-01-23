//
// Created by bombehub on 1/23/2019.
//  https://gist.github.com/MJjainam/96da275282b874de0c6d8ce05405a41c

//#define _POSIX_C_SOURCE >= 199309L

#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

#define SKIPLIST_MAX_LEVEL 3
#define NUM_OF_THREADS 2

typedef struct snode {
    int key;
    int value;
    struct snode **forward;

} snode;

typedef struct skiplist {
    int level;
    int size;
    struct snode *header;
} skiplist;


int threads_remaining= NUM_OF_THREADS;
int *list_size_array;

typedef struct arg_struct {
    //Or whatever information that you need
    skiplist *list;
    int key;
    int value;
    int thread_num;
} arg_struct;
pthread_t threads[NUM_OF_THREADS];
//int arr[] = {50000100,43,37,5};
//snode *header = (snode *)malloc(sizeof(struct snode));


int check_key = -1;
int arr[] = {4000005,5000110};
pthread_mutex_t mutex0 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

struct timespec tpBegin1,tpEnd1; //time data structure

double compute(struct timespec start,struct timespec end) //computes time in milliseconds given endTime and startTime timespec structures.
{
    double t;
    t=(end.tv_sec-start.tv_sec)*1000;
    t+=(end.tv_nsec-start.tv_nsec)*0.000001;

    return t;
}



skiplist *skiplist_init(skiplist *list)
{


    int i;
    list_size_array = (int *)malloc(sizeof(int) * (SKIPLIST_MAX_LEVEL+1));   //added

    snode *header = (snode *)malloc(sizeof(struct snode));
    list->header = header;
    header->key = INT_MAX;


    header->forward = (snode **)malloc(sizeof(snode*) * (SKIPLIST_MAX_LEVEL+1));
    for (i = 0; i <= SKIPLIST_MAX_LEVEL; i++) {
        header->forward[i] = list->header;

    }

    list->level = 1;
    list->size = 0;

    return list;

}

static int compute_level() //
{
    int level = 1;
    if(list_size_array[3] < list_size_array[1]/9){
        level = 3;
    }
    else if(list_size_array[2] < list_size_array[1]/3){
        level = 2;
    }
    return level;
}

int skiplist_insert(skiplist *list, int key, int value)
{
    snode *update[SKIPLIST_MAX_LEVEL+1];
    snode *x = list->header;
    int i, level;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }
    x = x->forward[1];

    if (key == x->key) {
        x->value = value;
        return 0;
    } else {
        level = compute_level();

        if (level > list->level) {
            for (i = list->level+1; i <= level; i++) {
                update[i] = list->header;
            }
            list->level = level;
        }

        x = (snode *)malloc(sizeof(snode));
        x->key = key;
        x->value = value;
        x->forward = (snode **)malloc(sizeof(snode*) * (level + 1));
        for (i = 1; i <= level; i++) {
            x->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = x;
        }

        for(int i=1;i<=level;i++)				//
        {
            list_size_array[i]+=1;
        }
    }



    return 0;
}

snode *skiplist_search(skiplist *list, int key)
{
    snode *x = list->header;
    int i;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
    }
    if (x->forward[1]->key == key) {
        return x->forward[1];
    } else {
        return NULL;
    }
    return NULL;
}

static void skiplist_node_free(snode *x)
{
    if (x) {
        free(x->forward);
        free(x);
    }
}

int skiplist_delete(skiplist *list, int key)
{
    int i;
    snode *update[SKIPLIST_MAX_LEVEL + 1];
    snode *x = list->header;
    for (i = list->level; i >= 1; i--) {
        while (x->forward[i]->key < key)
            x = x->forward[i];
        update[i] = x;
    }

    x = x->forward[1];
    if (x->key == key) {
        for (i = 1; i <= list->level; i++) {
            if (update[i]->forward[i] != x)
                break;
            update[i]->forward[1] = x->forward[i];
            list_size_array[i]-=1;                      //
        }
        skiplist_node_free(x);

        while (list->level > 1 && list->header->forward[list->level] == list->header)
            list->level--;
        return 0;
    }
    return 1;
}

static void skiplist_dump(skiplist *list)
{
    snode *x = list->header;
    while (x && x->forward[1] != list->header) {
        printf("%d[%d]->", x->forward[1]->key, x->forward[1]->value);
        x = x->forward[1];
    }
    printf("NIL\n");
}
//1. create a thread .Check if it can execute that. If yes then continue or wait for the other threads to finish and then continue.



int join_all_threads()
{
    while(threads_remaining>0)
    {
        // printf("waiting\n");
        usleep(1);
    }
    pthread_join(threads[0],NULL);
    pthread_join(threads[1],NULL);

}

void *parallel_insert(void *arg)
//int parallel_insert(skiplist *list, int key, int value)
{
    arg_struct *arg1;
    arg1 = (arg_struct *)arg;
    skiplist *list = arg1->list;
    int key = arg1->key;
    int value = arg1->value;
    pthread_mutex_lock(&mutex1);


    // snode *y = min_of_check_array(3);


    if(check_key!=-1)       //logic should come here
    {

        while(check_key!=-1 && key >= check_key)
        {
            usleep(1);
        }

    }
    else
    {
        check_key=key;
    }

    threads_remaining-=1;
    pthread_mutex_unlock(&mutex1);


    snode *update[SKIPLIST_MAX_LEVEL+1];
    snode *x = list->header;
    int i, level;
    for (i = list->level; i >= 1; i--) {
        if(key>=check_key)
        {
            pthread_mutex_lock(&mutex0);
        }
        while (x->forward[i]->key < key)
            x = x->forward[i];
        if(i==list->level && (check_key>x->key||check_key==-1)) check_key = x->key;
        update[i] = x;

        pthread_mutex_unlock(&mutex0);
    }
    x = x->forward[1];

    if (key == x->key) {
        x->value = value;
        return 0;
    } else {
        level = compute_level();

        if (level > list->level) {
            for (i = list->level+1; i <= level; i++) {
                update[i] = list->header;
            }
            list->level = level;
        }

        x = (snode *)malloc(sizeof(snode));
        x->key = key;
        x->value = value;
        x->forward = (snode **)malloc(sizeof(snode*) * (level + 1));

        for (i = 1; i <= level; i++) {
            x->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = x;
        }

        for(int i=1;i<=level;i++)               //
        {
            list_size_array[i]+=1;
        }
    }
    check_key = -1;


}





int main()
{
    int i;
    skiplist list;

    skiplist_init(&list);

    printf("Insert:--------------------\n");
    for(int i=5000000;i>=20;i-=2)
    {
        skiplist_insert(&list, i, i);
    }


    clock_gettime(CLOCK_REALTIME,&tpBegin1);
    for (i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) {
        arg_struct *arg = (arg_struct *)malloc(sizeof (arg_struct));
        arg->key = arr[i];
        arg->value = arr[i];
        arg->thread_num = i;
        arg->list = &list;
        printf("executing %dth thread,inserting %d parallely\n",i,arr[i]);
        pthread_create(&threads[i],NULL,parallel_insert,(void *)arg);

    }

    join_all_threads();

    clock_gettime(CLOCK_REALTIME,&tpEnd1);
    double time1 = compute(tpBegin1,tpEnd1);
    printf("time taken %f ms\n",time1);

    //skiplist_dump(&list);

    printf("Search:--------------------\n");
    int keys[] = {4000005,5000110};

    for (i = 0; i < sizeof(keys)/sizeof(keys[0]); i++) {
        snode *x = skiplist_search(&list, keys[i]);
        if (x) {
            printf("key = %d, %s\n", keys[i], "present");
        } else {
            printf("key = %d, not fuound\n", keys[i]);
        }
    }



    return 0;
}