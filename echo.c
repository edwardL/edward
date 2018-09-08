#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<sys/types.h>

#include<sys/resource.h>
#include<signal.h>
#include<fcntl.h>

#define bool int
#define false 0
#define true 1
#define IPADDRESS "127.0.0.1"
#define PORT 1883
#define MAXSIZE 1024
#define LISTENQ 512
#define FDSIZE 1024
#define EPOLLEVENTS 60000
#define MAXCONN 60000

// create socket
static int socket_bind(const char* ip , int port);
static void do_epoll(int listenfd);
static void handle_events(int epollfd,struct epoll_event *events, int num, int listenfd, char* buf);
// chuli jieshou dao de lianjie 
static void handle_accept(int epollfd, int listenfd);
static void do_read(int epollfd, int fd, char *buf);
static void do_write(int epollfd, int fd, char *buf);
static void add_event(int epollfd, int fd, int state);
static void modify_event(int epollfd, int fd, int state);
static void delete_event(int epollfd, int fd, int state);

void init_signal(void)
{
	signal(SIGCHLD, SIG_DFL);
	signal(SIGPIPE, SIG_IGN);
}

int set_fdlimit()
{
	struct rlimit rt;
	rt.rlim_max = rt.rlim_cur = MAXCONN;
	if(setrlimit(RLIMIT_NOFILE,&rt) == -1)
	{
		perror("setrlimit error");
		return -1;
	}
	return 0;
}

void daemon_run_method1()
{
	int pid;
	signal(SIGCHLD,SIG_IGN);
	pid = fork();
	if(pid < 0)
	{
		exit(-1);
	}
	else if(pid > 0)
	{
		exit(0);
	}
	setsid();
	int fd;
	fd = open("/dev/null",O_RDWR,0);
	if(fd != -1)
	{
		dup2(fd,STDIN_FILENO);
		dup2(fd,STDOUT_FILENO);
		dup2(fd,STDERR_FILENO);
	}
	if(fd > 2)
	{
		close(fd);
	}
}

bool daemon_run_method2()
{
	pid_t pid = fork();
	if(pid < 0)
	{
		return false;
	}
	else if(pid > 0)
	{
		exit(0);
	}
	/// set file mask
	umask(0);
	pid_t sid = setsid();
	if(sid < 0)
		return false;
	// change working dir
	if((chdir("/")) < 0)
	{
		return false;
	}
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);

	open("/dev/null",O_RDONLY);
	open("/dev/null",O_RDWR);
	open("/dev/null",O_RDWR);
	return true;
}

int main(int argc, char* argv[])
{
	// shezhi xinhao chuli 
	init_signal();

	if(set_fdlimit() < 0)
	{
		return -1;
	}
	bool bdeamon = false;
	if(bdeamon)
	{
		daemon_run_method1();
	}
	int listenfd;
	listenfd = socket_bind(IPADDRESS, PORT);
	listen(listenfd, LISTENQ);
	do_epoll(listenfd);
	return 0;
}

static int socket_bind(const char* ip , int port)
{
	int listenfd;
	struct sockaddr_in servaddr;
	listenfd = socket(AF_INET,SOCK_STREAM,0);
	if(listenfd == -1)
	{
		perror("socket error:");
		exit(1);
	}
	int reuse_addr = 1;
	if(setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&reuse_addr,sizeof(reuse_addr)) == -1)
	{
		return -1;
	}
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1)
	{
		perror("bind error\n");
		exit(1);
	}
	printf("listen on : %d, listenfd = %d\n",PORT, listenfd);
	return listenfd;
}

static void do_epoll(int listenfd)
{
	int epollfd;
	struct epoll_event events[EPOLLEVENTS];
	int ret;
	char buf[MAXSIZE];
	memset(buf,0,MAXSIZE);
	epollfd = epoll_create(FDSIZE);
	add_event(epollfd,listenfd,EPOLLIN);
	for(;;)
	{
		ret = epoll_wait(epollfd, events,EPOLLEVENTS,-1);
		handle_events(epollfd,events,ret,listenfd,buf);
	}
	close(epollfd);
}

static void handle_events(int epollfd, struct epoll_event *events, int num, int listenfd, char* buf)
{
	int i;
	int fd;
	for(i = 0; i < num; i++)
	{
		fd = events[i].data.fd;
		if((fd == listenfd) && (events[i].events & EPOLLIN))
			handle_accept(epollfd,listenfd);
		else if(events[i].events & EPOLLIN)
			do_read(epollfd,fd,buf);
		else if(events[i].events & EPOLLOUT)
			do_write(epollfd,fd,buf);
	}
}

static void handle_accept(int epollfd, int listenfd)
{
	int clifd;
	struct sockaddr_in cliaddr;
	socklen_t cliaddrlen = sizeof(cliaddr);
	clifd = accept(listenfd,(struct sockaddr*)&cliaddr,&cliaddrlen);
	if(clifd == -1)
		perror("accept error:");
	else
	{
		printf("accept a new client:%s:%d,fd=%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port,clifd);
		add_event(epollfd,clifd, EPOLLIN);
	}
}

static void do_read(int epollfd, int fd, char *buf)
{
	int nread;
	nread = read(fd,buf,MAXSIZE);
	if(nread == -1)
	{
		perror("read error:");
		close(fd);
		delete_event(epollfd,fd,EPOLLIN);
	}
	else if(nread == 0)
	{
		fprintf(stderr,"client close, fd = %d\n",fd);
		close(fd);
		delete_event(epollfd,fd,EPOLLIN);
	}
	else
	{
		printf("read message is %s,fd = %d\n",buf,fd);
		/// modify desc fd to writable
		modify_event(epollfd,fd,EPOLLOUT);
	}
}

static void do_write(int epollfd, int fd, char *buf)
{
	int nwrite;
	nwrite = write(fd,buf,strlen(buf));
	if(nwrite == -1)
	{
		perror("write error:");
		close(fd);
		delete_event(epollfd,fd,EPOLLOUT);
	}
	else
		modify_event(epollfd,fd,EPOLLIN);
	memset(buf,0,MAXSIZE);
}

static void add_event(int epollfd, int fd , int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&ev);
}

static void delete_event(int epollfd, int fd, int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epollfd,EPOLL_CTL_DEL,fd,&ev);
}

static void modify_event(int epollfd,int fd, int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = fd;
	epoll_ctl(epollfd,EPOLL_CTL_MOD,fd,&ev);
}




