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

#ifndef BBDT_MANAGER_H
#define BBDT_MANAGER_H

#include "bbDT_Common.h"

class TilingManager
{
public:
	// constructor
	TilingManager(RcSetting* _rcSetting)
	:	mCurrentWorkspace(0), rcSetting(_rcSetting)
	{
		init();
	}

	/// destructor
	~TilingManager()
	{
		clear();
	}

	void move(Direction dir, Target target);
	void focus(Direction dir, Target target);
	void focus(HWND hwnd);
	void expand(Direction dir);
	void resize(Direction dir);
	void toggleFullscreen();
	void toggleFloating();

	void addWindow(HWND hwnd, bool ignoreList = false);
	void removeWindow(HWND hwnd);
	void moveClient(Client* client, Direction dir = D_INVALID);
	void moveContainer(Container* container, Direction dir = D_INVALID);
	void updateWindow(HWND hwnd);
	void updateDesktopInfo();
	
	Client* getClient(HWND hwnd);
	void updateClientWorkspace(Client* client);
	Client* getFocusedClient();
	void setFocusedClient(Client* client);

	void removeBorder(HWND hwnd);
	void addBorder(HWND hwnd);

	void clear();
	void init();
	void reset();

private:
	void readInclusionFile();
	bool checkInclusionList(HWND hwnd);
	int getModuleName(HWND hwnd, char *buffer, int buffsize);

	void addWorkspace(int workspace);
	std::deque<Workspace*> mWorkspaces;	
	Workspace* mCurrentWorkspace;

	std::unordered_map<HWND, Client*> mClients;
	std::unordered_set<std::string> mInclusionList;

	RcSetting* rcSetting;
};

#endif
