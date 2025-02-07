#include"func.h"
extern CHATOBJECT sockfdarr[THREAD_NUM];
//工作线程模块
void *worker(void *argc)
{
    // void *p1;
    struct args *pw=(struct args *)argc;
    int connfd=pw->fd;
    sqlite3 *db=pw->db;
    int *targetfd=(int *)malloc(sizeof(int));
    MSG msg;
    while(recv(connfd,&msg,sizeof(MSG),0)>0)
    {
        // p1=&msg;
        // printf("****sizeof(msg)=%ld\n",sizeof(msg));
        // printf("****address(msg)=%p\n",p1);
        switch(msg.type)
        {
            case Register:
                // p1=&msg;
                // printf("sizeof(msg)=%ld\n",sizeof(msg));
                // printf("address(msg)=%p\n",p1);
                //printf("msg.chat:%d  msg.name=%s  msg.type=%d  msg.data=%s\n",msg.chat,msg.name,msg.type,msg.data);
                // printf("strlen(msg)=%d\n",strlen((char *)msg));
                printf("线程[%ld]正在执行工作 Register!\n",pthread_self());
                pw->do_register(connfd,&msg,db);
                printf("线程[%ld]完成工作 Register\n",pthread_self());
                break;
            case Login:
                // p1=&msg;
                // printf("sizeof(msg)=%ld\n",sizeof(msg));
                // printf("address(msg)=%p\n",p1);
                //printf("msg.chat:%d  msg.name=%s  msg.type=%d  msg.data=%s\n",msg.chat,msg.name,msg.type,msg.data);
                // printf("strlen(msg)=%d\n",strlen(msg));
                printf("线程[%ld]正在执行工作 Login!\n",pthread_self());
                pw->do_login(connfd,&msg,db);
                printf("线程[%ld]完成工作 Login\n",pthread_self());
                for(int i=0;i<THREAD_NUM;i++)
                {
                    printf("%s : %d %d\n",sockfdarr[i].name,sockfdarr[i].connfd,sockfdarr[i].banchat);
                }
                printf("到这里了吗！\n");
                break;
            case PUBCHAT:
                // p1=&msg;
                // printf("sizeof(msg)=%ld\n",sizeof(msg));
                // printf("address(msg)=%p\n",p1);
                //printf("msg.chat:%d  msg.name=%s  msg.type=%d  msg.data=%s\n",msg.chat,msg.name,msg.type,msg.data);
                printf("线程[%ld]正在执行工作 PUBCHAT!\n",pthread_self());
                pw->do_pubchat(connfd,&msg,db);
                printf("线程[%ld]完成工作 PUBCHAT\n",pthread_self());
                break;
            case PRICHAT:
                //printf("msg.chat:%d  msg.name=%s  msg.type=%d  msg.data=%s\n",msg.chat,msg.name,msg.type,msg.data);
                printf("线程[%ld]正在执行工作 PRICHAT!\n",pthread_self());
                pw->do_prichat(connfd,&msg,db);
                printf("线程[%ld]完成工作 PRICHAT\n",pthread_self());
                break;
            case SEEFRIEND:
                //printf("msg.chat:%d  msg.name=%s  msg.type=%d  msg.data=%s\n",msg.chat,msg.name,msg.type,msg.data);
                printf("线程[%ld]正在执行工作 SEEFRIEND!\n",pthread_self());
                pw->do_searchfriend(connfd,&msg,db);
                printf("线程[%ld]完成工作 SEEFRIEND\n",pthread_self());
                break;
            case BANCHAT:
                //printf("msg.chat:%d  msg.name=%s  msg.type=%d  msg.data=%s\n",msg.chat,msg.name,msg.type,msg.data);
                printf("线程[%ld]正在执行工作 BANCHAT!\n",pthread_self());
                pw->do_banchat(connfd,&msg,db);
                printf("线程[%ld]完成工作 BANCHAT\n",pthread_self());
                break;
            case NOBANCHAT:
                //printf("msg.chat:%d  msg.name=%s  msg.type=%d  msg.data=%s\n",msg.chat,msg.name,msg.type,msg.data);
                printf("线程[%ld]正在执行工作 NOBANCHAT!\n",pthread_self());
                pw->do_nobanchat(connfd,&msg,db);
                printf("线程[%ld]完成工作 NOBANCHAT\n",pthread_self());
                break;
            case DELMEM:
                //printf("msg.chat:%d  msg.name=%s  msg.type=%d  msg.data=%s\n",msg.chat,msg.name,msg.type,msg.data);
                printf("线程[%ld]正在执行工作 DELMEM!\n",pthread_self());
                pw->do_delmem(connfd,&msg,db);
                printf("线程[%ld]完成工作 DELMEM\n",pthread_self());
                break;
            case FILECHATHEAD:
                printf("线程[%ld]正在执行工作 FILECHATHEAD!\n",pthread_self());
                pw->do_filehead(connfd,&msg,db,targetfd);
                printf("线程[%ld]完成工作 FILECHATHEAD\n",pthread_self());
                break;
            case FILECHATDATA:
                printf("线程[%ld]正在执行工作 FILECHATDATA!\n",pthread_self());
                pw->do_filedata(connfd,&msg,db,targetfd);
                printf("线程[%ld]完成工作 FILECHATDATA\n",pthread_self());
                break;
            default:
                break;
        }
    }
    for(int i=0;i<THREAD_NUM;i++)
    {
        if(sockfdarr[i].connfd==connfd)
        {
            memset(sockfdarr[i].name,0,sizeof(sockfdarr[i].name));
            sockfdarr[i].connfd=0;
            sockfdarr[i].banchat=0;
            break;
        }
    }
    printf("客户端[%d]退出！\n",connfd);
    close(connfd);
    return NULL;
}




//传送文件
void do_filehead(int connfd,MSG *msg,sqlite3 *db,int *targetfd)
{
    
    int fd;
    int flag=0;
    char *filename=msg->data;
        
        // strcpy(filename,msg->data);
        printf("服务器接收到接收方name=%s\n",msg->name);
        printf("服务器接收到文件信息filename=%s\n",filename);
        // printf("fargetname=%s\n",fargetname);
        for(int i=0;i<THREAD_NUM;i++)
        {
            if(strcmp(sockfdarr[i].name,msg->name)==0)
            {
                *targetfd=sockfdarr[i].connfd;   //保存接收方的文件描述符
                flag=1;
                break;
            }
        }
        printf("接收方的文件描述符*targetfd=%d\n",*targetfd);
        if(-1==(fd=open(filename,O_WRONLY|O_CREAT|O_TRUNC,0664)))
        {
            printf("open error！\n");
            return ;
        }
        if(flag==0)//说明没找到
        {
            msg->type=FILECHAT;
            strcpy(msg->data,"NO MEMBER");
            send(connfd,msg,sizeof(MSG),0);
        }
        else{   //说明找到了
            send(*targetfd,msg,sizeof(MSG),0);
            strcpy(msg->data,"OK");
            send(connfd,msg,sizeof(MSG),0);
        }
        

    close(fd);
    return;
}

//传送文件
void do_filedata(int connfd,MSG *msg,sqlite3 *db,int *targetfd)
{
    int size=0;
    printf("进去do_filedata了吗?\n");
    int fd;
    // printf("fargetname=%s\n",fargetname);
    //printf("msg.name=%s msg.data=%s\n",msg->name,msg->data);
    char *filename=msg->name;   //这是文件名
    printf("filename=%s\n",filename);

    if(-1==(fd=open(filename,O_WRONLY|O_APPEND)))
    {
        printf("open error!\n");
        return ;
    }
    //printf("msg.name= %s 打开成功 msg.data=%s\n",msg->name,msg->data);
    if(-1==lseek(fd,SEEK_END,0))
    {
        perror("lseek");
        return ;
    }
    
    size=write(fd,msg->data,msg->size);
    // printf("服务器写字节大小write-size=%d\n",size);


    //开始转发
    send(*targetfd,msg,sizeof(MSG),0);
    msg->type=FILECHAT;
    strcpy(msg->data,"OK");
    send(connfd,msg,sizeof(MSG),0);
    
    close(fd);
}

//服务器端账号注册
void do_register(int connfd,MSG *msg,sqlite3 *db)
{
    // printf("进来到这里了！\n");
    char *errmsg;
    char sql[500]={0};
    // printf("%s %s\n",msg->name,msg->data);
    sprintf(sql,"insert into usr values('%s','%s');",msg->name,msg->data);
    if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_exec:%s\n",errmsg);
        strcpy(msg->data,"usr name already exist!");
    }
    else
    {
        printf("客户端注册账号成功！\n");
        strcpy(msg->data,"OK!");
    }
    if(-1==send(connfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return;
    }
    return;
}

//服务器账号登录
void do_login(int connfd,MSG *msg,sqlite3 *db)
{
    char *errmsg;
    char **pazresult;
    char sql[500];
    int nrow;
    int ncolumn;
    sprintf(sql,"select * from usr where name='%s' and pass='%s';",msg->name,msg->data);
    if(sqlite3_get_table(db,sql,&pazresult,&nrow,&ncolumn,&errmsg)!=SQLITE_OK)
    {
        printf("sqlite3_get_table:%s\n",errmsg);
        return;
    }
    if(nrow==1)
    {
        strcpy(msg->data,"OK");
        if(strncmp(msg->name,"admin",6)!=0)
        {
            for(int i=0;i<THREAD_NUM;i++)
            {
                if(sockfdarr[i].connfd==0)
                {
                    strcpy(sockfdarr[i].name,msg->name);
                    sockfdarr[i].connfd=connfd;
                    break;
                }
            }
        }
    }
    else
    {
        strcpy(msg->data,"usr/password wrong!");
    }
    if(-1==send(connfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return;
    }
    return;
}

//服务器处理群发消息
void do_pubchat(int connfd,MSG *msg,sqlite3 *db)
{
    char date[128]={};
    char sql[500];
    char *errmsg;
    printf("到这里了！PUBCHAT\n");
    printf("服务器：msg->data:%s\n",msg->data);
    int flag=0;
    for(int i=0;i<THREAD_NUM;i++)
    {
        if((sockfdarr[i].connfd==connfd) && (sockfdarr[i].banchat==1))
        {
            flag=1;
            printf("sockfdarr[i].banchat=%d and flag=%d \n",sockfdarr[i].banchat,flag);
            break;
        }
    }
    if(flag==1) //说明被禁言了，只能记录下这条消息
    {
        get_date(date);
        sprintf(sql,"insert into chatrecord values('%s','%s','%s','REFUSE');",msg->name,date,msg->data);
        if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
        {
            printf("sqlite3_exec:%s\n",errmsg);
            return;
        }
        strcpy(msg->data,"authority not enough!");
        if(-1==send(connfd,msg,sizeof(MSG),0))
        {
            perror("send");
            return;
        }
    }
    else
    {
        if(strncmp(msg->data,"^-^",4)==0)
        {
            strcpy(msg->data,"向大家做了一个笑脸！");
        }
        msg->type=CHAT;
        for(int i=0;i<THREAD_NUM;i++)
        {
            if((sockfdarr[i].connfd!=0) && (sockfdarr[i].connfd!=connfd))
            {
                if(-1==send(sockfdarr[i].connfd,msg,sizeof(MSG),0))
                {
                    perror("send");
                    return;
                }
                get_date(date);
                sprintf(sql,"insert into chatrecord values('%s','%s','%s','PUBLIC');",msg->name,date,msg->data);
                if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=SQLITE_OK)
                {
                    printf("sqlite3_exec:%s\n",errmsg);
                    return;
                }
            }
        }
        msg->type=PUBCHAT;
        strcpy(msg->data,"OK");
        if(-1==send(connfd,msg,sizeof(MSG),0))
        {
            perror("send");
            return;
        }
    }
    return;
}


//服务器处理在线用户查询
void do_searchfriend(int connfd,MSG *msg,sqlite3 *db)
{
    int number=0;
    for(int i=0;i<THREAD_NUM;i++)
    {
        if(sockfdarr[i].connfd)
        {
            strcpy(msg->name,sockfdarr[i].name);
            if(-1==send(connfd,msg,sizeof(MSG),0))
            {
                perror("send");
                return;
            }
            printf("msg.name：%s 发送给客户端\n",msg->name);
        }
        usleep(10000);
    }
    return;
}



//服务器进行禁言群用户
void do_banchat(int connfd,MSG *msg,sqlite3 *db)
{
    int flag=0;
    for(int i=0;i<THREAD_NUM;i++)
    {
        if(strcmp(msg->name,sockfdarr[i].name)==0)
        {
            sockfdarr[i].banchat=1;
            if(-1==send(sockfdarr[i].connfd,msg,sizeof(MSG),0))
            {
                perror("send");
                return ;
            }
            flag=1;
            break;
        }
    }
    if(flag==0)
    {
        strcpy(msg->data,"NO MEMBER!");
    }
    else
    {
        strcpy(msg->data,"OK");
    }
    if(-1==send(connfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return ;
    }
    return;
}

//服务器进行解除禁言群用户
void do_nobanchat(int connfd,MSG *msg,sqlite3 *db)
{
    int flag=0;
    for(int i=0;i<THREAD_NUM;i++)
    {
        if(strcmp(msg->name,sockfdarr[i].name)==0)
        {
            sockfdarr[i].banchat=0;
            if(-1==send(sockfdarr[i].connfd,msg,sizeof(MSG),0))
            {
                perror("send");
                return ;
            }
            flag=1;
            break;
        }
    }
    if(flag==0)
    {
        strcpy(msg->data,"NO MEMBER!");
    }
    else
    {
        strcpy(msg->data,"OK");
    }
    if(-1==send(connfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return ;
    }
    return;
}

//服务器进行踢出群用户
void do_delmem(int connfd,MSG *msg,sqlite3 *db)
{
    for(int i=0;i<THREAD_NUM;i++)
    {
        if(strcmp(sockfdarr[i].name,msg->name)==0)
        {
            msg->type=DELMEM;
            if(-1==send(sockfdarr[i].connfd,msg,sizeof(MSG),0))
            {
                perror("send");
                return;
            }
        }
    }
    return ;
}


//获取时间
int get_date(char *date)
{
    time_t t;
    struct tm *tp;
    time(&t);
    tp=localtime(&t);
    sprintf(date,"%d-%d-%d %d:%d:%d",tp->tm_year+1900,tp->tm_mon+1,
    tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
    return 0;
}



