#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "util.h"

void *thread_recv(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;

    util->recv->flag = THREAD_ALIVE;

    //select ...
    while(1) {
        pdu_init(util->pdu);

        util->sft->action->recv(util->sft, util->pdu, sizeof(SFT_PDU));

        switch(util->pdu->cmd) {
        case CMD_LS:
            util->recv->ls(arg);
            break;
        case CMD_GET:
            util->recv->get(arg);
            break;
        case CMD_PUT:
            util->recv->put(arg);
            break;
        case CMD_CLOSE:
            util->recv->close(arg);
            break;
        default:
            break;
        }

        sleep(1);
    }

    printf("thread recv close!!\n");
}

void *thread_ls(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;
    printf("\n ------------------------------ \n");
    printf("Receiver : Recv Command \"ls\" \n");
    printf("pdu arg : %s", util->pdu->arg);
    printf("\n ------------------------------ \n");

    return NULL;
}

void *thread_get(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;
    printf("\n ------------------------------ \n");
    printf("REceiver : Recv Command \"get\" \n");
    printf("pdu arg : %s", util->pdu->arg);
    printf("\n ------------------------------ \n");
    return NULL;
}

void *thread_put(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;
    printf("\n ------------------------------ \n");
    printf("REceiver : Recv Command \"put\" \n");
    printf("pdu arg : %s", util->pdu->arg);
    printf("\n ------------------------------ \n");
    return NULL;
}

void *thread_close(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;
    printf("\n ------------------------------ \n");
    printf("REceiver : Recv Command \"close\" \n");
    printf("pdu arg : %s", util->pdu->arg);
    printf("\n ------------------------------ \n");

    util->sft->action->send(util->sft, (void *)util->pdu, sizeof(SFT_PDU));

    sft_destroy(util->sft);
    util->sft = NULL;

    util->recv->flag = THREAD_DEAD;

    pthread_exit(NULL);
    return NULL;
}

int util_connection(struct __UTILITY_DATA *util)
{
    if(util->sft) {
        return -1;
    }

    //SFT Initial Setting... util->sft
    util->sft = sft_create(SFT_CLIENT);
    if(!util->sft)
        return -1;

    int res = 0;
    res = util->sft->action->init(util->sft);
    if(res) {
        sft_destroy(util->sft);
        return res;
    }

    res = util->sft->action->connect(util->sft, util->pdu->arg, SFT_PORT);
    if(res) {
        sft_destroy(util->sft);
        return res;
    }

    res = pthread_create(&util->recv->thd, NULL, util->recv->run, (void *)util);
    if(res != 0) {
        sft_destroy(util->sft);
        return res;
    }

    return 0;
}

int util_listen(struct __UTILITY_DATA *util)
{
    if(util->sft) {
        return -1;
    }

    util->sft = sft_create(SFT_SERVER);
    if(!util->sft) {
        return -1;
    }

    int res = util->sft->action->init(util->sft);
    if(res) {
        sft_destroy(util->sft);
        return res;
    }

    res = util->sft->action->connect(util->sft, NULL, SFT_PORT);
    if(res) {
        sft_destroy(util->sft);
        return res;
    }

    res = pthread_create(&util->recv->thd, NULL, util->recv->run, (void *)util);
    if(res != 0) {
        sft_destroy(util->sft);
        return res;
    }

    return 0;

}

int util_close(struct __UTILITY_DATA *util)
{
    if(!util->sft) {
        printf("Close Error : Please Listen / Connection First\n");
        return -1;
    }

    util->sft->action->send(util->sft, (void *)util->pdu, sizeof(SFT_PDU));

    pthread_join(util->recv->thd, NULL);

    return 0;
}

int util_put(struct __UTILITY_DATA *util)
{
    if(!util->sft) {
        printf("Put Error : Please Listen / Connection First\n");
        return -1;
    }

    util->sft->action->send(util->sft, (void *)util->pdu, sizeof(SFT_PDU));

    return 0;
}

int util_get(struct __UTILITY_DATA *util)
{
    if(!util->sft) {
        printf("Get Error : Please Listen / Connnection First\n");
        return -1;
    }

    util->sft->action->send(util->sft, (void *)util->pdu, sizeof(SFT_PDU));

    return 0;
}

int util_ls(struct __UTILITY_DATA *util)
{
    if(!util->sft) {
        printf("Ls Error : Please Listen / Connection First\n");
        return -1;
    }

    util->sft->action->send(util->sft, (void *)util->pdu, sizeof(SFT_PDU));

    return 0;
}

int util_quit(struct __UTILITY_DATA *util)
{

    if(util->sft) {
        util->pdu->cmd = CMD_CLOSE;
        util->action->close(util);
    }

    if(util->pdu) {
        pdu_free(util->pdu);
        util->pdu = NULL;
    }

    return 0;
}

int util_info(struct __UTILITY_DATA *util)
{
    printf("\n -------------------------------- \n");
    pdu_info(util->pdu);

    printf("THD : \n");

    if(util->recv->flag == THREAD_ALIVE)
        printf("\tThread Runing...\n");
    else
        printf("\tHave not create thread ... \n");

    if(util->sft)
        util->sft->action->info(util->sft);

    printf(" -------------------------------- \n");

    return 0;
}

int util_help(void)
{
    char help_message[] =
        "Following Command, Please Listen/Connection First\n"
        "listen             : Socket bind & accept\n"
        "connection [IP]    : Socket connect\n"
        "ls [Path]          : Move to destination and show all file or directory \n"
        "get [Path]         : Get remote side file\n"
        "put [Path]         : Put file to remote side\n"
        "close              : Close Coneection, Used listen / connection after close\n"
        "quit               : Quit this Program\n"
        "help               : Show command list\n"
        "info               : Show Something Information\n";

    printf("\n%s\n", help_message);

    return 0;
}

int util_init(struct __UTILITY_DATA *util)
{
    //pdu init
    util->pdu->cmd = CMD_UNKNOWN;
    memset(util->pdu->arg, 0, MAX_CMD_ARG_LEN);

    //recv init
    util->recv->flag = THREAD_DEAD;
    util->recv->thd = 0;

    //sft = null
    util->sft = NULL;

    return 0;
}

struct __UTILITY_DATA *util_create(void)
{

    SFT_PDU *pdu = pdu_new();
    if(!pdu)
        return NULL;

    static struct __RECV_THREAD recv = {
        .run = thread_recv,
        .ls = thread_ls,
        .get = thread_get,
        .put = thread_put,
        .close = thread_close
    };

    static struct __UTILITY_ACTION action = {
        .init = util_init,
        .listen = util_listen,
        .connection = util_connection,
        .ls = util_ls,
        .get = util_get,
        .put = util_put,
        .quit = util_quit,
        .info = util_info,
        .close = util_close,
        .help = util_help
    };

    UTILITY *util = (UTILITY *)malloc(sizeof(UTILITY));
    if(util) {
        memset(util, 0, sizeof(UTILITY));
        util->pdu = pdu;
        util->recv = &recv;
        util->action = &action;
    }

    util->action->init(util);

    return util;
}

int util_run(struct __UTILITY_DATA *util)
{
    unsigned char q_flag = 1;

    while(q_flag)
    {
        //set pdu init
        pdu_init(util->pdu);

        //set pdu cmd
        if(pdu_setcommand(util->pdu))
            continue;

        //execute / send cmd
        switch(util->pdu->cmd) {
        case CMD_CONNECTION:
            util->action->connection(util);
            break;
        case CMD_LISTEN:
            util->action->listen(util);
            break;
        case CMD_LS:
            util->action->ls(util);
            break;
        case CMD_GET:
            util->action->get(util);
            break;
        case CMD_PUT:
            util->action->put(util);
            break;
        case CMD_CLOSE:
            util->action->close(util);
            break;
        case CMD_QUIT:
            util->action->quit(util);
            q_flag = 0;
            break;
        case CMD_INFO:
            util->action->info(util);
            break;
        case CMD_HELP:
        default:
            util->action->help();
            break;
        }
    }

    return 0;
}

void util_destroy(struct __UTILITY_DATA *util)
{

    if(util->action)
        util->action->quit(util);

    free(util);

}

