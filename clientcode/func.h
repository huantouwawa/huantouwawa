#ifndef _FUNC_H
#define _FUNC_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <sys/epoll.h>
#include <sys/mman.h>

#define IP "192.168.136.135"
#define PORT 9999
#define EPOLL_SIZE 50 //epoll最大监听fd数量
#define N 32
#define Register 60
#define Login 61
#define PRICHAT 62
#define PUBCHAT 63
#define CHAT 64 //说明是群里发来的聊天消息
#define FRIENDCHAT 65   //说明是群里人发来的悄悄话
#define SEEFRIEND 66//说明是查看的群里人在线信息
#define BANCHAT 67  //说明是管理员账号发来的禁言群成员信息
#define NOBANCHAT 68    //说明是管理员账号发来的解禁群成员信息
#define DELMEM 69       //说明是管理员账号发来的踢出群成员信息
#define FILECHATHEAD 70     //说明是要发送文件信息
#define FILECHATDATA 71
#define FILECHAT 72
#define BLOCKSIZE   536870912		//一块的大小，512M，一块由一个线程负责
#define FILENAME_MAXLEN 30  //文件名最大长度
#define INT_SIZE    4               //int类型长度

typedef struct menu1
{
    int type; //Register注册 Login登录  PRICHAT群发 PUBCHAT私聊
    char name[N];
    char data[256];
    int size;
    int fileflag;   //为0说明文件还没发送完，为1说明发送完了

}MSG;

//注册模块
int do_register(int sockfd,MSG *msg);

//登录模块
int do_login(int sockfd,MSG *msg);

//登录后的聊天室模块
int chat(int sockfd,MSG *msg);

//群发消息模块
void do_pubchat(int sockfd,MSG *msg);

//私聊消息模块
void do_prichat(int sockfd,MSG *msg);

//查看当前在线群成员模块
int do_seefriend(int sockfd,MSG *msg);


//禁言群成员模块（管理员功能）
void do_banchat(int sockfd,MSG *msg);

//解除禁言群成员模块（管理员功能）
void do_nobanchat(int sockfd,MSG *msg);

//踢出群成员模块（管理员功能）
void do_delmem(int sockfd,MSG *msg);

//管理员账户登录模块
int adminchat(int sockfd,MSG *msg);

//发送文件模块
int do_file(int sockfd,MSG *msg);

#endif


