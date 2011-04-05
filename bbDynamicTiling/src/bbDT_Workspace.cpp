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

#include "bbDT_Workspace.h"
#include "bbDT_Manager.h"


void Workspace::addClient(Client* client)
{
	if (mColumns.empty())
		addColumn(); // will be focused

	getFocusedColumn()->addClient(client);
}


Column* Workspace::addColumn(Direction dir/* = D_INVALID*/)
{
	Column* newColumn = new Column(this, mRows);
	
	if (dir == D_LEFT)
		mColumns.push_front(newColumn);
	else
		mColumns.push_back(newColumn);
	
	mColumnsFocus.push_back(newColumn);

	return newColumn;
}


int Workspace::removeColumn(Column* column)
{	
	for (auto it = mColumnsFocus.begin(); it != mColumnsFocus.end(); it++)
	{
		if (*it == column)
		{
			mColumnsFocus.erase(it);
			break;
		}
	}

	// erase from vector after deleting
	auto it = std::find(mColumns.begin(), mColumns.end(), column);
	delete column;
	mColumns.erase(it);
	
	return mColumns.size();
}


// all columns must have the same number of container
void Workspace::handleNewRow(Column* column, Direction dir)
{
	for (auto it = mColumns.begin(); it != mColumns.end(); it++)
		if (*it != column && // stop recursive call
			(*it)->getContainerCount() < column->getContainerCount())
		{	
			(*it)->addContainer(dir);
		}

	if (mRowsHeightFactor.size() < column->getContainerCount())
		for (int i = column->getContainerCount() - mRowsHeightFactor.size(); i <= (int)column->getContainerCount(); i++)
			mRowsHeightFactor.push_back(1.f);
	else
		mRowsHeightFactor[column->getContainerCount()] = 1.f;

	update();
}


int Workspace::moveClient(Client* client, Direction dir)
{
	Container* container = client->getParentContainer();	
	int containerNo = container->getElementNumber();
	Column* column = container->getParentColumn();
	int colNo = column->getElementNumber();

	switch (dir)
	{
	case D_LEFT:
		if (column->isFirst())
		{
			if (column->getClientCount() > 1)
			{
				Column* newColumn = addColumn(dir);
				container->removeClient(client);
				newColumn->addClient(client, containerNo);
			}
		}
		else
		{
			container->removeClient(client);
			mColumns[colNo - 1]->addClient(client, containerNo);
		}
		break;

	case D_RIGHT:
		if (column->isLast())
		{
			if (column->getClientCount() > 1)
			{
				Column* newColumn = addColumn(dir);
				container->removeClient(client);
				newColumn->addClient(client, containerNo);
			}
		}
		else
		{
			container->removeClient(client);
			mColumns[colNo + 1]->addClient(client, containerNo);
		}
		break;
	
	default: return -1;
	}
	return column->getClientCount();
}


int Workspace::focus(Client* client, Direction dir)
{
	if (mColumns.size() < 2) return -1;
	Column* column = client->getParentContainer()->getParentColumn();
	int colNo = column->getElementNumber();

	switch (dir)
	{
	case D_LEFT:
		if (column->isFirst())
			setFocusedColumn(mColumns.back());
		else
			setFocusedColumn(mColumns[colNo - 1]);
		break;
	
	case D_RIGHT:
		if (column->isLast())
			setFocusedColumn(mColumns.front());
		else
			setFocusedColumn(mColumns[colNo + 1]);
		break;
	
	default: return -1;
	}

	return 0;
}


int Workspace::expand(Client* client, Direction dir)
{
	Container* container = client->getParentContainer();
	Column* column = container->getParentColumn();
	if (container->isShared() == S_VERTICAL) return -1;
	Container* targetContainer = 0;
	
	switch (dir)
	{
	case D_LEFT:
		if (column->isFirst()) return -1;
		for (auto it = mColumns.begin() + column->getElementNumber() - 1; it != --mColumns.begin(); it--)
		{
			if (std::find(container->getSharedContainer()->begin(),
				container->getSharedContainer()->end(), 
				(*it)->getContainer(container->getElementNumber())) != 
				container->getSharedContainer()->end()) continue;

			targetContainer = (*it)->getContainer(container->getElementNumber());
			break;
		}
		break;

	case D_RIGHT:
		if (column->isLast()) return -1;
		for (auto it = mColumns.begin() + column->getElementNumber() + 1; it != mColumns.end(); it++)
		{
			if (std::find(container->getSharedContainer()->begin(),
				container->getSharedContainer()->end(), 
				(*it)->getContainer(container->getElementNumber())) != 
				container->getSharedContainer()->end()) continue;

			targetContainer = (*it)->getContainer(container->getElementNumber());
			break;
		}
		break;
		
	default: return -1;
	}

	if (!targetContainer) return -1;
	if (container->addSharedContainer(targetContainer) == -1) return -1;	
	update();

	return 0;
}


void Workspace::setFocusedColumn(Column* column)
{
	for (auto it = mColumnsFocus.begin(); it != mColumnsFocus.end(); it++)
	{
		if (*it == column)
		{
			if (*it != mColumnsFocus.back())
			{
				mColumnsFocus.erase(it);
				mColumnsFocus.push_back(column);
			}
			break;
		}
	}
	if (!mColumnsFocus.empty())
		mColumnsFocus.back()->setFocused();
}


int Workspace::getClientCountOnRow(int row)
{
	if (mColumns.empty()) return 0;
	if ((int)mColumns.front()->getContainerCount() <= row || row < 0) return 0;
	
	int i = 0;
	for (auto it = mColumns.begin(); it != mColumns.end(); it++)
		i += (*it)->getContainer(row)->getClientCount();
	
	return i;
}


void Workspace::toggleFullscreen()
{
	mFullscreen = !mFullscreen;
	update();
}


void Workspace::cleanup()
{
	Container* container;

	// remove empty columns
	for (auto it = mColumns.begin(); it != mColumns.end();)
	{
		if ((*it)->getClientCount() == 0)
		{
			removeColumn(*it);
			it = mColumns.begin();
		}
		else ++it;
	}
	
	if (mColumns.empty())
		return;

	/* remove empty rows
	 * all columns are supposed to have the same number of container(row) */
	int rowCount = mColumns.back()->getContainerCount();
	for (int row = 0; row < rowCount; row++)
	{
		auto it = mColumns.begin();
		while (it != mColumns.end())
		{
			container = (*it)->getContainer(row);
			if (container != 0)
				if (container->getClientCount() != 0)
					break;
			it++;
		}

		if (it == mColumns.end())
		{
			// row is empty
			for (it = mColumns.begin(); it != mColumns.end(); it++)
			{
				container = (*it)->getContainer(row);
				if (container != 0)
				{
					(*it)->removeContainer(container);
				}
			}
		}
	}
}


void Workspace::update()
{
	if (mColumns.empty()) return;
	
	HWND hwnd = GetDesktopWindow();
	RECT rect;
	GetMonitorRect(hwnd, &rect, GETMON_FROM_WINDOW | GETMON_WORKAREA);
	mRect = &rect;
	mRows = mColumns.back()->getContainerCount();

	int workspaceBorder = isFullscreen() == true ? 
		mManager->getWorkspaceFullscreenBorderSize() : 
		mManager->getWorkspaceBorderSize();
	int colBorder = isFullscreen() == true ? 0 : mManager->getColumnBorderSize();

	mRect.X1 += workspaceBorder; mRect.X2 -= workspaceBorder;
	mRect.Y1 += workspaceBorder; mRect.Y2 -= workspaceBorder;

	float sumWidthSizeRatio = 0.f;
	for (auto it = mColumns.begin(); it != mColumns.end(); it++)
		sumWidthSizeRatio += (*it)->getWidthRatio();
	
	float baseWidth = mRect.getWidth() / sumWidthSizeRatio;
	float x = mRect.X1;
	float width;


	for (auto it = mColumns.begin(); it != mColumns.end(); it++)
	{
		if (*it == mColumns.back())
			width = mRect.getWidth() - (int)x;
		else
			width = (*it)->getWidthRatio() * baseWidth;

		(*it)->setPosX(x);
		(*it)->setWidth(width - ((*it)->isLast() ? 0 : colBorder));
		x += width;
	}

	for (auto it = mColumns.begin(); it != mColumns.end(); it++)
		(*it)->update();
}


int Workspace::getColumnNumber(Column* column)
{
	int i = 0;
	for (auto it = mColumns.begin(); it != mColumns.end(); it++, i++)
		if (*it == column) return i;
	return 0; // TODO trow an exception
}

