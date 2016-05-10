#ifndef RMILOGIN_H
#define RMILOGIN_H
#include "dalogin.h"
#include "rmibase.h"
#include<iostream>
using namespace std;

class RMILoginObject : public rmi::RMIObject
{
public:
	RMILoginObject()
	{
		m_method_list[dataaccessrmi::login::USER_LOGIN] = UserLogin;
	}
	virtual ~RMILoginObject()
	{

	}

	virtual void __free()
	{
		delete this;
	}
	virtual const char* __get_obj_id()
	{
		return dataaccessrmi::login::USER_LOGIN;
	}

	int __dispatch(const char* method, TLVUnserializer &in_param, TLVSerializer &out_param)
	{
		if(rmi::DispatchOK == (*m_method_list[method])(in_param,out_param))
		{
			return 0;
		}else
		{
		return -1;
		}	
	}

	static int UserLogin(TLVUnserializer &in_param,TLVSerializer &out_param)
	{
		int a,b;
		float c;
		in_param.Pop(&a);
		in_param.Pop(&b);
		in_param.Pop(&c);
		std::cout<<std::endl;
		std::cout <<a<<":"<<b<<":"<<c<< std::endl;

	////	out_param.Push(3.12f) && out_param.Push(2) && out_param.Push(222);
		return rmi::DispatchOK;
	}
};

#endif