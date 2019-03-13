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

int main (int argc, char ** argv)
{
int i, j;

char * line = NULL;
size_t line_len;

int res;
long max;
char inp;
tr_t parse;

ht_t * hash_table = NULL;
size_t prim_mem, sec_mem;

queue_t queue;
queue_t inactive;

qel_t * tmp = NULL;
tr_list_t * p = NULL;

    line_len = 0;

    queue.head = NULL;
    queue.tail = NULL;

    inactive.head = NULL;
    inactive.tail = NULL;

    getline(&line, &line_len, stdin);

    if(strncmp(line, TR, DLM - 1) == 0) {
        getline(&line, &line_len, stdin);

        if(strncmp(line, ACC, DLM) != 0) {
            sscanf(line, "%d %c %c %c %d", &i, &inp, &parse.out, &parse.mov, &parse.nxt); 
      
            j = (int)inp - ASC;

            prim_mem = i + 1;
            sec_mem = j + 1;

            init_hash_table(&hash_table, &prim_mem, &sec_mem);

            ht_list_insert(&hash_table[i].tr_list[j], &parse.out, &parse.mov, &parse.nxt);

            getline(&line, &line_len, stdin);

            while(strncmp(line, ACC, DLM) != 0) {
                sscanf(line, "%d %c %c %c %d", &i, &inp, &parse.out, &parse.mov, &parse.nxt); 
                
                j = (int)inp - ASC;

                if(i + 1 > prim_mem)
                    update_prim_ht(&hash_table, &prim_mem, &sec_mem, &i);

                if(parse.nxt + 1 > prim_mem)
                    update_prim_ht(&hash_table, &prim_mem, &sec_mem, &parse.nxt);

                if(j + 1 > hash_table[i].dim)
                    update_sec_ht(&hash_table[i], &sec_mem, &j);                

                ht_list_insert(&hash_table[i].tr_list[j], &parse.out, &parse.mov, &parse.nxt);

                getline(&line, &line_len, stdin);
            }

            getline(&line, &line_len, stdin);

            while(strncmp(line, MAX, DLM) != 0) {
                i = atoi(line);

                if(i + 1 > prim_mem)
                    update_prim_ht(&hash_table, &prim_mem, &sec_mem, &i);

                hash_table[i].acc = 1;

                getline(&line, &line_len, stdin);
            }

            getline(&line, &line_len, stdin);

            if(strncmp(line, RUN, DLM) != 0) {
                max = atol(line);

                getline(&line, &line_len, stdin);

                if(strncmp(line, RUN, DLM) == 0) {
                    while(getline(&line, &line_len, stdin) > 0) {
                        res = 0;                        
                        init_queue(&queue, &inactive, line);

                        while(queue.head != NULL) {
                            i = queue.head->st_pos;

                            if(queue.head->tp_pos >= 0) {
                                inp = queue.head->tape_pos[queue.head->tp_pos];
                                j = (int)inp - ASC;
                            } else {
                                inp = queue.head->tape_neg[-queue.head->tp_pos];
                                j = (int)inp - ASC;
                            }

                            if(queue.head->step > max) {
                                res = -1;

                                free_tapes(&queue, &inactive);
                            } else if(hash_table[i].acc == 1) {
                                res = 1;

                                free_tapes(&queue, &inactive);
                            } else {
                                if(j + 1 <= hash_table[i].dim) {
                                    for(p = hash_table[i].tr_list[j]; p != NULL; p = p->next) {
                                        if(inp == p->tr.out && p->tr.mov == 'S' && i == p->tr.nxt) {
                                            res = -1;
                                        } else {
                                            if(inactive.head != NULL) {
                                                tmp = inactive.head;
                                                inactive.head = inactive.head->next;

                                                if(inactive.head == NULL)
                                                    inactive.tail = NULL;
                                            } else {
                                                if((tmp = (qel_t *)malloc(sizeof(qel_t)))) {
                                                    //
                                                }
                                            }

                                            tmp->step = queue.head->step + 1;

                                            tmp->st_pos = p->tr.nxt;
                                            tmp->tp_pos = queue.head->tp_pos;

                                            tmp->dim_pos = queue.head->dim_pos;
                                            tmp->dim_neg = queue.head->dim_neg;

                                            tmp->users = queue.head->users;

                                            tmp->next = NULL;

                                            if(queue.head->tp_pos >= 0) {
                                                if(p->tr.mov == 'R' && tmp->tp_pos + 1 >= queue.head->dim_pos) {
                                                    if(inp == BLK && i == p->tr.nxt && hash_table[i].tr_list[j]->next == NULL) {
                                                        res = -2;
                                                    } else {
                                                        tmp->dim_pos *= 2;

                                                        if((tmp->tape_pos = (char *)malloc(sizeof(char) * tmp->dim_pos))) {
                                                            memcpy(tmp->tape_pos, queue.head->tape_pos, queue.head->dim_pos);

                                                            for(i = queue.head->dim_pos; i < tmp->dim_pos; i++)
                                                                tmp->tape_pos[i] = BLK;
                                                
                                                            if((tmp->tape_neg = (char *)malloc(sizeof(char) * tmp->dim_neg))) {
                                                                memcpy(tmp->tape_neg, queue.head->tape_neg, queue.head->dim_neg);

                                                                if((tmp->users = (int *)malloc(sizeof(int)))) {
                                                                    *tmp->users = 1;

                                                                    tmp->tape_pos[queue.head->tp_pos] = p->tr.out;
                                                                }     
                                                            }
                                                        }
                                                    }
                                                } else if(p->tr.out != queue.head->tape_pos[queue.head->tp_pos]) {
                                                    if((tmp->tape_pos = (char *)malloc(sizeof(char) * tmp->dim_pos))) {
                                                        memcpy(tmp->tape_pos, queue.head->tape_pos, queue.head->dim_pos);

                                                        if((tmp->tape_neg = (char *)malloc(sizeof(char) * tmp->dim_neg))) {
                                                            memcpy(tmp->tape_neg, queue.head->tape_neg, queue.head->dim_neg);

                                                            if((tmp->users = (int *)malloc(sizeof(int)))) {
                                                                *tmp->users = 1;

                                                                tmp->tape_pos[queue.head->tp_pos] = p->tr.out;
                                                            }
                                                        }
                                                    }
                                                } else {
                                                    tmp->tape_pos = queue.head->tape_pos;
                                                    tmp->tape_neg = queue.head->tape_neg;

                                                    *tmp->users += 1;
                                                }
                                            } else {
                                                if(p->tr.mov == 'L' && -tmp->tp_pos + 1 >= queue.head->dim_neg) {
                                                    if(inp == BLK && i == p->tr.nxt && hash_table[i].tr_list[j]->next == NULL) {
                                                        res = -2;
                                                    } else {
                                                        tmp->dim_neg *= 2;

                                                        if((tmp->tape_pos = (char *)malloc(sizeof(char) * tmp->dim_pos))) {
                                                            memcpy(tmp->tape_pos, queue.head->tape_pos, queue.head->dim_pos);
                                                
                                                            if((tmp->tape_neg = (char *)malloc(sizeof(char) * tmp->dim_neg))) {
                                                                memcpy(tmp->tape_neg, queue.head->tape_neg, queue.head->dim_neg);

                                                                for(i = queue.head->dim_neg; i < tmp->dim_neg; i++)
                                                                    tmp->tape_neg[i] = BLK;

                                                                if((tmp->users = (int *)malloc(sizeof(int)))) {
                                                                    *tmp->users = 1;

                                                                    tmp->tape_neg[-queue.head->tp_pos] = p->tr.out;
                                                                }
                                                            }
                                                        }
                                                    }
                                                } else if(p->tr.out != queue.head->tape_neg[-queue.head->tp_pos]) {
                                                    if((tmp->tape_pos = (char *)malloc(sizeof(char) * tmp->dim_pos))) {
                                                        memcpy(tmp->tape_pos, queue.head->tape_pos, queue.head->dim_pos);

                                                        if((tmp->tape_neg = (char *)malloc(sizeof(char) * tmp->dim_neg))) {
                                                            memcpy(tmp->tape_neg, queue.head->tape_neg, queue.head->dim_neg);

                                                            if((tmp->users = (int *)malloc(sizeof(int)))) {
                                                                *tmp->users = 1;

                                                                tmp->tape_neg[-queue.head->tp_pos] = p->tr.out;
                                                            }
                                                        }
                                                    }
                                                } else {
                                                    tmp->tape_pos = queue.head->tape_pos;
                                                    tmp->tape_neg = queue.head->tape_neg;

                                                    *tmp->users += 1;
                                                }
                                            }
                                            
                                            if(res != -2) {
                                                if(p->tr.mov == 'R')
                                                    tmp->tp_pos++;
                                                else if(p->tr.mov == 'L')
                                                    tmp->tp_pos--;

                                                queue.tail->next = tmp;
                                                queue.tail = tmp;
                                            } else {
                                                if(inactive.head == NULL) {
                                                    inactive.head = tmp;
                                                    inactive.head->next = NULL;

                                                    inactive.tail = inactive.head;
                                                } else {
                                                    inactive.tail->next = tmp;
                                                    inactive.tail = tmp;

                                                    inactive.tail->next = NULL;
                                                }

                                                res = -1;
                                            }
                                        }
                                    }
                                }

                                tmp = queue.head; 
                                queue.head = queue.head->next;

                                if(*tmp->users == 1) {
                                    free(tmp->tape_pos);
                                    free(tmp->tape_neg);

                                    free(tmp->users);
                                } else {
                                    *tmp->users -= 1;
                                }

                                if(inactive.head == NULL) {
                                    inactive.head = tmp;
                                    inactive.head->next = NULL;

                                    inactive.tail = inactive.head;
                                } else {
                                    inactive.tail->next = tmp;
                                    inactive.tail = tmp;

                                    inactive.tail->next = NULL;
                                }

                                if(queue.head == NULL)
                                    queue.tail = NULL;
                            }
                        }

                        if(queue.head != NULL)
                            free_tapes(&queue, &inactive);

                        if(res == -1)
                            printf("U\n");
                        else
                            printf("%d\n", res);
                    }
                }
            }
        }
    }

    free(line);

    return 0;
}

void init_hash_table(ht_t ** hash_table, size_t * prim_mem, size_t * sec_mem)
{
int i, j;

    if((*hash_table = (ht_t *)malloc(sizeof(ht_t) * *prim_mem)))
        for(i = 0; i < *prim_mem; i++) {
            (*hash_table)[i].acc = 0;
            (*hash_table)[i].dim = *sec_mem;

            if(((*hash_table)[i].tr_list = (tr_list_t **)malloc(sizeof(tr_list_t *) * *sec_mem)))
                for(j = 0; j < *sec_mem; j++)
                    (*hash_table)[i].tr_list[j] = NULL;
        }

    return ;
}

void ht_list_insert(tr_list_t ** head, char * out, char * mov, int * nxt)
{
tr_list_t * tmp;

    if((tmp = (tr_list_t *)malloc(sizeof(tr_list_t)))) {
        tmp->tr.out = *out;
        tmp->tr.mov = *mov;
        tmp->tr.nxt = *nxt;

        tmp->next = *head;

        *head = tmp;
    }

    return ;
}

void update_prim_ht(ht_t ** hash_table, size_t * prim_mem, size_t * sec_mem, int * new_mem)
{
int i, j;

    if((*hash_table = (ht_t *)realloc(*hash_table, sizeof(ht_t) * (*new_mem * 2)))) {
        for(i = *prim_mem; i < *new_mem * 2; i++) {
            (*hash_table)[i].acc = 0;
            (*hash_table)[i].dim = *sec_mem;

            if(((*hash_table)[i].tr_list = (tr_list_t **)malloc(sizeof(tr_list_t *) * *sec_mem)))
                for(j = 0; j < *sec_mem; j++)
                    (*hash_table)[i].tr_list[j] = NULL;
        }

        *prim_mem = *new_mem * 2;
    }

    return ;
}

void update_sec_ht(ht_t * hash_table_i, size_t * sec_mem, int * new_mem)
{
int j;

    if(*new_mem + 1 > *sec_mem)
        *sec_mem = *new_mem + 1;

    if((hash_table_i->tr_list = (tr_list_t **)realloc(hash_table_i->tr_list, 
        sizeof(hash_table_i->tr_list) * *sec_mem))) {
        for(j = hash_table_i->dim; j < *sec_mem; j++)
            hash_table_i->tr_list[j] = NULL;

        hash_table_i->dim = *sec_mem;
    }

    return ;
}

void init_queue(queue_t * queue, queue_t * inactive, char * line) 
{
int i;

    if(inactive->head != NULL) {
        queue->head = inactive->head;
        inactive->head = inactive->head->next;

        if(inactive->head == NULL)
            inactive->tail = NULL;
    } else {
        if((queue->head = (qel_t *)malloc(sizeof(qel_t)))) {
            //
        }
    }

    queue->head->step = 1;

    queue->head->st_pos = 0;
    queue->head->tp_pos = 0;

    queue->head->dim_pos = 0;

    for(i = 0; line[i] != '\n' && line[i] != '\r' && line[i] != '\0'; i++)
        queue->head->dim_pos++;

    if((queue->head->tape_pos = (char *)malloc(sizeof(char) * queue->head->dim_pos))) {
        memcpy(queue->head->tape_pos, line, queue->head->dim_pos);

        queue->head->dim_neg = 2;

        if((queue->head->tape_neg = (char *)malloc(sizeof(char) * queue->head->dim_neg))) {
            queue->head->tape_neg[1] = BLK;

            if((queue->head->users = (int *)malloc(sizeof(int)))) {
                *queue->head->users = 1;

                queue->head->next = NULL;

                queue->tail = queue->head;
            }
        }
    }  

    return ;
}

void free_tapes(queue_t * queue, queue_t * inactive)
{
qel_t * curr;

    if(inactive->head == NULL) {
        inactive->head = queue->head;
        inactive->tail = queue->tail;
    } else {
        inactive->tail->next = queue->head;
        inactive->tail = queue->tail;
    }

    curr = queue->head;
    
    while(queue->head) {
        queue->head = queue->head->next;

        if(*curr->users == 1) {
            free(curr->tape_pos);
            free(curr->tape_neg);

            free(curr->users);
        } else {
            *curr->users -= 1;
        }

        curr = queue->head;
    }

    queue->tail = NULL;

    return ;
}
