#ifndef PLATFORM_THREADEX_H_
#define PLATFORM_THREADEX_H_
#include"base/platform_thread.h"
#include "Log.h"

template<class T>
class CPlatformThreadEx :public rtc::PlatformThread
{
public:
	typedef bool (T::*ThreadFun)();
protected:
	T* m_obj;
	ThreadFun m_fun;
	static bool ThreadRunCallBack(void* arg)
	{
		CPlatformThreadEx<T>* obj = (CPlatformThreadEx<T>*)arg; //???
		if (obj != nullptr)
		{
			return obj->OnThreadRunCallBack();
		}
		return true;
	}
	bool OnThreadRunCallBack()
	{
		if (m_obj == nullptr || m_fun == nullptr)
		{
			LOGE << "obj is null or fun is null "<< m_obj<< " m_fun "<<m_fun;
			return false;
		}
		return (m_obj->*m_fun)();
	}
	
public:
	
	CPlatformThreadEx(T* obj, ThreadFun tfun, const char* threadname) 
		:rtc::PlatformThread(&CPlatformThreadEx::ThreadRunCallBack, this, threadname)
	{
		m_obj = obj;
		m_fun = tfun;
	}
	~CPlatformThreadEx()
	{
		Stop();
	}

};
#endif 