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

#ifndef BBDT_COLUMN_H
#define BBDT_COLUMN_H

#include "bbDT_Common.h"
#include "bbDT_Client.h"
#include "bbDT_Container.h"

class Column
{
public:
	///	constructor
	Column(Workspace* workspace, int row)
	:	mParentWorkspace(workspace), mX(0), mWidth(0), mWidthRatio(1.0f)
	{
		for (int i = 0; i < row; i++)
			addContainer();
	}

	/// destructor
	~Column()
	{
		for (auto it = mContainers.begin(); it != mContainers.end(); it++)
			delete *it;
		mContainers.clear();
		mContainersFocus.clear();
	}
	
	void addClient(Client* client, int element = -1);
	int moveClient(Client* client, Direction dir);
	int focus(Client* client, Direction dir);
	int expand(Client* client, Direction dir);

	int getClientCount();
	void setFocused();

	Container* addContainer(Direction dir = D_INVALID);
	int removeContainer(Container* container);
	
	int getPosX() {return mX;}
	int getWidth() {return mWidth;}
	void setPosX(int pos) {mX = pos;}
	void setWidth(int width) {mWidth = width;}

	float getWidthRatio() {return mWidthRatio;}
	void setWidthRatio(float ratio);
	
	unsigned int getContainerCount() {return mContainers.size();}

	Workspace* getParentWorkspace() {return mParentWorkspace;}
	int getElementNumber();

	Container* getContainer(int pos)
		{return (int)mContainers.size() > pos && pos >= 0 ? mContainers[pos] : 0;}
	int getContainerNumber(Container* container);

	void setFocusedContainer(Container* container);
	
	Container* getFocusedContainer()
		{return !mContainersFocus.empty() ? mContainersFocus.back() : 0;}

	Client* getFocusedClient()
		{return getFocusedContainer() ? getFocusedContainer()->getFocusedClient() : 0;}

	bool isFirst();
	bool isLast();

	void update();

private:
	Workspace* mParentWorkspace;

	int mX;
	int mWidth;
	float mWidthRatio;

	std::deque<Container*> mContainers;
	std::list<Container*> mContainersFocus; // ordered by focus (last first)
};

#endif
