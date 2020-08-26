#include <stdio.h>
#include <Windows.h>
#include <lmaccess.h>

#pragma comment(lib, "Netapi32.lib")

int wmain(int argc, wchar_t* argv[]) {
	if (argc != 2) {
		wprintf(L"Usage: %s <servername>\n", argv[0]);
		wprintf(L"       %s 192.168.232.128\n", argv[0]);
		exit(1);
	}

	LPCWSTR servername = argv[1];				// �Ѿ�����ipc���ӵ�IP
	LOCALGROUP_INFO_1* buff;			// LOCALGROUP_MEMBERS_INFO_2�ṹ������buff��Ż�ȡ������Ϣ
	DWORD dwPrefmaxlen = MAX_PREFERRED_LENGTH;	// ָ���������ݵ���ѡ��󳤶ȣ����ֽ�Ϊ��λ�����ָ��MAX_PREFERRED_LENGTH���ú�������������������ڴ�����
	DWORD dwEntriesread;						// ָ��һ��ֵ��ָ�룬��ֵ����ʵ��ö�ٵ�Ԫ������
	DWORD dwTotalentries;
	NetLocalGroupEnum(servername, 1, (LPBYTE*)&buff, dwPrefmaxlen, &dwEntriesread, &dwTotalentries, NULL);
	for (DWORD i = 0; i < dwEntriesread; i++)
	{
		wprintf(L"%s\n", buff[i].lgrpi1_name);
	}
}