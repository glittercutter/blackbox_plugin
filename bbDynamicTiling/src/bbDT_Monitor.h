/*
bbDynamicTiling
Copyright (C) 2011 Sebastien Raymond 

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by 
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see http://www.gnu.org/licenses/.
*/

#ifndef BBDT_MONITOR_H
#define BBDT_MONITOR_H

#include "bbDT_Common.h"

class Monitor
{
public:
	Monitor(HMONITOR hM, int no)
	:	mHMonitor(hM), mFlaggedForDeletion(false), mMonitorNo(no)
	{}

// 	void setRect(Rect& rect) {mRect = rect;}
	Rect& getRect() {return mRect;}

	void setFlaggedForDeletion(bool state) {mFlaggedForDeletion = state;}
	bool isFlaggedForDeletion() {return mFlaggedForDeletion;}

	int getMonitorNo() {return mMonitorNo;}

	void update()
	{
		RECT r;
		GetMonitorRect(mHMonitor, &r, GETMON_FROM_MONITOR | GETMON_WORKAREA);
		mRect = &r;
	}

private:
	Rect mRect;
	HMONITOR mHMonitor;
	bool mFlaggedForDeletion;
	int mMonitorNo;
};

#endif
