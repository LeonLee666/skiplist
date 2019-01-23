//
// Created by bombehub on 1/23/2019.
//   https://gist.github.com/zhpengg/2873424
/* Skip Lists: A Probabilistic Alternative to Balanced Trees */

#include <sys/times.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <pthread.h>
#include <time.h>

#define SKIPLIST_MAX_LEVEL 6

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

int *list_size_array;

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
    list_size_array = (int *)malloc(sizeof(int) * (SKIPLIST_MAX_LEVEL+1));
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
    /*while (rand() < RAND_MAX/2 && level < SKIPLIST_MAX_LEVEL)
        level++;*/
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


int main()
{
    int arr[] = {4000009,5000008}, i;
    skiplist list;
    skiplist_init(&list);


    printf("Inserting:--------------------\n");

    for(int i=5000000;i>=20;i-=2)
    {
        skiplist_insert(&list, i, i);
    }

    clock_gettime(CLOCK_REALTIME,&tpBegin1);
    for (i = 0; i < sizeof(arr)/sizeof(arr[0]); i++) {
        printf("inserting %d serially\n",arr[i] );
        skiplist_insert(&list, arr[i], arr[i]);
    }
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