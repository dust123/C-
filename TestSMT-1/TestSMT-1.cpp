// TestSMT-1.cpp : �������̨Ӧ�ó������ڵ㡣
//#include <DSRole.h>

#include <stdio.h>
#include<iostream> 
#include <string>    
#include <cstring>    
#include <thread>     

#include "stdafx.h"
//#include "Windows.h"

#include "MySqlConn.h"


#include <afxwin.h> 


#include <winspool.h>

#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "netapi32.lib") 
#pragma comment(lib, "Advapi32.lib")


using namespace std;

#define SERVICE_NAME  "srv_demo"

SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hServiceStatusHandle;
void WINAPI service_main(int argc, char** argv);
void WINAPI ServiceHandler(DWORD fdwControl);

TCHAR szSvcName[80];
SC_HANDLE schSCManager;
SC_HANDLE schService;
int uaquit;
FILE* logg;

DWORD WINAPI srv_core_thread(LPVOID para)
{
	MySqlConn DTconn; 
	bool isCon;
	isCon = DTconn.initConnection();


	HANDLE printerHandle;   //��ӡ���豸���

							//���򿪴�ӡ���豸�Ƿ�ɹ�
	if (!OpenPrinter(TEXT("Microsoft Print to PDF"), &printerHandle, NULL))
	{
		cout << "OpenPrinter fald" << endl;
	}
	else
	{
		//printf("OpenPrinter%d\n", (int)OpenPrinter);
		cout << "OpenPrinter%d" << OpenPrinter << endl;
	}

	DWORD nByteNeeded;
	DWORD nReturned;
	DWORD nByteUsed;


	PRINTER_INFO_2* pPrinterInfo = NULL;
	JOB_INFO_2* pJobInfo = NULL;

	CString strDOCname = _T("");
	CString strMachinename = _T("");
	CString strUsername = _T("");
	CString strPageSize = _T("");
	CString strPrintCopies = _T("");
	CString strPrintTotalPages = _T("");
	CString strPrintColor = _T("");
	CString strSubmitted = _T("");

	int markP = 0;
	while (true)
	{		//printf("in threadFUN\n");
			//ͨ������GetPrinter()�����õ���ҵ����

		GetPrinter(printerHandle, 2, NULL, 0, &nByteNeeded);
		pPrinterInfo = (PRINTER_INFO_2*)malloc(nByteNeeded);
		GetPrinter(printerHandle, 2, (LPBYTE)pPrinterInfo, nByteNeeded, &nByteUsed);


		//ͨ������EnumJobs()����ö�ٴ�ӡ����

		EnumJobs(printerHandle, 0, pPrinterInfo->cJobs, 2, NULL, 0, (LPDWORD)&nByteNeeded, (LPDWORD)&nReturned);

		pJobInfo = (JOB_INFO_2*)malloc(nByteNeeded);

		ZeroMemory(pJobInfo, nByteNeeded);

		EnumJobs(printerHandle, 0, pPrinterInfo->cJobs, 2, (LPBYTE)pJobInfo, nByteNeeded, (LPDWORD)&nByteUsed, (LPDWORD)&nReturned);


		//��⵱ǰ�Ƿ��д�ӡ����
		if (pPrinterInfo->cJobs != 0)
		{
			//printf("find job\n");
			//ͨ�� pJobInfo �����ṹ�� JOB_INFO_2 �� ȡ�ô�ӡ������Ϣ

			//ֽ������
			if (pJobInfo[0].pDevMode->dmPaperSize == DMPAPER_A4)
			{
				strPageSize = _T("A4");
			}
			else if (pJobInfo[0].pDevMode->dmPaperSize == DMPAPER_B5)
			{
				strPageSize = _T("B5");
			}
			//��ӡ�ļ�����
			strDOCname.Format(L"%s", pJobInfo[0].pDocument);

			//��ӡ����
			//strPrintCopies.Format("%d", pJobInfo[0].pDevMode->dmCopies);
			strPrintCopies.Format(L"%d", pJobInfo[0].pDevMode->dmCopies);

			//��ӡҳ��

			strPrintTotalPages.Format(L"%d", pJobInfo[0].TotalPages);
			//_ultoa_s(pJobInfo[0].TotalPages, tmpBuf,40, 10);
			//��ӡ������
			strMachinename.Format(L"%s", pJobInfo[0].pMachineName);

			//��ӡ�û���
			strUsername.Format(L"%s", pJobInfo[0].pUserName);

			//��ӡ��ɫ

			if (pJobInfo[0].pDevMode->dmColor == DMCOLOR_COLOR)
				strPrintColor = _T("��ɫ");
			else if (pJobInfo[0].pDevMode->dmColor == DMCOLOR_MONOCHROME)
				strPrintColor = _T("�ڰ�");

			//��ӡʱ��

			strSubmitted.Format(L"%d-%2d-%2d %2d:%2d:%2d",
				pJobInfo[0].Submitted.wYear, pJobInfo[0].Submitted.wMonth, pJobInfo[0].Submitted.wDay,
				pJobInfo[0].Submitted.wHour + 8, pJobInfo[0].Submitted.wMinute, pJobInfo[0].Submitted.wSecond);

			//���´�ӡ��״̬�б�ؼ�
			//UpdateDataPrinterStatusListCtrl(pJobInfo[0].pDocument, strPageSize,
			//	strPrintCopies, strPrintColor, strSubmitted);
			//if( strDOCname != pJobInfo[0].pDocument)
			//{
			//cout << "               �ļ���:  " << strDOCname << endl;
			//cout << "           ��ӡ������:  " << strMachinename << endl;
			//cout << "         ��ӡ��ǰ�û�:  " << strUsername << endl;
			//cout << "             ֽ������:  " << strPageSize << endl;
			//cout << "             ��ӡ����:  " << strPrintCopies << endl;
			//cout << "���ڴ�ӡ���Ѵ�ӡ��ҳ��:  " << strPrintTotalPages << endl;
			////cout << "���ڴ�ӡ���Ѵ�ӡ��ҳ��:  " << pJobInfo[0].PagesPrinted << endl;
			//cout << "        strPrintColor:  " << strPrintColor << endl;
			//cout << "             ��ӡʱ��:  " << strSubmitted << endl;
			//cout << "         ��ӡ��������:  " << pJobInfo[0].pPrintProcessor << endl;//���ڴ�ӡ��ҵ�Ĵ�ӡ������������ 
			markP = 1;
			//}

		} //end if //��⵱ǰ�Ƿ��д�ӡ����
		else
		{
			if (1 == markP)
			{
				cout << "               �ļ���:  " << strDOCname << endl;
				cout << "           ��ӡ������:  " << strMachinename << endl;
				cout << "         ��ӡ��ǰ�û�:  " << strUsername << endl;
				cout << "             ֽ������:  " << strPageSize << endl;
				cout << "             ��ӡ����:  " << strPrintCopies << endl;
				cout << "���ڴ�ӡ���Ѵ�ӡ��ҳ��:  " << strPrintTotalPages << endl;
				//cout << "���ڴ�ӡ���Ѵ�ӡ��ҳ��:  " << pJobInfo[0].PagesPrinted << endl;
				cout << "        strPrintColor:  " << strPrintColor << endl;
				cout << "             ��ӡʱ��:  " << strSubmitted << endl;

				Sleep(5000);

				string strSQL = "2522671ecd42167894f513e9e281ac0a";
				//DTconn.user_query(strSQL);
				DTconn.user_insert();



				pPrinterInfo = NULL;
				pJobInfo = NULL;

				strDOCname = _T("");
				strMachinename = _T("");
				strUsername = _T("");
				strPageSize = _T("");
				strPrintCopies = _T("");
				strPrintTotalPages = _T("");
				strPrintColor = _T("");
				strSubmitted = _T("");
				markP = 0;
			}

		}




	} //end while


	free(pPrinterInfo);
	//�رմ�ӡ���豸
	ClosePrinter(printerHandle);

	DTconn.FreeConnect();
	return NULL;
}


void WINAPI ServiceHandler(DWORD fdwControl)
{
	switch (fdwControl)
	{
	case SERVICE_CONTROL_STOP:
	case SERVICE_CONTROL_SHUTDOWN:
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		ServiceStatus.dwCheckPoint = 0;
		ServiceStatus.dwWaitHint = 0;
		uaquit = 1;
		//add you quit code here
		if (logg != NULL)
			fclose(logg);
		break;
	default:
		return;
	};
	if (!SetServiceStatus(hServiceStatusHandle, &ServiceStatus))
	{
		DWORD nError = GetLastError();
	}
}


void WINAPI service_main(int argc, char** argv)
{
	//ָ�������ִ���ļ�������- �磺��ʾ�÷���˽��   SERVICE_WIN32_OWN_PROCESS;
	//�����ִ���ļ���ֻ��һ�������ķ��񣬾Ͱ������Ա���ó�SERVICE_WIN32_OWN_PROCESS��
	//���ӵ�ж������Ļ��������ó�SERVICE_WIN32_SHARE_PROCESS��
	//������������־֮�⣬�����ķ�����Ҫ�����淢����������Ȼ���Ƽ�������������Ҫ�á�|�������������SERVICE_INTERACTIVE_PROCESS��
	//�����Ա��ֵ�ڷ�����������ھ��Բ�Ӧ�øı䡣
	ServiceStatus.dwServiceType = SERVICE_WIN32;

	//����֪ͨSCM�˷��������״̬   
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;//��ʼ���������ڿ�ʼ

	//ָ���������ʲô���Ŀ���֪ͨ��
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;

	//��������񱨸����Ĺؼ������ϣ������ȥ����һ��Win32������루Ԥ������WinError.h�У�����������dwWin32ExitCodeΪ��Ҫ�Ĵ��롣
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;

	//��һ������������������ǰ���¼���չ����ġ�����ԱdwCurrentState�����ó�SERVICE_START_PENDING��ʱ��Ӧ�ð�dwCheckPoint���0��
	ServiceStatus.dwCheckPoint = 0;

	//dwWaitHint���һ��������γ��Ժ�ȷ���ȽϺ��ʵĳ�ʱ������������������ܸ�Ч���С�
	ServiceStatus.dwWaitHint = 0;

	//ͨ��RegisterServiceCtrlHandler()�������Ƴ�����һ��ͨ�ŵ�Э�顣
	hServiceStatusHandle = RegisterServiceCtrlHandler(_T(SERVICE_NAME), ServiceHandler);
	if (hServiceStatusHandle == 0)
	{
		DWORD nError = GetLastError();
	}
	//add your init code here
	logg = fopen("d:\\test.txt", "w");
	//add your service thread here
	HANDLE task_handle = CreateThread(NULL, NULL, srv_core_thread, NULL, NULL, NULL);
	if (task_handle == NULL)
	{
		fprintf(logg, "create srv_core_thread failed\n");
	}

	// Initialization complete - report running status 
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwCheckPoint = 0;
	ServiceStatus.dwWaitHint = 9000;
	if (!SetServiceStatus(hServiceStatusHandle, &ServiceStatus))
	{
		DWORD nError = GetLastError();
	}

}
//do not change main function
int main(int argc, const char *argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[2];

	ServiceTable[0].lpServiceName = _T(SERVICE_NAME);
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)service_main;

	//SERVICE_TABLE_ENTRY�ṹ����Ҫ�����һ����Ա�鶼ΪNULL�����ǳ�֮Ϊ���ڱ���������ֵ��ΪNULL������ʾ�÷����ĩβ��
	//һ���������������ϵ���StartServiceCtrlDispatcher()֪ͨ������Ƴ����������ִ�У����ṩ�������ĵ�ַ��
	//StartServiceCtrlDispatcher()ֻ��Ҫһ����������SERVICE_TABLE_ENTRY�ṹ�����飬��Ϊÿ����������һ���̣߳�һֱ�ȵ����ǽ����ŷ��ء�
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	// ��������Ŀ��Ʒ��ɻ��߳�
	StartServiceCtrlDispatcher(ServiceTable);
	return 0;
}