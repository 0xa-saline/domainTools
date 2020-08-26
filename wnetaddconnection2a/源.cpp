#ifndef UNICODE
#define UNICODE
#endif
#pragma comment(lib, "mpr.lib")			// Winnet����Ҫ�Ķ�̬���ӿ�

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <Winnetwk.h>					// ��ͷ

// Need to link with Netapi32.lib and Mpr.lib

int wmain(int argc, wchar_t* argv[])
{

    DWORD dwRetVal;

    NETRESOURCE nr;
    DWORD dwFlags;

    if (argc != 4) {
        wprintf(L"Usage: %s <remotename> <username> <password>\n",
            argv[0]);
        wprintf(L"       %s \\\\contoso\\public testuser testpasswd\n",
            argv[0]);
        exit(1);
    }

    wprintf(L"Calling WNetAddConnection2 with\n");
    wprintf(L"  lpLocalName = %s\n", L"");
    wprintf(L"  lpRemoteName = %s\n", argv[1]);
    wprintf(L"  lpUsername = %s\n", argv[2]);
    wprintf(L"  lpPassword = %s\n", argv[3]);

    // Zero out the NETRESOURCE struct
    memset(&nr, 0, sizeof(NETRESOURCE));

    // Assign our values to the NETRESOURCE structure.

    nr.dwType = RESOURCETYPE_ANY;
    nr.lpLocalName = NULL;					// F:  ӳ�䵽���صĴ��̣����磺Z�̵�. ����ַ���Ϊ�գ�����lpLocalNameΪNULL����ú�����������������Դ�����ӣ��������ض��򱾵��豸
    nr.lpRemoteName = argv[1];				// \\192.168.232.128\temp	Ŀ��������Ź���Ĵ���
    nr.lpProvider = NULL;

    // Assign a value to the connection options
    dwFlags = CONNECT_UPDATE_PROFILE;
    //
    // Call the WNetAddConnection2 function to assign
    //   a drive letter to the share.
    //
    dwRetVal = WNetAddConnection2(&nr, argv[3], argv[2], dwFlags);
    //
    // If the call succeeds, inform the user; otherwise,
    //  print the error.
    //
    if (dwRetVal == NO_ERROR)
        wprintf(L"Connection added to %s\n", nr.lpRemoteName);
    else
        wprintf(L"WNetAddConnection2 failed with error: %u\n", dwRetVal);

    exit(1);
}