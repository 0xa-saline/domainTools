#ifndef UNICODE
#define UNICODE
#endif

#define _CRT_SECURE_NO_WARNINGS				// ���԰�ȫ������

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <Winnetwk.h>					// WNetAddConnection2
#include <lmaccess.h>
#include <iostream>
#include <vector>
#include<errno.h>
#include <iphlpapi.h>
#include <strsafe.h>
#include <tchar.h>
#include <locale.h>						// ����
#include <thread>
#include <mutex>
#include <time.h>
#include <string>			// std::

#pragma comment(lib,"iphlpapi.lib")		// ̽��������� sendarp
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "mpr.lib")			// Winnet����Ҫ�Ķ�̬���ӿ�	
#pragma comment(lib, "Netapi32.lib")			// Winnet����Ҫ�Ķ�̬���ӿ�	
#pragma comment(lib, "Kernel32.lib")