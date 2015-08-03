#include <windows.h>
#include <tchar.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <shellapi.h>

#define EP_VER _T("2.3")

static const TCHAR* g_EXIF_APP= _T("ExifPro.exe");
static const TCHAR* g_EXIF_SYM= _T("ExifPro.pdb");
static const TCHAR* g_EXIF_HELP= _T("ExifPro.chm");
static const TCHAR* g_UNINST_APP= _T("UnInstall.exe");
static const TCHAR* g_UNINST_KEY= _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ExifPro ") EP_VER;
static bool g_IsWindows64= false;


bool IsWow64()
{
	typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);

	BOOL is_wow_64= FALSE;

	LPFN_ISWOW64PROCESS IsWow64ProcessFn= (LPFN_ISWOW64PROCESS)::GetProcAddress(::GetModuleHandle(TEXT("kernel32")), "IsWow64Process");

	if (IsWow64ProcessFn != 0)
	{
		if (!IsWow64ProcessFn(GetCurrentProcess(), &is_wow_64))
		{
			// handle error
		}
	}

	return is_wow_64 != 0;
}

const size_t PATH_LIMIT= MAX_PATH * 2;

TCHAR* AppendFileName(const TCHAR* dest_dir, const TCHAR* file_name, TCHAR* out)
{
	_tcscpy_s(out, PATH_LIMIT, dest_dir);
	int len= _tcslen(out);

	if (len > 0 && out[len - 1] != _T('\\') && out[len - 1] != _T('/'))
		_tcscat_s(out, PATH_LIMIT, _T("\\"));

	_tcscat_s(out, PATH_LIMIT, file_name);

	return out;
}


void RemoveFileFromDir(const TCHAR* dest_dir, const TCHAR* file_name)
{
	TCHAR path[PATH_LIMIT];

	AppendFileName(dest_dir, file_name, path);

	::DeleteFile(path);
}


void RemoveExifFiles(const TCHAR* dest_dir)
{
	RemoveFileFromDir(dest_dir, g_EXIF_APP);
	RemoveFileFromDir(dest_dir, g_EXIF_SYM);
	RemoveFileFromDir(dest_dir, g_EXIF_HELP);
	RemoveFileFromDir(dest_dir, g_UNINST_APP);

	// remove image cache files

	TCHAR path_base[PATH_LIMIT]= _T("c:\\");

	ITEMIDLIST* pidl= 0;
	if (::SHGetSpecialFolderLocation(0, CSIDL_COMMON_APPDATA, &pidl) == NOERROR && pidl != 0)
		::SHGetPathFromIDList(pidl, path_base);
	else if (::SHGetSpecialFolderLocation(0, CSIDL_APPDATA, &pidl) == NOERROR && pidl != 0)
		::SHGetPathFromIDList(pidl, path_base);

	TCHAR path[PATH_LIMIT];
	AppendFileName(path_base, _T("MiK\\ExifPro"), path);

	if (g_IsWindows64)
		RemoveFileFromDir(path, _T("CacheDb64.bin"));		// x64
	else
		RemoveFileFromDir(path, _T("CacheDb.bin"));		// Unicode
	RemoveFileFromDir(path, _T("CacheDb.bin.ix"));	// Unicode

/*
#ifdef _WIN64
	RemoveFileFromDir(path, _T("CacheDb64.bin"));		// x64
#else
	RemoveFileFromDir(path, _T("CacheDbA.bin"));		// ASCII
	RemoveFileFromDir(path, _T("CacheDbA.bin.ix"));	// ASCII
#endif
*/
	// remove light table text file...

	_tcscpy_s(path_base, PATH_LIMIT, _T("c:\\"));

	ITEMIDLIST* pidl2= 0;
	if (::SHGetSpecialFolderLocation(0, CSIDL_APPDATA, &pidl2) == NOERROR && pidl2 != 0)
		::SHGetPathFromIDList(pidl2, path_base);

	AppendFileName(path_base, _T("MiK\\ExifPro"), path);

	RemoveFileFromDir(path, _T("LightTableContents.txt"));	// Unicode
//	RemoveFileFromDir(path, _T("LightTableContentsA.txt"));	// ASCII

	LPMALLOC malloc;
	if (::SHGetMalloc(&malloc) == NOERROR)
	{
		if (pidl != 0)
			malloc->Free(pidl);
		if (pidl2 != 0)
			malloc->Free(pidl2);

		malloc->Release();
	}
}


bool RemoveLinks(int specialFolderId)
{
	HKEY key= 0;
	if (::RegOpenKey(HKEY_LOCAL_MACHINE, g_UNINST_KEY, &key) != ERROR_SUCCESS)
		return false;

	TCHAR key_name[MAX_PATH];
	wsprintf(key_name, L"UninstLink_%d", specialFolderId);

	bool deleted= false;
	TCHAR path[MAX_PATH]= { 0 };
	DWORD type= 0;
	DWORD size= MAX_PATH * sizeof(path[0]);

	if (::RegQueryValueEx(key, key_name, 0, &type, reinterpret_cast<BYTE*>(path), &size) == ERROR_SUCCESS && *path != 0)
	{
		deleted = !!::DeleteFile(path);
	}

	::RegCloseKey(key);

	return deleted;
}


bool RemoveRegistryKeys()
{
	bool ok= true;

	if (::RegDeleteKey(HKEY_LOCAL_MACHINE, g_UNINST_KEY) != ERROR_SUCCESS)
		ok = false;

	// delete this key and all subkeys
	LONG res= ::SHDeleteKey(HKEY_CURRENT_USER, _T("Software\\MKowalski\\ExifPro\\") EP_VER);

	if (res != ERROR_SUCCESS)
		ok = false;

	res = ::SHDeleteKey(HKEY_CLASSES_ROOT, _T("ExifPro.catalog"));

	if (res != ERROR_SUCCESS)
		ok = false;

	//char msg[200];
	//wsprintf(msg, "key removed: %d", res);
	//::MessageBox(0, msg, _T("Removing ExifPro"), MB_ICONWARNING | MB_OK);

	return ok;
}


int WINAPI WinMain(HINSTANCE h, HINSTANCE h2, LPSTR psz, int n)
{
	// Is this the Original EXE or the clone EXE?
	// If the command-line 1  argument, this is the Original EXE
	// If the command-line >1 argument, this is the clone EXE

	if (__argc == 1)
	{
		if (::MessageBox(0, _T("Do you want to remove ExifPro Photo Viewer?"), _T("Removing ExifPro ") EP_VER, MB_ICONQUESTION | MB_YESNO) != IDYES)
			return 0;

		g_IsWindows64 = IsWow64();

		// Original EXE: Spawn clone EXE to delete this EXE
//MessageBox(0, _T("starting temp executable"), "msg 0", MB_OK);

		// Copy this EXEcutable image into the user's temp directory
		TCHAR pathOrig[_MAX_PATH], pathClone[_MAX_PATH];
		::GetModuleFileName(NULL, pathOrig, _MAX_PATH);
		::GetTempPath(_MAX_PATH, pathClone);
		::GetTempFileName(pathClone, _T("Del"), 0, pathClone);
		::CopyFile(pathOrig, pathClone, FALSE);

		// Open the clone EXE using FILE_FLAG_DELETE_ON_CLOSE
		HANDLE file= ::CreateFile(pathClone, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_DELETE_ON_CLOSE, NULL);

//MessageBox(0, _T("opening cur executable"), "msg 1", MB_OK);

		// Spawn the clone EXE passing it our EXE's process handle
		// and the full path name to the Original EXE file.
		TCHAR cmdLine[512];
		HANDLE processOrig= ::OpenProcess(SYNCHRONIZE, TRUE, GetCurrentProcessId());
//MessageBox(0, _T("preparing parameters"), "msg 2", MB_OK);
		wsprintf(cmdLine, _T("%s %d \"%s\""), pathClone, processOrig, pathOrig);

		STARTUPINFO si;
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);

		PROCESS_INFORMATION pi;
		ZeroMemory(&pi, sizeof(pi));

//MessageBox(0, _T("creating process"), "msg 3", MB_OK);
		::CreateProcess(NULL, cmdLine, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
		::CloseHandle(processOrig);
		::CloseHandle(file);

//MessageBox(0, _T("temp started, exiting main process"), "msg 4", MB_OK);

		// This original process can now terminate.
	}
	else if (__argc == 3)
	{
		int argc= 0;
		wchar_t** list= ::CommandLineToArgvW(::GetCommandLine(), &argc);

		if (argc != 3)
		{
			MessageBox(0, _T("Missing command line arguments"), L"ExifPro Uninstaller", MB_OK);
			return 1;
		}

//MessageBox(0, _T("executing temp file"), L"msg 5", MB_OK);

//MessageBox(0, list[0], L"msg 6", MB_OK);
//MessageBox(0, list[1], L"msg 7", MB_OK);
//MessageBox(0, list[2], L"msg 8", MB_OK);

		// Clone EXE: When original EXE terminates, delete it
		HANDLE processOrig = (HANDLE) _wtoi(list[1]);
		::WaitForSingleObject(processOrig, INFINITE);
		::CloseHandle(processOrig);
		::DeleteFile(list[2]);

		TCHAR path[MAX_PATH];
		_tcscpy_s(path, MAX_PATH, list[2]);

		if (TCHAR* sep= _tcsrchr(path, _T('\\')))
		{
			*sep = 0;

			RemoveExifFiles(path);

			// Insert code here to remove the subdirectory too (if desired).
			::RemoveDirectory(path);

			// remove links (if registry entries say so)
			RemoveLinks(CSIDL_DESKTOP);
			RemoveLinks(CSIDL_PROGRAMS);

			RemoveRegistryKeys();
		}

		::LocalFree(list);
		list = 0;

		// The system will delete the clone EXE automatically
		// because it was opened with FILE_FLAG_DELETE_ON_CLOSE

		//TODO: tmp file is not deleted for some reason...
	}

	return 0;
}
