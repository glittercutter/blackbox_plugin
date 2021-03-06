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
#include "bbDT_Manager.h"

void Client::setRect(const Rect& rect)
{
    if (mRect == rect) return;
    mRect = rect;
}

void Client::update()
{
    if (!mParentContainer->getParentColumn()->getParentWorkspace()->isCurrent()) return;
    int border = mParentContainer->getParentColumn()->getParentWorkspace()->isFullscreen() == true ? 0 :
        mParentContainer->getParentColumn()->getParentWorkspace()->getManager()->getClientBorderSize();

    SetWindowPos(
        mHwnd, NULL,
        mRect.X1, mRect.Y1,
        mRect.getWidth(), mRect.getHeight() - (isLast() ? 0 : border), 
        SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOSENDCHANGING);
}

void Client::setFocused()
{
    if (!mParentContainer->getParentColumn()->getParentWorkspace()->isCurrent()) return;
    SetForegroundWindow(mHwnd);
}

bool Client::isFirst() {return getElementNumber() == 0;}
bool Client::isLast() {return getElementNumber() == mParentContainer->getClientCount() - 1;}

int Client::getElementNumber() {return mParentContainer->getClientNumber(this);}
