#ifndef BUFFER_QUENE_H_
#define BUFFER_QUENE_H_
#include"base/criticalsection.h"
#include"base/event.h"
#include <list>
typedef void(*pfnFreeBuffer)(void *ptr);

template<typename T, pfnFreeBuffer pfreefun = nullptr>
class CBufferQuene
{
	//static_assert();
protected:
	std::list<T> m_bufflist;
	rtc::CriticalSection m_sect;
	uint32_t m_maxcount = INT32_MAX;
	rtc::Event m_readableevent;
	rtc::Event m_writeableevent;

	bool m_enable = true;
public:
	CBufferQuene() :m_readableevent(false, false),
		m_writeableevent(false, false)
	{
		m_enable = true;
	}
	void SetMaxCount(int count)
	{
		m_maxcount = count;
	}
	bool PushData(T indata, bool block = false)
	{
		if (!m_enable)
		{
			return m_enable;
		}
		bool lpushok = false;
		do 
		{
			m_sect.Enter();
			if (m_bufflist.size() < m_maxcount)
			{
				lpushok = true;
				m_bufflist.push_back(indata);
				m_readableevent.Set();
			}
			m_sect.Leave();
			if (!lpushok && block)
			{
				m_writeableevent.Wait(rtc::Event::kForever);
			}
			else
			{
				break;
			}
		} while (!lpushok && m_enable);
		return lpushok;
	}
	bool PullData(T* outdata, bool block = false)
	{
		if (!m_enable)
		{
			return m_enable;
		}
		bool bgotdata = false;
		do 
		{
			m_sect.Enter();
			if (m_bufflist.size() == 0)
			{
				bgotdata = false;
			}
			else
			{
				bgotdata = true;
				*outdata = m_bufflist.front();
				m_bufflist.pop_front();
				m_writeableevent.Set();
			}
			m_sect.Leave();

			
			if (!bgotdata)
			{
				if (block)
				{
					m_readableevent.Wait(rtc::Event::kForever);
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		} while (!bgotdata && m_enable);
		return bgotdata;
	}
	bool Clear()
	{
		LOGD << "Clear start";

		rtc::CritScope autolock(&m_sect);
		LOGD << "autolock enter";
		if (pfreefun != nullptr)
		{
			//http://blog.csdn.net/markman101/article/details/7172918?spm=5176.100239.blogcont41500.5.TX7Mrg
			typename  std::list<T>::iterator ite = m_bufflist.begin();
			while (ite != m_bufflist.end())
			{
				pfreefun((void*)*ite);
				ite++;
			}
		}
		m_bufflist.clear();
		m_writeableevent.Set();
		LOGD << "Clear end";

		return true;
	}
	int Size()
	{
		rtc::CritScope autolock(&m_sect);
		return m_bufflist.size(); //?????
	}

	void SetEnable(bool bEnable)
	{
		m_enable = bEnable;
		if (!m_enable)
		{
			m_readableevent.Set();
			m_writeableevent.Set();
		}
	}
	~CBufferQuene()
	{
		SetEnable(false);
		Clear();
	}
};
#endif