#include <stdio.h>
#include <Windows.h>
#include <lmaccess.h>

#pragma comment(lib, "Netapi32.lib")

int wmain(int argc, wchar_t* argv[]) {
	if (argc != 3) {
		wprintf(L"Usage: %s <localgroupname> <servername>\n", argv[0]);
		wprintf(L"       %s administrators \\\\192.168.232.128\n", argv[0]);
		exit(1);
	}

	LPCWSTR servername = argv[2];				// �Ѿ�����ipc���ӵ�IP
	LPCWSTR TargetGroup = argv[1];				// ��������
	LOCALGROUP_MEMBERS_INFO_2* buff;			// LOCALGROUP_MEMBERS_INFO_2�ṹ������buff��Ż�ȡ������Ϣ
	DWORD dwPrefmaxlen = MAX_PREFERRED_LENGTH;	// ָ���������ݵ���ѡ��󳤶ȣ����ֽ�Ϊ��λ�����ָ��MAX_PREFERRED_LENGTH���ú�������������������ڴ�����
	DWORD dwEntriesread;						// ָ��һ��ֵ��ָ�룬��ֵ����ʵ��ö�ٵ�Ԫ������
	DWORD dwTotalentries;
	NetLocalGroupGetMembers(servername, TargetGroup, 2, (LPBYTE*)&buff, dwPrefmaxlen, &dwEntriesread, &dwTotalentries, NULL);
	// wprintf(L"dwEntriesread: %d\ndwTotalentries: %d\n", dwEntriesread, dwTotalentries);
	for (DWORD i = 0; i < dwEntriesread; i++) {
		wprintf(L"%s\n", buff[i].lgrmi2_domainandname);
		// wprintf(L"SID:%d\n", buff[i].lgrmi2_sid);				// sid�����Ǻ���Ҫ������
		// wprintf(L"SIDUSAGE:%d\n",buff[i].lgrmi2_sidusage);
	}
}