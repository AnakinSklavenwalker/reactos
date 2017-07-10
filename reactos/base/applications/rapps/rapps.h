#pragma once

#ifndef _RAPPS_H
#define _RAPPS_H

#include <tchar.h>
#include <stdarg.h>

#define WIN32_NO_STATUS
#define _INC_WINDOWS
#define COM_NO_WINDOWS_H
#define COBJMACROS

#include <windef.h>
#include <winbase.h>
#include <winreg.h>
#include <wingdi.h>
#include <winnls.h>
#include <winuser.h>
#include <wincon.h>
#include <richedit.h>
#include <shellapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <stdio.h>
#include <strsafe.h>
#include <ndk/rtlfuncs.h>
#include <atlcoll.h>
#include <atlstr.h> 
#include <rappsmsg.h>

#include "resource.h"

#ifdef USE_CERT_PINNING
  #define CERT_ISSUER_INFO "BE\r\nGlobalSign nv-sa\r\nGlobalSign Domain Validation CA - SHA256 - G2"
  #define CERT_SUBJECT_INFO "Domain Control Validated\r\n*.reactos.org"
#endif

#define APPLICATION_DATABASE_URL L"https://svn.reactos.org/packages/rappmgr.cab"

#define SPLIT_WIDTH 4
#define MAX_STR_LEN 256
#define LISTVIEW_ICON_SIZE 24
#define TREEVIEW_ICON_SIZE 24

/* EnumType flags for EnumInstalledApplications */
#define ENUM_ALL_COMPONENTS    30
#define ENUM_APPLICATIONS      31
#define ENUM_UPDATES           32
/* EnumType flags for EnumAvailableApplications */
#define ENUM_ALL_AVAILABLE     0
#define ENUM_CAT_AUDIO         1
#define ENUM_CAT_VIDEO         2
#define ENUM_CAT_GRAPHICS      3
#define ENUM_CAT_GAMES         4
#define ENUM_CAT_INTERNET      5
#define ENUM_CAT_OFFICE        6
#define ENUM_CAT_DEVEL         7
#define ENUM_CAT_EDU           8
#define ENUM_CAT_ENGINEER      9
#define ENUM_CAT_FINANCE       10
#define ENUM_CAT_SCIENCE       11
#define ENUM_CAT_TOOLS         12
#define ENUM_CAT_DRIVERS       13
#define ENUM_CAT_LIBS          14
#define ENUM_CAT_OTHER         15

#define ENUM_INSTALLED_MIN ENUM_ALL_COMPONENTS
#define ENUM_INSTALLED_MAX ENUM_UPDATES
#define ENUM_AVAILABLE_MIN ENUM_ALL_AVAILABLE
#define ENUM_AVAILABLE_MAX ENUM_CAT_OTHER

#define IS_INSTALLED_ENUM(a) (a >= ENUM_INSTALLED_MIN && a <= ENUM_INSTALLED_MAX)
#define IS_AVAILABLE_ENUM(a) (a >= ENUM_AVAILABLE_MIN && a <= ENUM_AVAILABLE_MAX)

typedef enum
{
    None,
    OpenSource,
    Freeware,
    Trial,
    Max = Trial,
    Min = None
} LICENSE_TYPE, *PLICENSE_TYPE;

/* aboutdlg.cpp */
VOID ShowAboutDialog(VOID);

/* available.cpp */
typedef struct
{
    INT Category;
    LICENSE_TYPE LicenseType;
    ATL::CStringW szName;
    ATL::CStringW szRegName;
    ATL::CStringW szVersion;
    ATL::CStringW szLicense;
    ATL::CStringW szDesc;
    ATL::CStringW szSize;
    ATL::CStringW szUrlSite;
    ATL::CStringW szUrlDownload;
    ATL::CStringW szCDPath;
    ATL::CStringW szLanguages;

    /* caching mechanism related entries */
    ATL::CStringW cFileName;
    FILETIME ftCacheStamp;

    /* optional integrity checks (SHA-1 digests are 160 bit = 40 characters in hex string form) */
    ATL::CStringW szSHA1;

} APPLICATION_INFO, *PAPPLICATION_INFO;

extern ATL::CAtlList<PAPPLICATION_INFO> InfoList;

typedef struct
{
    HKEY hRootKey;
    HKEY hSubKey;
    ATL::CStringW szKeyName;

} INSTALLED_INFO, *PINSTALLED_INFO;

typedef struct
{
    BOOL bSaveWndPos;
    BOOL bUpdateAtStart;
    BOOL bLogEnabled;
    WCHAR szDownloadDir[MAX_PATH];
    BOOL bDelInstaller;
    /* Window Pos */
    BOOL Maximized;
    INT Left;
    INT Top;
    INT Width;
    INT Height;
    /* Proxy settings */
    INT Proxy;
    WCHAR szProxyServer[MAX_PATH];
    WCHAR szNoProxyFor[MAX_PATH];

} SETTINGS_INFO, *PSETTINGS_INFO;

/* available.cpp */
typedef BOOL (CALLBACK *AVAILENUMPROC)(PAPPLICATION_INFO Info);
BOOL EnumAvailableApplications(INT EnumType, AVAILENUMPROC lpEnumProc);
BOOL ShowAvailableAppInfo(INT Index);
BOOL UpdateAppsDB(VOID);
VOID FreeCachedAvailableEntries(VOID);

/* installdlg.cpp */
BOOL InstallApplication(INT Index);

/* installed.cpp */
typedef BOOL (CALLBACK *APPENUMPROC)(INT ItemIndex, ATL::CStringW &lpName, PINSTALLED_INFO Info);
BOOL EnumInstalledApplications(INT EnumType, BOOL IsUserKey, APPENUMPROC lpEnumProc);
BOOL GetApplicationString(HKEY hKey, LPCWSTR lpKeyName, LPWSTR szString);
BOOL GetApplicationString(HKEY hKey, LPCWSTR RegName, ATL::CStringW& String);

BOOL ShowInstalledAppInfo(INT Index);
BOOL UninstallApplication(INT Index, BOOL bModify);
BOOL IsInstalledApplication(const ATL::CStringW& RegName, BOOL IsUserKey, REGSAM keyWow);
VOID RemoveAppFromRegistry(INT Index);

BOOL InstalledVersion(ATL::CStringW& szVersionResult, const ATL::CStringW& RegName, BOOL IsUserKey, REGSAM keyWow);

/* winmain.cpp */
extern HWND hMainWnd;
extern HINSTANCE hInst;
extern INT SelectedEnumType;
extern SETTINGS_INFO SettingsInfo;
VOID SaveSettings(HWND hwnd);
VOID FillDefaultSettings(PSETTINGS_INFO pSettingsInfo);

/* loaddlg.cpp */
BOOL DownloadApplication(INT Index);
VOID DownloadApplicationsDB(LPCWSTR lpUrl);

/* misc.cpp */
INT GetSystemColorDepth(VOID);
int GetWindowWidth(HWND hwnd);
int GetWindowHeight(HWND hwnd);
int GetClientWindowWidth(HWND hwnd);
int GetClientWindowHeight(HWND hwnd);
VOID CopyTextToClipboard(LPCWSTR lpszText);
VOID SetWelcomeText(VOID);
VOID ShowPopupMenu(HWND hwnd, UINT MenuID, UINT DefaultItem);
BOOL StartProcess(ATL::CStringW & Path, BOOL Wait);
BOOL StartProcess(LPWSTR lpPath, BOOL Wait);
BOOL GetStorageDirectory(ATL::CStringW &lpDirectory);
BOOL ExtractFilesFromCab(LPCWSTR lpCabName, LPCWSTR lpOutputPath);
VOID InitLogs(VOID);
VOID FreeLogs(VOID);
BOOL WriteLogMessage(WORD wType, DWORD dwEventID, LPCWSTR lpMsg);

UINT ParserGetString(const ATL::CStringW& KeyName, const ATL::CStringW& FileName, ATL::CStringW& ReturnedString);
UINT ParserGetInt(const ATL::CStringW& KeyName, const ATL::CStringW& FileName);

/* settingsdlg.cpp */
VOID CreateSettingsDlg(HWND hwnd);

/* gui.cpp */
HWND CreateMainWindow();
DWORD_PTR ListViewGetlParam(INT item);
INT ListViewAddItem(INT ItemIndex, INT IconIndex, LPWSTR lpName, LPARAM lParam);
VOID SetStatusBarText(LPCWSTR szText);
VOID NewRichEditText(LPCWSTR szText, DWORD flags);
VOID InsertRichEditText(LPCWSTR szText, DWORD flags);

VOID SetStatusBarText(const ATL::CStringW& szText);
INT ListViewAddItem(INT ItemIndex, INT IconIndex, ATL::CStringW & Name, LPARAM lParam);
VOID NewRichEditText(const ATL::CStringW& szText, DWORD flags);
VOID InsertRichEditText(const ATL::CStringW& szText, DWORD flags);
extern HWND hListView;
extern ATL::CStringW szSearchPattern;

/* integrity.cpp */
BOOL VerifyInteg(LPCWSTR lpSHA1Hash, LPCWSTR lpFileName);

//extern HWND hTreeView;
//BOOL CreateTreeView(HWND hwnd);
//HTREEITEM TreeViewAddItem(HTREEITEM hParent, LPWSTR lpText, INT Image, INT SelectedImage, LPARAM lParam);

#endif /* _RAPPS_H */
