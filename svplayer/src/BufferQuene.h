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

	bool m_bWriteEnable = true;
	//bool m_enable = true;
	bool m_bReadEnable = true;
public:
	CBufferQuene() :m_readableevent(false, false),
		m_writeableevent(false, false)
	{
		m_bReadEnable = m_bWriteEnable = true;
	}
	void SetMaxCount(int count)
	{
		m_maxcount = count;
	}
	bool PushData(T indata, bool block = false)
	{
		if (!m_bWriteEnable)
		{
			return m_bWriteEnable;
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
				if (!m_bReadEnable)
				{
					break;
				}
				m_writeableevent.Wait(rtc::Event::kForever);
			}
			else
			{
				break;
			}
		} while (!lpushok && m_bWriteEnable);
		return lpushok;
	}
	bool PullData(T* outdata, bool block = false)
	{
		if (!m_bReadEnable)
		{
			return m_bReadEnable;
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
					if (!m_bWriteEnable)
					{
						break;
					}
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
		} while (!bgotdata && m_bReadEnable);
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
	void SetPushEnable(bool enable)
	{
		m_bWriteEnable = enable;
		if (!m_bWriteEnable)
		{
			m_readableevent.Set();
			m_writeableevent.Set();
		}
	}
	void SetPullEnable(bool enable)
	{
		m_bReadEnable = enable;
		if (!m_bReadEnable)
		{
			m_readableevent.Set();
			m_writeableevent.Set();
		}
	}
	void SetEnable(bool bEnable)
	{
		m_bReadEnable = m_bWriteEnable = bEnable;
		if (!bEnable)
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