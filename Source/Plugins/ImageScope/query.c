/*******************************************************************************
*
*  (C) COPYRIGHT AUTHORS, 2020
*
*  TITLE:       QUERY.C
*
*  VERSION:     1.00
*
*  DATE:        29 June 2020
*
*  ImageScope main logic.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
* ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
* PARTICULAR PURPOSE.
*
*******************************************************************************/

#include "global.h"

PVOID supHeapAlloc(
    _In_ SIZE_T Size
)
{
    return RtlAllocateHeap(NtCurrentPeb()->ProcessHeap, HEAP_ZERO_MEMORY, Size);
}

BOOL supHeapFree(
    _In_ PVOID Memory
)
{
    return RtlFreeHeap(NtCurrentPeb()->ProcessHeap, 0, Memory);
}

ULONG_PTR FORCEINLINE ALIGN_UP_32(
    _In_ ULONG_PTR p)
{
    return (p + 3) & (~(ULONG_PTR)3);
}

BOOL PEImageEnumVarFileInfo(
    _In_ PIMGVSTRING hdr,
    _In_ PEnumVarInfoCallback vcallback,
    _In_opt_ PVOID cbparam)
{
    ULONG_PTR   vlimit = (ULONG_PTR)hdr + hdr->vshdr.wLength;
    PDWORD      value;
    DWORD       uzero = 0;

    for (
        // first child structure
        hdr = (PIMGVSTRING)ALIGN_UP_32((ULONG_PTR)hdr + sizeof(IMGVARINFO));
        (ULONG_PTR)hdr < vlimit;
        hdr = (PIMGVSTRING)ALIGN_UP_32((ULONG_PTR)hdr + hdr->vshdr.wLength))
    {
        if (hdr->vshdr.wValueLength == 0)
            value = &uzero;
        else
            value = (PDWORD)ALIGN_UP_32((ULONG_PTR)&hdr->szKey + (1 + wcslen(hdr->szKey)) * sizeof(WCHAR));

        if (!vcallback(hdr->szKey, *value, cbparam))
            return FALSE;
    }

    return TRUE;
}

BOOL PEImageEnumStrings(
    _In_ PIMGVSTRING hdr,
    _In_ PEnumStringInfoCallback callback,
    _In_ PWCHAR langid,
    _In_opt_ PVOID cbparam)
{
    ULONG_PTR   vlimit = (ULONG_PTR)hdr + hdr->vshdr.wLength;
    PWCHAR      value;

    for (
        // first child structure
        hdr = (PIMGVSTRING)ALIGN_UP_32((ULONG_PTR)hdr + sizeof(IMGSTRINGTABLE));
        (ULONG_PTR)hdr < vlimit;
        hdr = (PIMGVSTRING)ALIGN_UP_32((ULONG_PTR)hdr + hdr->vshdr.wLength))
    {
        if (hdr->vshdr.wValueLength == 0)
            value = L"";
        else
            value = (PWCHAR)ALIGN_UP_32((ULONG_PTR)&hdr->szKey + (1 + wcslen(hdr->szKey)) * sizeof(WCHAR));

        if (!callback(hdr->szKey, value, langid, cbparam))
            return FALSE;
    }

    return TRUE;
}

BOOL PEImageEnumStringFileInfo(
    _In_ PIMGSTRINGTABLE hdr,
    _In_ PEnumStringInfoCallback callback,
    _In_opt_ PVOID cbparam)
{
    ULONG_PTR   vlimit = (ULONG_PTR)hdr + hdr->vshdr.wLength;

    for (
        // first child structure
        hdr = (PIMGSTRINGTABLE)ALIGN_UP_32((ULONG_PTR)hdr + sizeof(IMGSTRINGINFO));
        (ULONG_PTR)hdr < vlimit;
        hdr = (PIMGSTRINGTABLE)ALIGN_UP_32((ULONG_PTR)hdr + hdr->vshdr.wLength))
    {
        if (!PEImageEnumStrings((PIMGVSTRING)hdr, callback, hdr->wIdKey, cbparam))
            return FALSE;
    }

    return TRUE;
}

VS_FIXEDFILEINFO* PEImageEnumVersionFields(
    _In_ HMODULE module,
    _In_ PEnumStringInfoCallback scallback,
    _In_opt_ PEnumVarInfoCallback vcallback,
    _Inout_opt_ PVOID cbparam)
{
    HGLOBAL     rptr = NULL;
    ULONG_PTR   vlimit, ids[3];

    VS_FIXEDFILEINFO* vinfo = NULL;
    PIMGVSVERSIONINFO   hdr;
    NTSTATUS status;
    SIZE_T datasz = 0;

    do {
        if (!scallback)
            break;

        ids[0] = (ULONG_PTR)RT_VERSION;                     //type
        ids[1] = 1;                                         //id
        ids[2] = MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL); //lang

        status = LdrResSearchResource(module, (ULONG_PTR*)&ids, 3, 0,
            (LPVOID*)&rptr, (ULONG_PTR*)&datasz, NULL, NULL);

        if (!NT_SUCCESS(status)) {
            SetLastError(RtlNtStatusToDosError(status));
            break;
        }

        // root structure
        hdr = (PIMGVSVERSIONINFO)rptr;
        vlimit = (ULONG_PTR)hdr + hdr->vshdr.wLength;

        if (hdr->vshdr.wValueLength)
            vinfo = (VS_FIXEDFILEINFO*)((ULONG_PTR)hdr + sizeof(IMGVSVERSIONINFO));

        for (
            // first child structure
            hdr = (PIMGVSVERSIONINFO)ALIGN_UP_32((ULONG_PTR)hdr + hdr->vshdr.wValueLength + sizeof(IMGVSVERSIONINFO));
            (ULONG_PTR)hdr < vlimit;
            hdr = (PIMGVSVERSIONINFO)ALIGN_UP_32((ULONG_PTR)hdr + hdr->vshdr.wLength))
        {

            if ((_strcmp(hdr->wIdString, L"StringFileInfo") == 0) && scallback)
                if (!PEImageEnumStringFileInfo((PIMGSTRINGTABLE)hdr, scallback, cbparam))
                    break;

            if (vcallback) {
                if ((_strcmp(hdr->wIdString, L"VarFileInfo") == 0))
                    if (!PEImageEnumVarFileInfo((PIMGVSTRING)hdr, vcallback, cbparam))
                        break;
            }
        }

    } while (FALSE);

    return vinfo;
}

/*
* OpenAndMapSection
*
* Purpose:
*
* Open given section and return pointer to it mapped view.
*
*/
NTSTATUS OpenAndMapSection(
    _Out_ HANDLE* BaseAddress,
    _Out_ SIZE_T* ViewSize,
    _In_ LPWSTR RootDirectoryName,
    _In_ LPWSTR ObjectName)
{
    NTSTATUS ntStatus;
    HANDLE dirHandle = NULL, sectionHandle = NULL;
    OBJECT_ATTRIBUTES objAttr;
    UNICODE_STRING uString;
    SECTION_BASIC_INFORMATION sbi;
    SIZE_T bytesReturned;

    *BaseAddress = NULL;
    *ViewSize = 0;

    if ((RootDirectoryName == NULL) ||
        (ObjectName == NULL))
    {
        return STATUS_INVALID_PARAMETER;
    }

    __try {

        RtlInitUnicodeString(&uString, RootDirectoryName);
        InitializeObjectAttributes(&objAttr, &uString, OBJ_CASE_INSENSITIVE, NULL, NULL);

        ntStatus = NtOpenDirectoryObject(&dirHandle,
            DIRECTORY_QUERY, &objAttr);

        if (!NT_SUCCESS(ntStatus))
            __leave;

        RtlInitUnicodeString(&uString, ObjectName);
        objAttr.RootDirectory = dirHandle;

        ntStatus = NtOpenSection(&sectionHandle,
            SECTION_QUERY | SECTION_MAP_READ, &objAttr);

        if (!NT_SUCCESS(ntStatus))
            __leave;

        //
        // Check if this is image mapped file.
        //
        ntStatus = NtQuerySection(sectionHandle,
            SectionBasicInformation,
            (PVOID)&sbi,
            sizeof(sbi),
            &bytesReturned);

        if (!NT_SUCCESS(ntStatus))
            __leave;

        if (!((sbi.AllocationAttributes & SEC_IMAGE) &&
            (sbi.AllocationAttributes & SEC_FILE)))
        {
            ntStatus = STATUS_NOT_SUPPORTED;
            __leave;
        }

        ntStatus = NtMapViewOfSection(sectionHandle,
            NtCurrentProcess(),
            BaseAddress,
            0,
            0,
            NULL,
            ViewSize,
            ViewUnmap,
            0,
            PAGE_READONLY);

    }
    __finally {
        if (AbnormalTermination())
            ntStatus = STATUS_ACCESS_VIOLATION;
        if (dirHandle)
            NtClose(dirHandle);
        if (sectionHandle)
            NtClose(sectionHandle);
    }

    return ntStatus;
}
