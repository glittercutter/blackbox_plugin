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

#ifndef BBDT_WORKSPACE_H
#define BBDT_WORKSPACE_H

#include "bbDT_Common.h"
#include "bbDT_Column.h"

class Workspace
{
public:
	// constructor
	Workspace(unsigned int number)
	:	mWorkspaceNumber(number), mRows(1), mFullscreen(false), mIsCurrent(false) {}
	
	/// destructor
	~Workspace()
	{
		for (auto it = mColumns.begin(); it != mColumns.end(); it++)
			delete *it;

		mColumns.clear();
		mColumnsFocus.clear();
	}

	void addClient(Client* client);
	int moveClient(Client* client, Direction dir);
	int focus(Client* client, Direction dir);
	int expand(Client* client, Direction dir);
	
	Column* addColumn(Direction direction = D_INVALID);
	int removeColumn(Column* column);

	void handleNewRow(Column* column, Direction dir);

	void setFocusedColumn(Column* col);
	Column* getFocusedColumn() {return !mColumnsFocus.empty() ? mColumnsFocus.back() : 0;}
	Client* getFocusedClient() {return getFocusedColumn() ? getFocusedColumn()->getFocusedClient() : 0;}
	Column* getColumn(int colNo) {return mColumns[colNo];}

	int getClientCountOnRow(int row);
	int getColumnCount() {return mColumns.size();}
	int getColumnNumber(Column* column);

	Rect& getRect() {return mRect;}
	float getRowHeightFactor(int rowNo) {return (int)mRowsHeightFactor.size() > rowNo ? mRowsHeightFactor[rowNo] : 1.f;}
	void setRowHeightFactor(int rowNo, float value)
	{
		if ((int)mRowsHeightFactor.size() < rowNo) return;
		mRowsHeightFactor[rowNo] = value;
		if (mRowsHeightFactor[rowNo] < MIN_RATIO) mRowsHeightFactor[rowNo] = MIN_RATIO;
	}
	void resizeRowHeightFactor(int rowNo, float value)
	{
		if ((int)mRowsHeightFactor.size() < rowNo) return;
		mRowsHeightFactor[rowNo] += value;
		if (mRowsHeightFactor[rowNo] < MIN_RATIO) mRowsHeightFactor[rowNo] = MIN_RATIO;
	}

	unsigned int getElementNumber() {return mWorkspaceNumber;}

	void toggleFullscreen();
	bool isFullscreen() {return mFullscreen;}

	void setCurrent(bool state = true) {mIsCurrent = state;}
	bool isCurrent() {return mIsCurrent;}

	void cleanup();
	void update();

private:
	unsigned int mWorkspaceNumber;
	
	Rect mRect;
	int mRows;
	
	bool mFullscreen;
	bool mIsCurrent;

	std::deque<Column*> mColumns;
	std::deque<float> mRowsHeightFactor;
	std::list<Column*> mColumnsFocus;
};


#endif
