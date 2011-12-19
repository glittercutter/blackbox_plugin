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

#ifndef BBDT_COMMON_H
#define BBDT_COMMON_H

#include <assert.h>
#include "blackbox_header/BBApi.h"
#include "blackbox_header/bblib.h"
#include "blackbox_header/bbversion.h"
#include <stdlib.h>

#include <algorithm>
#include <deque>
#include <list>
#include <string>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

#define INCLUSION_FILENAME "inclusions.rc"

// class declaration
class Client;
class Column;
class Container;
class TilingManager;
class Monitor;
class Workspace;

char* find_config_file(char *rcpath, const char *file);

enum Target
{
    T_CLIENT,
    T_CONTAINER
};

enum Direction
{
    D_INVALID = 0,
    D_UP,
    D_RIGHT,
    D_DOWN,
    D_LEFT
};

inline Direction swapDirection(Direction dir)
{
    switch (dir)
    {
    case D_INVALID: return dir;
    case D_UP: return D_DOWN;
    case D_RIGHT: return D_LEFT;
    case D_DOWN: return D_UP;
    case D_LEFT: return D_RIGHT;
    }
    return dir;
}

struct Rect
{
    /// constructor
    Rect(int x1, int y1, int x2, int y2)
    :    X1(x1), Y1(y1), X2(x2), Y2(y2) {}
    Rect()
    :    X1(0), Y1(0), X2(0), Y2(0) {}
    
    Rect& operator = (LPRECT rect)
    {
        X1 = rect->left;
        Y1 = rect->top;
        X2 = rect->right;
        Y2 = rect->bottom;
        return *this;
    }

    bool operator == (const Rect& rect) const
    {
        if (X1 == rect.X1)
            if (Y1 == rect.Y1)
                if (X2 == rect.X2)
                    if (Y2 == rect.Y2)
                        return true;
        return false;
    }
    
    int getWidth() {return X2 - X1;} const
    int getHeight() {return Y2 - Y1;} const

    std::string print() const
    {
        std::stringstream ss;
        ss << "x1=" << X1 << " y1=" << Y1 << " x2=" << X2 << " y2=" << Y2;
        return ss.str();
    }

    int X1, Y1, X2, Y2;
};

struct RCSetting
{
    bool enableLowLevelKeyHook;

    int workspaceFullscreenBorderSize;
    int workspaceBorderSize;
    int columnBorderSize;
    int containerBorderSize;
    int clientBorderSize;
    
    float minSizeFactor;
    int resizePixel;
};

#endif
