#include "TaskScheduler.h"


void TaskSche::CopySelf()
{
	WCHAR pathF[MAX_LEN_FILENAME];
	GetModuleFileName(NULL, pathF, sizeof(pathF));
	WCHAR dest[] = _T("C:\\windows\\temp\\tempsh.exe");
	wprintf(L"%s", pathF);
	CopyFile(pathF, dest, false);
}


void TaskSche::copyFile(string source, string dest) {
	ifstream  src(source, ios::binary);
	ofstream  dst(dest, ios::binary);
	dst << src.rdbuf();
	dst.close();
	src.close();
}


int TaskSche::isFileExist(LPSTR lpFilePath)
{
	/* Check for existence */
	if ((_access(lpFilePath, 0)) != -1)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


int TaskSche::TaskAdd(LPCWSTR wszTaskName, wstring wstrTaskTime, wstring wstrProgram, wstring args)
{
	// https://docs.microsoft.com/zh-cn/windows/win32/taskschd/time-trigger-example--c---
	// https://docs.microsoft.com/zh-cn/windows/win32/taskschd/daily-trigger-example--c---

	setlocale(LC_ALL, "");

	// ��ʼ��COM���
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		printf("\nCoInitializeEx failed: %x", hr);
		return 1;
	}


	// ���������ȫ�ȼ�
	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);
	if (FAILED(hr))
	{
		printf("\nCoInitializeSecurity failed: %x", hr);
		CoUninitialize();
		return 1;
	}

	// ���üƻ��������� 
	// LPCWSTR wszTaskName = L"StateGrid";
	wprintf(L"TaskName:%s\n", wszTaskName);

	// ����ִ��·��
	wstring wstrExePath = _wgetenv(_bstr_t(L"WINDIR"));		// ��ȡ���ַ��Ļ�������
	wstrExePath += L"\\SYSTEM32\\";
	wstrExePath += wstrProgram;


	// ��������������� 
	// Link: https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nn-taskschd-itaskservice		
	// https://docs.microsoft.com/en-us/windows/win32/api/combaseapi/nf-combaseapi-cocreateinstance
	ITaskService* pService = NULL;
	hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pService);
	if (FAILED(hr))
	{
		printf("Failed to create an instance of ITaskService: %x", hr);
		CoUninitialize();
		return 1;
	}

	// ����Ŀ�������ΪԶ�����ӻ򱾵ط�����   https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nf-taskschd-itaskservice-connect
	hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());	//Ĭ�ϱ���
	if (FAILED(hr))
	{
		printf("ITaskService::Connect failed: %x", hr);
		pService->Release();
		CoUninitialize();
		return 1;
	}

	// ��ȡ�����ļ��в������д�������
	ITaskFolder* pRootFolder = NULL;   https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nf-taskschd-itaskservice-getfolder
	// �ƻ�����·��
	hr = pService->GetFolder(_bstr_t(L"\\Microsoft\\Windows\\AppID"), &pRootFolder);			// _bstr_t ��wstrת��Ϊbstr
	if (FAILED(hr))
	{
		printf("Cannot get Root folder pointer: %x", hr);
		pService->Release();
		CoUninitialize();
		return 1;
	}
	wprintf(L"Task Path��\\Microsoft\\Windows\\AppID\n");

	// ����Ƿ��Ѿ������ƻ�����
	IRegisteredTask* pExistingTask = NULL;
	hr = pRootFolder->GetTask(_bstr_t(wszTaskName), &pExistingTask);
	if (hr == S_OK)
	{
		printf("Task exist!\n");
		return 1;
	}
	printf("Create New Task\n");

	// �������ͬ������ɾ����
	// pRootFolder->DeleteTask(_bstr_t(wszTaskName), 0);

	// �����ƻ�����������������ƻ�����
	ITaskDefinition* pTask = NULL;				// https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nn-taskschd-itaskdefinition
	hr = pService->NewTask(0, &pTask);				// https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nf-taskschd-itaskservice-newtask
	pService->Release();  // COM clean up.  Pointer is no longer used.
	if (FAILED(hr))
	{
		printf("Failed to CoCreate an instance of the TaskService class: %x", hr);
		pRootFolder->Release();
		CoUninitialize();
		return 1;
	}


	// ʹ��IRegistrationInfo���������Ļ�����Ϣ���		
	// https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nn-taskschd-iregistrationinfo
	// ��ȡ�������������������ע����Ϣ���������������������������Լ������ע�����ڡ� 
	// ITaskDefinition :: get_RegistrationInfo   https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nf-taskschd-itaskdefinition-get_registrationinfo
	IRegistrationInfo* pRegInfo = NULL;
	hr = pTask->get_RegistrationInfo(&pRegInfo);
	if (FAILED(hr))
	{
		printf("\nCannot get identification pointer: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	// ������������
	hr = pRegInfo->put_Author(_bstr_t(L"Microsoft Corporation"));
	pRegInfo->Release();
	if (FAILED(hr))
	{
		printf("\nCannot put identification info: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}




	// ��������İ�ȫƾ֤		https://docs.microsoft.com/zh-cn/windows/win32/api/taskschd/nn-taskschd-iprincipal
	IPrincipal* pPrincipal = NULL;
	hr = pTask->get_Principal(&pPrincipal);		// ��ȡ��������������壬�������ṩ����İ�ȫƾ�ݡ�
	if (FAILED(hr))
	{
		printf("\nCannot get principal pointer: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	// ���ù���Ϊ����ʽ��¼
	pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);		// ʹ���û���ǰ�ĵ�¼��Ϣ
	//pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
	pPrincipal->put_UserId(_bstr_t(L"NT AUTHORITY\\SYSTEM"));		// ��systemȨ��ִ�У����Ե�ǰ�û�Ȩ����Ҫ�ǹ���ԱȨ��

	// ���������������Ϣ,���ƻ�����ѡ�����������ĸ�����Ϣ		https://docs.microsoft.com/zh-cn/windows/win32/api/taskschd/nn-taskschd-itasksettings
	ITaskSettings* pTaskSettings = NULL;
	pTask->get_Settings(&pTaskSettings);

	// Ϊ������Ϣ��ֵ
	pTaskSettings->put_StartWhenAvailable(VARIANT_TRUE);			// Ĭ��ΪVARIANT_TRUE	https://docs.microsoft.com/zh-cn/windows/win32/api/taskschd/nf-taskschd-itasksettings-put_startwhenavailable

	// ���������idle����
	IIdleSettings* pIdleSettings = NULL;
	pTaskSettings->get_IdleSettings(&pIdleSettings);
	pIdleSettings->put_WaitTimeout(_bstr_t(L"PT5M"));

	// ��������Ĳ������У�������һ������󣬻��ܹ�����������
	pTaskSettings->put_MultipleInstances(TASK_INSTANCES_PARALLEL);			//  https://docs.microsoft.com/zh-cn/windows/win32/api/taskschd/nf-taskschd-itasksettings-get_multipleinstances   https://docs.microsoft.com/zh-cn/windows/win32/taskschd/taskschedulerschema-multipleinstancespolicytype-simpletype


	//����������
	ITriggerCollection* pTriggerCollection = NULL;
	hr = pTask->get_Triggers(&pTriggerCollection);			// ��ȡ������������������Ĵ������ļ��ϡ�
	if (FAILED(hr))
	{
		printf("\nCannot get trigger collection: %x", hr);
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return 1;
	}

	ITrigger* pTrigger = NULL;

	// ������
	/*
	���������´�����https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nf-taskschd-itriggercollection-create
	���ԣ�https://docs.microsoft.com/en-us/windows/win32/api/mstask/ns-mstask-task_trigger
	https://docs.microsoft.com/zh-cn/windows/win32/api/mstask/nf-mstask-itasktrigger-settrigger
	TASK_TRIGGER_LOGON: ���ض��û���¼ʱ��������
	TASK_TRIGGER_TIME: ��һ����ض�ʱ�䴥������
	*/

	// ��ʱ������
	/*
	�����������е�Ƶ���Լ������������ظ��ظ�ģʽ��ʱ�䣺https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nn-taskschd-irepetitionpattern

	ITrigger :: put_StartBoundary ���ü�����������ں�ʱ�䡣���ں�ʱ�����������¸�ʽ��YYYY-MM-DDTHH��MM��SS��+-��HH��MM����ʽ�ģ�+-��HH��MM���ֶ�����Э������ʱ��UTC��֮ǰ��֮����ض�Сʱ���ͷ����������磬2005��10��11��1:21:17��������UTC��ʱ������Сʱ������дΪ2005-10-11T13��21��17
	https://docs.microsoft.com/en-us/windows/win32/api/taskschd/nf-taskschd-itrigger-put_startboundary

	repetitionType ָ��ģʽ�ظ���ʱ�䡣���ַ����ĸ�ʽΪPnYnMnDTnHnMnS������nY��������nM��������nD���������� T��������/ʱ��ָ�����nH��Сʱ����nM�Ƿ�������nS�����������磬PT5Mָ��5���ӣ�P1M4DT2H5Mָ��һ���£����죬��Сʱ������ӣ���
	https://docs.microsoft.com/zh-cn/windows/win32/taskschd/taskschedulerschema-duration-repetitiontype-element


	*/

	// ÿ��
	hr = pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);
	pTriggerCollection->Release();
	ITimeTrigger* pTimeTrigger = NULL;
	pTrigger->QueryInterface(IID_ITimeTrigger, (void**)&pTimeTrigger);
	pTimeTrigger->put_Id(_bstr_t(L"Trigger0"));
	pTimeTrigger->put_StartBoundary(_bstr_t(L"2000-04-01T00:00:00"));		// ���ü�����������ں�ʱ��:��2000-04-01��ʼ��ÿ���00:00:00����
	pTimeTrigger->put_EndBoundary(_bstr_t(L"2030-05-02T23:59:59"));		// ����ͣ�ô����������ں�ʱ��:��2030-05-02 23:59:59����
	IRepetitionPattern* pRepetitionPattern = NULL;
	pTimeTrigger->get_Repetition(&pRepetitionPattern);
	pTimeTrigger->Release();
	pRepetitionPattern->put_Duration(_bstr_t(L""));			// ����ģʽ�ظ���ʱ�䡣����ڳ���ʱ����δָ���κ�ֵ�����ģʽ���������ظ�
	// pRepetitionPattern->put_Interval(_bstr_t(L"PT30M"));			// ����ÿ��������������֮���ʱ�䡣ÿ����ô���
	pRepetitionPattern->put_Interval(_bstr_t(wstrTaskTime.data()));			// ����ÿ��������������֮���ʱ�䡣ÿ����ô���
	pRepetitionPattern->Release();



	// ÿ��
	/*
	hr = pTriggerCollection->Create(TASK_TRIGGER_DAILY, &pTrigger);
	pTriggerCollection->Release();
	IDailyTrigger* pDailyTrigger = NULL;
	pTrigger->QueryInterface(IID_IDailyTrigger, (void**)&pDailyTrigger);
	pDailyTrigger->put_Id(_bstr_t(L"Trigger0"));
	pDailyTrigger->put_StartBoundary(_bstr_t(L"2000-04-01T00:00:00"));		// ���ü�����������ں�ʱ��:��2000-04-01��ʼ��ÿ���00:00:00����
	pDailyTrigger->put_EndBoundary(_bstr_t(L"2030-05-02T23:59:59"));		// ����ͣ�ô����������ں�ʱ��:��2030-05-02 23:59:59����
	pDailyTrigger->put_DaysInterval((short)1);								// ���üƻ��и���֮��ļ�������1����ÿ�ռƻ������2����ÿ��һ���ʱ���
	IRepetitionPattern* pRepetitionPattern = NULL;
	pDailyTrigger->get_Repetition(&pRepetitionPattern);
	pDailyTrigger->Release();
	pRepetitionPattern->put_Duration(_bstr_t(L""));			// ����ģʽ�ظ���ʱ�䡣����ڳ���ʱ����δָ���κ�ֵ�����ģʽ���������ظ�
	pRepetitionPattern->put_Interval(_bstr_t(L"PT1M"));			// ����ÿ��������������֮���ʱ�䡣ÿ����ô���
	pRepetitionPattern->Release();
	*/


	// ʱ�䴥����
	/*
	pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);
	ITimeTrigger* pTimeTrigger = NULL;
	pTrigger->QueryInterface(IID_ITimeTrigger, (void**)&pTimeTrigger);
	pTimeTrigger->put_Id(_bstr_t(L"Trigger1"));
	pTimeTrigger->put_EndBoundary(_bstr_t(L"2020-03-29T20:00:00"));
	pTimeTrigger->put_StartBoundary(_bstr_t(L"2020-03-26T13:00:00"));
	*/

	// ��¼������
	/*
	pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
	ILogonTrigger* pLogonTrigger = NULL;
	pTrigger->QueryInterface(IID_ILogonTrigger, (void**)&pLogonTrigger);
	pLogonTrigger->put_Id(_bstr_t(L"Trigger2"));
	//pLogonTrigger->put_UserId(_bstr_t(L"desktop-gdep6gd\\user"));
	//pLogonTrigger->put_EndBoundary(_bstr_t(L"2020-03-29T20:00:00"));
	pLogonTrigger->put_StartBoundary(_bstr_t(L"2020-03-25T20:00:00"));
	*/


	// ����������
	/*
	pTriggerCollection->Create(TASK_TRIGGER_BOOT, &pTrigger);
	IBootTrigger* pBootTrigger;
	pTrigger->QueryInterface(IID_IBootTrigger, (void**)&pBootTrigger);
	pBootTrigger->put_Id(_bstr_t(L"Trigger3"));
	pBootTrigger->put_EndBoundary(_bstr_t(L"2020-03-29T20:00:00"));
	pBootTrigger->put_StartBoundary(_bstr_t(L"2020-03-25T20:00:00"));
	*/

	// ����������
	IActionCollection* pActionCollection = NULL;
	pTask->get_Actions(&pActionCollection);
	IAction* pAction = NULL;
	pActionCollection->Create(TASK_ACTION_EXEC, &pAction);		// TASK_ACTION_EXEC: �ò���ִ�������в��������磬�ò����������нű���������ִ���ļ������ߣ�����ṩ���ĵ����ƣ����ҵ��������Ӧ�ó���ʹ���ĵ�����Ӧ�ó���
	IExecAction* pExecAction = NULL;							// IExecAction ��ʾִ�������в����Ĳ�����
	pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
	pExecAction->put_Path(_bstr_t(wstrExePath.c_str()));		// ��ȡ�����ÿ�ִ���ļ���·����
	pExecAction->Release();

	// ִ�г���Ĳ���

	// wstring args(L"/c C:\\windows\\temp\\StateGrid.exe");
	// args.append(cmd);
	// args += argv[2];

	wprintf(L"Command:%s %s\n", wstrProgram.data(), args.data());
	pExecAction->put_Arguments(_bstr_t(args.data()));


	// pExecAction->put_Arguments(_bstr_t(L"/c calc"));

	IRegisteredTask* pRegistredTask = NULL;
	pRootFolder->RegisterTaskDefinition(_bstr_t(wszTaskName), pTask, TASK_CREATE_OR_UPDATE,
		_variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(), &pRegistredTask);

	cout << "\n �����������.\n" << endl;
	wprintf(L"**********\n");
	CoUninitialize();
	return 0;

}