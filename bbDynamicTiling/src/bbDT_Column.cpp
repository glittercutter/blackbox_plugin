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

#include "bbDT_Column.h"
#include "bbDT_Workspace.h"
#include "bbDT_Manager.h"


void Column::addClient(Client* client, int element/* = -1*/)
{
	if (mContainers.empty())
		addContainer(); // will be focused

	if (element == -1)
		getFocusedContainer() ? getFocusedContainer()->addClient(client) :
			mContainers.back()->addClient(client);
	else
		mContainers[element]->addClient(client);
}


/// return '-1' if the client can't be moved inside the column
int Column::moveClient(Client* client, Direction dir)
{
	Container* container = client->getParentContainer();
	int containerNo = container->getElementNumber();

	switch (dir)
	{
	case D_UP:
		if (container->isFirst())
		{
			if (mParentWorkspace->getClientCountOnRow(containerNo) > 1)
			{
				Container* newContainer = addContainer(dir);
				container->removeClient(client);
				newContainer->addClient(client);
			}
			else return -1;
		}
		else
		{
			mContainers[containerNo]->removeClient(client);
			mContainers[containerNo - 1]->addClient(client, D_DOWN);
		}
		break;

	case D_DOWN:
		if (container->isLast())
		{
			if (mParentWorkspace->getClientCountOnRow(containerNo) > 1)
			{
				Container* newContainer = addContainer(dir);
				container->removeClient(client);
				newContainer->addClient(client);
			}
			else return -1;
		}
		else
		{
			mContainers[containerNo]->removeClient(client);
			mContainers[containerNo + 1]->addClient(client, D_UP);
		}
		break;
	
	default: return -1;
	}
	return container->getClientCount();
}


int Column::focus(Client* client, Direction dir)
{
	if (mContainers.size() < 2) return -1;

	switch (dir)
	{
	case D_UP:
		if (client->getParentContainer()->isFirst())
		{
			for (auto it = mContainers.rbegin(); it != mContainers.rend(); it++)
			{
				if ((*it)->getClientCount() != 0)
				{
					setFocusedContainer(*it);
					break;
				}
			}
		}
		else
		{
			int i;
			for (i = client->getParentContainer()->getElementNumber() - 1; i >= 0; i--)
			{
				// search non-empty container
				if (mContainers[i]->getClientCount() != 0)
				{
					setFocusedContainer(mContainers[i]);
					break;
				}
			}
			if (i < 0) // no container found, search from the end
			{
				for (auto it = mContainers.rbegin(); it != mContainers.rend(); it++)
				{
					if ((*it)->getClientCount() != 0)
					{
						setFocusedContainer(*it);
						break;
					}
				}
			}
		}
		break;
	
	case D_DOWN:
		if (client->getParentContainer()->isLast())
		{
			for (auto it = mContainers.begin(); it != mContainers.end(); it++)
			{
				if ((*it)->getClientCount() != 0)
				{
					setFocusedContainer(*it);
					break;
				}
			}
		}
		else
		{
			int i;
			for (i = client->getParentContainer()->getElementNumber() + 1; i < (int)mContainers.size(); i++)
			{
				// search non-empty container
				if (mContainers[i]->getClientCount() != 0)
				{
					setFocusedContainer(mContainers[i]);
					break;
				}
			}
			if (i == (int)mContainers.size()) // no container found, search from the beginning
			{
				for (auto it = mContainers.begin(); it != mContainers.end(); it++)
				{
					if ((*it)->getClientCount() != 0)
					{
						setFocusedContainer(*it);
						break;
					}
				}
			}
		}
		break;
	
	default: return -1;
	}
	return 0;
}


int Column::expand(Client* client, Direction dir)
{
	Container* container = client->getParentContainer();
	if (container->isShared() == S_HORIZONTAL) return -1;
	Container* targetContainer = 0;
	
	switch (dir)
	{
	case D_UP:
		if (container->isFirst()) return -1;
		for (auto it = mContainers.begin() + container->getElementNumber() - 1; it != --mContainers.begin(); it--)
		{
			if (std::find(container->getSharedContainer()->begin(),
				container->getSharedContainer()->end(), *it) != 
				container->getSharedContainer()->end()) continue;

			targetContainer = *it;
			break;
		}
		break;

	case D_DOWN:
		if (container->isLast()) return -1;
		for (auto it = mContainers.begin() + container->getElementNumber() + 1; it != mContainers.end(); it++)
		{
			if (std::find(container->getSharedContainer()->begin(),
				container->getSharedContainer()->end(), *it) != 
				container->getSharedContainer()->end()) continue;
			
			targetContainer = *it;
			break;
		}
		break;
	
	default:
		return -1;
	}
	
	if (!targetContainer) return -1;
	if (container->addSharedContainer(targetContainer) == -1) return -1;	
	update();
	return 0;
}


Container* Column::addContainer(Direction dir/* = D_INVALID*/)
{
	Container* newContainer = new Container(this);
	if (dir == D_UP)
		mContainers.push_front(newContainer);
	else 
		mContainers.push_back(newContainer);
	
	mContainersFocus.push_back(newContainer);	
	mParentWorkspace->handleNewRow(this, dir);
	return newContainer;
}


int Column::removeContainer(Container* container)
{
	if (!container) return mContainers.size();

	for (auto it = mContainersFocus.begin(); it != mContainersFocus.end(); it ++)
	{
		if (*it == container)
		{
			mContainersFocus.erase(it);
			break;
		}
	}

	auto it = std::find(mContainers.begin(), mContainers.end(), container);
	delete container;
	mContainers.erase(it);
	return mContainers.size();
}


int Column::getClientCount()
{
	int j = 0;
	for (auto it = mContainers.begin(); it != mContainers.end(); it++)
		j += (*it)->getClientCount();
	
	return j;
}


void Column::setFocusedContainer(Container* container)
{
	if (container->getClientCount() == 0)
	{	
		// find closest non-empty container
		int containerNo = container->getElementNumber();
		int i = 0, closest = -1;
		for (auto it = mContainers.begin(); it != mContainers.end(); it++, i++)
		{
			if ((*it)->getClientCount() == 0 || *it == container) continue;
			if (abs(i - containerNo) < abs(closest - containerNo) ||
				closest == -1) closest = i;
		}
		if (closest != -1) container = mContainers[closest];
	}

	if (container != mContainersFocus.back())
	{
		auto it = mContainersFocus.begin();
		for (; it != mContainersFocus.end(); it++)
			if (*it == container) break;

		mContainersFocus.erase(it);
		mContainersFocus.push_back(container);
	}
	container->setFocused();
}


void Column::setFocused()
{
	if (!mContainersFocus.empty())
		setFocusedContainer(mContainersFocus.back());
}


void Column::setWidthRatio(float ratio)
{
	float minFactor = mParentWorkspace->getManager()->getMinSizeFactor();
	mWidthRatio = ratio;
	if (mWidthRatio < minFactor)
		mWidthRatio = minFactor;
}


void Column::update()
{
	float y = (float)mParentWorkspace->getRect().Y1;
	float height;
	
	float sumRowHeightRatio = 0.f;
	for (auto it = mContainers.begin(); it != mContainers.end(); it++)
	{
		sumRowHeightRatio += mParentWorkspace->getRowHeightFactor((*it)->getElementNumber());
	}

	float baseRowHeight = mParentWorkspace->getRect().getHeight() / sumRowHeightRatio;

	int border = 
		mParentWorkspace->isFullscreen() == true ?
		0 : mParentWorkspace->getManager()->getContainerBorderSize();

	for (auto it = mContainers.begin(); it != mContainers.end(); it++)
	{	
		if (*it == mContainers.back())
			height = mParentWorkspace->getRect().Y2 - (int)y;
		else
			height = baseRowHeight * mParentWorkspace->getRowHeightFactor((*it)->getElementNumber());

		(*it)->setPosY(y);
		(*it)->setHeight(height - ((*it)->isLast() ? 0 : border));
		y += height;
	}

	for (auto it = mContainers.begin(); it != mContainers.end(); it++)
		(*it)->update();
}

bool Column::isFirst() {return getElementNumber() == 0;}
bool Column::isLast() {return getElementNumber() == mParentWorkspace->getColumnCount() - 1;}

int Column::getContainerNumber(Container* container)
{
	int i = 0;
	for (auto it = mContainers.begin(); it != mContainers.end(); it++, i++)
		if (*it == container) return i;
	return 0; // TODO trow an exception
}

int Column::getElementNumber() {return mParentWorkspace->getColumnNumber(this);}

