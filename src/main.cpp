#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "Wininet.lib")

#include <iostream>
#include <cstdint>
#include <string>
#include <exception>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using std::cout;
using std::wcout;
using std::endl;
using std::string;
using std::wstring;

wstring last_error_to_str(DWORD error_code)
{
    auto constexpr buffer_size = 512;
    WCHAR buffer[buffer_size]{};

    DWORD result = FormatMessageW(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        error_code,
        0, // Default language
        buffer,
        buffer_size,
        NULL
    );

    if (result == 0)
    {
        return wstring(L"????");
    }

    return wstring(buffer);
}

int wmain()
{
    try
    {
        auto* internet = InternetOpenW(
            L"YF",
            INTERNET_OPEN_TYPE_DIRECT,
            NULL,
            NULL,
            0);

        if (!internet)
        {
            wcout << "Error: call to InternetOpenW() failed: "
                << last_error_to_str(GetLastError()) << endl;
            return 1;
        }

        const wchar_t* headers =
            L"User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:47.0)\r\n"
            L"Accept: application/json\r\n";

        const wchar_t* url = L"https://httpbin.org/get";
        auto* connect = InternetOpenUrlW(
            internet,
            url,
            headers,
            static_cast<DWORD>(-1L),
            INTERNET_FLAG_RELOAD,
            0);

        if (!connect)
        {
            wcout << "Error: call to InternetOpenUrlW() failed: "
                << last_error_to_str(GetLastError()) << endl;
            InternetCloseHandle(internet);
            return 1;
        }

        char buffer[1024] {};
        DWORD bytes_read = 0;
        
        InternetReadFile(connect, buffer, sizeof(buffer), &bytes_read);
        while (bytes_read > 0) 
        {    
            cout.write(buffer, bytes_read);
            InternetReadFile(connect, buffer, sizeof(buffer), &bytes_read);
        }

        InternetCloseHandle(connect);
        InternetCloseHandle(internet);

        return 0;
    }
    catch (const std::exception& e)
    {
        cout << "Exception: " << e.what() << endl;
    }

    return 1;
}
