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

#include "bbDT_Client.h"
#include "bbDT_Column.h"
#include "bbDT_Container.h"
#include "bbDT_Workspace.h"

#define WINDOW_BORDER 1


void Client::setRect(const Rect& rect)
{
	if (mRect == rect) return;
	mRect = rect;
	mNeedUpdate = true;
}


void Client::update()
{
	if (!mParentContainer->getParentColumn()->getParentWorkspace()->isCurrent()) return;
	if (!mNeedUpdate) return;
	int border = 
		mParentContainer->getParentColumn()->getParentWorkspace()->isFullscreen() ==
		true ? 0 : WINDOW_BORDER;

	SetWindowPos(
		mHwnd, NULL,
		mRect.X1 + border, mRect.Y1 + border, 
		mRect.getWidth() - (border*2), mRect.getHeight() - (border*2), 
		SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);

	mNeedUpdate = false;
}


void Client::setFocused()
{
	if (!mParentContainer->getParentColumn()->getParentWorkspace()->isCurrent()) return;
	SetForegroundWindow(mHwnd);
}

bool Client::isFirst() {return getElementNumber() == 0;}
bool Client::isLast() {return getElementNumber() == mParentContainer->getClientCount() - 1;}

int Client::getElementNumber() {return mParentContainer->getClientNumber(this);}

