// TestSMT-1.cpp : 定义控制台应用程序的入口点。
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


	HANDLE printerHandle;   //打印机设备句柄

							//检测打开打印机设备是否成功
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
			//通过调用GetPrinter()函数得到作业数量

		GetPrinter(printerHandle, 2, NULL, 0, &nByteNeeded);
		pPrinterInfo = (PRINTER_INFO_2*)malloc(nByteNeeded);
		GetPrinter(printerHandle, 2, (LPBYTE)pPrinterInfo, nByteNeeded, &nByteUsed);


		//通过调用EnumJobs()函数枚举打印任务

		EnumJobs(printerHandle, 0, pPrinterInfo->cJobs, 2, NULL, 0, (LPDWORD)&nByteNeeded, (LPDWORD)&nReturned);

		pJobInfo = (JOB_INFO_2*)malloc(nByteNeeded);

		ZeroMemory(pJobInfo, nByteNeeded);

		EnumJobs(printerHandle, 0, pPrinterInfo->cJobs, 2, (LPBYTE)pJobInfo, nByteNeeded, (LPDWORD)&nByteUsed, (LPDWORD)&nReturned);


		//检测当前是否有打印任务
		if (pPrinterInfo->cJobs != 0)
		{
			//printf("find job\n");
			//通过 pJobInfo 即（结构体 JOB_INFO_2 ） 取得打印具体信息

			//纸张类型
			if (pJobInfo[0].pDevMode->dmPaperSize == DMPAPER_A4)
			{
				strPageSize = _T("A4");
			}
			else if (pJobInfo[0].pDevMode->dmPaperSize == DMPAPER_B5)
			{
				strPageSize = _T("B5");
			}
			//打印文件名称
			strDOCname.Format(L"%s", pJobInfo[0].pDocument);

			//打印份数
			//strPrintCopies.Format("%d", pJobInfo[0].pDevMode->dmCopies);
			strPrintCopies.Format(L"%d", pJobInfo[0].pDevMode->dmCopies);

			//打印页数

			strPrintTotalPages.Format(L"%d", pJobInfo[0].TotalPages);
			//_ultoa_s(pJobInfo[0].TotalPages, tmpBuf,40, 10);
			//打印机器名
			strMachinename.Format(L"%s", pJobInfo[0].pMachineName);

			//打印用户名
			strUsername.Format(L"%s", pJobInfo[0].pUserName);

			//打印颜色

			if (pJobInfo[0].pDevMode->dmColor == DMCOLOR_COLOR)
				strPrintColor = _T("彩色");
			else if (pJobInfo[0].pDevMode->dmColor == DMCOLOR_MONOCHROME)
				strPrintColor = _T("黑白");

			//打印时间

			strSubmitted.Format(L"%d-%2d-%2d %2d:%2d:%2d",
				pJobInfo[0].Submitted.wYear, pJobInfo[0].Submitted.wMonth, pJobInfo[0].Submitted.wDay,
				pJobInfo[0].Submitted.wHour + 8, pJobInfo[0].Submitted.wMinute, pJobInfo[0].Submitted.wSecond);

			//更新打印机状态列表控件
			//UpdateDataPrinterStatusListCtrl(pJobInfo[0].pDocument, strPageSize,
			//	strPrintCopies, strPrintColor, strSubmitted);
			//if( strDOCname != pJobInfo[0].pDocument)
			//{
			//cout << "               文件名:  " << strDOCname << endl;
			//cout << "           打印机器名:  " << strMachinename << endl;
			//cout << "         打印当前用户:  " << strUsername << endl;
			//cout << "             纸张类型:  " << strPageSize << endl;
			//cout << "             打印份数:  " << strPrintCopies << endl;
			//cout << "正在打印（已打印）页数:  " << strPrintTotalPages << endl;
			////cout << "正在打印（已打印）页数:  " << pJobInfo[0].PagesPrinted << endl;
			//cout << "        strPrintColor:  " << strPrintColor << endl;
			//cout << "             打印时间:  " << strSubmitted << endl;
			//cout << "         打印处理器名:  " << pJobInfo[0].pPrintProcessor << endl;//用于打印作业的打印处理器的名称 
			markP = 1;
			//}

		} //end if //检测当前是否有打印任务
		else
		{
			if (1 == markP)
			{
				cout << "               文件名:  " << strDOCname << endl;
				cout << "           打印机器名:  " << strMachinename << endl;
				cout << "         打印当前用户:  " << strUsername << endl;
				cout << "             纸张类型:  " << strPageSize << endl;
				cout << "             打印份数:  " << strPrintCopies << endl;
				cout << "正在打印（已打印）页数:  " << strPrintTotalPages << endl;
				//cout << "正在打印（已打印）页数:  " << pJobInfo[0].PagesPrinted << endl;
				cout << "        strPrintColor:  " << strPrintColor << endl;
				cout << "             打印时间:  " << strSubmitted << endl;

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
	//关闭打印机设备
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
	//指明服务可执行文件的类型- 如：表示该服务私有   SERVICE_WIN32_OWN_PROCESS;
	//如果可执行文件中只有一个单独的服务，就把这个成员设置成SERVICE_WIN32_OWN_PROCESS；
	//如果拥有多个服务的话，就设置成SERVICE_WIN32_SHARE_PROCESS。
	//除了这两个标志之外，如果你的服务需要和桌面发生交互（当然不推荐这样做），就要用“|”运算符附加上SERVICE_INTERACTIVE_PROCESS。
	//这个成员的值在服务的生存期内绝对不应该改变。
	ServiceStatus.dwServiceType = SERVICE_WIN32;

	//用于通知SCM此服务的现行状态   
	ServiceStatus.dwCurrentState = SERVICE_START_PENDING;//初始化服务，正在开始

	//指明服务接受什么样的控制通知。
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_PAUSE_CONTINUE;

	//是允许服务报告错误的关键，如果希望服务去报告一个Win32错误代码（预定义在WinError.h中），它就设置dwWin32ExitCode为需要的代码。
	ServiceStatus.dwWin32ExitCode = 0;
	ServiceStatus.dwServiceSpecificExitCode = 0;

	//是一个服务用来报告它当前的事件进展情况的。当成员dwCurrentState被设置成SERVICE_START_PENDING的时候，应该把dwCheckPoint设成0，
	ServiceStatus.dwCheckPoint = 0;

	//dwWaitHint设成一个经过多次尝试后确定比较合适的超时毫秒数，这样服务才能高效运行。
	ServiceStatus.dwWaitHint = 0;

	//通过RegisterServiceCtrlHandler()与服务控制程序建立一个通信的协议。
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

	//SERVICE_TABLE_ENTRY结构数组要求最后一个成员组都为NULL，我们称之为“哨兵”（所有值都为NULL），表示该服务表末尾。
	//一个服务启动后，马上调用StartServiceCtrlDispatcher()通知服务控制程序服务正在执行，并提供服务函数的地址。
	//StartServiceCtrlDispatcher()只需要一个至少有两SERVICE_TABLE_ENTRY结构的数组，它为每个服务启动一个线程，一直等到它们结束才返回。
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	// 启动服务的控制分派机线程
	StartServiceCtrlDispatcher(ServiceTable);
	return 0;
}