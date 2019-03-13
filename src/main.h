#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DLM 3
#define ASC 48

#define TR "tr"
#define ACC "acc"
#define MAX "max"
#define RUN "run"
#define BLK '_'

struct tr_s {
    int nxt;
    char out, mov;
};

typedef struct tr_s tr_t;

struct tr_list_s {
    tr_t tr;

    struct tr_list_s * next;
};

typedef struct tr_list_s tr_list_t;

struct ht_s {
    int acc;
    
    tr_list_t ** tr_list;
    size_t dim;
};

typedef struct ht_s ht_t;

struct qel_s {
    long step;

    int st_pos;
    int tp_pos;
    
    char * tape_pos;
    size_t dim_pos;

    char * tape_neg;
    size_t dim_neg;

    int * users;

    struct qel_s * next;
};

typedef struct qel_s qel_t;

struct queue_s {
    qel_t * head;
    qel_t * tail;
};

typedef struct queue_s queue_t;

void init_hash_table(ht_t **, size_t *, size_t *);
void ht_list_insert(tr_list_t **, char *, char *, int *);

void update_prim_ht(ht_t **, size_t *, size_t *, int *);
void update_sec_ht(ht_t *, size_t *, int *);

void init_queue(queue_t *, queue_t *, char *);
void free_tapes(queue_t *, queue_t *);
