#include "os.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <qdebug.h>

const int N = 50;
static int BUFFER_SIZE = 5;//buf1的大小
static int PRODUCT_CNT = 15;//产品结束的个数
//static int PRODUCER_SPEED = 1;//put的速度
//static int CONSUMER1_SPEED = 2;//get1的速度
//static int CONSUMER2_SPEED = 1;//get2的速度
//static int MOVER1_SPEED = 2;//move1的速度
//static int MOVER2_SPEED = 2;//move2的速度
//static int PRODUCER_NUM = 1;//producer线程的个数
//static int CONSUMER1_NUM = 1;//consumer1线程的个数
//static int CONSUMER2_NUM = 1;//consumer1线程的个数
//static int MOVER1_NUM = 1;//mover1线程的个数
//static int MOVER2_NUM = 1;//mover2线程的个数

static clock_t start_t,end_t;
static int ii = 1,cnt_c = 0,cnt_m = 0;
//static pthread_t tid_p[N],tid_m1[N],tid_m2[N],tid_c1[N],tid_c2[N];
void OS::test(){
        qDebug()<<"nmd";
}
struct pthread_con{
    pthread_t tid[N];
    int p_num;
    int p_speed;
};
static pthread_con tp,tm1,tm2,tc1,tc2;

struct product{
    int buf[N];//生产商品值
    pthread_mutex_t lock;//互斥锁
    sem_t sem_empty,sem_full;//信号量
    int readpos,writepos;//读写位置
    int size; //缓冲区的大小
    int id;
};
static product buffer[4];

//初始化 信号量、互斥量、读写位置
void init(struct product *p){
    //初始化信号量
    int err;
    err = sem_init(&p->sem_empty,0,(unsigned int)p->size-1);
    if(err != 0) qDebug()<<"sem_init sem_empty fail "<<errno<<" !\n";
    err = sem_init(&p->sem_full,0,0);
    if(err != 0) qDebug()<<"sem_init sem_full fail "<<errno<<"\n";
    //初始化互斥量
    p->lock = PTHREAD_MUTEX_INITIALIZER;
    //初始化读写位置
    p->readpos = p->writepos = 0;
}
void finish(struct product *p){
    //销毁信号量
    sem_destroy(&p->sem_empty);
    sem_destroy(&p->sem_full);
    //销毁互斥量
    pthread_mutex_destroy(&p->lock);
    //读写位置归零
    p->readpos = p->writepos = 0;
    return ;
}
void put(struct product *p,int &data,int flag){//flag 0-代表为生产者 1-代表搬运者的后半段
    //判断缓冲池是否满了
    if((p->writepos+1)%(p->size) == p->readpos){
        qDebug()<<"****:in buf"<<p->id<<" producter wait for empty src\n";
    }
    sem_wait(&p->sem_empty);
    //互斥锁
    pthread_mutex_lock(&p->lock);
    p->buf[p->writepos] = data;
    p->writepos  = (p->writepos+1)%(p->size);
    qDebug()<<"put the"<< data<<" product into buf"<<p->id<<" successfully\n";
    if(flag == 0) ii++;
    else cnt_m++;
    pthread_mutex_unlock(&p->lock);
    sem_post(&p->sem_full);
}
int get(struct product *p,int flag){
    int data;
    //如果缓冲池空了
    if(p->readpos == p->writepos)
        qDebug()<<"**: in buf"<<p->id<<"consumer wait for full\n";
    sem_wait(&p->sem_full);
    pthread_mutex_lock(&p->lock);

    data = p->buf[p->readpos];
    p->readpos = (p->readpos+1)%(p->size);
    qDebug()<<"get product" <<data<<" from buf"<<p->id<<" successfully\n";
    if(flag == 0)
        cnt_c++;

    pthread_mutex_unlock(&p->lock);
    sem_post(&p->sem_empty);
    return data;
}
//生产线程
void *producer(void *arg){
    while(1){
        sleep(tp.p_speed);
        if(ii > PRODUCT_CNT) break;
        put(&buffer[1],ii,0);
    }
    // printf("put the %d product into buf1....\n",ii);
    qDebug()<<"*****************producer stopped*****************\n";
    return (void *)0;
}
//消费线程
void *consumer1(void *arg){
    int data;
    while(1){
        sleep(tc1.p_speed);
        if(cnt_c >= PRODUCT_CNT){
            qDebug()<<"*****************consumer1 stopped***********************\n";
            end_t  = clock();
            qDebug()<<"总耗时为："<<(double)(end_t-start_t)/CLOCKS_PER_SEC;
//            exit(-1);
        }
        data = get(&buffer[2],0);
    }

    return (void *)0;
}
void *consumer2(void *arg){
    int data;
    while(1){
        sleep(tc2.p_speed);
        if(cnt_c >= PRODUCT_CNT) {
            qDebug()<<"*****************consumer2 stopped***********************\n";
            end_t  = clock();
            qDebug()<<"总耗时为："<<(double)(end_t-start_t)/CLOCKS_PER_SEC;
//            exit(-1);
        }
        data = get(&buffer[3],0);
    }

    return (void *)0;
}
//搬运线程
void *mover1(void *arg){
    int data;
    while(1){
        sleep(tm1.p_speed);
        if(cnt_m >= PRODUCT_CNT)
            break;
        data = get(&buffer[1],1);
//        qDebug()<<"move to buf2....\n";
        put(&buffer[2],data,1);
        // cnt_m++;
    }
//    qDebug()<<"*****************mover1 stopped***********************\n";
    return (void *)0;
}
void *mover2(void *arg){
    int data;
    while(1){
        sleep(tm2.p_speed);
//        if(cnt_m >= PRODUCT_CNT){
//            break;
//        }
        data = get(&buffer[1],1);
//        qDebug()<<"move to buf3....\n";
        put(&buffer[3],data,1);
        // cnt_m++;
    }
//    qDebug()<<"*****************mover2 stopped***********************\n";
    return (void *)0;
}
void OS::start(){
    void *retval;
    //初始化信号量、互斥量、各个buffer
    for(int i = 1;i <= 3; ++i)
        init(&buffer[i]),buffer[i].id = i;

    //创建线程    void *retval;
    for(int i = 1; i <= tp.p_num; ++i)
        pthread_create(&tp.tid[i],NULL,producer,NULL);
    for(int i = 1; i <= tm1.p_num; ++i)
        pthread_create(&tm1.tid[i],NULL,mover1,NULL);
    for(int i = 1; i <= tm2.p_num; ++i)
        pthread_create(&tm2.tid[i],NULL,mover2,NULL);
    for(int i = 1; i <= tc1.p_num; ++i)
        pthread_create(&tc1.tid[i],NULL,consumer1,NULL);
    for(int i = 1; i <= tc2.p_num; ++i)
        pthread_create(&tc2.tid[i],NULL,consumer2,NULL);

    //等待线程结束
//    for(int i = 1; i <= tp.p_num; ++i)
//        pthread_join(tp.tid[i],&retval);
//    for(int i = 1; i <= tm1.p_num; ++i)
//        pthread_join(tm1.tid[i],&retval);
//    for(int i = 1; i <= tm2.p_num; ++i)
//        pthread_join(tm2.tid[i],&retval);
//    for(int i = 1; i <= tc1.p_num; ++i)
//        pthread_join(tc1.tid[i],&retval);
//    for(int i = 1; i <= tc2.p_num; ++i)
//        pthread_join(tc2.tid[i],&retval);

    //销毁
    for(int i = 1; i <= 3; ++i)
        finish(&buffer[i]);
    return ;
}

OS::OS()
{
    //    scanf("%d %d %d",&buffer[1].size,&buffer[2].size,&buffer[3].size);
        buffer[1].size= buffer[2].size= buffer[3].size = 5;
    //    scanf("%d %d %d %d %d",&tp.p_num,&tm1.p_num,&tm2.p_num,&tc1.p_num,&tc2.p_num);
        tp.p_num = tm1.p_num = tm2.p_num = tc1.p_num = tc2.p_num = 2;
    //    scanf("%d %d %d %d %d",&tp.p_speed,&tm1.p_speed,&tm2.p_speed,&tc1.p_speed,&tc2.p_speed);
        tp.p_speed = tm1.p_speed = tm2.p_speed = tc1.p_speed = tc2.p_speed = 1;
        start();
}
