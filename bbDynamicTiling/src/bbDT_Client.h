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

#ifndef BBDT_CLIENT_H
#define BBDT_CLIENT_H

#include "bbDT_Common.h"


class Client
{
public:
	// constructor
	Client(HWND hwnd)
	:	mParentContainer(0), mHwnd(hwnd)
	{
		// get floating rect
		RECT rect;
		if (GetWindowRect(hwnd, &rect))
			mFloatingRect = &rect;
	}
	// destructor
	~Client()
	{
		// restore floating rect
		SetWindowPos(
			mHwnd, NULL,
			mFloatingRect.X1, mFloatingRect.Y1, 
			mFloatingRect.getWidth(), mFloatingRect.getHeight(), 
			SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
	}
	
	HWND getWindow() {return mHwnd;}
	Container* getParentContainer() {return mParentContainer;}
	void setParentContainer(Container* container) {mParentContainer = container;}

	const Rect* getRect() {return &mRect;}
	void setRect(const Rect& rect);

	int getElementNumber();

	void drawBorder();
	void setFocused();

	bool isFirst();
	bool isLast();

	void update();

private:
	Container* mParentContainer;
	HWND mHwnd;
	Rect mRect;
	Rect mFloatingRect;
};

#endif
