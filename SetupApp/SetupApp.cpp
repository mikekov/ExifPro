// SetupApp: ExifPro installation routines
//

#include "stdafx.h"
#include "resource.h"

#define EP_VER _T("2.3")

static HINSTANCE g_instance_handle= 0;
static bool g_unicode_system= false;
static const TCHAR* g_EXIF_APP= _T("ExifPro.exe");
static const TCHAR* g_EXIF_SYM= _T("ExifPro.pdb");
static const TCHAR* g_EXIF_HELP= _T("ExifPro.chm");
static const TCHAR* g_UNINST_APP= _T("UnInstall.exe");
static const TCHAR* g_COMM_CTL_UPD= _T("50comupd.exe");
static const TCHAR* g_INSTALLER= _T("ExifPro Installer");
static const TCHAR* g_UNINST_KEY= _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ExifPro ") EP_VER;
#define PACKVERSION(major,minor) MAKELONG(minor,major)
bool g_update_common_ctrls= false;
static const TCHAR* g_TEMPLATE_1= _T("Frames.html");
static const TCHAR* g_TEMPLATE_2= _T("Grid.html");
static const TCHAR* g_TEMPLATE_3= _T("FrameImage.html");
static const TCHAR* g_TEMPLATE_4= _T("GridImage.html");
static const TCHAR* g_TEMPLATE_5= _T("FrameSet.html");
static bool g_IsWindows64= false;


bool IsWow64()
{
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	BOOL is_wow_64= FALSE;

	LPFN_ISWOW64PROCESS IsWow64ProcessFn= (LPFN_ISWOW64PROCESS)::GetProcAddress(::GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (IsWow64ProcessFn != nullptr)
	{
		if (!IsWow64ProcessFn(GetCurrentProcess(), &is_wow_64))
		{
			// handle error
		}
	}

	return is_wow_64 != 0;
}

typedef HRESULT (__stdcall *fnSHAutoComplete)(HWND edit, DWORD flags);

static fnSHAutoComplete FindSHAutoCompleteFn()
{
	if (HINSTANCE hinstDll= ::LoadLibrary(_T("Shlwapi.dll")))
	{
		// look for SHAutoComplete function

		fnSHAutoComplete pfnAutoComplete= reinterpret_cast<fnSHAutoComplete>(::GetProcAddress(hinstDll, "SHAutoComplete"));

		if (pfnAutoComplete != nullptr)
			return pfnAutoComplete;	// return fn address, do not free loaded library

		::FreeLibrary(hinstDll);
	}

	return nullptr;
}

static fnSHAutoComplete pfnAutoCompleteFn= FindSHAutoCompleteFn();


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     pCmdLine,
                     int       nCmdShow)
{
	INT_PTR CALLBACK DialogProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	g_instance_handle = hInstance;
	g_unicode_system = ::GetVersion() < DWORD(0x80000000);

	if (!g_unicode_system)
	{
		::MessageBox(0, _T("Windows 9x/Me is not supported by this version of ExifPro. Please use ExifPro version 1.0."), g_INSTALLER, MB_ICONERROR | MB_OK);
		return 0;
	}

	g_IsWindows64 = IsWow64();

	::CoInitialize(nullptr);

	::DialogBox(hInstance, MAKEINTRESOURCE(IDD_SETUP), nullptr, DialogProc);

	::CoUninitialize();

	return 0;
}


DWORD GetDllVersion(LPCTSTR lpszDllName)
{
	DWORD version= 0;

	if (HINSTANCE dll= ::LoadLibrary(lpszDllName))
	{
		DLLGETVERSIONPROC DllGetVersionFn= reinterpret_cast<DLLGETVERSIONPROC>(::GetProcAddress(dll, "DllGetVersion"));

		// Because some DLLs might not implement this function, you
		// must test for it explicitly. Depending on the particular 
		// DLL, the lack of a DllGetVersion function can be a useful
		// indicator of the version.

		if (DllGetVersionFn)
		{
			DLLVERSIONINFO dvi;
			ZeroMemory(&dvi, sizeof dvi);
			dvi.cbSize = sizeof dvi;

			if (SUCCEEDED(DllGetVersionFn(&dvi)))
				version = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);
		}

		::FreeLibrary(dll);
	}

	return version;
}


int CALLBACK BrowseCallbackProc(HWND hWnd, UINT msg, LPARAM lParam, LPARAM lpData)
{
	switch (msg)
	{
	case BFFM_INITIALIZED:
		::SendMessage(hWnd, BFFM_SETSELECTION, true, lpData);
		break;
	}
	return 0;
}


bool DoSelect(HWND hWnd, const TCHAR* title, const TCHAR* folder, TCHAR* output)
{
	TCHAR buf[MAX_PATH + 4];
	BROWSEINFO bi;
	bi.hwndOwner      = hWnd;
	bi.pidlRoot       = NULL;
	bi.pszDisplayName = buf;
	bi.lpszTitle      = title;
	bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_VALIDATE | BIF_EDITBOX | BIF_NEWDIALOGSTYLE; // | BIF_STATUSTEXT;
	bi.lpfn           = BrowseCallbackProc;
	bi.lParam         = reinterpret_cast<LPARAM>(const_cast<TCHAR*>(folder));
	bi.iImage         = 0;

	ITEMIDLIST* item_list= ::SHBrowseForFolder(&bi);

	if (item_list == NULL)
		return false;

	*output = 0;
	::SHGetPathFromIDList(item_list, output);

	IMallocPtr malloc;
	if (::SHGetMalloc(&malloc) == NOERROR)
		malloc->Free(item_list);

	return true;
}


void BrowseDir(HWND hWnd)
{
	const int MAX= MAX_PATH * 2;
	TCHAR path[MAX];
	::GetDlgItemText(hWnd, IDC_FOLDER, path, MAX);

	if (::GetFileAttributes(path) == 0-1)
	{
		if (TCHAR* sep= _tcsrchr(path, '\\'))
			*sep = 0;
	}

	TCHAR selected[MAX];
	if (!DoSelect(hWnd, _T("Select ExifPro destination folder:"), path, selected))
		return;

	::SetDlgItemText(hWnd, IDC_FOLDER, selected);
}


// open file stored in resources and copy it to the designated place
//
bool CopyRsrcFile(int rsc_id, const TCHAR* dest_file)
{
	const TCHAR* resource_name= MAKEINTRESOURCE(rsc_id);
	const TCHAR* resource_type= _T("FILE");

	HRSRC rsrc= ::FindResource(g_instance_handle, resource_name, resource_type);
	if (rsrc == NULL)
		return false;

	HGLOBAL global= ::LoadResource(g_instance_handle, rsrc);
	if (global == NULL)
		return false;

	DWORD size= ::SizeofResource(g_instance_handle, rsrc);

	BYTE* data= reinterpret_cast<BYTE*>(::LockResource(global));
	if (data == NULL)
		return false;

	HANDLE file= ::CreateFile(dest_file, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

	if (file == INVALID_HANDLE_VALUE)
		return false;

	DWORD written= 0;
	bool ok= false;
	if (::WriteFile(file, data, size, &written, 0) && written == size)
		ok = true;	// so far OK

	if (!::CloseHandle(file))
		ok = false;

	if (ok)
		return true;	// file copied

	::DeleteFile(dest_file);

	return false;
}


// add uninstall entry for 'add/remove programs' applet
//
bool CreateUninstallKey(const TCHAR* uninst_app)
{
	HKEY key= 0;
	if (::RegCreateKey(HKEY_LOCAL_MACHINE, g_UNINST_KEY, &key) != ERROR_SUCCESS)
		return false;

	TCHAR* name= _T("ExifPro ") EP_VER _T(" Photo Viewer");
	if (::RegSetValueEx(key, _T("DisplayName"), 0, REG_SZ, reinterpret_cast<const BYTE*>(name), _tcslen(name) * sizeof(name[0])) != ERROR_SUCCESS)
	{
		::RegCloseKey(key);
		return false;
	}

	if (::RegSetValueEx(key, _T("UninstallString"), 0, REG_SZ, reinterpret_cast<const BYTE*>(uninst_app), _tcslen(uninst_app) * sizeof(uninst_app[0])) != ERROR_SUCCESS)
	{
		::RegCloseKey(key);
		return false;
	}

	::RegCloseKey(key);

	return true;
}


TCHAR* AppendFileName(const TCHAR* dest_dir, const TCHAR* file_name, TCHAR* out)
{
	const size_t max= MAX_PATH * 2;
	_tcscpy_s(out, max, dest_dir);
	int len= _tcslen(out);

	if (len > 0 && out[len - 1] != _T('\\') && out[len - 1] != _T('/'))
		_tcscat_s(out, max, _T("\\"));

	_tcscat_s(out, max, file_name);

	return out;
}


bool CopyFileToDir(const TCHAR* dest_dir, const TCHAR* file_name, int rsrc_id)
{
	TCHAR path[MAX_PATH * 2];

	AppendFileName(dest_dir, file_name, path);

	return CopyRsrcFile(rsrc_id, path);
}


bool CopyExifFiles(const TCHAR* dest_dir)
{
	TCHAR dir[MAX_PATH * 2];
	AppendFileName(dest_dir, _T("Templates"), dir);
	DWORD attribs= ::GetFileAttributes(dir);
	if (attribs == ~0 || (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0)
		if (!::CreateDirectory(dir, 0))
			return false;

	if (!CopyFileToDir(dest_dir, g_EXIF_APP, g_IsWindows64 ? IDR_EXIFPRO_x64 : IDR_EXIFPRO_x86))
		return false;
	if (!CopyFileToDir(dest_dir, g_EXIF_SYM, g_IsWindows64 ? IDR_EXIFPRO_x64_DBG : IDR_EXIFPRO_x86_DBG))
		return false;
	if (!CopyFileToDir(dest_dir, g_EXIF_HELP, IDR_EXIFPRO_HELP))
		return false;
	if (!CopyFileToDir(dest_dir, g_UNINST_APP, IDR_UNINSTALL))
		return false;
	if (!CopyFileToDir(dest_dir, g_COMM_CTL_UPD, IDR_COMM_CTL_UPD))
		return false;

	// HTML template files
	if (!CopyFileToDir(dir, g_TEMPLATE_1, IDR_TEMPLATE_1))
		return false;
	if (!CopyFileToDir(dir, g_TEMPLATE_2, IDR_TEMPLATE_2))
		return false;
	if (!CopyFileToDir(dir, g_TEMPLATE_3, IDR_TEMPLATE_3))
		return false;
	if (!CopyFileToDir(dir, g_TEMPLATE_4, IDR_TEMPLATE_4))
		return false;
	if (!CopyFileToDir(dir, g_TEMPLATE_5, IDR_TEMPLATE_5))
		return false;

	return true;
}


void RemoveFileFromDir(const TCHAR* dest_dir, const TCHAR* file_name)
{
	TCHAR path[MAX_PATH * 2];

	AppendFileName(dest_dir, file_name, path);

	::DeleteFile(path);
}


void RemoveExifFiles(const TCHAR* dest_dir)
{
	RemoveFileFromDir(dest_dir, g_EXIF_APP);
	RemoveFileFromDir(dest_dir, g_EXIF_HELP);
	RemoveFileFromDir(dest_dir, g_UNINST_APP);
	RemoveFileFromDir(dest_dir, g_COMM_CTL_UPD);
}


bool CreateUninstallLinkInfo(const TCHAR* link, const TCHAR* link_key)
{
	HKEY key= 0;
	if (::RegOpenKey(HKEY_LOCAL_MACHINE, g_UNINST_KEY, &key) != ERROR_SUCCESS)
		return false;

	bool result= ::RegSetValueEx(key, link_key, 0, REG_SZ, reinterpret_cast<const BYTE*>(link), _tcslen(link) * sizeof(link[0])) == ERROR_SUCCESS;

	::RegCloseKey(key);

	return result;
}


HRESULT CreateLink(const TCHAR* path_to_obj, const TCHAR* path_link, const TCHAR* description)
{
	IShellLinkPtr shell_link;
	if (HRESULT hr= shell_link.CreateInstance(CLSID_ShellLink))
		return hr;

	shell_link->SetPath(path_to_obj);
	shell_link->SetDescription(description);

	IPersistFilePtr file;
	if (HRESULT hr= shell_link.QueryInterface(IID_IPersistFile, &file))
		return hr;

#ifndef _UNICODE
	WCHAR path[MAX_PATH];
	::MultiByteToWideChar(CP_ACP, 0, path_link, -1, path, MAX_PATH);
	return file->Save(path, true);
#else
	return file->Save(path_link, true);
#endif
}


bool CreateLinkToExifPro(const TCHAR* exif_pro_path, const TCHAR* description, const TCHAR* link_name, int special_folder_id)
{
	ITEMIDLIST* idl= 0;
	if (::SHGetSpecialFolderLocation(0, special_folder_id, &idl) == 0)
	{
		TCHAR path[2 * MAX_PATH];
		::SHGetPathFromIDList(idl, path);

		IMallocPtr malloc;
		if (::SHGetMalloc(&malloc) == 0)
			malloc->Free(idl);

		TCHAR link[2 * MAX_PATH];
		AppendFileName(path, link_name, link);

		if (CreateLink(exif_pro_path, link, description) != S_OK)
			return false;

		TCHAR key[MAX_PATH];
		wsprintf(key, L"UninstLink_%d", special_folder_id);
		CreateUninstallLinkInfo(link, key);

		return true;
	}

	return false;
}


bool InstallExifPro(const TCHAR* dest_dir, bool add_desk_icon, bool add_prog_icon)
{
	DWORD attribs= ::GetFileAttributes(dest_dir);
	if (attribs == INVALID_FILE_ATTRIBUTES || (attribs & FILE_ATTRIBUTE_DIRECTORY) == 0)
	{
		if (!::CreateDirectory(dest_dir, 0))
		{
			::MessageBox(0, _T("Cannot create destination folder."), g_INSTALLER, MB_ICONERROR | MB_OK);
			return false;
		}
	}

	if (!CopyExifFiles(dest_dir))
	{
		RemoveExifFiles(dest_dir);

		::MessageBox(0, _T("Copying files failed."), g_INSTALLER, MB_ICONERROR | MB_OK);
		return false;
	}

	TCHAR uninstall[MAX_PATH * 2];
	AppendFileName(dest_dir, g_UNINST_APP, uninstall);

	if (!CreateUninstallKey(uninstall))
	{
		RemoveExifFiles(dest_dir);

		::MessageBox(0, _T("Cannot create registry entries."), g_INSTALLER, MB_ICONERROR | MB_OK);
		return false;
	}

	TCHAR exif[MAX_PATH * 2]= { 0 };
	AppendFileName(dest_dir, g_EXIF_APP, exif);

	const TCHAR* exif_link_name= _T("ExifPro ") EP_VER _T(".lnk");
	const TCHAR* exif_description= _T("ExifPro ") EP_VER _T(" Photo Browser & Viewer");

	// add desk icon?
	if (add_desk_icon)
		CreateLinkToExifPro(exif, exif_description, exif_link_name, CSIDL_DESKTOP);

	// add programs icon?
	if (add_prog_icon)
		CreateLinkToExifPro(exif, exif_description, exif_link_name, CSIDL_PROGRAMS);

	// verify comm ctl lib version
	DWORD version= GetDllVersion(_T("comctl32.dll"));

	if (g_update_common_ctrls)
	{
		// execute common ctl update

		TCHAR update[MAX_PATH * 2]= { 0 };
		AppendFileName(dest_dir, g_COMM_CTL_UPD, update);

		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof sei);

		sei.cbSize = sizeof sei;
		sei.fMask = SEE_MASK_NOCLOSEPROCESS;
		sei.hwnd = 0;
		sei.lpVerb = _T("open");
		sei.lpFile = update;
		sei.lpParameters = _T("/Q");
		sei.lpDirectory = dest_dir;
		sei.nShow = SW_MINIMIZE;
		if (!::ShellExecuteEx(&sei) || sei.hProcess == 0)
		{
			::MessageBox(0, _T("Cannot execute CommCtl32 updater."), g_INSTALLER, MB_ICONERROR | MB_OK);
			return false;
		}

		// wait till it's ready before removing it...
		//::WaitForSingleObject(sei.hProcess, INFINITE);

		::CloseHandle(sei.hProcess);

		return true;
	}

	// remove updater
	RemoveFileFromDir(dest_dir, g_COMM_CTL_UPD);

	return true;
}


HBRUSH g_brWhite= 0;

INT_PTR CALLBACK LicenseDialogProc(
	HWND hWnd,		// handle to dialog box
	UINT msg,		// message
	WPARAM wParam,	// first message parameter
	LPARAM lParam)	// second message parameter
{

	switch (msg)
	{
	case WM_INITDIALOG:
		g_brWhite = ::CreateSolidBrush(::GetSysColor(COLOR_WINDOW));

		{
			const TCHAR* resource_name= MAKEINTRESOURCE(IDR_LICENSE);
			const TCHAR* resource_type= _T("FILE");

			if (HRSRC rsrc= ::FindResource(g_instance_handle, resource_name, resource_type))
			{
				if (HGLOBAL global= ::LoadResource(g_instance_handle, rsrc))
				{
					DWORD size= ::SizeofResource(g_instance_handle, rsrc);
					const int MAX= size + 50;
					TCHAR* text= new TCHAR[MAX];

					if (BYTE* data= reinterpret_cast<BYTE*>(::LockResource(global)))
					{
						if (text)
						{
							int len= ::MultiByteToWideChar(CP_ACP, 0, LPCSTR(data), size, text, MAX);
							//memcpy(text, data, size);
							if (len > 0 && len < MAX)
								text[len] = 0;
							text[MAX - 1] = 0;
							::SetDlgItemText(hWnd, IDC_INFO, text);

							delete [] text;

							break;
						}
					}
				}
			}
		}
		::EndDialog(hWnd, wParam);
		break;


	case WM_CTLCOLORSTATIC:
		return INT_PTR(g_brWhite);


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			::EndDialog(hWnd, wParam);
			return true;
		}
	}

	return 0;
}


void ShowLicense()
{
	::DialogBox(g_instance_handle, MAKEINTRESOURCE(IDD_LICENSE), 0, LicenseDialogProc);
}


/////////////////////////////////////////////////////////////////////////////////////////

HFONT g_fntTitle= 0;
HFONT g_fntLicense= 0;
HIMAGELIST g_camera_image= 0;


INT_PTR CALLBACK DialogProc(
	HWND hWnd,		// handle to dialog box
	UINT msg,		// message
	WPARAM wParam,	// first message parameter
	LPARAM lParam)	// second message parameter
{

	switch (msg)
	{
	case WM_INITDIALOG:

		{
			DWORD common_control_lib_version= GetDllVersion(_T("comctl32.dll"));

			g_update_common_ctrls = common_control_lib_version < PACKVERSION(5,80);	// anything below 5.80 is worth upgrading
			//{
			//	::MessageBox(hWnd, _T("ExifPro requires Common Controls library (ComCtl32.dll) version 4.71 or higher.\n\n")
			//		_T("This library can be found on the Microsoft download site either as a stand-alone \n")
			//		_T("component or part of the Internet Explorer 4.0 or later install package."),
			//		g_INSTALLER, MB_OK | MB_ICONERROR);
			//	::EndDialog(hWnd, IDCANCEL);
			//	return false;
			//}
		}

		if (g_update_common_ctrls)
		{
			// hide it--comctl updater needs to restart
			::ShowWindow(::GetDlgItem(hWnd, IDC_RUN), SW_HIDE);
		}

		{
			HICON icon= ::LoadIcon(g_instance_handle, MAKEINTRESOURCE(IDR_MAINFRAME));
			::SendMessage(hWnd, WM_SETICON, ICON_SMALL, LPARAM(icon));
			::SendMessage(hWnd, WM_SETICON, ICON_BIG, LPARAM(icon));
		}

		{
			HDC dc= ::GetWindowDC(hWnd);
			int log_pixels_y= ::GetDeviceCaps(dc, LOGPIXELSY);
			::ReleaseDC(hWnd, dc);

			// create title font
			{
				HFONT font= static_cast<HFONT>(::GetStockObject(DEFAULT_GUI_FONT));
				LOGFONT lf;
				::GetObject(font, sizeof lf, &lf);
				lf.lfWeight = FW_BOLD;
				lf.lfHeight = -MulDiv(18, log_pixels_y, 96);
				lf.lfWidth = 0;
				lf.lfQuality = ANTIALIASED_QUALITY;
				_tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("Tahoma"));
				g_fntTitle = ::CreateFontIndirect(&lf);
			}
			// create license info font
			{
				HFONT font= static_cast<HFONT>(::GetStockObject(DEFAULT_GUI_FONT));
				LOGFONT lf;
				::GetObject(font, sizeof lf, &lf);
				lf.lfWeight = FW_NORMAL;
				lf.lfHeight = -MulDiv(9, log_pixels_y, 96);
				lf.lfWidth = 0;
				//lf.lfQuality = ANTIALIASED_QUALITY;
				_tcscpy_s(lf.lfFaceName, LF_FACESIZE, _T("Small Fonts"));
				g_fntLicense = ::CreateFontIndirect(&lf);
			}
		}

		g_camera_image = ::ImageList_LoadImage(g_instance_handle, MAKEINTRESOURCE(IDB_CAMERA), 48, 0, CLR_NONE, IMAGE_BITMAP, LR_CREATEDIBSECTION);

		{
			TCHAR path[2 * MAX_PATH]= _T("c:\\Program Files");
			ITEMIDLIST* idl= 0;
			if (g_IsWindows64)
			{
				if (HINSTANCE dll= ::LoadLibrary(_T("shell32.dll")))
				{
					typedef HRESULT (STDAPICALLTYPE *FN_SHGetKnownFolderIDList)(REFKNOWNFOLDERID, DWORD, HANDLE, PIDLIST_ABSOLUTE*);

					FN_SHGetKnownFolderIDList SHGetKnownFolderIDListFn= reinterpret_cast<FN_SHGetKnownFolderIDList>(::GetProcAddress(dll, "SHGetKnownFolderIDList"));

					if (SHGetKnownFolderIDListFn)
					{
						if (SHGetKnownFolderIDListFn(FOLDERID_ProgramFilesX64, 0, HANDLE(0), &idl) == S_OK)
						{
							::SHGetPathFromIDList(idl, path);

							IMallocPtr malloc;
							if (::SHGetMalloc(&malloc) == 0)
								malloc->Free(idl);
						}
					}

					::FreeLibrary(dll);
				}

				if (idl == 0)
				{
					TCHAR buffer[MAX_PATH];
					DWORD d= ::GetEnvironmentVariable(_T("ProgramW6432"), buffer, MAX_PATH);
					if (d > 0)
						_tcscpy_s(path, MAX_PATH, buffer);
				}
			}
			else if (::SHGetSpecialFolderLocation(hWnd, CSIDL_PROGRAM_FILES, &idl) == 0)
			{
				::SHGetPathFromIDList(idl, path);

				IMallocPtr malloc;
				if (::SHGetMalloc(&malloc) == 0)
					malloc->Free(idl);
			}
			_tcscat_s(path, MAX_PATH * 2, _T("\\ExifPro ") EP_VER);

			::SetDlgItemText(hWnd, IDC_FOLDER, path);

			if (pfnAutoCompleteFn)
				(*pfnAutoCompleteFn)(::GetDlgItem(hWnd, IDC_FOLDER), SHACF_FILESYSTEM);
		}

		::CheckDlgButton(hWnd, IDC_RUN, 1);
		::CheckDlgButton(hWnd, IDC_ADD_ICON, 1);
		::CheckDlgButton(hWnd, IDC_PROGRAMS, 1);

		if (HWND edit= ::GetDlgItem(hWnd, IDC_FOLDER))
			::SendMessage(edit, EM_LIMITTEXT, MAX_PATH - 16, 0);

		::SendMessage(hWnd, WM_NEXTDLGCTL, WPARAM(::GetDlgItem(hWnd, IDOK)), 1L);

		return false;


	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
			{
				TCHAR path[MAX_PATH]= { 0 };
				if (::GetDlgItemText(hWnd, IDC_FOLDER, path, MAX_PATH) > 0)
				{
					HCURSOR cursor= ::SetCursor(::LoadCursor(0, IDC_WAIT));

					if (InstallExifPro(path, !!::IsDlgButtonChecked(hWnd, IDC_ADD_ICON), !!::IsDlgButtonChecked(hWnd, IDC_PROGRAMS)))
					{
						if (!g_update_common_ctrls)	// if comctl updated it waits for a restart
						{
							// launch ExifPro?
							if (::IsDlgButtonChecked(hWnd, IDC_RUN))
							{
								TCHAR exif[MAX_PATH * 2]= { 0 };
								AppendFileName(path, g_EXIF_APP, exif);
								::ShellExecute(0, _T("open"), exif, 0, 0, SW_SHOWNORMAL);
							}
							else
							{
								::MessageBox(hWnd, _T("ExifPro was installed successfully."), g_INSTALLER, MB_ICONINFORMATION | MB_OK);
							}
						}

						::EndDialog(hWnd, wParam);
					}
				}
			}
			return true;

		case IDCANCEL:
			::EndDialog(hWnd, wParam);
			return TRUE;

		case IDC_BROWSE_DIR:
			BrowseDir(hWnd);
			break;

		case ID_LICENSE:
			ShowLicense();
			break;
		}
		return TRUE;


	case WM_ERASEBKGND:
		if (HDC dc= HDC(wParam))
		{
			RECT rect;
			::GetClientRect(hWnd, &rect);

			RECT rectTop= rect;
			rectTop.bottom = 50;
			rect.top = rectTop.bottom;

			::SetBkMode(dc, OPAQUE);

			::SetBkColor(dc, RGB(255,255,255));
			::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rectTop, 0, 0, 0);

			::ImageList_Draw(g_camera_image, 0, dc, 14, 7, ILD_NORMAL);

			HGDIOBJ hOld= ::SelectObject(dc, g_fntTitle);
			::SetTextColor(dc, RGB(0,0,0));
			
			const TCHAR* title= _T("ExifPro ") EP_VER _T(" Installer");
			::ExtTextOut(dc, 74, 13, 0, 0, title, _tcslen(title), 0);
			::SelectObject(dc, hOld);

			::SetBkColor(dc, ::GetSysColor(COLOR_3DSHADOW));
			RECT rectLine= rectTop;
			rectLine.top = rectLine.bottom - 1;
			::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rectLine, 0, 0, 0);

			::SetBkColor(dc, ::GetSysColor(COLOR_3DFACE));
			::ExtTextOut(dc, 0, 0, ETO_OPAQUE, &rect, 0, 0, 0);

			hOld = ::SelectObject(dc, g_fntLicense);
			::SetTextColor(dc, RGB(0,0,0));

			const TCHAR* info= _T("By installing this product, you are agreeing to be bound by the terms of the license.");
			::ExtTextOut(dc, rect.left + 16, rect.bottom - 16, 0, 0, info, _tcslen(info), 0);
			::SelectObject(dc, hOld);
		}
		return true;
	}

	return 0;
}
