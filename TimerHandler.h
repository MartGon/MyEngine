#pragma once

class TimerHandler
{
public:
	virtual void onTimerFinish(void* param) = 0;
};