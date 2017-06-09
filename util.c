#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<sys/time.h>

#include "util.h"
#include "heapsort.h"

static char curr_addr[MAX_PATH_LEN] = "./";
pthread_mutex_t sft_lock;

int str_cmp(const void *arg1, const void *arg2)
{
    char *s1 = (char *)arg1;
    char *s2 = (char *)arg2;

    if(strchr(s1, '/')) {
        if(strchr(s2, '/'));
        else
            return -1;
    }
    else
        if(strchr(s2, '/'))
            return 1;


    if(s1[0] == s2[0]) {
        int len1 = strlen(s1);
        int len2 = strlen(s2);
        if(len1 == len2)
            return 0;
        if(len1 > len2)
            return 1;
        if(len1 < len2)
            return -1;
    }
    if(s1[0] > s2[0])
        return 1;
    if(s1[0] < s2[0])
        return -1;

    return 0;
}

void str_swap(void *arg1, void *arg2, int size)
{
    char temp[size];
    strncpy(temp, (char *)arg1, size);
    strncpy((char *)arg1, (char *)arg2, size);
    strncpy((char *)arg2, temp, size);
}

void *thread_recv(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;

    util->recv->flag = THREAD_ALIVE;

    int recvfd = 0;
    if(util->sft->type == SFT_SERVER)
        recvfd = util->sft->remotefd;
    else
        recvfd = util->sft->sockfd;

    fd_set recvset;
    FD_ZERO(&recvset);

    //select ...
    while(util->recv->flag) {

        pthread_mutex_lock(&sft_lock);

        FD_SET(recvfd, &recvset);

        struct timeval timeout = {
            .tv_sec = 0,
            .tv_usec = 0
        };

        if(select(recvfd + 1, &recvset, NULL, NULL, &timeout) == -1) {
            fprintf(stderr, "Select Function Failure!\n");
            pthread_exit(NULL);
        }

        if(FD_ISSET(recvfd, &recvset) == 0) {
            pthread_mutex_unlock(&sft_lock);
            sleep(1);
            continue;
        }

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

        pthread_mutex_unlock(&sft_lock);
    }

    pthread_exit(NULL);
}

void *thread_ls(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;

    struct dirent **ls_dir;

    int count = scandir((char*)util->pdu->arg, &ls_dir, NULL, NULL);
    if(count < 0) {
        pdu_init(util->pdu);
        util->pdu->cmd = CMD_ERROR;
        snprintf(util->pdu->arg, MAX_CMD_ARG_LEN, "Scandir Failure %d", count);
        util->sft->action->send(util->sft, (void *)util->pdu, util->pdu->pdulen);
        fprintf(stderr, "Scandir Failure!\n");
        return NULL;
    }

    char copy_list[count][NAME_MAX];
    memset(copy_list, 0, count * NAME_MAX);

    for(int index = 0; index < count; ++index) {
        strncpy(copy_list[index], ls_dir[index]->d_name, NAME_MAX);
        if(ls_dir[index]->d_type == DT_DIR) {
            printf("DIR : %s", ls_dir[index]->d_name);
            strncpy(strchr(copy_list[index], '\0'), "/\0", NAME_MAX);
            printf(", %s\n", copy_list[index]);
        }
        free(ls_dir[index]);
    }
    free(ls_dir);

    for(int index = 0; index < count; ++index)
        printf("copy list [%d] = %s\n", index, copy_list[index]);

    heapify((void*)copy_list, count, NAME_MAX, str_cmp, str_swap);

    heapsorting((void*)copy_list, count, NAME_MAX, str_cmp, str_swap);

    for(int index = 0; index < count; ++index) {
        strncpy((char *)util->pdu->arg, copy_list[index], MAX_CMD_ARG_LEN);
        util->sft->action->send(util->sft, (void *)util->pdu, util->pdu->pdulen);
    }

    pdu_init(util->pdu);
    util->sft->action->send(util->sft, (void *)util->pdu, util->pdu->pdulen);

    return NULL;
}

void *thread_get(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;

    FILE *readfd = fopen((char *)util->pdu->arg, "r");
    if(readfd == NULL) {
        pdu_error(util->pdu, "remote open file error");
        perror("fopen");
        util->sft->action->send(util->sft, (void *)util->pdu, util->pdu->pdulen);
        return NULL;
    }

    while(fgets((char *)util->pdu->arg, MAX_CMD_ARG_LEN, readfd) != NULL)
        util->sft->action->send(util->sft, (void *)util->pdu, util->pdu->pdulen);

    pdu_init(util->pdu);
    util->sft->action->send(util->sft, (void *)util->pdu, util->pdu->pdulen);

    fclose(readfd);

    return NULL;
}

void *thread_put(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;

    FILE *writefd = fopen((char *)util->pdu->arg, "w+");
    if(writefd == NULL) {
        pdu_error(util->pdu, "remote open file error");
        perror("fopen");
        util->sft->action->send(util->sft, (void *)util->pdu, util->pdu->pdulen);
        return NULL;
    }

    while(1) {
        util->sft->action->recv(util->sft, (void *)util->pdu, util->pdu->pdulen);
        if(util->pdu->cmd != CMD_PUT)
            break;

        fputs((char *)util->pdu->arg, writefd);
    }

    fclose(writefd);

    return NULL;
}

void *thread_close(void *arg)
{
    struct __UTILITY_DATA *util = (struct __UTILITY_DATA *)arg;
    printf("\n ------------------------------ \n");
    printf("REceiver : Recv Command \"close\" \n");
    printf("\n ------------------------------ \n");

    util->sft->action->send(util->sft, (void *)util->pdu, sizeof(SFT_PDU));

    sft_destroy(util->sft);
    util->sft = NULL;

    util->recv->flag = THREAD_DEAD;

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
        fprintf(stderr, "SFT Create Failure\n");
        return -1;
    }

    int res = util->sft->action->init(util->sft);
    if(res) {
        fprintf(stderr, "sft init failure\n");
        sft_destroy(util->sft);
        return res;
    }

    res = util->sft->action->connect(util->sft, NULL, SFT_PORT);
    if(res) {
        fprintf(stderr, "sft connection failure\n");
        sft_destroy(util->sft);
        return res;
    }

    res = pthread_create(&util->recv->thd, NULL, util->recv->run, (void *)util);
    if(res != 0) {
        perror("pthread create");
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

    //open file
    FILE *readfd = fopen((char *)util->pdu->arg, "r");
    if(readfd == NULL) {
        perror("fopen");
        return -1;
    }

    //Generator remote addr
    char temp[MAX_PATH_LEN];
    snprintf(temp, MAX_PATH_LEN, "%s/%s", curr_addr, (char *)util->pdu->arg);
    strncpy((char *)util->pdu->arg, temp, MAX_CMD_ARG_LEN);

    //Lock -> send command & transfer file -> Unlock
    pthread_mutex_lock(&sft_lock);

    util->sft->action->send(util->sft, (void *)util->pdu, sizeof(SFT_PDU));
    while(fgets((char *)util->pdu->arg, MAX_CMD_ARG_LEN, readfd))
        util->sft->action->send(util->sft, (void *)util->pdu, sizeof(SFT_PDU));

    pdu_init(util->pdu);
    util->sft->action->send(util->sft, (void *)util->pdu, sizeof(SFT_PDU));

    pthread_mutex_unlock(&sft_lock);

    //Close file
    fclose(readfd);

    //Finish ! if Error print else writen curraddr
    if(util->pdu->cmd == CMD_ERROR)
        printf("put : %s\n", (char *)util->pdu->arg);
    else {
        *(strrchr(temp, '/')) = '\0';
        strncpy(curr_addr, temp, MAX_PATH_LEN);
    }

    return 0;
}

int util_get(struct __UTILITY_DATA *util)
{
    if(!util->sft) {
        printf("Get Error : Please Listen / Connnection First\n");
        return -1;
    }

    //Check user enter address
    char temp[MAX_PATH_LEN];
    if(strncmp((char*)util->pdu->arg, "/", 1) == 0)
        strncpy(temp, (char *)util->pdu->arg, MAX_PATH_LEN);
    else {
        if(strncmp((char *)util->pdu->arg, "./", 2) == 0)
            snprintf(temp, MAX_PATH_LEN, "%s/%s", curr_addr, (char *)util->pdu->arg+2);
        else
            snprintf(temp, MAX_PATH_LEN, "%s/%s", curr_addr, (char *)util->pdu->arg);
        strncpy((char *)util->pdu->arg, temp, MAX_CMD_ARG_LEN);
    }

    //Local Open File Name
    FILE *writefd = fopen(strrchr(temp, '/')+1, "w+");
    if(writefd == NULL) {
        fprintf(stderr, "Open Local File Error!\n");
        return -1;
    }

    //Lock Mutex & Action
    //finish transfer unlock
    pthread_mutex_lock(&sft_lock);

    util->sft->action->send(util->sft, (void *)util->pdu, util->pdu->pdulen);

    while(1) {
        util->sft->action->recv(util->sft, (void *)util->pdu, util->pdu->pdulen);
        if(util->pdu->cmd != CMD_GET)
            break;

        if(fputs((char *)util->pdu->arg, writefd) == EOF)
            perror("fputs");
    }

    pthread_mutex_unlock(&sft_lock);

    //Finish ! if Error print else writen curraddr
    if(util->pdu->cmd == CMD_ERROR)
        printf("get : %s\n", (char *)util->pdu->arg);
    else {
        *(strrchr(temp, '/')) = '\0';
        strncpy(curr_addr, temp, MAX_PATH_LEN);
    }

    fclose(writefd);


    return 0;
}

int util_ls(struct __UTILITY_DATA *util)
{

    if(!util->sft) {
        printf("Ls Error : Please Listen / Connection First\n");
        return -1;
    }

    char temp[MAX_PATH_LEN];

    if(strncmp((char*)util->pdu->arg, "/", 1) == 0)
        strncpy(temp, (char *)util->pdu->arg, MAX_PATH_LEN);
    else {
        if(strncmp((char *)util->pdu->arg, "./", 2) == 0)
            snprintf(temp, MAX_PATH_LEN, "%s/%s", curr_addr, (char *)util->pdu->arg+2);
        else
            snprintf(temp, MAX_PATH_LEN, "%s/%s", curr_addr, (char *)util->pdu->arg);
        strncpy((char *)util->pdu->arg, temp, MAX_CMD_ARG_LEN);
    }

    pthread_mutex_lock(&sft_lock);

    util->sft->action->send(util->sft, (void *)util->pdu, util->pdu->pdulen);

    do {
        util->sft->action->recv(util->sft, (void *)util->pdu, util->pdu->pdulen);

        printf("%s\n", (char *)util->pdu->arg);

    }while(util->pdu->cmd == CMD_LS);

    pthread_mutex_unlock(&sft_lock);

    if(util->pdu->cmd != CMD_ERROR)
        strncpy(curr_addr, temp, MAX_PATH_LEN);

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

    //mutex init
    pthread_mutex_init(&sft_lock, NULL);

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
    pthread_mutex_destroy(&sft_lock);

    if(util->action)
        util->action->quit(util);

    free(util);
}

