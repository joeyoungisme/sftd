#ifndef UTIL_H
#define UTIL_H

#include <pthread.h>
#include "sft.h"
#include "pdu.h"

#define MAX_CMD_LEN             256
#define MAX_CMD_ARG_LEN         256
#define MAX_PATH_LEN            256

typedef struct __UTILITY_DATA {
    struct __SFT_PDU *pdu;
    struct __SFT_DATA *sft;

    struct __RECV_THREAD *recv;
    struct __UTILITY_ACTION *action;
}UTILITY;

typedef enum {
    THREAD_DEAD = 0,
    THREAD_ALIVE
}THREAD_FLAG;

struct __RECV_THREAD {
    THREAD_FLAG flag;
    pthread_t thd;

    void *(*run)(void *arg);
    void *(*ls)(void *arg);
    void *(*get)(void *arg);
    void *(*put)(void *arg);
    void *(*close)(void *arg);
};

struct __UTILITY_ACTION {
    int (*init)(struct __UTILITY_DATA *util);
    int (*listen)(struct __UTILITY_DATA *util);
    int (*connection)(struct __UTILITY_DATA *util);
    int (*ls)(struct __UTILITY_DATA *util);
    int (*get)(struct __UTILITY_DATA *util);
    int (*put)(struct __UTILITY_DATA *util);
    int (*close)(struct __UTILITY_DATA *util);
    int (*quit)(struct __UTILITY_DATA *util);
    int (*info)(struct __UTILITY_DATA *util);
    int (*help)(void);
};

void *thread_recv(void *arg);
void *thread_ls(void *arg);
void *thread_get(void *arg);
void *thread_put(void *arg);
void *thread_close(void *arg);

int util_connection(struct __UTILITY_DATA *util);
int util_listen(struct __UTILITY_DATA *util);
int util_close(struct __UTILITY_DATA *util);
int util_put(struct __UTILITY_DATA *util);
int util_get(struct __UTILITY_DATA *util);
int util_quit(struct __UTILITY_DATA *util);
int util_ls(struct __UTILITY_DATA *util);
int util_info(struct __UTILITY_DATA *util);
int util_help(void);

struct __UTILITY_DATA *util_create(void);

int util_run(struct __UTILITY_DATA *util);

void util_destroy(struct __UTILITY_DATA *util);

//COMMAND command_get(void);

//if need arg return 1, if not need arg return 0
//int command_need_arg(COMMAND cmd);

//int command_get_arg(char *arg, unsigned int length);

#endif
