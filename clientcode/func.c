#include"func.h"



//注册模块
int do_register(int sockfd,MSG *msg)
{
    msg->type=Register;
    printf("请输入你的账户名：");
    scanf("%s",msg->name);
    getchar();
    printf("请输入密码：");
    scanf("%s",msg->data);
    getchar();
    if(-1==send(sockfd,(void *)msg,sizeof(MSG),0))
    {
        perror("send");
        return -1;
    }
    if(-1==recv(sockfd,(void *)msg,sizeof(MSG),0))
    {
        perror("recv");
        return -1;
    }
    printf("%s\n",msg->data);
    return 0;
}

//登录模块
int do_login(int sockfd,MSG *msg)
{
    msg->type=Login;
    printf("请输入您的账户名：");
    scanf("%s",msg->name);
    getchar();
    printf("请输入密码：");
    scanf("%s",msg->data);
    getchar();
    if(-1==send(sockfd,(void *)msg,sizeof(MSG),0))
    {
        perror("send");
        return -1;
    }
    if(-1==recv(sockfd,(void *)msg,sizeof(MSG),0))
    {
        perror("recv");
        return -1;
    }
    if(strncmp(msg->data,"OK",3)==0)
    {
        printf("登录成功！\n");
        return 1;
    }
    else
    {
        printf("登录失败！\n");
    }
    return 0;
}

//登录后的聊天室模块（普通成员）
int chat(int sockfd,MSG *msg)
{
    int choose;
    char name[32];
    strcpy(name,msg->name);
    printf("**********欢迎加入聊天室！***************\n");
    while(1)
    {
        printf("请输入你的选项：\n");
        printf("1,群发消息  2,私聊消息  3,发送文件   4,查看当前在线用户     5,退出聊天室\n");
        scanf("%d",&choose);
        
        switch (choose)
        {
            case 1:
                printf("客户端主线程将 群发消息 发送给服务器！\n");
                do_pubchat(sockfd,msg);
                strcpy(msg->name,name);
                break;
            case 2:
                printf("客户端主线程将 私聊消息 发送给服务器！\n");
                do_prichat(sockfd,msg);
                strcpy(msg->name,name);
                break;
            case 3:
                printf("客户端主线程将 要发送文件 告知给服务器！\n");
                do_file(sockfd,msg);
                strcpy(msg->name,name);
                break;
            case 4:
                do_seefriend(sockfd,msg);
                strcpy(msg->name,name);
                break;
            case 5:
                return 0;
            default:
                printf("输入错误!\n");
                break;
        }
    }
    return 0;
}

//群发消息模块
void do_pubchat(int sockfd,MSG *msg)
{
    msg->type=PUBCHAT;
    printf("msg->type=%d\n",msg->type);
    printf("请输入你要发送的消息：\n");
    scanf("%s",msg->data);
    getchar();
    printf("msg->data:%s\n",msg->data);
    if(-1==send(sockfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return;
    }
    printf("群发消息发给服务器！\n");
    // if(-1==recv(sockfd,msg,sizeof(MSG),0))
    // {
    //     perror("recv");
    //     return;
    // }
    // if(strncmp(msg->data,"OK",3)==0)
    // {
    //     printf("发送成功！\n");
    // }
    // else
    // {
    //     printf("发送失败（可能是管理员禁止您的发言！）\n"); 
    // }
    return;
}

//私聊消息模块
void do_prichat(int sockfd,MSG *msg)
{
    msg->type=PRICHAT;
    printf("msg->type=%d\n",msg->type);
    printf("请输入要发送的消息：");
    scanf("%s",msg->data);
    getchar();
    printf("请输入要发送的成员！\n");
    scanf("%s",msg->name);
    getchar();
    if(-1==send(sockfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return;
    }
    printf("我悄悄发送的对象是：%s\n",msg->name);
    printf("悄悄话消息发送给服务器进行转发！\n");
    return;
}

//查看当前在线群成员模块
int do_seefriend(int sockfd,MSG *msg)
{
    msg->type=SEEFRIEND;
    if(-1==send(sockfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return -1;
    }
    printf("想要查看群里在线人数消息发给服务器！\n");
    return 0;
}

//禁言群成员模块（管理员功能）
void do_banchat(int sockfd,MSG *msg)
{
    msg->type=BANCHAT;
    printf("请输入你要禁言的群成员账户名：\n");
    scanf("%s",msg->name);
    getchar();
    if(-1==send(sockfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return;
    }
    if(-1==recv(sockfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return;
    }
    if(strncmp(msg->data,"OK",3)==0)
    {
        printf("禁言成功！\n");
    }
    else
    {
        printf("禁言失败（可能是用户不在线！）\n");
    }
    return;
}

//解除禁言群成员模块（管理员功能）
void do_nobanchat(int sockfd,MSG *msg)
{
    msg->type=NOBANCHAT;
    printf("请输入你要禁言的群成员账户名：\n");
    scanf("%s",msg->name);
    getchar();
    if(-1==send(sockfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return;
    }
    if(-1==recv(sockfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return;
    }
    if(strncmp(msg->data,"OK",3)==0)
    {
        printf("解除禁言成功！\n");
    }
    else
    {
        printf("解除禁言失败（可能是用户不在线或已下线！）\n");
    }
    return;
}

//踢出群成员模块（管理员功能）
void do_delmem(int sockfd,MSG *msg)
{
    msg->type=DELMEM;
    printf("请输入你要踢出的群成员账户名：\n");
    scanf("%s",msg->name);
    getchar();
    if(-1==send(sockfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return;
    }
    return;
}

//管理员账户登录模块
int adminchat(int sockfd,MSG *msg)
{
    int choose;
    while(1)
    {
        printf("请输入您的选项：\n");
        printf("1,禁言群成员  2,解禁群成员  3,踢出群成员    4,退出\n");
        scanf("%d",&choose);
        getchar();
        switch(choose)
        {
            case 1:
                do_banchat(sockfd,msg);
                break;

            case 2:
                do_nobanchat(sockfd,msg);
                break;
            case 3:
                do_delmem(sockfd,msg);
                break;
            case 4:
                return 0;
            default:
                printf("输入错误！\n");
                break;
        }
    }
}




//发送文件
int do_file(int sockfd,MSG *msg)
{
    int fd=0;
    int size=0;
    int i=1;
    char objname[32]={0};
    msg->type=FILECHATHEAD;
    msg->fileflag=0;
    printf("请输入您要发送文件的对象:\n");
    scanf("%s",msg->name);
    getchar();
    strcpy(objname,msg->name);
    char filename[N]={0};
    printf("请输入文件名 :\n");
    scanf("%s",filename);
    getchar();
    printf("sizeof(filename)=%ld\n",sizeof(filename));
    strcpy(msg->data,filename);
    printf("sizeof(msg->data)=%ld\n",sizeof(msg->data));
    // printf("strlen(filename)=%ld\n",strlen(filename));

    //第一次发送msg.data是文件名,msg.name是接收人
    if((fd = open(msg->data, O_RDONLY)) == -1 )
	{
		printf("open error！\n");
		return -1;
	}
    if(-1==send(sockfd,msg,sizeof(MSG),0))
    {
        perror("send");
        return -1;
    }
    // printf("1  msg.type=%d msg.name=%s msg.data(filename)=%s\n",msg->type,msg->name,msg->data);

    usleep(1000);
    msg->type=FILECHATDATA;
    strcpy(msg->name,filename);
    memset(msg->data,0,sizeof(msg->data));
    while(1)
    {
        size=read(fd,msg->data,255);
        if(size==255)
        {
            msg->size=255;
        }
        else if(size>0)
        {
            msg->size=size;
            if(-1==send(sockfd,msg,sizeof(MSG),0))
            {
                perror("send");
                return -1;
            }
            // printf("size=%d\n",size);
            
            // printf("2  msg.type=%d msg.name(filename)=%s msg.data=%s\n",msg->type,msg->name,msg->data);
            memset(msg->data,0,sizeof(msg->data));
            return 0;
        }
        if(-1==send(sockfd,msg,sizeof(MSG),0))
        {
            perror("send");
            return -1;
        }
        printf("i=%d size=%d\n",i++,size);
        memset(msg->data,0,sizeof(msg->data));
        usleep(1000);
    }


    close(fd);
    return 0;

}


