/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2020
*
*  TITLE:       UI.H
*
*  VERSION:     1.00
*
*  DATE:        10 July 2020
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
#define T_IMS_PROP TEXT("ImsProp")

#define EMPTY_STRING TEXT("")

#define PRINTF_BUFFER_LENGTH 100

#define IMAGESCOPE_WNDTITLE T_PLUGIN_NAME

typedef struct _GUI_CONTEXT {
    INT CurrentDPI;
    HWND MainWindow;
    HWND StatusBar;
    HWND TreeList;
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
    TabSection = 0,
    TabVSInfo = 1,
    TabStrings = 2,
    TabMax
} IMS_TAB_ID;

typedef struct _IMS_TAB {
    UINT ResourceId;
    IMS_TAB_ID TabId;
    WNDPROC WndProc;
    LPTSTR TabCaption;
} IMS_TAB;

typedef struct _TL_SUBITEMS_FIXED {
    ULONG       ColorFlags;
    COLORREF    BgColor;
    COLORREF    FontColor;
    PVOID       UserParam;
    ULONG       Count;
    LPTSTR      Text[2];
} TL_SUBITEMS_FIXED, * PTL_SUBITEMS_FIXED;

typedef struct _VALUE_DESC {
    LPWSTR lpDescription;
    DWORD dwValue;
} VALUE_DESC, * PVALUE_DESC;

LRESULT CALLBACK MainWindowProc(
    _In_ HWND hwnd,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);

BOOL RunUI(
    _In_ GUI_CONTEXT* Context);
