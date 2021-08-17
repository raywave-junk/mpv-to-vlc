#include <iostream>
#include <Windows.h>
#include "WinReg.h"

using winreg::RegKey;

__forceinline auto replace(std::string& str, const std::string& from, const std::string& to) -> bool {
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

auto WinMain(HINSTANCE, HINSTANCE, char* lpCmdLine, int) -> int
{
    std::wstring regSubKey = L"SOFTWARE\\VideoLAN\\VLC";
    std::wstring regSubKey_fallback = L"SOFTWARE\\WOW6432Node\\VideoLAN\\VLC";
    RegKey key{ HKEY_LOCAL_MACHINE, regSubKey};
    auto value = key.TryGetStringValue(L"InstallDir");
    if (!value.has_value())
    {
        key.Close();
        key.Open(HKEY_LOCAL_MACHINE, regSubKey_fallback);
        value = key.TryGetStringValue(L"InstallDir");
        if (!value.has_value())
        {
            MessageBoxA(NULL, "Failed to get VLC installation path", "mpv2vlc", MB_OK | MB_ICONERROR);
            return 1;
        }
    }

    auto vlcDirW = value.value();
    auto vlcPathW = vlcDirW + L"\\vlc.exe";
    std::string vlcPath(vlcPathW.begin(), vlcPathW.end());
    std::string args = lpCmdLine;
    replace(args, "--title=", "--meta-title \"");
    replace(args, "--user-agent=", "--http-user-agent=\"");
    replace(args, "--save-position-on-quit", "");

    while (args.find("\"-") != std::string::npos)
        replace(args, "\"-", "-");

    SHELLEXECUTEINFO si = { sizeof(SHELLEXECUTEINFO) };
    si.lpVerb = "open";
    si.lpFile = vlcPath.c_str();
    si.lpParameters = args.c_str();
    si.nShow = SW_SHOW;

    ShellExecuteEx(&si);

    return 0;
}