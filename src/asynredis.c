#include  "asynreids.h"


// #include "../memwatch-2.71/memwatch.h"

// #ifndef SIGSEGV
// #error "SIGNAL.H does not define SIGSEGV; running this program WILL cause a core dump/crash!"
// #endif

// #ifndef MEMWATCH
// #error "You really, really don't want to run this without memwatch. Trust me."
// #endif

// #if !defined(MW_STDIO) && !defined(MEMWATCH_STDIO)
// #error "Define MW_STDIO and try again, please."
// #endif

int changeDeviceMemory(char *key, char *value)
{
    char dev[10];
    memset(dev, 0, 10);
    strncat(dev, key, 4);
    int devIndex = getDevIndex(dev);
    int keyIndex = getKeyIndex(key);
    if (devIndex < 0 || keyIndex < 0)
    {
        return -1;
    }
    if (strncmp(dma.dma[devIndex]->dev, dev, 4) != 0)
    {
        printf("dev not cmp %s", dev);
    }
    //printf("dev = %s\n", dma.dma[devIndex]->dev);

    switch (key[4])
    {
    case '0': //AI
        dma.dma[devIndex]->AI[keyIndex] = atof(value);
        break;
    case '1': //AO
        dma.dma[devIndex]->AO[keyIndex] = atof(value);
        break;
    case '2': //AV
        dma.dma[devIndex]->AV[keyIndex] = atof(value);
        break;
    case '3': //BI
        dma.dma[devIndex]->BI[keyIndex] = atoi(value);
        break;
    case '4': //BO
        dma.dma[devIndex]->BO[keyIndex] = atoi(value);
        break;
    case '5': //BV
        dma.dma[devIndex]->BV[keyIndex] = atoi(value);
        break;
    default: //other
        break;
    }
    return 0;
}
void getCallback(redisAsyncContext *c, void *r, void *privdata)
{
    redisReply *reply = r;
    if (reply->elements == 4)
    {
        char *str = strdup(reply->element[3]->str);
        //printf("\n %s\n", str);
        char key[50];
        char type[100];
        char val[10240];
        sscanf(str, "%s\r\n%s\r\n%s", key, type, val);
        if (strncmp(type, "Present_Value", 13) == 0)
        {
            changeDeviceMemory(key, val);
        }
        printf("pubdata =  %s %s %s ", key, type, val);
        free(str);
        exit(0);
    }

    //freeReplyObject(reply);
    if (reply == NULL)
        return;
    //printf("argv[%s]: %s\n", (char *)privdata, reply->str);
}

void asyn()
{
    signal(SIGPIPE, SIG_IGN);
    struct event_base *base = event_base_new();
    redisAsyncContext *c = redisAsyncConnect("127.0.0.1", 6379);
    if (c->err)
    {
        printf("Error:redis %s\n", c->errstr);
    }
    redisLibeventAttach(c, base);
    redisAsyncSetConnectCallback(c, connectCallback);
    redisAsyncSetDisconnectCallback(c, disconnectCallback);
    redisAsyncCommand(c, getCallback, (char *)"sub", "PSUBSCRIBE *");
    event_base_dispatch(base);
    //环境  回调函数 私人传值 命令 参数。。。
}

//int main(int argc, char **argv)
//{
//    redisInit();
//    initDeviceMemoryAll();
//    signal(14, signal_handler);
//    set_timer();
//    printf("%d\n", getDevIndex("1001"));
//    printf("keyindex =  (%d)\n", getKeyIndex("1001108"));
//    redisSubscribeRun();
//    return 0;
//}
void connectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK)
    {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Connected...\n");
}
void disconnectCallback(const redisAsyncContext *c, int status)
{
    if (status != REDIS_OK)
    {
        printf("Error: %s\n", c->errstr);
        return;
    }
    printf("Disconnected...\n");
}
