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

#ifndef BBDT_CONTAINER_H
#define BBDT_CONTAINER_H

#include "bbDT_Common.h"
#include "bbDT_Client.h"

enum Shared
{
	S_INVALID = 0,
	S_HORIZONTAL,
	S_VERTICAL
};


class Container
{
public:
	///	constructor
	Container(Column* col)
	:	mParentColumn(col), mSharedBy(0) {}
	
	/// destructor
	~Container()
	{
		clearSharedContainer();
		if (mSharedBy) mSharedBy->removeShare(this);
	}

	void addClient(Client* client, Direction = D_INVALID);
	int removeClient(Client* client);
	int moveClient(Client* client, Direction dir);
	int focus(Client* client, Direction dir);

	int getClientCount() {return mClients.size();}
	void setFocusedClient(Client* client);
	void setFocused();
	
	int requestSharing(Container* container);
	int releaseSharing(Container* container);
	void removeShare(Container* container);
	void clearSharedContainer();
	std::list<Container*>* getSharedContainer() {return &mSharedContainers;}
	int addSharedContainer(Container* container);

	Client* getFocusedClient() {return !mClientsFocus.empty() ? mClientsFocus.back() : 0;}
	std::deque<Client*>* getClients() {return &mClients;}

	int getPosY() {return mY;}
	int getHeight() {return mHeight;}
	void setPosY(int pos) {mY = pos;}
	void setHeight(int height) {mHeight = height;}
	void getSharedDimension(Rect* rect);

	Column* getParentColumn() {return mParentColumn;}
	int getElementNumber();
	int getClientNumber(Client* client);

	bool isFirst();
	bool isLast();
	Shared isShared();	

	void update();

private:
	Column* mParentColumn;
	
	int mY;
	int mHeight;
	
	// containers can expand in empty ones
	std::list<Container*> mSharedContainers;
	Container* mSharedBy;

	std::deque<Client*> mClients;
	std::list<Client*> mClientsFocus; // ordered by focus (last first)
};

#endif
