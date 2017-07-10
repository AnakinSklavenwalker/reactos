/*
 * PROJECT:         ReactOS Applications Manager
 * LICENSE:         GPL - See COPYING in the top level directory
 * FILE:            base/applications/rapps/settingsdlg.cpp
 * PURPOSE:         Settings Dialog
 * PROGRAMMERS:     Dmitry Chapyshev           (dmitry@reactos.org)
 *                  Alexander Shaposhnikov     (chaez.san@gmail.com)
 */

#include "rapps.h"

SETTINGS_INFO NewSettingsInfo;

#define IS_CHECKED(a, b) \
    a = (SendDlgItemMessage(hDlg, b, BM_GETCHECK, 0, 0) == BST_CHECKED) ? TRUE : FALSE

BOOL
ChooseFolder(HWND hwnd)
{
    BOOL bRet = FALSE;
    BROWSEINFO bi;
    ATL::CStringW szBuf;

    szBuf.LoadStringW(hInst, IDS_CHOOSE_FOLDER_TEXT);

    ZeroMemory(&bi, sizeof(bi));
    bi.hwndOwner = hwnd;
    bi.pidlRoot = NULL;
    bi.lpszTitle = szBuf.GetString();
    bi.ulFlags = BIF_USENEWUI | BIF_DONTGOBELOWDOMAIN | BIF_RETURNONLYFSDIRS | /* BIF_BROWSEFILEJUNCTIONS | */ BIF_VALIDATE;

    szBuf.Empty();
    if (SUCCEEDED(CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
    {
        LPITEMIDLIST lpItemList = SHBrowseForFolder(&bi);
        if (lpItemList && SHGetPathFromIDList(lpItemList, szBuf.GetBuffer(MAX_PATH)))
        {
            szBuf.ReleaseBuffer();
            if (!szBuf.IsEmpty())
            {
                SetDlgItemTextW(hwnd, IDC_DOWNLOAD_DIR_EDIT, szBuf.GetString());
                bRet = TRUE;
            }
        }
        else
            szBuf.ReleaseBuffer();

        CoTaskMemFree(lpItemList);
        CoUninitialize();
    }

    return bRet;
}

static VOID
InitSettingsControls(HWND hDlg, PSETTINGS_INFO Info)
{
    SendDlgItemMessage(hDlg, IDC_SAVE_WINDOW_POS, BM_SETCHECK, Info->bSaveWndPos, 0);
    SendDlgItemMessage(hDlg, IDC_UPDATE_AVLIST, BM_SETCHECK, Info->bUpdateAtStart, 0);
    SendDlgItemMessage(hDlg, IDC_LOG_ENABLED, BM_SETCHECK, Info->bLogEnabled, 0);
    SendDlgItemMessage(hDlg, IDC_DEL_AFTER_INSTALL, BM_SETCHECK, Info->bDelInstaller, 0);

    SetWindowTextW(GetDlgItem(hDlg, IDC_DOWNLOAD_DIR_EDIT),
                   Info->szDownloadDir);

    CheckRadioButton(hDlg, IDC_PROXY_DEFAULT, IDC_USE_PROXY, IDC_PROXY_DEFAULT + Info->Proxy);

    if (IDC_PROXY_DEFAULT + Info->Proxy == IDC_USE_PROXY)
    {
        EnableWindow(GetDlgItem(hDlg, IDC_PROXY_SERVER), TRUE);
        EnableWindow(GetDlgItem(hDlg, IDC_NO_PROXY_FOR), TRUE);
    }

    SetWindowTextW(GetDlgItem(hDlg, IDC_PROXY_SERVER), Info->szProxyServer);
    SetWindowTextW(GetDlgItem(hDlg, IDC_NO_PROXY_FOR), Info->szNoProxyFor);
}

static
INT_PTR CALLBACK
SettingsDlgProc(HWND hDlg, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    switch (Msg)
    {
    case WM_INITDIALOG:
    {
        NewSettingsInfo = SettingsInfo;
        InitSettingsControls(hDlg, &SettingsInfo);
    }
    break;

    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case IDC_CHOOSE:
            ChooseFolder(hDlg);
            break;

        case IDC_SAVE_WINDOW_POS:
            IS_CHECKED(NewSettingsInfo.bSaveWndPos, IDC_SAVE_WINDOW_POS);
            break;

        case IDC_UPDATE_AVLIST:
            IS_CHECKED(NewSettingsInfo.bUpdateAtStart, IDC_UPDATE_AVLIST);
            break;

        case IDC_LOG_ENABLED:
            IS_CHECKED(NewSettingsInfo.bLogEnabled, IDC_LOG_ENABLED);
            break;

        case IDC_DEL_AFTER_INSTALL:
            IS_CHECKED(NewSettingsInfo.bDelInstaller, IDC_DEL_AFTER_INSTALL);
            break;

        case IDC_PROXY_DEFAULT:
            NewSettingsInfo.Proxy = 0;
            EnableWindow(GetDlgItem(hDlg, IDC_PROXY_SERVER), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_NO_PROXY_FOR), FALSE);
            break;

        case IDC_NO_PROXY:
            NewSettingsInfo.Proxy = 1;
            EnableWindow(GetDlgItem(hDlg, IDC_PROXY_SERVER), FALSE);
            EnableWindow(GetDlgItem(hDlg, IDC_NO_PROXY_FOR), FALSE);
            break;

        case IDC_USE_PROXY:
            NewSettingsInfo.Proxy = 2;
            EnableWindow(GetDlgItem(hDlg, IDC_PROXY_SERVER), TRUE);
            EnableWindow(GetDlgItem(hDlg, IDC_NO_PROXY_FOR), TRUE);
            break;

        case IDC_DEFAULT_SETTINGS:
            FillDefaultSettings(&NewSettingsInfo);
            InitSettingsControls(hDlg, &NewSettingsInfo);
            break;

        case IDOK:
        {
            ATL::CStringW szDir;
            ATL::CStringW szProxy;
            ATL::CStringW szNoProxy;
            DWORD dwAttr;

            GetWindowTextW(GetDlgItem(hDlg, IDC_DOWNLOAD_DIR_EDIT),
                           szDir.GetBuffer(MAX_PATH), MAX_PATH);
            szDir.ReleaseBuffer();

            GetWindowTextW(GetDlgItem(hDlg, IDC_PROXY_SERVER),
                           szProxy.GetBuffer(MAX_PATH), MAX_PATH);
            szProxy.ReleaseBuffer();
            ATL::CStringW::CopyChars(NewSettingsInfo.szProxyServer,
                                     _countof(NewSettingsInfo.szProxyServer),
                                     szProxy.GetString(),
                                     szProxy.GetLength() + 1);

            GetWindowTextW(GetDlgItem(hDlg, IDC_NO_PROXY_FOR),
                           szNoProxy.GetBuffer(MAX_PATH), MAX_PATH);
            szNoProxy.ReleaseBuffer();
            ATL::CStringW::CopyChars(NewSettingsInfo.szNoProxyFor,
                                     _countof(NewSettingsInfo.szNoProxyFor),
                                     szNoProxy.GetString(),
                                     szNoProxy.GetLength() + 1);

            dwAttr = GetFileAttributesW(szDir.GetString());
            if (dwAttr != INVALID_FILE_ATTRIBUTES &&
                (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
            {
                ATL::CStringW::CopyChars(NewSettingsInfo.szDownloadDir,
                                         _countof(NewSettingsInfo.szDownloadDir),
                                         szDir.GetString(),
                                         szDir.GetLength() + 1);
            }
            else
            {
                ATL::CStringW szMsgText;
                szMsgText.LoadStringW(hInst, IDS_CHOOSE_FOLDER_ERROR);

                if (MessageBoxW(hDlg, szMsgText.GetString(), NULL, MB_YESNO) == IDYES)
                {
                    if (CreateDirectoryW(szDir.GetString(), NULL))
                    {
                        EndDialog(hDlg, LOWORD(wParam));
                    }
                }
                else
                {
                    SetFocus(GetDlgItem(hDlg, IDC_DOWNLOAD_DIR_EDIT));
                    break;
                }
            }

            SettingsInfo = NewSettingsInfo;
            SaveSettings(GetParent(hDlg));
            EndDialog(hDlg, LOWORD(wParam));
        }
        break;

        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            break;
        }
    }
    break;
    }

    return FALSE;
}

VOID
CreateSettingsDlg(HWND hwnd)
{
    DialogBoxW(hInst,
               MAKEINTRESOURCEW(IDD_SETTINGS_DIALOG),
               hwnd,
               SettingsDlgProc);
}
