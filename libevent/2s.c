#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<event.h>
#include<event2/bufferevent.h>

void accept_cb(int fd, short events, void* arg);
void socket_read_cb(struct bufferevent* bev, void* arg);
void event_cb(struct bufferevent *bev, short event, void* arg);
int tcp_server_init(int port, int listen_num);

int main(int argc, char** argv)
{
	int listener = tcp_server_init(9999,10);
	if(listener == -1)
	{
		perror("tcp_server_init error");
		return -1;
	}
	struct event_base* base = event_base_new();
	struct event* ev_listen = event_new(base,listener,EV_READ|EV_PERSIST,accept_cb,base);
	event_add(ev_listen,NULL);

	event_base_dispatch(base);
	event_base_free(base);

	return 0;
}

void accept_cb(int fd, short events , void* arg)
{
	evutil_socket_t sockfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);

	sockfd = accept(fd,(struct sockaddr*)&client,&len);
	evutil_make_socket_nonblocking(sockfd);
	printf("accept a client %d\n",sockfd);

	struct event_base* base = (struct event_base*)arg;
	struct bufferevent* bev = bufferevent_socket_new(base,sockfd,BEV_OPT_CLOSE_ON_FREE);
	bufferevent_setcb(bev,socket_read_cb,NULL,event_cb,arg);

	bufferevent_enable(bev,EV_READ|EV_PERSIST);
}

void socket_read_cb(struct bufferevent* bev, void* arg)
{
	char msg[1024];
	size_t len = bufferevent_read(bev,msg,sizeof(msg));
	msg[len] = '\0';
	printf("recv the client msg:%s",msg);

	char reply_msg[1024] = "i have received the msg: ";
	strcat(reply_msg + strlen(reply_msg),msg);
	bufferevent_write(bev,reply_msg,strlen(reply_msg));
}

void event_cb(struct bufferevent* bev, short event, void* arg)
{
	if(event & BEV_EVENT_EOF)
		printf("connection closed\n");
	else if(event & BEV_EVENT_ERROR)
		printf("some other error\n");

	bufferevent_free(bev);
}

typedef struct sockaddr SA;
int tcp_server_init(int port, int listen_num)
{
	evutil_socket_t listener;
	listener = socket(AF_INET,SOCK_STREAM,0);
	if(listener == -1)
		return -1;
	evutil_make_listen_socket_reuseable(listener);

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = 0;
	sin.sin_port = htons(port);

	bind(listener,(SA*)&sin,sizeof(sin));
	listen(listener,listen_num);

	evutil_make_socket_nonblocking(listener);

	return listener;
}



