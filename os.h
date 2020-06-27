#ifndef OS_H
#define OS_H
#include <QString>

class OS
{
public:
    OS();
    QString TEXT_BUFFER1,TEXT_BUFFER2,TEXT_BUFFER3;//每个Buffer的内容
    int  NUM_BLOCK,FULL_BUFFR1,FULL_BUFFR2,FULL_BUFFR3;//每个buffer的字符个数
    int EMPTY_BUFFR1,EMPTY_BUFFR2,EMPTY_BUFFR3;//每个buffer的剩余空间
    int BUFFER1_SIZE;//buffer1的大小
    int BUFFER2_SIZE;//buffer2的大小
    int BUFFER3_SIZE;//buffer2的大小
    int PRODUCER_SPEED ;//put的速度
    int CONSUMER1_SPEED;//get1的速度
    int CONSUMER2_SPEED ;//get2的速度
    int MOVER1_SPEED ;//move1的速度
    int MOVER2_SPEED ;//move2的速度
    int PRODUCER_NUM ;//producer线程的个数
    int CONSUMER1_NUM ;//consumer1线程的个数
    int CONSUMER2_NUM;//consumer1线程的个数
    int MOVER1_NUM ;//mover1线程的个数
    int MOVER2_NUM ;//mover2线程的个数
    void test();
    void pause_();
    void let_us_go();
    void start();

};

#endif // OS_H
