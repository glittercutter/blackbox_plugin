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

#include "bbDT_Common.h"
#include "bbDT_Container.h"
#include "bbDT_Manager.h"
#include "bbDT_Workspace.h"

/* ---------------------------------- */
/* plugin info */

const char szAppName[]        = "bbDynamicTiling";
const char szInfoVersion[]    = "0.1.2";
const char szInfoAuthor[]     = "Sebastien Raymond";
const char szInfoRelDate[]    = "2011-12-19";
const char szInfoLink[]       = "http://glittercutter.github.com";
const char szInfoEmail[]      = "";
const char szVersion[]        = "bbDynamicTiling 0.1.2"; /* fallback for pluginInfo() */
const char szCopyright[]      = "Sebastien Raymond 2011";

/* ---------------------------------- */
/* Dependencies on the plugin-name */

/* prefix for our broadcast messages */
#define BROAM_PREFIX        "@bbDynamicTiling."

#define BROAM_CONTAINER     "container."
#define BROAM_MOVE          "move."
#define BROAM_FOCUS         "focus."
#define BROAM_EXPAND        "expand."
#define BROAM_RESIZE        "resize."

#define BROAM_FULLSCREEN    "fullscreen"
#define BROAM_FLOAT         "float"

#define BROAM_UP            "up"
#define BROAM_RIGHT         "right"
#define BROAM_DOWN          "down"
#define BROAM_LEFT          "left"

/* configuration file */
#define RC_FILE "bbDynamicTiling.rc"

/* prefix for items in the configuration file */
#define RC_PREFIX "bbDynamicTiling."
#define RC_KEY(key) (RC_PREFIX key ":")

/* prefix for unique menu id's */
#define MENU_ID(key) ("bbDynamicTiling_ID" key)

/* ---------------------------------- */
/* Interface declaration */

#ifdef __cplusplus
extern "C" {
#endif
    DLL_EXPORT int beginPlugin(HINSTANCE hPluginInstance);
    DLL_EXPORT int beginSlitPlugin(HINSTANCE hPluginInstance, HWND hSlit);
    DLL_EXPORT int beginPluginEx(HINSTANCE hPluginInstance, HWND hSlit);
    DLL_EXPORT void endPlugin(HINSTANCE hPluginInstance);
    DLL_EXPORT LPCSTR pluginInfo(int field);
#ifdef __cplusplus
}
#endif

/* ---------------------------------- */
/* Global */

HINSTANCE g_hInstance;
HHOOK g_lowLevelHook;
RCSetting g_rcSetting;
HWND g_bbhWnd; // blackbox window
HWND g_hwnd; // our plugin window
char g_rcPath[MAX_PATH]; // full path to configuration file
bool g_under_gbblean;
bool g_under_gxoblite;
TilingManager* myTilingManager;

/* ---------------------------------- */
/* some function prototypes */

void ReadRCSettings(void);
void WriteRCSettings(void);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wparam, LPARAM lparam);
int scan_broam(struct msg_test *msg_test, const char *test);
void eval_broam(struct msg_test *msg_test, int mode, void *pValue);
Direction getDirection(const char* str);

enum eval_broam_modes
{
    M_BOL = 1,
    M_INT = 2,
    M_STR = 3
};
    
/* ------------------------------------------------------------------ */
/* The startup interface */

/* slit interface */
int beginPluginEx(HINSTANCE hPluginInstance, HWND hSlit)
{
    WNDCLASS wc;

    /* --------------------------------------------------- */
    /* This plugin can run in one instance only. If g_bbhWnd
       is set it means we are already loaded. */

    if (g_bbhWnd)
    {
        MessageBox(g_bbhWnd, "Do not load me twice!", szVersion,
                   MB_OK | MB_ICONERROR | MB_TOPMOST);
        return 1; /* 1 = failure */
    }

    /* --------------------------------------------------- */
    /* grab some global information */

    g_bbhWnd = GetBBWnd();
    g_hInstance = hPluginInstance;

    if (0 == memcmp(GetBBVersion(), "bbLean", 6))
        g_under_gbblean = true;
    else if (0 == memcmp(GetBBVersion(), "bb", 2))
        g_under_gxoblite = true;

    /* --------------------------------------------------- */
    /* register the window class */

    memset(&wc, 0, sizeof wc);
    wc.lpfnWndProc = WndProc;      /* window procedure */
    wc.hInstance = g_hInstance;    /* hInstance of .dll */
    wc.lpszClassName = szAppName;  /* window class name */
    wc.style = 0;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hIcon = NULL;
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;

    if (!RegisterClass(&wc))
    {
        MessageBox(g_bbhWnd, "Error registering window class", szVersion,
                   MB_OK | MB_ICONERROR | MB_TOPMOST);
        return 1; /* 1 = failure */
    }

    ReadRCSettings();

    /* --------------------------------------------------- */
    /* register low level hook */
    if (g_rcSetting.enableLowLevelKeyHook)
    {
        g_lowLevelHook = SetWindowsHookEx(
            WH_KEYBOARD_LL,
            LowLevelKeyboardProc,  /* hook procedure */
            g_hInstance,           /* hInstance of .dll */
            0
        );
    }

    /* --------------------------------------------------- */
    /* create the window */

    g_hwnd = CreateWindowEx(
        0,             /* window ex-style */
        szAppName,     /* window class name */
        NULL,          /* window caption text */
        0,             /* window style */
        0,             /* x position */
        0,             /* y position */
        0,             /* window width */
        0,             /* window height */
        HWND_MESSAGE,  /* parent window */
        NULL,          /* window menu */
        g_hInstance,   /* hInstance of .dll */
        NULL           /* creation data */
    );

    myTilingManager = new TilingManager(&g_rcSetting);

    return 0; /* 0 = success */
}

/* no-slit interface */
int beginPlugin(HINSTANCE hPluginInstance)
{
    return beginPluginEx(hPluginInstance, NULL);
}

/* ------------------------------------------------------------------ */
/* on unload... */
void endPlugin(HINSTANCE hPluginInstance)
{
    /* Destroy the window... */
    DestroyWindow(g_hwnd);

    /* Unregister window class... */
    UnregisterClass(szAppName, hPluginInstance);
    
    /* Unhook low level keyboard hook... */
    if (g_rcSetting.enableLowLevelKeyHook)
        UnhookWindowsHookEx(g_lowLevelHook);
    
    delete myTilingManager;
}

/* ------------------------------------------------------------------ */
/* pluginInfo is used by Blackbox for Windows to fetch information
   about a particular plugin. */
LPCSTR pluginInfo(int index)
{
    switch (index)
    {
        case PLUGIN_NAME:      return szAppName;       /* Plugin name */
        case PLUGIN_VERSION:   return szInfoVersion;   /* Plugin version */
        case PLUGIN_AUTHOR:    return szInfoAuthor;    /* Author */
        case PLUGIN_RELEASE:   return szInfoRelDate;   /* Release date, preferably in yyyy-mm-dd format */
        case PLUGIN_LINK:      return szInfoLink;      /* Link to author's website */
        case PLUGIN_EMAIL:     return szInfoEmail;     /* Author's email */
        default:               return szVersion;       /* Fallback: Plugin name + version, e.g. "MyPlugin 1.0" */
    }
}

/* ------------------------------------------------------------------ */

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int msgs[] = { BB_RECONFIGURE, BB_TASKSUPDATE, BB_BROADCAST, BB_DESKTOPINFO, 0};

    switch (message)
    {
        case WM_CREATE:
            SendMessage(g_bbhWnd, BB_REGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
            break;

        case WM_DESTROY:
            SendMessage(g_bbhWnd, BB_UNREGISTERMESSAGE, (WPARAM)hwnd, (LPARAM)msgs);
            break;

        /* ---------------------------------------------------------- */
        /* Blackbox sends a "BB_RECONFIGURE" message on style changes etc. */

        case BB_RECONFIGURE:
            ReadRCSettings();
            break;

        case BB_TASKSUPDATE:
            switch (lParam)
            {
                case TASKITEM_ADDED:
                    myTilingManager->addWindow((HWND)wParam);
                    myTilingManager->focus((HWND)wParam);
                    break;
                case TASKITEM_MODIFIED:
                    myTilingManager->updateWindow(NULL);
                    break;
                case TASKITEM_ACTIVATED:
                    myTilingManager->focus((HWND)wParam);
                    break;
                case TASKITEM_REMOVED:
                    myTilingManager->removeWindow((HWND)wParam);
                    break;
                case TASKITEM_REFRESH:
                    myTilingManager->updateWindow(NULL);
                    break;
                case TASKITEM_FLASHED:
                    break;
            }
            break;

        /* ---------------------------------------------------------- */
        /* Blackbox sends Broams to all windows... */

        case BB_BROADCAST:
        {
            const char *msg = (LPCSTR)lParam;

            /* if the broam is not for us, return now */
            if (0 != memcmp(msg, BROAM_PREFIX, sizeof BROAM_PREFIX - 1))
                break;
            msg += sizeof BROAM_PREFIX - 1;
            
            // move
            if (0 == strncmp(msg, BROAM_MOVE, sizeof BROAM_MOVE - 1))
            {
                msg += sizeof BROAM_MOVE - 1;
                if (0 == strncmp(msg, BROAM_CONTAINER, sizeof BROAM_CONTAINER - 1))
                {
                    msg += sizeof BROAM_CONTAINER - 1;
                    myTilingManager->move(getDirection(msg), T_CONTAINER);
                }
                else
                    myTilingManager->move(getDirection(msg), T_CLIENT);
            }

            // focus
            else if (0 == strncmp(msg, BROAM_FOCUS, sizeof BROAM_FOCUS - 1))
            {
                msg += sizeof BROAM_FOCUS - 1;
                if (0 == strncmp(msg, BROAM_CONTAINER, sizeof BROAM_CONTAINER - 1))
                {
                    msg += sizeof BROAM_CONTAINER - 1;
                    myTilingManager->focus(getDirection(msg), T_CONTAINER);
                }
                else
                    myTilingManager->focus(getDirection(msg), T_CLIENT);
            }

            // expand
            else if (0 == strncmp(msg, BROAM_EXPAND, sizeof BROAM_EXPAND - 1))
            {
                msg += sizeof BROAM_EXPAND - 1;
                myTilingManager->expand(getDirection(msg));
            }

            // resize
            else if (0 == strncmp(msg, BROAM_RESIZE, sizeof BROAM_RESIZE - 1))
            {
                msg += sizeof BROAM_RESIZE - 1;
                myTilingManager->resize(getDirection(msg));
            }

            // toggle fullscreen
            else if (0 == strncmp(msg, BROAM_FULLSCREEN, sizeof BROAM_FULLSCREEN - 1))
            {
                myTilingManager->toggleFullscreen();
            }
            
            // toggle floating
            else if (0 == strncmp(msg, BROAM_FLOAT, sizeof BROAM_FLOAT - 1))
            {
                myTilingManager->toggleFloating();
            }
            
            break;
        }
        
        case BB_DESKTOPINFO:
            myTilingManager->updateDesktopInfo();
            break;

        /* ---------------------------------------------------------- */
        /* prevent the user from closing the plugin with alt-F4 */
        case WM_CLOSE:
            break;

        default:
            return DefWindowProc(hwnd, message, wParam, lParam); /* We aren't handling this message so return DefWindowProc */
    }
    return 0;
}


/* ------------------------------------------------------------------ */
/* catch Win+L key combination */

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM wparam, LPARAM lparam)
{
    KBDLLHOOKSTRUCT& kllhs = *(KBDLLHOOKSTRUCT*)lparam;
    if (code == HC_ACTION)
    {
        // Test for an 'L' keypress with either Win key down and no other modifier
        if (wparam == WM_KEYDOWN && kllhs.vkCode == 'L' && 
            (GetAsyncKeyState(VK_LWIN) < 0 || GetAsyncKeyState(VK_RWIN) < 0) &&
            GetAsyncKeyState(VK_SHIFT) >= 0 && GetAsyncKeyState(VK_CONTROL) >= 0 &&
            GetAsyncKeyState(VK_MENU) >= 0)
        {
            myTilingManager->focus(D_RIGHT, T_CLIENT);

            // Return non-zero to halt message propagation
            // and prevent the Win+L hotkey from getting activated. (LOCKING)
            return 1;
        }
    }
    return CallNextHookEx(0, code, wparam, lparam);
}

/* ------------------------------------------------------------------ */

Direction getDirection(const char* str)
{
    Direction dir = D_INVALID;
    if (0 == strncmp(str, BROAM_UP, sizeof BROAM_UP))
        dir = D_UP;
    else if (0 == strncmp(str, BROAM_RIGHT, sizeof BROAM_RIGHT))
        dir = D_RIGHT;
    else if (0 == strncmp(str, BROAM_DOWN, sizeof BROAM_DOWN))
        dir = D_DOWN;
    else if (0 == strncmp(str, BROAM_LEFT, sizeof BROAM_LEFT))
        dir = D_LEFT;
    return dir;
}

/* ------------------------------------------------------------------ */
/* Read the configuration file */

void ReadRCSettings(void)
{
    /* Locate configuration file */
    FindRCFile(g_rcPath, RC_FILE, g_hInstance);

    /* Read our settings. (If the config file does not exist,
    the Read... functions give us just the defaults.) */

    g_rcSetting.enableLowLevelKeyHook = ReadBool(g_rcPath, RC_KEY("enableLowLevelKeyHook"), true);

    g_rcSetting.workspaceFullscreenBorderSize = ReadInt(g_rcPath, RC_KEY("workspaceFullscreenBorderSize"), 0);
    g_rcSetting.workspaceBorderSize  = ReadInt(g_rcPath, RC_KEY("workspaceBorderSize"), 0);
    g_rcSetting.columnBorderSize = ReadInt(g_rcPath, RC_KEY("columnBorderSize"), 1);
    g_rcSetting.containerBorderSize = ReadInt(g_rcPath, RC_KEY("containerBorderSize"), 1);    
    g_rcSetting.clientBorderSize = ReadInt(g_rcPath, RC_KEY("clientBorderSize"), 1);

    g_rcSetting.minSizeFactor = strtod(ReadString(g_rcPath, RC_KEY("minSizeFactor"), "0.05"), NULL);
    g_rcSetting.resizePixel = ReadInt(g_rcPath, RC_KEY("resizePixel"), 25);
}

/* ------------------------------------------------------------------ */
char* find_config_file(char *g_rcPath, const char *file)
{
    bool (*pFindRCFile)(LPSTR g_rcPath, LPCSTR rcfile, HINSTANCE plugin_instance);
    *(FARPROC*)&pFindRCFile = GetProcAddress(GetModuleHandle(NULL), "FindRCFile");
    
    if (pFindRCFile) pFindRCFile(g_rcPath, file, g_hInstance);
    else set_my_path(g_hInstance, g_rcPath, file);
    
    return g_rcPath;
}
