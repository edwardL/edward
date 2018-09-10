#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<errno.h>
#include<signal.h>
#include<sys/resource.h>
#include<fcntl.h>

#define CONFIG_MIN_RESERVED_FDS 32
#define CONFIG_FDSET_INCR (CONFIG_MIN_RESERVED_FDS + 96)

static const char* request = "GET /msg_server HTTP/1.1\r\nConnection: keep-alive\r\n\r\n";
int stop = 0;

int setnonblocking(int fd)
{
	int old_option = fcntl(fd,F_GETFL);
	int new_option = old_option | O_NONBLOCK;
	fcntl(fd,F_SETFL,new_option);
	return old_option;
}

void addfd(int epollfd, int fd)
{
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLOUT | EPOLLERR;
	epoll_ctl(epollfd,EPOLL_CTL_ADD,fd,&event);
	setnonblocking(fd);
}

bool write_nbytes(int sockfd, const char* buffer, int len)
{
	int bytes_write = 0;
	printf("write out %d bytes to socket %d\n",len,sockfd);
	while(1)
	{
		bytes_write = send(sockfd,buffer,len,0);
		if(bytes_write == -1)
			return false;
		else if(bytes_write == 0)
			return false;
		
		len -= bytes_write;
		buffer = buffer + bytes_write;
		if(len <= 0)
			return true;
	}
}

bool read_once(int sockfd, char* buffer, int len)
{
	int bytes_read = 0;
	memset(buffer,'\0',len);
	bytes_read = recv(sockfd,buffer,len,0);
	if(bytes_read == -1)
	{
		return false;
	}
	else if(bytes_read == 0)
	{
		return false;
	}
	printf("read in %d bytes from socket %d with content: %s\n",bytes_read,sockfd,buffer);
	return true;
}

void start_conn(int epoll_fd, int num, const char* ip , int port, int space)
{
	if(num <= 0 || port <= 0 || space <= 0)
	{
		exit(0);
	}

	struct sockaddr_in address;
	bzero(&address,sizeof(address));
	address.sin_family = AF_INET;
	inet_pton(AF_INET,ip,&address.sin_addr);
	address.sin_port = htons(port);

	for(int i = 0; i < num ; ++i)
	{
		if((i % space) == 0)
		{
			 sleep(1); // 1s
		}
	
		int sockfd = socket(PF_INET, SOCK_STREAM,0);
		if(sockfd < 0)
		{
			continue;
		}
		if( connect(sockfd, (struct sockaddr*)&address, sizeof(address)))
		{
			printf("build connection %d\n",i);
			addfd(epoll_fd,sockfd);
		}
		else
		{
			printf("create failed\n");
		}
	}
}

void close_conn(int epollfd, int sockfd)
{
	epoll_ctl(epollfd, EPOLL_CTL_DEL, sockfd,0);
	close(sockfd);
}

void daemonize(void)
{
	int fd;
	if(fork() != 0)exit(0);
	setsid(); // create a new session
	if((fd = open("/dev/null",O_RDWR)) != -1){
		dup2(fd,STDIN_FILENO);
		dup2(fd,STDOUT_FILENO);
		dup2(fd,STDERR_FILENO);
		if(fd > STDERR_FILENO) close(fd);
	}
}

void adjustOpenFilesLimit(int maxclients) { //come from /redis/server.c/adjustOpenFilesLimit()
    rlim_t maxfiles = maxclients+CONFIG_MIN_RESERVED_FDS;
    struct rlimit limit;
 
    if (getrlimit(RLIMIT_NOFILE,&limit) == -1) {
        printf("Unable to obtain the current NOFILE limit (%s), assuming 1024 and setting the max clients configuration accordingly.",
               strerror(errno));
        maxclients = 1024-CONFIG_MIN_RESERVED_FDS;
    } else {
        rlim_t oldlimit = limit.rlim_cur;
 
        /* Set the max number of files if the current limit is not enough
         * for our needs. */
        if (oldlimit < maxfiles) {
            rlim_t bestlimit;
            int setrlimit_error = 0;
 
            /* Try to set the file limit to match 'maxfiles' or at least
             * to the higher value supported less than maxfiles. */
            bestlimit = maxfiles;
            while(bestlimit > oldlimit) {
                rlim_t decr_step = 16;
 
                limit.rlim_cur = bestlimit;
                limit.rlim_max = bestlimit;
                if (setrlimit(RLIMIT_NOFILE,&limit) != -1) break;
                setrlimit_error = errno;
 
                /* We failed to set file limit to 'bestlimit'. Try with a
                 * smaller limit decrementing by a few FDs per iteration. */
                if (bestlimit < decr_step) break;
                bestlimit -= decr_step;
            }
            /* Assume that the limit we get initially is still valid if
             * our last try was even lower. */
            if (bestlimit < oldlimit) bestlimit = oldlimit;
            if (bestlimit < maxfiles) {
                unsigned int old_maxclients = maxclients;
                maxclients = bestlimit-CONFIG_MIN_RESERVED_FDS;
                /* maxclients is unsigned so may overflow: in order
                 * to check if maxclients is now logically less than 1
                 * we test indirectly via bestlimit. */
                if (bestlimit <= CONFIG_MIN_RESERVED_FDS) {
                    printf("Your current 'ulimit -n' "
                           "of %llu is not enough for the server to start. "
                           "Please increase your open file limit to at least "
                           "%llu. Exiting.",
                           (unsigned long long) oldlimit,
                           (unsigned long long) maxfiles);
                    exit(1);
                }
                printf("You requested maxclients of %d "
                       "requiring at least %llu max file descriptors.",
                       old_maxclients,
                       (unsigned long long) maxfiles);
                printf("Server can't set maximum open files "
                       "to %llu because of OS error: %s.",
                       (unsigned long long) maxfiles, strerror(setrlimit_error));
                printf("Current maximum open files is %llu. "
                       "maxclients has been reduced to %d to compensate for "
                       "low ulimit. "
                       "If you need higher maxclients increase 'ulimit -n'.",
                       (unsigned long long) bestlimit, maxclients);
            } else {
                printf("Increased maximum number of open files "
                       "to %llu (it was originally set to %llu).",
                       (unsigned long long) maxfiles,
                       (unsigned long long) oldlimit);
            }
        }
    }
}

void signal_exit_func(int signo)
{
	printf("exit sig is %d\n",signo);
	stop = 1;
}

void signal_exit_handler()
{
	struct sigaction sa;
	memset(&sa,0,sizeof(sa));
	sa.sa_handler = signal_exit_func;
	sigaction(SIGINT,&sa,NULL);
	sigaction(SIGTERM,&sa,NULL);
	sigaction(SIGQUIT,&sa,NULL);
	
	sigaction(SIGKILL,&sa,NULL);
	sigaction(SIGSTOP,&sa,NULL);
}

int main(int argc, char* argv[])
{
	signal(SIGPIPE,SIG_IGN);
	signal_exit_handler();

	int background = 0;
	if(background)
	{
		daemonize();
	}
	
	assert(argc == 5);
	int maxclients = atoi(argv[3])  + CONFIG_FDSET_INCR;
	adjustOpenFilesLimit(maxclients);

	int epoll_fd = epoll_create(1024);
	start_conn(epoll_fd,atoi(argv[3]), argv[1],atoi(argv[2]),atoi(argv[4]));
	epoll_event *events = (epoll_event*)malloc(sizeof(struct epoll_event) * (maxclients) );

	char buffer[2048];
	while(!stop)
	{
		int fds = epoll_wait(epoll_fd,events,maxclients,2000);
		for(int i = 0; i < fds; i++)
		{
			int sockfd = events[i].data.fd;
			if(events[i].events & EPOLLIN)
			{
				if(!read_once(sockfd, buffer,2048))
				{
					close_conn(epoll_fd,sockfd);
				}
				struct epoll_event event;
				event.events = EPOLLOUT | EPOLLET | EPOLLERR;
				event.data.fd = sockfd;
				epoll_ctl(epoll_fd,EPOLL_CTL_MOD,sockfd,&event);
			}
			else if(events[i].events & EPOLLOUT)
			{
				if(!write_nbytes(sockfd,request ,strlen(request)))
				{
					close_conn(epoll_fd,sockfd);
				}
				struct epoll_event event;
				event.events = EPOLLIN | EPOLLERR;
				event.data.fd = sockfd;
				epoll_ctl(epoll_fd,EPOLL_CTL_MOD,sockfd,&event);
			}
			else if(events[i].events & EPOLLERR)
			{
				close_conn(epoll_fd,sockfd);
			}
		}
	}

	close(epoll_fd);
	if(events)
	{
		free(events);
	}
	printf("exit!\n");
}
