// ������򻷾��£������û�Ȩ�ޣ��������أ���������У�����IPC���Ӽ���ö�����û���
#pragma once
#ifndef UNICODE
#define UNICODE
#endif
#define _CRT_SECURE_NO_WARNINGS				// ���԰�ȫ������

#include <iostream>
#include <Windows.h>
#include <string>
#include <sddl.h>
#include <vector>
#include <thread>
#include <mutex>
#include <stdio.h>
#include <locale.h>						// ����

#define BUFFSIZE 1024

std::mutex mtx;
int StartSid;

// �ַ����ָ�
std::wstring splitString(std::wstring strSrc, std::wstring pattern)
{
	std::wstring result;
	// ������ַ�����󣬿��Խ�ȡ���һ������
	std::wstring strcom = strSrc.append(pattern);
	// wprintf(L"%s\n", strcom);
	auto pos = strSrc.find(pattern);
	auto len = strcom.size();

	// wprintf(L"%d", std::wstring::npos);
	while (pos != std::wstring::npos)
	{
		std::wstring coStr = strcom.substr(0, pos);

		strcom = strcom.substr(pos + pattern.size(), len);
		pos = strcom.find(pattern);

		if (pos == -1)
		{
			break;
		}
		result.append(coStr);
		result.append(L"-");
	}

	return result;
}

// ��ȡ���administrator��sid
BOOL user2sid(LPCTSTR lpSystemName, LPCTSTR lpAccountName, PSID Sid)
{
	// LPCTSTR lpSystemName = lpSystemName;			// ������������ TEXT("192.168.52.2");
	// LPCTSTR lpAccountName = lpAccountName;		// ���û��� TEXT("hack\\administrator");
	PSID pSid = Sid;
	DWORD cbSid = 1;			// ����Sid�������Ĵ�С

	WCHAR ReferencedDomainName[BUFFSIZE];
	ZeroMemory(ReferencedDomainName, BUFFSIZE);
	DWORD cchReferencedDomainName = 1;

	UCHAR buffer[4];
	PSID_NAME_USE peUse = (PSID_NAME_USE)buffer;		// ָ�����PSID_NAME_USEֵ��ָʾ�ʻ����ͣ��ı�����ָ�� 

	BOOL bRtnBool = TRUE;

	// ��һ��ִ����Ϊ�˻�ȡcbSid��cchReferencedDomainName��ֵ
	bRtnBool = LookupAccountName(
		lpSystemName,				// ������������
		lpAccountName,				// ���û���
		pSid,
		(LPDWORD)&cbSid,
		ReferencedDomainName,
		(LPDWORD)&cchReferencedDomainName,
		peUse);
	// wprintf(L"cbSid:%d\ncchReferencedDomainName:%d\n", cbSid, cchReferencedDomainName);

	// �ڶ���ִ���ǻ�ȡpSid����Ϊ��Ҫ��һ����cbSid��cchReferencedDomainName���
	bRtnBool = LookupAccountName(
		lpSystemName,
		lpAccountName,
		pSid,
		(LPDWORD)&cbSid,
		ReferencedDomainName,
		(LPDWORD)&cchReferencedDomainName,
		peUse);

	if (bRtnBool == TRUE)
	{
		return TRUE;
	}
	else
	{
		printf("Error : %d\n", GetLastError());
	}
	return FALSE;
};

// ͨ��sid�����û���
std::wstring sid2user(PSID Sid, LPCTSTR lpSystemName)
{
	// LPCTSTR lpSystemName = TEXT("192.168.3.142");		// ������������
	PSID pSid = Sid;				// SID

	WCHAR Name[BUFFSIZE];			// ����sid������û���
	ZeroMemory(Name, BUFFSIZE);		// ����ڴ�
	DWORD cchName = 1;			// ����Name����Ļ�������С

	WCHAR ReferencedDomainName[BUFFSIZE];
	ZeroMemory(ReferencedDomainName, BUFFSIZE);		// ����ڴ�
	DWORD cchReferencedDomainName = 1;					// ����ReferencedDomainName����Ļ�������С

	UCHAR buffer[4];
	PSID_NAME_USE peUse = (PSID_NAME_USE)buffer;		// ָ�����PSID_NAME_USEֵ��ָʾ�ʻ����ͣ��ı�����ָ�� 

	BOOL bRtnBool = TRUE;
	// ��һ��ִ����Ϊ�˻�ȡcchName��cchReferencedDomainName
	bRtnBool = LookupAccountSid(
		lpSystemName,         
		pSid,
		Name,
		(LPDWORD)&cchName,
		ReferencedDomainName,
		(LPDWORD)&cchReferencedDomainName,
		peUse);

	// wprintf(L"cchName:%d\cchReferencedDomainName:%d\n", cchName, cchReferencedDomainName);

	// �ڶ���ִ���ǻ�ȡName����Ϊ��Ҫ��һ����cchName��cchReferencedDomainName���
	bRtnBool = LookupAccountSid(
		lpSystemName,                          // name of local or remote computer
		pSid,                     // security identifier
		Name,                      // account name buffer
		(LPDWORD)&cchName,          // size of account name buffer 
		ReferencedDomainName,
		(LPDWORD)&cchReferencedDomainName,
		peUse);                        // SID type

	
	if (bRtnBool == TRUE)
	{
		std::wstring domainUser;
		domainUser = (std::wstring)ReferencedDomainName + L"\\" + (std::wstring)Name;
		// wprintf(L"%s\\%s\n", ReferencedDomainName, Name);
		return domainUser;
	}
	else
	{
		// printf("error: %d\n", GetLastError());
		return L"";
	}
	
	
};

// ��ʼö��
void start(int i, std::wstring userSidPrefix, int iEndSid, LPCTSTR lpSystemName)
{

	while (StartSid <= iEndSid)
	{
		int num = 0;
		if (mtx.try_lock())			// ��
		{
			num = StartSid;
			++StartSid;
			mtx.unlock();			// ����
		}

		std::wstring domainUser;
		PSID pSid2;
		std::wstring a = std::to_wstring(num);
		std::wstring userSid;
		userSid = userSidPrefix + a;				// ƴ�ӳ�������sid
		// wprintf(L"%s\t", userSid.data());
		ConvertStringSidToSid((LPCWSTR)userSid.data(), &pSid2);			// ���ַ���ת��ΪSid
		domainUser = sid2user(pSid2, lpSystemName);
		
		if (domainUser != L"")
		{
			wprintf(L"[%d] %s\n", num, domainUser.data());
		}
		delete pSid2;
		// Sleep(2000);
	}
	
}

int wmain(int argc, wchar_t* argv[])
{
	setlocale(LC_ALL, "");							// ��������
	if (argc != 6) {
		wprintf(L"Usage: %s <DC-IP> <domainname\\username> <start Sid> <end Sid> <t_num>\n", argv[0]);
		wprintf(L"       %s \\\\192.168.52.2 hack\\administrator 1000 2000 100\n", argv[0]);
		wprintf(L"       %s \\\\���IP ����\\���û���<Ĭ��administrator> ��ʼSid ĩβSid ���߳���Ŀ\n", argv[0]);
		exit(0);
	}

	LPCTSTR lpSystemName = argv[1];			// ���IP
	LPCTSTR lpAccountName = argv[2];		// hack\\administrator
	std::wstring wszStartSid = argv[3];			// ��ʼSid
	std::wstring wszEndSid = argv[4];			// ĩβSid
	std::wstring wszThreadNum = argv[5];		// �߳���Ŀ
	int iStartSid = std::stoi(wszStartSid.data());
	int iEndSid = std::stoi(wszEndSid.data());
	int iThreadNum = std::stoi(wszThreadNum.data());
	wprintf(L"DC-IP: %s\n", lpSystemName);
	wprintf(L"domainname\\username: %s\n", lpAccountName);
	wprintf(L"start Sid: %d\n", iStartSid);
	wprintf(L"end Sid: %d\n", iEndSid);
	wprintf(L"t_num: %d\n", iThreadNum);
	wprintf(L"------------------------------------------------------\n");
	
	StartSid = iStartSid;			// ȫ�ֱ���

	PSID Sid;
	UCHAR buffer1[2048];
	Sid = buffer1;

	if (!user2sid(lpSystemName, lpAccountName, Sid))
	{
		wprintf(L"user2sid error!");
		exit(0);
	};

	LPWSTR sid;
	ConvertSidToStringSid(Sid, &sid);				// ��Sid������ת��Ϊ�ַ���
	wprintf(L"%s sid: %s\n", lpAccountName, sid);
	
	std::wstring userSidPrefix;				// sid��ǰ׺ S-1-5-21-675012476-827261145-2327888524-
	userSidPrefix = splitString(sid, L"-");
	wprintf(L"sid Prefix: %s\n", userSidPrefix);
	
	wprintf(L"------------------------------------------------------\n");

	std::thread* Threads = new std::thread[iThreadNum];
	for (int i = 0; i < iThreadNum; i++)
		Threads[i] = std::thread(start, i, userSidPrefix, iEndSid, lpSystemName);
	for (int i = 0; i < iThreadNum; i++)
		Threads[i].join();
	delete[] Threads;
	
	wprintf(L"EnuDomainUser End!\n");
}
