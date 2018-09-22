#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <iostream>
#include <string.h>

using namespace std;

#define MAX_EVENTS 500
struct myevent_s
{
    int fd;
    void (*call_back)(int fd, int events, void* arg);
    int events;
    void *arg;
    int status;  // 1: in epoll wait list , 0 not in
    char buff[128];  // recv data buff
    int len;
    long last_active;
};

void EventSet(myevent_s *ev, int fd, void(*call_back)(int,int,void*),void* arg)
{
    ev->fd = fd;
    ev->call_back = call_back;
    ev->events = 0;
    ev->arg = arg;
    ev->status = 0;
    ev->last_active = time(NULL);
}

void EventAdd(int epollfd, int events , myevent_s *ev)
{
    struct epoll_event evp = {0,{0}};
    int op;
    evp.data.ptr = ev;
    evp.events = ev->events = events;
    if(ev->status == 1){
        op = EPOLL_CTL_MOD;
    }else{
        op = EPOLL_CTL_ADD;
        ev->status = 1;
    }
    if(epoll_ctl(epollfd,op,ev->fd,&evp) < 0)
        printf("Event Add Failed[fd=%d]\n",ev->fd);
    else
        printf("Event Add Ok[fd=%d]\n",ev->fd);
}

void EventDel(int epollfd, myevent_s *ev)
{
    struct epoll_event evp = {0,{0}};
    if(ev->status != 1) return;
    evp.data.ptr = ev;
    ev->status = 0;
    epoll_ctl(epollfd,EPOLL_CTL_DEL,ev->fd,&evp);
}

int g_epollfd;
myevent_s g_Events[MAX_EVENTS + 1];
void RecvData(int fd, int events, void *arg);
void SendData(int fd, int events, void *arg);

void AcceptConn(int fd, int events , void *arg)
{
    struct sockaddr_in sin;
    socklen_t len = sizeof(struct sockaddr_in);
    int nfd, i;
    if((nfd = accept(fd,(struct sockaddr*)&sin,&len)) == -1)
    {
        if(errno != EAGAIN && errno != EINTR)
        {
            printf("%s: bad accept",__func__);
        }
        return;
    }
    do
    {
        for(i = 0; i < MAX_EVENTS; i++)
        {
            if(g_Events[i].status == 0)
            {
                break;
            }
        }
        if(i == MAX_EVENTS)
        {
            printf("%s:max connection limit[%d].",__func__,MAX_EVENTS);
            break;
        }
        if(fcntl(nfd,F_SETFL,O_NONBLOCK) < 0) break;
        EventSet(&g_Events[i],nfd,RecvData,&g_Events[i]);
        EventAdd(g_epollfd,EPOLLIN | EPOLLET, &g_Events[i]);
        printf("new conn[%s:%d][time:%d]\n",inet_ntoa(sin.sin_addr),ntohs(sin.sin_port),g_Events[i].last_active);
    }while(0);
}

void RecvData(int fd, int events, void *arg)
{
    struct myevent_s *ev = (struct myevent_s*) arg;
    int len;
    len = recv(fd,ev->buff,sizeof(ev->buff) - 1, 0);
    EventDel(g_epollfd,ev);
    if(len > 0)
    {
        ev->len = len;
        ev->buff[len] = '\0';
        printf("C[%d]:%s\n",fd,ev->buff);
        EventSet(ev,fd, SendData,ev);
        EventAdd(g_epollfd,EPOLLOUT | EPOLLET , ev);
    }
    else if(len == 0)
    {
        close(ev->fd);
        printf("[fd=%d] closed gracefully./n", fd);
    }
    else
    {
        close(ev->fd);
        printf("recv[fd=%d] error[%d]:%s/n", fd, errno, strerror(errno));
    }
}

void SendData(int fd,  int events , void *arg)
{
    struct myevent_s *ev = (struct myevent_s*)arg;
    int len;
    len = send(fd,ev->buff,ev->len,0);
    ev->len = 0;
    EventDel(g_epollfd,ev);
    if(len > 0)
    {
        EventSet(ev,fd,RecvData,ev);
        EventAdd(g_epollfd,EPOLLIN|EPOLLET,ev);
    }
    else
    {
        close(ev->fd);
        printf("recv[fd=%d] error[%d]\n",fd,errno);
    }
}

void InitListenSocket(int epollfd, short port)
{
    int listenfd = socket(AF_INET,SOCK_STREAM,0);
    fcntl(listenfd,F_SETFL,O_NONBLOCK);
    printf("server listen fd=%d\n",listenfd);
    EventSet(&g_Events[MAX_EVENTS],listenfd,AcceptConn,&g_Events[MAX_EVENTS]);
    EventAdd(epollfd,EPOLLIN | EPOLLET, &g_Events[MAX_EVENTS]);
    // bind and listen
    struct sockaddr_in sin;
    bzero(&sin,sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);
    bind(listenfd,(const sockaddr*)&sin,sizeof(sin));
    listen(listenfd, 5);
}

int main(int argc, char **argv)
{
    short port = 12345;
    if(argc == 2){
        port = atoi(argv[1]);
    }
    g_epollfd = epoll_create(MAX_EVENTS);
    if(g_epollfd < 0) printf("create epollfd failed.%d\n",g_epollfd);
    InitListenSocket(g_epollfd,port);

    struct epoll_event events[MAX_EVENTS];
    printf("server running on port : %d\n",port);
    int checkPos = 0;
    while(1) {
        long now = time(NULL);
        for(int i = 0; i < 100; i++, checkPos++)
        {
            if(checkPos == MAX_EVENTS) checkPos = 0;
            if(g_Events[checkPos].status != 1) continue;
            long duration = now - g_Events[checkPos].last_active;
            if(duration >= 60)
            {
                close(g_Events[checkPos].fd);  
                printf("[fd=%d] timeout[%d--%d]./n", g_Events[checkPos].fd, g_Events[checkPos].last_active, now);  
                EventDel(g_epollfd, &g_Events[checkPos]);                
            }
        }
        int fds = epoll_wait(g_epollfd,events,MAX_EVENTS,1000);
        if(fds < 0) {
            printf("epoll_wait error , exit");
            break;
        }
        for(int i = 0; i < fds; i++)
        {
            myevent_s *ev = (struct myevent_s*)events[i].data.ptr;
            if((events[i].events & EPOLLIN) && (ev->events& EPOLLIN))
            {
                close(g_Events[checkPos].fd);  
                printf("[fd=%d] timeout[%d--%d]./n", g_Events[checkPos].fd, g_Events[checkPos].last_active, now);  
                EventDel(g_epollfd, &g_Events[checkPos]); (ev->fd,events[i].events,ev->arg);
            }
            if((events[i].events & EPOLLOUT) && (ev->events & EPOLLOUT))
            {
                ev->call_back(ev->fd,events[i].events,ev->arg);
            }
        }
    }
}


