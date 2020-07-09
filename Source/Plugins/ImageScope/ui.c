/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2020
*
*  TITLE:       UI.C
*
*  VERSION:     1.00
*
*  DATE:        04 July 2020
*
*  WinObjEx64 ImageScope UI.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/

#include "global.h"

INT_PTR CALLBACK TabsWndProc(
    _In_ HWND hWnd,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam);

IMS_TAB ImsTabs[] = {
    { IDD_TABDLG_SECTION, TabSection, TabsWndProc, TEXT("Section") },
    { IDD_TABDLG_VSINFO, TabVSInfo, TabsWndProc, TEXT("VersionInfo") },
    { IDD_TABDLG_STRINGS, TabStrings, TabsWndProc, TEXT("Strings") }
};

/*
* StatusBarSetText
*
* Purpose:
*
* Display status in status bar part.
*
*/
VOID StatusBarSetText(
    _In_ HWND hwndStatusBar,
    _In_ WPARAM partIndex,
    _In_ LPWSTR lpText
)
{
    SendMessage(hwndStatusBar, SB_SETTEXT, partIndex, (LPARAM)lpText);
}

/*
* AddListViewColumn
*
* Purpose:
*
* Insert list view column.
*
*/
INT AddListViewColumn(
    _In_ HWND ListViewHwnd,
    _In_ INT ColumnIndex,
    _In_ INT SubItemIndex,
    _In_ INT OrderIndex,
    _In_ INT ImageIndex,
    _In_ INT Format,
    _In_ LPWSTR Text,
    _In_ INT Width,
    _In_ INT DpiValue
)
{
    LVCOLUMN column;

    column.mask = LVCF_TEXT | LVCF_SUBITEM | LVCF_FMT | LVCF_WIDTH | LVCF_ORDER | LVCF_IMAGE;
    column.fmt = Format;
    column.cx = ScaleDPI(Width, DpiValue);
    column.pszText = Text;
    column.iSubItem = SubItemIndex;
    column.iOrder = OrderIndex;
    column.iImage = ImageIndex;

    return ListView_InsertColumn(ListViewHwnd, ColumnIndex, &column);
}

/*
* VsInfoStringsEnumCallback
*
* Purpose:
*
* VERSION_INFO enumeration callback.
*
*/
BOOL CALLBACK VsInfoStringsEnumCallback(
    _In_ PWCHAR key,
    _In_ PWCHAR value,
    _In_ PWCHAR langid,
    _In_opt_ LPVOID cbparam
)
{
    LV_ITEM lvItem;
    INT itemIndex;
    HWND hwndList = (HWND)cbparam;
    WCHAR szLangId[128];

    if (hwndList == 0)
        return 0;

    RtlSecureZeroMemory(&lvItem, sizeof(lvItem));
    lvItem.mask = LVIF_TEXT;
    lvItem.pszText = key;
    lvItem.iItem = MAXINT;
    itemIndex = ListView_InsertItem(hwndList, &lvItem);

    lvItem.iSubItem = 1;
    lvItem.pszText = value;
    lvItem.iItem = itemIndex;
    ListView_SetItem(hwndList, &lvItem);

    szLangId[0] = 0;
    StringCchPrintf(szLangId, _countof(szLangId), TEXT("0x%ws"), langid);

    lvItem.iSubItem = 2;
    lvItem.pszText = szLangId;
    lvItem.iItem = itemIndex;
    ListView_SetItem(hwndList, &lvItem);

    return TRUE;//continue enum
}

/*
* VsInfoTabOnInit
*
* Purpose:
*
* Initialize VersionInfo tab dialog page.
*
*/
VOID VsInfoTabOnInit(
    _In_ HWND hWndDlg,
    _In_ GUI_CONTEXT* Context
)
{
    HWND hwndList = GetDlgItem(hWndDlg, IDC_LIST);

    AddListViewColumn(hwndList,
        0,
        0,
        0,
        I_IMAGENONE,
        LVCFMT_LEFT,
        TEXT("Name"),
        120,
        Context->CurrentDPI);

    AddListViewColumn(hwndList,
        1,
        1,
        1,
        I_IMAGENONE,
        LVCFMT_LEFT,
        TEXT("Value"),
        300,
        Context->CurrentDPI);

    AddListViewColumn(hwndList,
        2,
        2,
        2,
        I_IMAGENONE,
        LVCFMT_LEFT,
        TEXT("LangId"),
        100,
        Context->CurrentDPI);

    ListView_SetExtendedListViewStyle(hwndList,
        LVS_EX_FULLROWSELECT | /*LVS_EX_GRIDLINES |*/ LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER);

    SetWindowTheme(hwndList, TEXT("Explorer"), NULL);
    PEImageEnumVersionFields(Context->SectionAddress, &VsInfoStringsEnumCallback, NULL, (LPVOID)hwndList);
}

/*
* SectionTabOnInit
*
* Purpose:
*
* Initialize Section tab dialog page.
*
*/
VOID SectionTabOnInit(
    _In_ HWND hWndDlg,
    _In_ GUI_CONTEXT* Context
)
{
    RECT rc;
    HWND hwndList;
    HDITEM hdritem;

    GetClientRect(hWndDlg, &rc);
    hwndList = CreateWindowEx(WS_EX_STATICEDGE, WC_TREELIST, NULL,
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | TLSTYLE_COLAUTOEXPAND | TLSTYLE_LINKLINES,
        0, 0,
        rc.right, rc.bottom,
        hWndDlg, NULL, NULL, NULL);

    if (hwndList) {

        RtlSecureZeroMemory(&hdritem, sizeof(hdritem));
        hdritem.mask = HDI_FORMAT | HDI_TEXT | HDI_WIDTH;
        hdritem.fmt = HDF_LEFT | HDF_BITMAP_ON_RIGHT | HDF_STRING;
        hdritem.cxy = ScaleDPI(220, Context->CurrentDPI);
        hdritem.pszText = TEXT("Field");
        TreeList_InsertHeaderItem(hwndList, 0, &hdritem);
        hdritem.cxy = ScaleDPI(130, Context->CurrentDPI);
        hdritem.pszText = TEXT("Value");
        TreeList_InsertHeaderItem(hwndList, 1, &hdritem);
        hdritem.cxy = ScaleDPI(210, Context->CurrentDPI);
        hdritem.pszText = TEXT("Additional Information");
        TreeList_InsertHeaderItem(hwndList, 2, &hdritem);

    }

    Context->TreeList = hwndList;

    //
    // TBD FIXME
    //
}

/*
* StringsTabOnShow
*
* Purpose:
*
* Strings page WM_SHOWWINDOW handler.
*
*/
#pragma warning(push)
#pragma warning(disable: 6262)
VOID StringsTabOnShow(
    _In_ HWND hWndDlg,
    _In_ GUI_CONTEXT* Context
)
{
    INT nLength;
    PVOID heapHandle = NULL;
    HWND hwndList = GetDlgItem(hWndDlg, IDC_LIST);
    PSTRING_PTR chain;
    WCHAR szBuffer[UNICODE_STRING_MAX_CHARS];
    LV_ITEM lvItem;

    supSetWaitCursor(TRUE);
    ShowWindow(hwndList, SW_HIDE);

    __try {

        heapHandle = HeapCreate(0, UNICODE_STRING_MAX_CHARS * sizeof(WCHAR), 0);
        if (heapHandle == NULL)
            __leave;

        chain = EnumImageStringsA(
            heapHandle,
            Context->SectionAddress,
            (ULONG)Context->SectionViewSize);

        while (chain) {

            nLength = MultiByteToWideChar(CP_ACP, 0,
                (PCHAR)RtlOffsetToPointer(Context->SectionAddress, chain->ofpstr),
                chain->length,
                szBuffer,
                UNICODE_STRING_MAX_CHARS);

            if (nLength) {
                
                szBuffer[nLength] = 0;

                lvItem.mask = LVIF_TEXT;
                lvItem.pszText = szBuffer;
                lvItem.iItem = INT_MAX;
                ListView_InsertItem(hwndList, &lvItem);
            }

            chain = chain->pnext;
        }

        chain = EnumImageStringsW(
            heapHandle,
            Context->SectionAddress,
            (ULONG)Context->SectionViewSize);

        while (chain) {

            _strncpy(szBuffer,
                UNICODE_STRING_MAX_CHARS,
                (PWCHAR)RtlOffsetToPointer(Context->SectionAddress, chain->ofpstr),
                chain->length);

            lvItem.mask = LVIF_TEXT;
            lvItem.pszText = szBuffer;
            lvItem.iItem = INT_MAX;
            ListView_InsertItem(hwndList, &lvItem);

            chain = chain->pnext;
        }

    }
    __finally {
        supSetWaitCursor(FALSE);
        ShowWindow(hwndList, SW_SHOW);
        if (heapHandle)
            RtlDestroyHeap(heapHandle);
    }
}
#pragma warning(pop)

/*
* StringsTabOnInit
*
* Purpose:
*
* Initialize Strings tab page dialog.
*
*/
VOID StringsTabOnInit(
    _In_ HWND hWndDlg,
    _In_ GUI_CONTEXT* Context
)
{
    HWND hwndList = GetDlgItem(hWndDlg, IDC_LIST);

    if (hwndList) {

        AddListViewColumn(hwndList,
            0,
            0,
            0,
            I_IMAGENONE,
            LVCFMT_LEFT,
            TEXT("Printable strings"),
            MAX_PATH,
            Context->CurrentDPI);

        ListView_SetExtendedListViewStyle(hwndList, LVS_EX_FULLROWSELECT | LVS_EX_DOUBLEBUFFER);
    }
}

/*
* TabOnInit
*
* Purpose:
*
* Tab window WM_INITDIALOG handler.
*
*/
VOID TabOnInit(
    _In_ HWND hWndDlg,
    _In_ GUI_CONTEXT* Context
)
{
    INT iSel;

    iSel = TabCtrl_GetCurSel(Context->TabHeader->hwndTab);

    switch (iSel) {

    case TabSection:
        SectionTabOnInit(hWndDlg, Context);
        break;
    case TabVSInfo:
        VsInfoTabOnInit(hWndDlg, Context);
        break;
    case TabStrings:
        StringsTabOnInit(hWndDlg, Context);
        break;
    default:
        break;
    }
}

/*
* TabOnShow
*
* Purpose:
*
* Tab window WM_SHOWWINDOW handler.
*
*/
INT_PTR TabOnShow(
    _In_ HWND hWndDlg,
    _In_ BOOL fShow
)
{
    INT iSel;
    GUI_CONTEXT* Context = GetProp(hWndDlg, T_IMS_PROP);

    if (Context == NULL)
        return 0;

    iSel = TabCtrl_GetCurSel(Context->TabHeader->hwndTab);

    switch (iSel) {

    case TabStrings:
        if (fShow)
            StringsTabOnShow(hWndDlg, Context);
        break;
    default:
        break;
    }

    return 1;
}

/*
* TabsWndProc
*
* Purpose:
*
* Tab control window handler.
*
*/
INT_PTR CALLBACK TabsWndProc(
    _In_ HWND hWnd,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    switch (uMsg) {

    case WM_INITDIALOG:
        SetProp(hWnd, T_IMS_PROP, (HANDLE)lParam);
        TabOnInit(hWnd, (GUI_CONTEXT*)lParam);
        break;

    case WM_SHOWWINDOW:
        return TabOnShow(hWnd, (wParam != 0));

    case WM_DESTROY:
        RemoveProp(hWnd, T_IMS_PROP);
        break;

    default:
        break;
    }

    return 0;
}

/*
* OnTabResize
*
* Purpose:
*
* Tab window WM_RESIZE handler.
*
*/
VOID CALLBACK OnTabResize(
    _In_ TABHDR* TabHeader
)
{
    RECT hwndRect;
    INT iSel;
    HWND hwndList = 0;
    GUI_CONTEXT* Context;

    iSel = TabCtrl_GetCurSel(TabHeader->hwndTab);
    GetClientRect(TabHeader->hwndDisplay, &hwndRect);

    switch (iSel) {

    case TabSection:
        Context = (GUI_CONTEXT*)GetProp(TabHeader->hwndDisplay, T_IMS_PROP);
        if (Context) {
            hwndList = Context->TreeList;
        }
        break;

    case TabVSInfo:
    case TabStrings:
        hwndList = GetDlgItem(TabHeader->hwndDisplay, IDC_LIST);
        break;

    default:
        return;
    }

    if (hwndList) SetWindowPos(hwndList,
        0,
        0,
        0,
        hwndRect.right,
        hwndRect.bottom,
        SWP_NOOWNERZORDER);
}

/*
* OnTabSelChange
*
* Purpose:
*
* Tab window selection change callback.
*
*/
VOID CALLBACK OnTabSelChange(
    _In_ TABHDR* TabHeader,
    _In_ INT SelectedTab
)
{
    UNREFERENCED_PARAMETER(SelectedTab);

    //destroy previous window
    if (TabHeader->hwndDisplay != NULL)
        DestroyWindow(TabHeader->hwndDisplay);
}

/*
* OnResize
*
* Purpose:
*
* WM_SIZE handler.
*
*/
VOID OnResize(
    _In_ HWND hWnd
)
{
    GUI_CONTEXT* Context;
    RECT r, szr;

    Context = (GUI_CONTEXT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (Context) {

        SendMessage(Context->StatusBar, WM_SIZE, 0, 0);
        RedrawWindow(Context->StatusBar, NULL, 0, RDW_ERASE | RDW_INVALIDATE | RDW_ERASENOW);

        GetClientRect(hWnd, &r);
        GetClientRect(Context->StatusBar, &szr);

        //resize of the tab control
        if (Context->TabHeader != NULL) {

            SetWindowPos(Context->TabHeader->hwndTab, HWND_TOP,
                0, 0, r.right, r.bottom - szr.bottom, 0);

            TabResizeTabWindow(Context->TabHeader);

            UpdateWindow(Context->TabHeader->hwndDisplay);

        }
    }
}

/*
* OnNotify
*
* Purpose:
*
* WM_NOTIFY handler.
*
*/
VOID OnNotify(
    _In_ HWND hWnd,
    _In_ LPNMHDR nmhdr
)
{
    GUI_CONTEXT* Context;

    if (g_PluginQuit)
        return;

    Context = (GUI_CONTEXT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (Context) {
        TabOnChangeTab(Context->TabHeader, nmhdr);
    }
}

VOID OnGetMinMax(
    _In_ HWND hWnd,
    _In_ PMINMAXINFO mmInfo
)
{
    GUI_CONTEXT* Context;
    Context = (GUI_CONTEXT*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if (Context) {
        mmInfo->ptMinTrackSize.x = ScaleDPI(640, Context->CurrentDPI);
        mmInfo->ptMinTrackSize.y = ScaleDPI(480, Context->CurrentDPI);
    }
}

/*
* MainWindowProc
*
* Purpose:
*
* Main window procedure.
*
*/
LRESULT CALLBACK MainWindowProc(
    _In_ HWND hWnd,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    switch (uMsg) {

    case WM_CLOSE:
        PostQuitMessage(0);
        break;

    case WM_SIZE:
        OnResize(hWnd);
        break;

    case WM_NOTIFY:
        OnNotify(hWnd, (LPNMHDR)lParam);
        break;

    case WM_GETMINMAXINFO:
        if (lParam) {
            OnGetMinMax(hWnd, (PMINMAXINFO)lParam);
        }
        break;

    default:
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/*
* RunUI
*
* Purpose:
*
* Create main window, run message loop.
*
*/
BOOL RunUI(
    _In_ GUI_CONTEXT* Context
)
{
    INT i;
    INITCOMMONCONTROLSEX icex;

    BOOL rv, mAlloc = FALSE;
    MSG msg1;
    SIZE_T sz;
    LPWSTR lpTitle;
    WCHAR szClassName[100];

    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_TREEVIEW_CLASSES | ICC_BAR_CLASSES | ICC_TAB_CLASSES;
    InitCommonControlsEx(&icex);

#pragma warning(push)
#pragma warning(disable: 6031)
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
#pragma warning(pop)

    Context->CurrentDPI = Context->ParamBlock.uiGetDPIValue(NULL);

    //
    // Window class once.
    //
    StringCchPrintf(szClassName,
        RTL_NUMBER_OF(szClassName),
        TEXT("%wsWndClass"),
        g_Plugin->Name);

    sz = (MAX_PATH +
        _strlen(Context->ParamBlock.Object.ObjectDirectory) +
        _strlen(Context->ParamBlock.Object.ObjectName)) * sizeof(WCHAR);

    lpTitle = supHeapAlloc(sz);
    if (lpTitle) {

        StringCchPrintf(lpTitle,
            sz / sizeof(WCHAR),
            TEXT("Viewing :: %ws\\%ws"),
            Context->ParamBlock.Object.ObjectDirectory,
            Context->ParamBlock.Object.ObjectName);

        mAlloc = TRUE;
    }
    else
        lpTitle = IMAGESCOPE_WNDTITLE;

    //
    // Create main window.
    //
    Context->MainWindow = CreateWindowEx(
        0,
        szClassName,
        lpTitle,
        WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        ScaleDPI(640, Context->CurrentDPI),
        ScaleDPI(480, Context->CurrentDPI),
        NULL,
        NULL,
        g_ThisDLL,
        NULL);

    if (mAlloc)
        supHeapFree(lpTitle);

    if (Context->MainWindow == 0) {
        kdDebugPrint("Could not create main window, err = %lu\r\n", GetLastError());
        return FALSE;
    }

    SetWindowLongPtr(Context->MainWindow, GWLP_USERDATA, (LONG_PTR)Context);

    //
    // Status Bar window.
    //
    Context->StatusBar = CreateWindowEx(
        0,
        STATUSCLASSNAME,
        NULL,
        WS_VISIBLE | WS_CHILD,
        0,
        0,
        0,
        0,
        Context->MainWindow,
        NULL,
        g_ThisDLL,
        NULL);

    if (Context->StatusBar == 0) {
        kdDebugPrint("Could not create statusbar window, err = %lu\r\n", GetLastError());
        return FALSE;
    }

    Context->TabHeader = TabCreateControl(
        g_ThisDLL,
        Context->MainWindow,
        NULL,
        (TABSELCHANGECALLBACK)&OnTabSelChange,
        (TABRESIZECALLBACK)&OnTabResize,
        (TABCALLBACK_ALLOCMEM)&supHeapAlloc,
        (TABCALLBACK_FREEMEM)&supHeapFree);

    if (Context->TabHeader == NULL) {
        kdDebugPrint("Could not create tabcontrol window\r\n");
        return FALSE;
    }

    for (i = 0; i < _countof(ImsTabs); i++) {

        TabAddPage(Context->TabHeader,
            ImsTabs[i].ResourceId,
            ImsTabs[i].WndProc,
            ImsTabs[i].TabCaption,
            I_IMAGENONE,
            (LPARAM)Context);

    }

    TabOnSelChanged(Context->TabHeader);

    //call resize
    SendMessage(Context->MainWindow, WM_SIZE, 0, 0);

    do {
        rv = GetMessage(&msg1, NULL, 0, 0);

        if (rv == -1)
            break;

        TranslateMessage(&msg1);
        DispatchMessage(&msg1);

    } while ((rv != 0) && (g_PluginQuit == FALSE));

    DestroyWindow(Context->MainWindow);

    return TRUE;
}
