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

#include "bbDT_Manager.h"

#include "bbDT_Column.h"
#include "bbDT_Workspace.h"


void TilingManager::move(Direction dir, Target target)
{
	if (target == T_CLIENT)
		moveClient(getFocusedClient(), dir);
	else
	{
		Client* client = getFocusedClient();
		if (client)
			moveContainer(client->getParentContainer(), dir);
	}
}


void TilingManager::focus(Direction dir, Target target)
{
	Client* client = getFocusedClient();
	if (!client) return;
	if (dir == D_INVALID) return;

	Container* container = client->getParentContainer();
	Column* column = container->getParentColumn();
	Workspace* workspace = column->getParentWorkspace();
	
	int r = 0;
	if (target == T_CLIENT)
		r = container->focus(client, dir);

	if (target == T_CONTAINER || r == -1)
	{
		r = column->focus(client, dir);
		if (r == -1)
			r = workspace->focus(client, dir);
	}
}


void TilingManager::focus(HWND hwnd)
{
	Client* client = getClient(hwnd);
	if (client)
		setFocusedClient(client);
}


void TilingManager::expand(Direction dir)
{
	Client* client = getFocusedClient();
	Column* column = client->getParentContainer()->getParentColumn();
	Workspace* workspace = column->getParentWorkspace();
	
	if (!client) return;
	if (dir == D_INVALID) return;
	
	if (column->expand(client, dir) == -1)
		workspace->expand(client, dir);
}


void TilingManager::resize(Direction dir)
{
	Client* client = getFocusedClient();
	Container* container = client->getParentContainer();
	Column* column = container->getParentColumn();
	Workspace* workspace = column->getParentWorkspace();
	
	if (!client) return;
	if (dir == D_INVALID) return;
	
	switch (dir)
	{
	case D_UP:
		workspace->resizeRowHeightFactor(container->getElementNumber(), mRCSetting->resizeFactor);
		break;

	case D_RIGHT:
		column->setWidthRatio(column->getWidthRatio() + mRCSetting->resizeFactor);
		break;

	case D_DOWN:
		workspace->resizeRowHeightFactor(container->getElementNumber(), -mRCSetting->resizeFactor);
		break;

	case D_LEFT:
		column->setWidthRatio(column->getWidthRatio() - mRCSetting->resizeFactor);
		break;
	
	default: return;
	}

	workspace->update();
}


void TilingManager::toggleFullscreen()
{
	if (mCurrentWorkspace)
		mCurrentWorkspace->toggleFullscreen();
}


void TilingManager::toggleFloating()
{
	HWND hwnd = GetTask(GetActiveTask());
	if (getClient(hwnd))
		removeWindow(hwnd);
	else
		addWindow(hwnd, true);
}


void TilingManager::addWindow(HWND hwnd, bool ignoreList/* = false*/)
{
	if (!ignoreList) if (!checkInclusionList(hwnd)) return;
	
	// check if we have this window already
	if (mClients.find(hwnd) != mClients.end()) return;
	
	unsigned int workspaceNo = GetTaskWorkspace(hwnd);
	if (mWorkspaces.size() <= workspaceNo)
		addWorkspace(workspaceNo);
	
	Client* newClient = new Client(hwnd);
	///	add client in map for fast lookup
	mClients.insert(std::pair<HWND, Client*>(hwnd, newClient));
	
	mWorkspaces[workspaceNo]->addClient(newClient);

	removeBorder(hwnd);

	mWorkspaces[workspaceNo]->update();
}


void TilingManager::removeWindow(HWND hwnd)
{
	Client* client = getClient(hwnd);
	if (!client) return;

	Container* container = client->getParentContainer();
	if (container->removeClient(client) < 1)
		container->getParentColumn()->getParentWorkspace()->cleanup();
	
	addBorder(hwnd);

	mClients.erase(hwnd);
	delete client;

	unsigned int workspaceNo = GetTaskWorkspace(hwnd);
	if (mWorkspaces.size() > workspaceNo)
	{
		mWorkspaces[workspaceNo]->cleanup();
		mWorkspaces[workspaceNo]->update();
	}
	else
	{
// 		cleanupAll();
// 		updateAll();
	}
}


void TilingManager::moveClient(Client* client, Direction direction)
{
	if (!client) return;
	Container* container = client->getParentContainer();
	Column* column = container->getParentColumn();
	Workspace* workspace = column->getParentWorkspace();

	int r = container->moveClient(client, direction);
	if (r == -1)
	{
		r = column->moveClient(client, direction);
		if (r == -1)
		{
			r = workspace->moveClient(client, direction);
			if (r == -1) {}
			else
			{
				if (r == 0) workspace->cleanup();
				workspace->update();
			}
		}
		else
		{
			if (r == 0) workspace->cleanup();
			workspace->update();
		}
	}
	else
	{
		if (r == 0) workspace->cleanup();
		container->update();
	}

	setFocusedClient(client);
}


void TilingManager::moveContainer(Container* container, Direction direction/* = D_INVALID*/)
{
	if (container->getClientCount() == 0) return;

	Client* focusedClient = container->getFocusedClient();
	Container* targetContainer = 0;
	Column* column = container->getParentColumn();
	Workspace* workspace = column->getParentWorkspace();

	switch (direction)
	{
	case D_INVALID:
		return;
	
	case D_UP:
		if (container->isFirst())
		{
			if (workspace->getClientCountOnRow(container->getElementNumber()) == 1) return;
			targetContainer = column->addContainer(direction);
		}
		else
			targetContainer = column->getContainer(
				container->getElementNumber() - 1);
		break;
	
	case D_RIGHT:
		if (column->isLast())
		{	
			if (column->getClientCount() == 1) return;
			targetContainer = workspace->addColumn(direction)->
				getContainer(container->getElementNumber());
		}
		else
			targetContainer = workspace->getColumn(column->getElementNumber() + 1)->
				getContainer(container->getElementNumber());
		break;
	
	case D_DOWN:
		if (container->isLast())
		{
			if (workspace->getClientCountOnRow(column->getElementNumber()) == 1) return;
			targetContainer = column->addContainer(direction);
		}
		else
			targetContainer = column->getContainer(container->getElementNumber() + 1);
		break;
	
	case D_LEFT:
		if (column->isFirst())
		{	
			if (column->getClientCount() == 1) return;
			targetContainer = workspace->addColumn(direction)->
				getContainer(container->getElementNumber());
		}
		else
			targetContainer = workspace->getColumn(column->getElementNumber() - 1)->
				getContainer(container->getElementNumber());
		break;
	}

	std::deque<Client*> originContainerClients = *container->getClients();
	std::deque<Client*> targetContainerClients = *targetContainer->getClients();
	for (auto it = originContainerClients.begin(); it != originContainerClients.end(); it++)
	{
		container->removeClient(*it);
		targetContainer->addClient(*it);
	}

	for (auto it = targetContainerClients.begin(); it != targetContainerClients.end(); it++)
	{
		targetContainer->removeClient(*it);
		container->addClient(*it);
	}

	workspace->cleanup();
	workspace->update();
	setFocusedClient(focusedClient);
}


// TODO need HWND from 'TASKITEM_REFRESH' events
void TilingManager::updateWindow(HWND hwnd)
{
	/*
	if (hwnd != 0)
	{
		Client* client = getClient(hwnd);
		if (!client) return;

		updateClientWorkspace(client);
	}
	else
	*/ 
		for (auto it = mClients.begin(); it != mClients.end(); it++)
			updateClientWorkspace(it->second);
}


void TilingManager::updateClientWorkspace(Client* client)
{
	unsigned int workspaceNo = GetTaskWorkspace(client->getWindow());
	unsigned int oldWorkspaceNo = client->getParentContainer()->getParentColumn()->
								  getParentWorkspace()->getElementNumber();
	if (workspaceNo != oldWorkspaceNo)
	{
		if (workspaceNo < 0 || workspaceNo >= mWorkspaces.size() ||
			oldWorkspaceNo < 0 || oldWorkspaceNo >= mWorkspaces.size()) return;

		client->getParentContainer()->removeClient(client);
		
		if (mWorkspaces.size() <= workspaceNo)
			addWorkspace(workspaceNo);
		mWorkspaces[workspaceNo]->addClient(client);
		mWorkspaces[oldWorkspaceNo]->cleanup();
		mWorkspaces[oldWorkspaceNo]->update();
		mWorkspaces[workspaceNo]->update();
	}
}


void TilingManager::updateDesktopInfo()
{
	// TODO multiple screen handling
	DesktopInfo deskInfo;
	GetDesktopInfo(&deskInfo);
	if (deskInfo.isCurrent)
	{
		if (mCurrentWorkspace)
			mCurrentWorkspace->setCurrent(false);
		if (deskInfo.number >= (int)mWorkspaces.size())
			addWorkspace(deskInfo.number);

		mCurrentWorkspace = mWorkspaces[deskInfo.number];
		mCurrentWorkspace->setCurrent();
	}
	mCurrentWorkspace->update();
}


Client* TilingManager::getClient(HWND hwnd)
{
	auto it = mClients.find(hwnd);
	if (it == mClients.end()) return 0;
	return it->second;
}


Client* TilingManager::getFocusedClient()
{
	Client* client = mWorkspaces[GetTaskWorkspace(GetTask(GetActiveTask()))]->getFocusedClient();
	if (client) return client;
	return 0;
}


void TilingManager::setFocusedClient(Client* client)
{
	if (!client) return;
	Container* container = client->getParentContainer();
	container->setFocusedClient(client);
	Column* column = container->getParentColumn();
	column->setFocusedContainer(container);
	column->getParentWorkspace()->setFocusedColumn(column);
}


///	add missing workspaces from '0' to 'workspace'
void TilingManager::addWorkspace(int workspace)
{
	Workspace* newWorkspace;
	for (int i = mWorkspaces.size(); i <= workspace; i++)
	{
		newWorkspace = new Workspace(i, this);
		mWorkspaces.insert(mWorkspaces.begin() + i, newWorkspace); 
	}
}


// TODO read module name too
void TilingManager::readInclusionFile()
{
	mInclusionList.clear();
	
	char exclusionspath[MAX_PATH];
    FILE *fp = FileOpen(find_config_file(exclusionspath, INCLUSION_FILE));
    if (fp) for (;;)
    {
        char line_buffer[256];
        if (false == ReadNextCommand(fp, line_buffer, sizeof line_buffer))
        {
            FileClose(fp);
            break;
        }
		mInclusionList.insert(std::string(line_buffer));
	}
}


///	check if the window class is in the inclusion list
bool TilingManager::checkInclusionList(HWND hwnd)
{
	char className[200]; className[0] = 0;
	GetClassName(hwnd, className, sizeof className);

// TODO 'getModuleName' always return blackbox.exe ??

// 	char fileName[200]; fileName[0] = 0;
// 	getModuleName(hwnd, fileName, sizeof fileName);
// 	
// 	dbg_printf("%s:%s, %s:%s\n", "class", className, "module", fileName);
	if (mInclusionList.find(className) != mInclusionList.end())
		return true;

	return false;
}

// TODO always return blackbox.exe ??
int TilingManager::getModuleName(HWND hwnd, char *buffer, int buffsize)
{
    char sFileName[MAX_PATH]; HINSTANCE hi; int i, r;
    hi = (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
    r = GetModuleFileName(hi, sFileName, MAX_PATH);
    if (0 == r)
    {
		dbg_printf("%s", GetLastError());
	    r = GetModuleFileName(NULL, sFileName, MAX_PATH);
    }
	for (i = r; i && sFileName[i-1] != '\\'; i--);
    r -= i;
    if (r >= buffsize)
        r = buffsize-1;
    memcpy(buffer, sFileName + i, r);
    buffer[r] = 0;
    return r;
}


void TilingManager::removeBorder(HWND hwnd)
{
	SetWindowLong(hwnd, GWL_STYLE, (GetWindowLong(hwnd, GWL_STYLE) & ~(WS_CAPTION | WS_SIZEBOX)));
}


void TilingManager::addBorder(HWND hwnd)
{
	SetWindowLong(hwnd, GWL_STYLE, (GetWindowLong(hwnd, GWL_STYLE) | (WS_CAPTION | WS_SIZEBOX)));
	SetWindowPos(hwnd, 0, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER);
}


int TilingManager::getWorkspaceNumber(Workspace* workspace)
{
	int i = 0;
	for (auto it = mWorkspaces.begin(); it != mWorkspaces.end(); it++, i++)
		if (*it == workspace) return i;
	return 0; // TODO trow an exception
}


void TilingManager::clear()
{
	for (auto it = mWorkspaces.begin(); it != mWorkspaces.end(); it++)
		delete *it;
	
	mWorkspaces.clear();
	
	for (auto it = mClients.begin(); it != mClients.end(); it++)
	{
		addBorder(it->first);
		delete it->second;
	}
	mClients.clear();
}


void TilingManager::init()
{	
	updateDesktopInfo();
	readInclusionFile();
	
	for (int i = 0; i < GetTaskListSize(); i++)
		addWindow(GetTask(i));

	for (auto it = mWorkspaces.begin(); it != mWorkspaces.end(); it++)
	{
		(*it)->cleanup();
		(*it)->update();
	}
}


void TilingManager::reset()
{
	clear();	
	init();
}

