#ifndef PLAYER_EVENTOBSERVER_H_
#define PLAYER_EVENTOBSERVER_H_
#include "Common.h"
class  PlayEventObserver
{
public:
	virtual void OnEvent(PlayEvent evt, long data1, void* data2) = 0;
	virtual ~PlayEventObserver()
	{
	}
};
#endif 