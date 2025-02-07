#include"func.h"

//客户端的子线程用来进行登录后的一直接收消息
void* run(void *arg)
{
    MSG msg;
    int sockfd=*(int *)arg;
    int flag=0;
    int fd;
    int size;
    // printf("子线程等待信息中！\n");
    while(1)
    {
        memset(&msg,0,sizeof(MSG));
        if(-1==recv(sockfd,&msg,sizeof(MSG),0))
        {
            perror("recv");
            continue;
        }
        if(msg.type==FILECHATHEAD)
        {
            char *filename=msg.data;
            printf("接收方第一次接收filename=%s\n",filename);
            if(-1==(fd=open(filename,O_WRONLY|O_CREAT|O_TRUNC,0664)))
            {
                printf("open error！\n");
                return NULL;
            }
            close(fd);
            continue;
        }
        if(msg.type==FILECHATDATA)
        {
            char *filename=msg.name;    //这是文件名字
            printf("filename=%s\n",filename);

            if(-1==(fd=open(filename,O_WRONLY|O_APPEND)))
            {
                printf("open error!\n");
                return NULL;
            }
            printf("接收方filename= %s 打开成功\n",filename);
            if(-1==lseek(fd,SEEK_END,0))
            {
                perror("lseek");
                return NULL;
            }
            size=write(fd,msg.data,msg.size);
            printf("write-size=%d\n",size);
            close(fd);
            continue;
        }
        if(msg.type==FILECHAT)
        {
            if(strncmp(msg.data,"OK",3)==0)
            {
                printf("文件名发送成功！\n");
            }
            else
            {
                printf("当前接收端客户不在线！\n");
            }
            continue;
        }
        if(msg.type==DELMEM)
        {
            printf("您被踢出当前群聊！\n");
            exit(0);
        }
        if(msg.type==BANCHAT)
        {
            printf("msg.data=%s msg.type=%d msg.name=%s\n",msg.data,msg.type,msg.name);
            printf("您已经被管理员禁言！\n");
            continue;
        }
        if(msg.type==NOBANCHAT)
        {
            printf("您已经被管理员解除禁言！\n");
            continue;
        }
        if(msg.type==SEEFRIEND)
        {
            if(flag==0)
            {
                printf("当前群中在线的用户：\n");
                flag=1;
            }
            printf("%s \n",msg.name);
            continue;
        }
        if(msg.type==CHAT)  //说明是群里发来的消息
        {
            printf("%s : %s\n",msg.name,msg.data);
            continue;
        }
        if(msg.type==FRIENDCHAT)    //说明是群里人发来的悄悄话
        {
            printf("%s(悄悄话) : %s\n",msg.name,msg.data);
            continue;
        }
        if(msg.type==PRICHAT && (strcmp(msg.data,"FEIEND NOT ONLINE!")==0))
        {
            printf("群聊中没有此用户！\n");
            continue;
        }
        if(strncmp(msg.data,"OK",3)==0)
        {
            if(msg.type==PUBCHAT)
            {
                printf("子线程：群发消息成功！\n");
            }
            else if(msg.type==PRICHAT)
            {
                printf("子线程：私聊消息发送成功！\n");
            }
        }
        // printf("msg.data=%s\n",msg.data);
        if(strcmp(msg.data,"authority not enough!")==0)
        {
            printf("发送失败（可能是管理员禁止您的发言！）\n"); 
        }
    }
    return NULL;
}



int main(int argc, char const *argv[])
{
    int port=PORT;
    int sockfd;
    int choose;
    char name[32];
    MSG msg;
    pthread_t tid;
    int MSG_LEN=sizeof(MSG);
    if(argc>1)
    {
        port=atoi(argv[1]);
    }
    sockfd=socket(AF_INET,SOCK_STREAM,0);
    if(sockfd==-1)
    {
        perror("socket");
        return -1;
    }
    struct sockaddr_in serveraddr,clientaddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(port);
    serveraddr.sin_addr.s_addr=inet_addr(IP);
    if(-1==connect(sockfd,(struct sockaddr *)&serveraddr,sizeof(serveraddr)))
    {
        perror("connect");
        return -1;
    }
    while(1)
    {
        printf("请选择注册或登录账户：\n");
        printf("1,注册账户      2,登录账户      3,退出\n");
        scanf("%d",&choose);
        getchar();
        switch (choose)
        {
            case 1:
                do_register(sockfd,&msg);
                break;
            case 2:
                if(do_login(sockfd,&msg)==1)
                {
                    if(strncmp(msg.name,"admin",6)==0)
                    {
                        printf("说明登录的是管理员账号，进入管理员界面！\n");
                        adminchat(sockfd,&msg);
                    }
                    else
                    {
                        if(pthread_create(&tid,NULL,run,&sockfd)!=0)
                        {
                            printf("创建用于接收的子线程失败！\n");
                            return -1;
                        }
                        printf("创建的子线程[%ld]用于接收消息！\n",tid);
                        chat(sockfd,&msg);
                        if (pthread_cancel(tid)!= 0) {
                            printf("终止子线程失败\n");
                            return 0;
                        }
                    }
                }
                else
                {
                    printf("账户不存在或密码错误！\n");
                }
                break;
            case 3:
                close(sockfd);
                exit(0);
            default:
                break;
        }
    }
    close(sockfd);
    return 0;
}

