// TestSMT-1.cpp : 定义控制台应用程序的入口点。
//#include <DSRole.h>

#include "stdafx.h"
//#include "Windows.h"

#include "MySqlConn.h"

#define SERVICE_NAME "PrintT"

#include <afxwin.h>  

#include <winspool.h>

#pragma comment(lib, "winspool.lib")
#pragma comment(lib, "netapi32.lib") 
#pragma comment(lib, "Advapi32.lib")

//LPWSTR怎样转成string
#include <atlconv.h>

//getMAC/IP
#include <Iphlpapi.h> 
#pragma comment(lib,"Iphlpapi.lib") //需要添加Iphlpapi.lib库


//map
#include <map> 
//文件路径
#include <direct.h>

#include <iomanip>

#include <fstream>

#include <stdio.h>
#include<iostream> 
#include <string>    
#include <cstring>    
#include <thread>     

#pragma comment(lib,"User32.lib")
using namespace std;



SERVICE_STATUS ServiceStatus;
SERVICE_STATUS_HANDLE hServiceStatusHandle;
void WINAPI service_main(int argc, char** argv);
void WINAPI ServiceHandler(DWORD fdwControl);

TCHAR szSvcName[180];
SC_HANDLE schSCManager;
SC_HANDLE schService;
int uaquit;
//FILE* logg;

//--------------------------------------------
//取路径
string GetProgramDir()
{
	TCHAR exeFullPath[MAX_PATH]; // Full path  
	GetModuleFileName(NULL, exeFullPath, MAX_PATH);
	string strPath = __TEXT("");
	strPath = exeFullPath;    // Get full path of the file  
	size_t pos = strPath.find_last_of(L'\\', strPath.length());

	return strPath.substr(0, pos);  // Return the directory without the file name  
}


DWORD WINAPI srv_core_thread(LPVOID para)
{
	//FILE *fp;
	//fopen_s(&fp, "e:\\debug.txt", "a+");
	//fclose(fp);

	//数据库
	MySqlConn DTconn; 
	bool isCon;
	isCon = DTconn.initConnection();
	
	//配置文件
	//--------------------------------------------------------------------------------------------------------
	//读配置文件
	map<string, string> mapConfig;

	ifstream configFile;
	string path = "";
	string FilePath = "";
	string strPrint = ""; 

	char *buffer=NULL;

	//也可以将buffer作为输出参数
	if ((buffer = _getcwd(NULL, 0)) == NULL)
	{
		::MessageBox(NULL, "读取配置文件路径出错", "路径错误", 0);
		exit(-1);
	}
	else
	{
		path = buffer;
		free(buffer);
		//ofstream OutFile("d://Test.txt"); //利用构造函数创建txt文本，并且打开该文本
		//OutFile << path.c_str();  //把字符串内容"This is a Test!"，写入Test.txt文件
		//OutFile.close();            //关闭Test.txt文件
		//printf("%s\n", buffer);

	}

	size_t pos = path.find("sys");

	if ((-1 == pos) || (-1 == pos))
	{
		cout << "登录OK" << std::endl;
	}
	else
	{
		path = GetProgramDir();
	}

	
	FilePath = path;
	path += "\\Psetting.conf";
	//path = "F:\\VS2015\\VC++\\Threads\\Threads\\setting.conf";
 
	//fopen_s(&fp, "e:\\debug.txt", "a+");
	//_ftprintf(fp, _T("%s\n"), path.c_str());
	//fclose(fp);
	

	configFile.open(path.c_str());
	string str_line;
	if (configFile.is_open())
	{
		//cout << "configFile is_open" << endl;
		while (!configFile.eof())
		{
			getline(configFile, str_line);
			if (str_line.find('#') == 0) //过滤掉注释信息，即如果首个字符为#就过滤掉这一行
			{
				continue;
			}
			size_t pos = str_line.find('=');
			string str_key = str_line.substr(0, pos);
			string str_value = str_line.substr(pos + 1);
			//cout << "str_key is:" << str_key << endl;
			//cout << "str_value is:" << str_value << endl;
			mapConfig.insert(pair<string, string>(str_key, str_value));
		}

		//cout << "map is_open" << endl;
		map<string, string>::iterator iter_configMap;
		iter_configMap = mapConfig.find(string("print"));

		if (iter_configMap != mapConfig.end())
		{
			//cout << "path is:" << iter_configMap->second << endl;
			strPrint = iter_configMap->second;
			 
		}

 

	}
	else
	{
		::MessageBox(NULL, "读取配置文件出错", "错误", 0);
		//cout << "Cannot open config file setting.ini, path: ";
		exit(-1);
	}


	//--------------------------------------------------------------------------------------------------------

	//打印
	HANDLE printerHandle;   //打印机设备句柄

	//检测打开打印机设备是否成功
	//if (!OpenPrinter(TEXT("Microsoft Print to PDF"), &printerHandle, NULL))
	//Pname = TEXT( strPrint.c_str() );
	if (!OpenPrinter((LPSTR)strPrint.c_str(), &printerHandle, NULL))
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

	CString strDOCname			= _T("");
	CString strMachinename		= _T("");
	CString strUsername			= _T("");
	CString strPageSize			= _T("");
	CString strPrintCopies		= _T("");
	CString strPrintTotalPages  = _T("");
	CString strPrintColor		= _T("");
	CString strSubmitted		= _T("");

	int markP = 0;
	CString CstrSQL = _T("");
	string strSQL = "";

	//ip MAC
	//记录网卡数量
	int netCardNum = 0;
	//记录每张网卡上的IP地址数量
	//int IPnumPerNetCard = 0;
	string strMAC = "";
	string strIP = "";
	char mactemp[10];
	int nRel = 0;
	unsigned long stSize = 0;

	//时间
	char chTime[40] = { 0 };
	SYSTEMTIME sdt = { 0 };



	while (true)
	{		
		
		//printf("in threadFUN\n");
		//通过调用GetPrinter()函数得到作业数量
		Sleep(200);
		GetPrinter(printerHandle, 2, NULL, 0, &nByteNeeded);
		pPrinterInfo = (PRINTER_INFO_2*)malloc(nByteNeeded);
		GetPrinter(printerHandle, 2, (LPBYTE)pPrinterInfo, nByteNeeded, &nByteUsed);

		
		//fopen_s(&fp, "e:\\debug.txt", "a+");
		//_ftprintf(fp, _T("%s\n"), "in while");
		//fclose(fp);

		//检测当前是否有打印任务
		if (pPrinterInfo->cJobs != 0)
		{


			//通过调用EnumJobs()函数枚举打印任务

			EnumJobs(printerHandle, 0, pPrinterInfo->cJobs, 2, NULL, 0, (LPDWORD)&nByteNeeded, (LPDWORD)&nReturned);

			pJobInfo = (JOB_INFO_2*)malloc(nByteNeeded);

			ZeroMemory(pJobInfo, nByteNeeded);

			EnumJobs(printerHandle, 0, pPrinterInfo->cJobs, 2, (LPBYTE)pJobInfo, nByteNeeded, (LPDWORD)&nByteUsed, (LPDWORD)&nReturned);


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
			strDOCname.Format("%s", pJobInfo[0].pDocument);

			//打印份数
			//strPrintCopies.Format("%d", pJobInfo[0].pDevMode->dmCopies);
			strPrintCopies.Format("%d", pJobInfo[0].pDevMode->dmCopies);

			//打印页数

			strPrintTotalPages.Format("%d", pJobInfo[0].TotalPages);
			//_ultoa_s(pJobInfo[0].TotalPages, tmpBuf,40, 10);
			//打印机器名
			strMachinename.Format("%s", pJobInfo[0].pMachineName);

			//打印用户名
			strUsername.Format("%s", pJobInfo[0].pUserName);

			//打印颜色

			if (pJobInfo[0].pDevMode->dmColor == DMCOLOR_COLOR)
				strPrintColor = _T("彩色");
			else if (pJobInfo[0].pDevMode->dmColor == DMCOLOR_MONOCHROME)
				strPrintColor = _T("黑白");

			//打印时间

			//strSubmitted.Format("%d-%02d-%02d %02d:%02d:%02d",
			//	pJobInfo[0].Submitted.wYear, pJobInfo[0].Submitted.wMonth, pJobInfo[0].Submitted.wDay,
			//	pJobInfo[0].Submitted.wHour + 8, pJobInfo[0].Submitted.wMinute, pJobInfo[0].Submitted.wSecond);

		 
			markP = 1;
		 

		} //end if //检测当前是否有打印任务
		else
		{
			if (1 == markP)
			{ 
				
				//fopen_s(&fp, "e:\\debug.txt", "a+");
				//_ftprintf(fp, _T("%s\n"), "in if (1 == markP)");
				//fclose(fp);
				
				//--------------------------------------------------------------------
				//IP MAC
				strIP = "";
				strMAC = "";
				//string 不限制长度的字符串，以“\0” 作为结尾符。长度可达 4G
				//PIP_ADAPTER_INFO结构体指针存储本机网卡信息
				PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
				//得到结构体大小,用于GetAdaptersInfo参数
				stSize = sizeof(IP_ADAPTER_INFO);
				//调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量;其中stSize参数既是一个输入量也是一个输出量
				nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);

				if (ERROR_BUFFER_OVERFLOW == nRel)
				{
					//如果函数返回的是ERROR_BUFFER_OVERFLOW
					//则说明GetAdaptersInfo参数传递的内存空间不够,同时其传出stSize,表示需要的空间大小
					//这也是说明为什么stSize既是一个输入量也是一个输出量
					//释放原来的内存空间
					delete pIpAdapterInfo;
					//重新申请内存空间用来存储所有网卡信息
					pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
					//再次调用GetAdaptersInfo函数,填充pIpAdapterInfo指针变量
					nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
				}
				if (ERROR_SUCCESS == nRel)
				{
					//输出网卡信息
					//可能有多网卡,因此通过循环去判断
					while (pIpAdapterInfo)
					{
						if (strMAC.size() > 10)
						{
							strMAC += "/";
							//strIP += "/";
						}
						if (strIP.size() > 10)
						{
							//strMAC += "/";
							strIP += "/";
						}
						//if (strMAC.size) {}
						for (DWORD i = 0; i < pIpAdapterInfo->AddressLength; i++)
							if (i < pIpAdapterInfo->AddressLength - 1)
							{
								//printf("%02X-", pIpAdapterInfo->Address[i]);
								sprintf_s(mactemp, sizeof(mactemp), "%02X-", pIpAdapterInfo->Address[i]);
								strMAC += mactemp;
								//strMAC += "-";
							}
							else
							{
								//printf("%02X\n", pIpAdapterInfo->Address[i]);
								sprintf_s(mactemp, sizeof(mactemp), "%02X", pIpAdapterInfo->Address[i]);
								strMAC += mactemp;
								strMAC += "";
							}
						//cout << "网卡IP地址如下：" << endl;
						//可能网卡有多IP,因此通过循环去判断
						IP_ADDR_STRING *pIpAddrString = &(pIpAdapterInfo->IpAddressList);
						do
						{

							strIP += pIpAddrString->IpAddress.String;

							pIpAddrString = pIpAddrString->Next;
						} while (pIpAddrString);

						pIpAdapterInfo = pIpAdapterInfo->Next;
						//cout << "--------------------------------------------------------------------" << endl;
					}//endwhile

				}
				//释放内存空间
				if (pIpAdapterInfo)
				{
					delete pIpAdapterInfo;
				}
				//chTime, szBufCPName, strIP.c_str(), strMAC.c_str()
				if (0 == strIP.length()) { strIP = "未到本机IP"; }
				if (0 == strMAC.length()) { strIP = "未到本机MAC"; }
				//IP不支持IPV6
				//--------------------------------------------------------------------

				//-----------------------------------------
				//时间
				/*取时间*/
				//使用系统时间，打印的时间有时不准确
				GetLocalTime(&sdt);
				sprintf_s(chTime, sizeof(chTime), "%d-%02d-%02d %02d:%02d:%02d",
					sdt.wYear,
					sdt.wMonth,
					sdt.wDay,
					sdt.wHour,
					sdt.wMinute,
					sdt.wSecond);
				if (0 == strlen(chTime))
				{
					strcpy_s(chTime, "2055-01-01 01:01:01"); //string 到char[] chTime = "2019-03-26 12:44:08";
				}
				//-----------------------------------------

				//fopen_s(&fp, "e:\\debug.txt", "a+");
				//_ftprintf(fp, _T("%s\n"), "befor DTconn.user_query(strMAC) ");
				//fclose(fp);

				DTconn.user_query(strMAC);
				if ("on" == DTconn.strPprintMark)
				{
					strUsername = DTconn.strPUserName.c_str();
				}
				
				//fopen_s(&fp, "e:\\debug.txt", "a+");
				//_ftprintf(fp, _T("%s\n"), "after DTconn.user_query(strMAC) ");
				//fclose(fp);

				CstrSQL = "insert into PrintDB(Pfilename,PMachinename,PIP,PMac,PUserName,PpageSize ,PCopies ,Ppage,PColor,Ptime ,Premark)\
 values('" + strDOCname + "','" + strMachinename + "','"\
+ strIP.c_str() + "','" + strMAC.c_str() + "','"\
+ strUsername + "','" + strPageSize + "','"\
+ strPrintCopies + "','" + strPrintTotalPages + "','"\
+ strPrintColor + "','" + chTime + "','')";

				USES_CONVERSION;
				//strSQL = W2A(CstrSQL.GetBuffer(0));//unicode编码
				strSQL = CstrSQL.GetBuffer(0);
				
				
				//fopen_s(&fp, "e:\\debug.txt", "a+");
				//_ftprintf(fp, _T("%s\n"), strSQL.c_str());
				//fclose(fp);


				//FilePath = GetProgramDir();
				//FilePath = FilePath + "\\Debug.txt";
				//ofstream f1(FilePath);
				//if (f1)
				//{
				//	f1 << strSQL.c_str() << endl;
				//	f1.close();
				//}


				DTconn.user_insert(strSQL);


		 
				pPrinterInfo = NULL;
				pJobInfo = NULL;

				strDOCname			= _T("");
				strMachinename		= _T("");
				strUsername			= _T("");
				strPageSize			= _T("");
				strPrintCopies		= _T("");
				strPrintTotalPages  = _T("");
				strPrintColor		= _T("");
				strSubmitted		= _T("");
				CstrSQL				= _T("");
				strSQL              = "";
				strIP               = "";
				strMAC              = "";
				memset(chTime, 0, sizeof(chTime));
				memset(mactemp, 0, sizeof(mactemp));
				nRel   = 0;
				stSize = 0;
				sdt    = { 0 };
				markP  = 0;
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
		//if (logg != NULL)
		//	fclose(logg);
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
	//logg = fopen("d:\\test.txt", "w");
	//add your service thread here
	HANDLE task_handle = CreateThread(NULL, NULL, srv_core_thread, NULL, NULL, NULL);
	if (task_handle == NULL)
	{
		//fprintf(logg, "create srv_core_thread failed\n");
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
	
	//ofstream OutFile("D:\\test1.txt"); //利用构造函数创建txt文本，并且打开该文本
	//OutFile << argv;  //把字符串内容"This is a Test!"，写入Test.txt文件
	 

	//FILE* fopentest = fopen("D:\\test1.txt", "a");
	//for (int i = 0; i < argc; ++i)
	//{
	//	fprintf(fopentest, "main: %s\n\r", argv[i]);
	//	//OutFile << argv[i] << "\r\n";
	//}
	//fclose(fopentest);
	
	//OutFile.close();

 

	//服务规则建立
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