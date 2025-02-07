#include "func.h"
#include "tpool.h"
CHATOBJECT sockfdarr[THREAD_NUM]={0};
//服务器处理私聊消息
void do_prichat(int connfd,MSG *msg,sqlite3 *db)
{
    char date[128]={};
    char sql[500];
    char *errmsg;
    int flag=0;
    char name[32];
    
    for(int i=0;i<THREAD_NUM;i++)
    {
        if((sockfdarr[i].connfd!=0) && (sockfdarr[i].connfd==connfd))
        {
            strcpy(name,sockfdarr[i].name);
            break;
        }
    }
    printf("到这里了！PRICHAT\n");
    printf("服务器：msg->data:%s\n",msg->data);
    int chatflag=0;
    for(int i=0;i<THREAD_NUM;i++)
    {
        if((sockfdarr[i].connfd==connfd) && (sockfdarr[i].banchat==1))
        {
            chatflag=1;
            break;
        }
    }
    if(chatflag==1)
    {
        strcpy(msg->data,"authority not enough!");
        if(-1==send(connfd,msg,sizeof(MSG),0))
        {
            perror("send");
            return;
        }
        get_date(date);
        sprintf(sql,"insert into chatrecord values('%s','%s','%s','REFUSE');",msg->name,date,msg->data);
        if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
        {
            printf("sqlite3_exec:%s\n",errmsg);
            return;
        }
    }
    else
    {
        msg->type=FRIENDCHAT;
        for(int i=0;i<THREAD_NUM;i++)
        {
            if((sockfdarr[i].connfd!=0) && (strcmp(msg->name,sockfdarr[i].name)==0))
            {
                get_date(date);
                sprintf(sql,"insert into chatrecord values('%s','%s','%s','%s(PRICHAT)');",name,date,msg->data,msg->name);
                if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
                {
                    printf("sqlite3_exec:%s\n",errmsg);
                    return;
                }
                strcpy(msg->name,name);
                if(-1==send(sockfdarr[i].connfd,msg,sizeof(MSG),0))
                {
                    perror("send");
                    return;
                }
                flag=1;
                break;
            }
        }
        if(flag==0)
        {
            get_date(date);
            sprintf(sql,"insert into chatrecord values('%s','%s','%s','NOT FIND(PRICHAT)');",name,date,msg->data);
            if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
            {
                printf("sqlite3_exec:%s\n",errmsg);
                return;
            }
            msg->type=PRICHAT;
            strcpy(msg->data,"FEIEND NOT ONLINE!");
            if(-1==send(connfd,msg,sizeof(MSG),0))
            {
                perror("send");
                return;
            }
            printf("告诉客户端，此账户不在线！\n");
        }
        else
        {
            msg->type=PRICHAT;
            strcpy(msg->data,"OK");
            if(-1==send(connfd,msg,sizeof(MSG),0))
            {
                perror("send");
                return;
            }
        }
    }
    return;
}

/*******************main函数******************** */
int main(int argc, char const *argv[])
{
    int port=PORT;
    sqlite3 *db;
    int sockfd;
    // char *targetname;
    // if(pthread_mutex_init(&sockfdarrlock,NULL)!=0)
    // {
    //     printf("pthread_mutex_init失败！\n");
    //     return -1;
    // }
    if(argc>1)
    {
        port=atoi(argv[1]);
    }
    if(sqlite3_open(DATABASE,&db)!=SQLITE_OK)
    {
        printf("sqlite3_open:%s\n",sqlite3_errmsg(db));
        return -1;
    }
    else
    {
        printf("打开数据库成功！\n");
    }
//如果数据库中没有相应的表，则创建相应的表
    if(sqlite3_exec(db,"create table if not exists usr(name text primary key,pass text);",NULL,NULL,NULL)!=SQLITE_OK)
    {
        perror("sqlite_exec");
        return -1;
    }
    if(sqlite3_exec(db,"create table if not exists chatrecord(name text,date text,word text,object text);",NULL,NULL,NULL)!=SQLITE_OK)
    {
        perror("sqlite_exec");
        return -1;
    }
    if(sqlite3_exec(db,"insert into usr values('admin',123);",NULL,NULL,NULL)!=SQLITE_OK)
    {
        printf("管理员账户已经存在！\n");
    }
    else
    {
        printf("管理员账户创建成功！\n");
    }
//创建线程池子
    if (tpool_create(THREAD_NUM) != 0)
    {
        printf("tpool_create failed\n");
        exit(-1);
    }
    printf("--- 线程池创建成功，阻塞等待接收任务中！ ---\n");
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        perror("socket");
        return -1;
    }
    struct sockaddr_in serveraddr;
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(port);
    serveraddr.sin_addr.s_addr=inet_addr(IP);
    if(-1==bind(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))
    {
        perror("bind");
        return -1;
    }
    if(-1==listen(sockfd,5))
    {
        perror("listen");
        return -1;
    }
    socklen_t sockaddr_len = sizeof(struct sockaddr);
    static struct epoll_event ev,events[EPOLL_SIZE];
    int epfd = epoll_create(EPOLL_SIZE);
    ev.events=EPOLLIN;
    ev.data.fd=sockfd;
    epoll_ctl(epfd,EPOLL_CTL_ADD,sockfd,&ev);
    while(1)
    {
        int event_count=epoll_wait(epfd,events,EPOLL_SIZE,-1);
        int i=0;
        for(;i<event_count;i++)
        {
            if(events[i].data.fd==sockfd)
            {
                int connfd;
                struct sockaddr_in clientaddr;
                while((connfd=accept(sockfd,(struct sockaddr *)&clientaddr,&sockaddr_len))>0)
                {
                    // pthread_mutex_lock(&sockfdarrlock);

                    // pthread_mutex_unlock(&sockfdarrlock);
                    printf("客户端[%d]进入连接......\n",connfd);
                    struct args *p_args = (struct args *)malloc(sizeof(struct args));
                    
                    p_args->fd=connfd;
                    
                    p_args->db=db;
                    p_args->do_register=do_register;
                    p_args->do_login=do_login;
                    p_args->do_pubchat=do_pubchat;
                    p_args->do_prichat=do_prichat;
                    p_args->do_searchfriend=do_searchfriend;
                    p_args->do_banchat=do_banchat;
                    p_args->do_nobanchat=do_nobanchat;
                    p_args->do_delmem=do_delmem;
                    p_args->do_filehead=do_filehead;
                    p_args->do_filedata=do_filedata;
                    tpool_add_work(worker,(void *)p_args);   
                }
            }
        }
    }
    return 0;
}
