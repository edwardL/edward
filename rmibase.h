#ifndef RMIBASE_H
#define RMIBASE_H

#include <map>
#include <string>
#include "tlvprotocol.h"


namespace rmi
{
	enum ERMIDispatchStatus
	{
		DispatchOK,
		DispatchObjectNotExist,
		DispatchMethodNotExist,
		DispatchParamError,
		DispatchOutParamBuffTooShort,
		SessionDisconnect,
	};

	////����˵��ö���
	class RMIObject
	{
	public :
		typedef int(*RMIMethod)(TLVUnserializer &in_param,TLVSerializer &out_param);
		RMIObject(){};
		virtual ~RMIObject(){};

		int __dispatch(const char* method, TLVUnserializer &in_param, TLVSerializer &out_param);

		virtual const char* __get_obj_id() = 0;
	protected:
		typedef std::map<std::string,RMIMethod> MethodList;
		MethodList m_method_list;
	};

}


#endif