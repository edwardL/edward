#include<iostream>
#include "tlvprotocol.h"
#include<WinSock.h>
#include "include/mysql.h"
#pragma  comment(lib,"libmysql.lib")
using namespace std;
#include "rmilogin.h"
#include "pointer_util.h"
#include "timerquest.h"
using namespace rmi;

void func()
{
	cout<<"hello in fuc"<<endl;
}

int main()
{
	cout<<"hello world";

	TLVSerializer s;
	char buff[256];
	s.Reset(buff,256);
	bool ret = s.Push(1) && s.Push(2) && s.Push(3.12f);
	
	
	TLVUnserializer un;
	un.Reset(s.Ptr(),10);
/*	int a,b;
	un.Pop(&a);
	un.Pop(&b);
// 	char s[100];
// 	un.Pop(s);
	
	float f;
	un.Pop(&f);
	printf("%d--------\n",a);
	printf("\n%d\n",b);
	printf("%f\n",f);
	printf("\n%s\n",buff);
*/

	RMILoginObject* login = new RMILoginObject();
	TLVSerializer tmp;
	TLVUnserializer un2;
	login->__dispatch(login->__get_obj_id(),un,tmp);


//	RMILoginObject* login_array = new RMILoginObject[10000000];
	//SAFE_DELETE(login);
	//SAFE_DELETE_ARRAY(login_array);
//	delete login;
//	delete[] login_array;
	//AccessMySQL* tmp = new AccessMySQL();
	char* msg;
	//tmp->ConnMySQL("192.168.4.225",3306,"testdbname","root","edward","utf8",&msg);

	TimerQuest* quest = new TimerQuest();
	
	quest->AddDelayQuest(func,5);
	int i = 0;
	while(i < 10)
	{
		i++;
		quest->Update(i,0);
	}
	delete quest;
	return 0;
}