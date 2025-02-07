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
#include <sqlite3.h>


#define IP "192.168.136.135"
#define PORT 9999
#define EPOLL_SIZE 50 //epoll最大监听fd数量
#define DATABASE "my.db"
#define N 32
#define Register 60
#define Login 61
#define PRICHAT 62
#define PUBCHAT 63
#define THREAD_NUM 8
#define CHAT 64 //说明是群里发来的聊天消息
#define FRIENDCHAT 65
#define SEEFRIEND 66//说明是查看的群里人在线信息
#define BANCHAT 67  //说明是管理员账号发来的禁言群成员信息
#define NOBANCHAT 68    //说明是管理员账号发来的解禁群成员信息
#define DELMEM 69       //说明子管理员账号发来的踢出群成员信息
#define FILECHATHEAD 70     //说明是要发送文件信息
#define FILECHATDATA 71
#define FILECHAT 72
#define BLOCKSIZE   536870912		//一块的大小，512M，一块由一个线程负责
#define FILENAME_MAXLEN 30  //文件名最大长度
#define INT_SIZE    4      


typedef struct
{
    char name[N];
    int connfd;
    int banchat;    //1为禁止发言
}CHATOBJECT;    //用于记录msg.name与connfd的对应关系



typedef struct menu1
{
    int type; //Register注册 Login登录  PRICHAT群发 PUBCHAT私聊
    char name[N];
    char data[256];
    int size;

}MSG;


struct args{
    int fd;
    sqlite3 *db;
    void  (*do_register)(int connfd,MSG *msg,sqlite3 *db);
    void (*do_login)(int connfd,MSG *msg,sqlite3 *db);
    void (*do_prichat)(int connfd,MSG *msg,sqlite3 *db);
    void (*do_pubchat)(int connfd,MSG *msg,sqlite3 *db);
    void (*do_searchfriend)(int connfd,MSG *msg,sqlite3 *db);
    void (*do_banchat)(int connfd,MSG *msg,sqlite3 *db);
    void (*do_nobanchat)(int connfd,MSG *msg,sqlite3 *db);
    void (*do_delmem)(int connfd,MSG *msg,sqlite3 *db);
    void (*do_filehead)(int connfd,MSG *msg,sqlite3 *db,int *targetfd);
    void (*do_filedata)(int connfd,MSG *msg,sqlite3 *db,int *fargetfd);
};


//工作线程模块
void *worker(void *argc);

//服务器端账号注册
void do_register(int connfd,MSG *msg,sqlite3 *db);

//服务器账号登录
void do_login(int connfd,MSG *msg,sqlite3 *db);

//服务器处理群发消息
void do_pubchat(int connfd,MSG *msg,sqlite3 *db);

//服务器处理在线用户查询
void do_searchfriend(int connfd,MSG *msg,sqlite3 *db);

//服务器进行禁言群用户
void do_banchat(int connfd,MSG *msg,sqlite3 *db);

//服务器进行解除禁言群用户
void do_nobanchat(int connfd,MSG *msg,sqlite3 *db);

//服务器进行踢出群用户
void do_delmem(int connfd,MSG *msg,sqlite3 *db);

//获取时间
int get_date(char *date);

//传送文件
void do_filehead(int connfd,MSG *msg,sqlite3 *db,int *targetfd);

void do_filedata(int connfd,MSG *msg,sqlite3 *db,int *targetfd);
#endif



