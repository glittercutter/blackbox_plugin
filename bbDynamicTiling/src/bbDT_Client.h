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
	:	mHwnd(hwnd), mHeightFactor(1.0f) {}	
	
	HWND getWindow() {return mHwnd;}
	Container* getParentContainer() {return mParentContainer;}
	void setParentContainer(Container* container) {mParentContainer = container;}

	const Rect* getRect() {return &mRect;}
	void setRect(const Rect& rect);

	float getHeightFactor() {return mHeightFactor;}

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
	bool mNeedUpdate;
};

#endif
