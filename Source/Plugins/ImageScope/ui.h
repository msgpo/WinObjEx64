/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2020
*
*  TITLE:       UI.H
*
*  VERSION:     1.00
*
*  DATE:        01 July 2020
*
*  WinObjEx64 ImageScope UI constants, definitions and includes.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/

#pragma once

#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")

#define DefaultSystemDpi            96

#define ScaleDPI(Value, CurrentDPI) MulDiv(Value, CurrentDPI, DefaultSystemDpi)

#define T_PLUGIN_NAME TEXT("ImageScope")

#define IMAGESCOPE_WNDTITLE T_PLUGIN_NAME

typedef struct _GUI_CONTEXT {
    INT CurrentDPI;
    HWND MainWindow;
    HWND StatusBar;
    HANDLE WorkerThread;
    PVOID SectionAddress;
    SIZE_T SectionViewSize;

    TABHDR *TabHeader;

    //
    // WinObjEx64 data and pointers.
    //
    WINOBJEX_PARAM_BLOCK ParamBlock;
} GUI_CONTEXT, *PGUI_CONTEXT;

#define IDC_TAB 8086

typedef enum _IMS_TAB_ID {
    TabVSInfo = 0,
    TabStrings = 1,
    TabMax
} IMS_TAB_ID;

typedef struct _IMS_TAB {
    UINT ResourceId;
    IMS_TAB_ID TabId;
    WNDPROC WndProc;
    LPTSTR TabCaption;
} IMS_TAB;

LRESULT CALLBACK MainWindowProc(
    _In_ HWND hwnd,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);

BOOL RunUI(
    _In_ GUI_CONTEXT* Context);
