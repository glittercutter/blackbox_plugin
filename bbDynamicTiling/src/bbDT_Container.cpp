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

#include "bbDT_Container.h"
#include "bbDT_Workspace.h"

void Container::addClient(Client* client, Direction dir/* = D_INVALID*/)
{    
    if (mSharedBy)
    {
        mSharedBy->addClient(client, dir);
        return;
    }

    client->setParentContainer(this);
    if (dir == D_UP)
        mClients.push_front(client);
    else
        mClients.push_back(client);
    
    mClientsFocus.push_back(client);
}

int Container::removeClient(Client* client)
{
    mClients.erase(mClients.begin() + client->getElementNumber());

    for (auto it = mClientsFocus.begin(); it != mClientsFocus.end(); it ++)
    {
        if (*it == client)
        {
            mClientsFocus.erase(it);
            break;
        }
    }

    return mClients.size();
}

void Container::setFocusedClient(Client* client)
{
    // find client and move it to the end
    for (auto it = mClientsFocus.begin(); it != mClientsFocus.end(); it++)
    {
        if (*it == client)
        {
            if (*it != mClientsFocus.back())
            {
                mClientsFocus.erase(it);
                mClientsFocus.push_back(client);
            }
            break;
        }
    }
    client->setFocused();
}

void Container::setFocused()
{
    if (!mClientsFocus.empty())
        mClientsFocus.back()->setFocused();
}

/// return '-1' if the client can't be moved inside the container
int Container::moveClient(Client* client, Direction dir)
{
    int clientNo = client->getElementNumber();
    switch (dir)
    {
    case D_UP:
        // check if the client is already at the top of the container
        if (client->isFirst()) return -1;
        mClients[clientNo] = mClients[clientNo - 1];
        mClients[clientNo - 1] = client;
        break;

    case D_DOWN:
        // check if the client is already at the bottom of the container
        if (client->isLast()) return -1;
        mClients[clientNo] = mClients[clientNo + 1];
        mClients[clientNo + 1] = client;
        break;
    
    default: return -1;
    }

    return mClients.size();
}

int Container::focus(Client* client, Direction dir)
{
    if (mClients.size() < 2) return -1;
    
    switch (dir)
    {
    case D_UP:    
        if (client == mClients.front())
            setFocusedClient(mClients.back());
        else
            setFocusedClient(mClients[client->getElementNumber() - 1]);
        break;

    case D_DOWN:
        if (client == mClients.back())
            setFocusedClient(mClients.front());
        else
            setFocusedClient(mClients[client->getElementNumber() + 1]);
        break;
    
    default: return -1;
    }
    return 0;
}

void Container::update()
{
    if (mClients.empty()) return;

    float y, height;
    Rect rect;

    if (isShared())
    {
        getSharedDimension(&rect);
    }
    else
    {
        rect.X1 = mParentColumn->getPosX();
        rect.X2 = mParentColumn->getWidth() + rect.X1;
        rect.Y1 = mY;
        rect.Y2 = mY + mHeight;
    }

    y = rect.Y1;
    float baseHeight = rect.getHeight() / (float)mClients.size();

    for (auto it = mClients.begin(); it != mClients.end(); it++)
    {
        if (mParentColumn->getParentWorkspace()->isFullscreen())
        {
            (*it)->setRect(mParentColumn->getParentWorkspace()->getRect());
        }
        else
        {
            if (*it == mClients.back())
                height = rect.getHeight() - ((int)y - rect.Y1);
            else
                height = baseHeight;

            (*it)->setRect(Rect(rect.X1, y, rect.X2, y + height));
            y += height;
        }
    }

    for (auto it = mClients.begin(); it != mClients.end(); it++)
        (*it)->update();
}

void Container::getSharedDimension(Rect* rect)
{
    int min, max, tmp;

    if (isShared() == S_HORIZONTAL)
    {
        min = mParentColumn->getPosX();
        max = mParentColumn->getWidth() + min;
        for (auto it = mSharedContainers.begin(); it != mSharedContainers.end(); it++)
        {
            tmp = (*it)->getParentColumn()->getPosX();
            if (tmp < min) min = tmp;
            tmp = (*it)->getParentColumn()->getWidth() + tmp;
            if (tmp > max) max = tmp;
        }
        rect->X1 = min;    rect->X2 = max;
        rect->Y1 = mY;    rect->Y2 = mHeight + mY;
    }
    else if (isShared() == S_VERTICAL)
    {
        min = mY;
        max = mHeight + min;
        for (auto it = mSharedContainers.begin(); it != mSharedContainers.end(); it++)
        {
            tmp = (*it)->getPosY();
            if (tmp < min) min = tmp;
            tmp = (*it)->getHeight() + tmp;
            if (tmp > max) max = tmp;
        }
        rect->X1 = mParentColumn->getPosX(); rect->X2 = mParentColumn->getWidth() + rect->X1;
        rect->Y1 = min;    rect->Y2 = max;
    }
}

// called by other containers
int Container::requestSharing(Container* container)
{
    if (mSharedBy) return -1;
    if (!mClients.empty()) return -1;

    mSharedBy = container;
    return 0;
}

// called by other containers
int Container::releaseSharing(Container* container)
{
    if (mSharedBy != container) return -1;
    mSharedBy = 0;
    return 0;
}

void Container::removeShare(Container* container)
{
    switch (isShared())
    {
    case S_HORIZONTAL:
        if (container->getParentColumn()->getElementNumber() < mParentColumn->getElementNumber())
            for (auto it = mSharedContainers.begin(); it != mSharedContainers.end();)
            {
                if ((*it)->getParentColumn()->getElementNumber() < mParentColumn->getElementNumber())
                {
                    (*it)->releaseSharing(this);
                    it = mSharedContainers.erase(it);
                }
                else it++;
            }
        else if (container->getParentColumn()->getElementNumber() > mParentColumn->getElementNumber())
            for (auto it = mSharedContainers.begin(); it != mSharedContainers.end();)
            {
                if ((*it)->getParentColumn()->getElementNumber() > mParentColumn->getElementNumber())
                {
                    (*it)->releaseSharing(this);
                    it = mSharedContainers.erase(it);
                }
                else it++;
            }
        else
        {
            (container)->releaseSharing(this);
            mSharedContainers.erase(std::find(mSharedContainers.begin(), mSharedContainers.end(), container));
        }
        break;
    
    case S_VERTICAL:
        if (container->getElementNumber() < getElementNumber())
            for (auto it = mSharedContainers.begin(); it != mSharedContainers.end();)
            {
                if ((*it)->getElementNumber() < getElementNumber())
                {
                    (*it)->releaseSharing(this);
                    it = mSharedContainers.erase(it);
                }
                else it++;
            }
        else if (container->getElementNumber() > getElementNumber())
            for (auto it = mSharedContainers.begin(); it != mSharedContainers.end();)
            {
                if ((*it)->getElementNumber() > getElementNumber())
                {
                    (*it)->releaseSharing(this);
                    it = mSharedContainers.erase(it);
                }
                else it++;
            }
        else
        {
            (container)->releaseSharing(this);
            mSharedContainers.erase(std::find(mSharedContainers.begin(), mSharedContainers.end(), container));
        }
        break;

    default: return;
    }
}

void Container::clearSharedContainer()
{
    for (auto it = mSharedContainers.begin(); it != mSharedContainers.end(); it++)
        (*it)->releaseSharing(this);
    mSharedContainers.clear();
}

int Container::addSharedContainer(Container* container)
{
    if (container->requestSharing(this) == -1) return -1;
    // request accepted
    mSharedContainers.push_back(container);
    return 0;
}

bool Container::isFirst() {return getElementNumber() == 0;}
bool Container::isLast() {return getElementNumber() == (int)mParentColumn->getContainerCount() - 1;}

Shared Container::isShared()
{
    if (!mSharedContainers.empty())
        return (mSharedContainers.front()->getParentColumn() == mParentColumn) ?
            S_VERTICAL : S_HORIZONTAL;
    return S_INVALID;
}

int Container::getClientNumber(Client* client)
{
    int i = 0;
    for (auto it = mClients.begin(); it != mClients.end(); it++, i++)
        if (*it == client) return i;
    return 0; // TODO trow an exception
}

int Container::getElementNumber() {return mParentColumn->getContainerNumber(this);}
