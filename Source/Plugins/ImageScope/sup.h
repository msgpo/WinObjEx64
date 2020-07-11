/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2020
*
*  TITLE:       SUP.H
*
*  VERSION:     1.87
*
*  DATE:        11 July 2020
*
*  Common header file for the plugin support routines.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/
#pragma once

PVOID supHeapAlloc(
    _In_ SIZE_T Size);

BOOL supHeapFree(
    _In_ PVOID Memory);

PVOID supVirtualAllocEx(
    _In_ SIZE_T Size,
    _In_ ULONG AllocationType,
    _In_ ULONG Protect);

PVOID supVirtualAlloc(
    _In_ SIZE_T Size);

BOOL supVirtualFree(
    _In_ PVOID Memory);

VOID supSetWaitCursor(
    _In_ BOOL fSet);

NTSTATUS supOpenSection(
    _Out_ HANDLE* SectionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ LPWSTR RootDirectoryName,
    _In_ LPWSTR ObjectName);

NTSTATUS supOpenAndMapSection(
    _Out_ HANDLE* BaseAddress,
    _Out_ SIZE_T* ViewSize,
    _In_ LPWSTR RootDirectoryName,
    _In_ LPWSTR ObjectName);

BOOL supSaveDialogExecute(
    _In_ HWND OwnerWindow,
    _Inout_ LPWSTR SaveFileName,
    _In_ LPWSTR lpDialogFilter);

BOOL supListViewExportToFile(
    _In_ LPWSTR FileName,
    _In_ HWND WindowHandle,
    _In_ HWND ListView,
    _In_ INT ColumnCount);

VOID supStatusBarSetText(
    _In_ HWND hwndStatusBar,
    _In_ WPARAM partIndex,
    _In_ LPWSTR lpText);

HTREEITEM supTreeListAddItem(
    _In_ HWND TreeList,
    _In_opt_ HTREEITEM hParent,
    _In_ UINT mask,
    _In_ UINT state,
    _In_ UINT stateMask,
    _In_opt_ LPWSTR pszText,
    _In_opt_ PVOID subitems);

INT supAddListViewColumn(
    _In_ HWND ListViewHwnd,
    _In_ INT ColumnIndex,
    _In_ INT SubItemIndex,
    _In_ INT OrderIndex,
    _In_ INT ImageIndex,
    _In_ INT Format,
    _In_ LPWSTR Text,
    _In_ INT Width,
    _In_ INT DpiValue);
