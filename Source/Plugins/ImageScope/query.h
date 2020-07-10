/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2020
*
*  TITLE:       QUERY.H
*
*  VERSION:     1.00
*
*  DATE:        10 July 2020
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/

#pragma once

typedef BOOL(CALLBACK* PEnumStringInfoCallback)(
    _In_ PWCHAR key,
    _In_ PWCHAR value,
    _In_ PWCHAR langid,
    _In_opt_ LPVOID cbparam);

typedef BOOL(CALLBACK* PEnumVarInfoCallback)(
    _In_ PWCHAR key,
    _In_ DWORD value,
    _In_opt_ LPVOID cbparam);

typedef struct _IMGVSHDR {
    WORD    wLength;
    WORD    wValueLength;
    WORD    wType;
} IMGVSHDR, * PIMGVSHDR;

typedef struct _IMGVSVERSIONINFO {
    IMGVSHDR    vshdr;
    WCHAR       wIdString[17];
} IMGVSVERSIONINFO, * PIMGVSVERSIONINFO;

typedef struct _IMGSTRINGINFO {
    IMGVSHDR    vshdr;
    WCHAR       wIdKey[15];
} IMGSTRINGINFO, * PIMGSTRINGINFO;

typedef struct _IMGVARINFO {
    IMGVSHDR    vshdr;
    WCHAR       wIdKey[13];
} IMGVARINFO, * PIMGVARINFO;

typedef struct _IMGSTRINGTABLE {
    IMGVSHDR    vshdr;
    WCHAR       wIdKey[9];
} IMGSTRINGTABLE, * PIMGSTRINGTABLE;

typedef struct _IMGVSTRING {
    IMGVSHDR    vshdr;
    WCHAR       szKey[1];
} IMGVSTRING, * PIMGVSTRING;

typedef struct _STRING_PTR {
    ULONG               ofpstr;
    ULONG               length;
    struct _STRING_PTR* pnext;
} STRING_PTR, * PSTRING_PTR;

PVOID supHeapAlloc(
    _In_ SIZE_T Size);

BOOL supHeapFree(
    _In_ PVOID Memory);

VS_FIXEDFILEINFO* PEImageEnumVersionFields(
    _In_ HMODULE module,
    _In_ PEnumStringInfoCallback scallback,
    _In_opt_ PEnumVarInfoCallback vcallback,
    _Inout_opt_ PVOID cbparam);

NTSTATUS OpenSection(
    _Out_ HANDLE* SectionHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ LPWSTR RootDirectoryName,
    _In_ LPWSTR ObjectName);

NTSTATUS OpenAndMapSection(
    _Out_ HANDLE* BaseAddress,
    _Out_ SIZE_T* ViewSize,
    _In_ LPWSTR RootDirectoryName,
    _In_ LPWSTR ObjectName);

PSTRING_PTR EnumImageStringsW(
    _In_ PVOID HeapHandle,
    _In_ PWCHAR Buffer,
    _In_ ULONG Size);

PSTRING_PTR EnumImageStringsA(
    _In_ PVOID HeapHandle,
    _In_ PCHAR Buffer,
    _In_ ULONG Size);

VOID supSetWaitCursor(
    _In_ BOOL fSet);
