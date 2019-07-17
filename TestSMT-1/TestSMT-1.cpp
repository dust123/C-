// TestSMT-1.cpp : �������̨Ӧ�ó������ڵ㡣
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

//LPWSTR����ת��string
#include <atlconv.h>

//getMAC/IP
#include <Iphlpapi.h> 
#pragma comment(lib,"Iphlpapi.lib") //��Ҫ���Iphlpapi.lib��


//map
#include <map> 
//�ļ�·��
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
//ȡ·��
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

	//���ݿ�
	MySqlConn DTconn; 
	bool isCon;
	isCon = DTconn.initConnection();
	
	//�����ļ�
	//--------------------------------------------------------------------------------------------------------
	//�������ļ�
	map<string, string> mapConfig;

	ifstream configFile;
	string path = "";
	string FilePath = "";
	string strPrint = ""; 

	char *buffer=NULL;

	//Ҳ���Խ�buffer��Ϊ�������
	if ((buffer = _getcwd(NULL, 0)) == NULL)
	{
		::MessageBox(NULL, "��ȡ�����ļ�·������", "·������", 0);
		exit(-1);
	}
	else
	{
		path = buffer;
		free(buffer);
		//ofstream OutFile("d://Test.txt"); //���ù��캯������txt�ı������Ҵ򿪸��ı�
		//OutFile << path.c_str();  //���ַ�������"This is a Test!"��д��Test.txt�ļ�
		//OutFile.close();            //�ر�Test.txt�ļ�
		//printf("%s\n", buffer);

	}

	size_t pos = path.find("sys");

	if ((-1 == pos) || (-1 == pos))
	{
		cout << "��¼OK" << std::endl;
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
			if (str_line.find('#') == 0) //���˵�ע����Ϣ��������׸��ַ�Ϊ#�͹��˵���һ��
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
		::MessageBox(NULL, "��ȡ�����ļ�����", "����", 0);
		//cout << "Cannot open config file setting.ini, path: ";
		exit(-1);
	}


	//--------------------------------------------------------------------------------------------------------

	//��ӡ
	HANDLE printerHandle;   //��ӡ���豸���

	//���򿪴�ӡ���豸�Ƿ�ɹ�
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
	//��¼��������
	int netCardNum = 0;
	//��¼ÿ�������ϵ�IP��ַ����
	//int IPnumPerNetCard = 0;
	string strMAC = "";
	string strIP = "";
	char mactemp[10];
	int nRel = 0;
	unsigned long stSize = 0;

	//ʱ��
	char chTime[40] = { 0 };
	SYSTEMTIME sdt = { 0 };



	while (true)
	{		
		
		//printf("in threadFUN\n");
		//ͨ������GetPrinter()�����õ���ҵ����
		Sleep(200);
		GetPrinter(printerHandle, 2, NULL, 0, &nByteNeeded);
		pPrinterInfo = (PRINTER_INFO_2*)malloc(nByteNeeded);
		GetPrinter(printerHandle, 2, (LPBYTE)pPrinterInfo, nByteNeeded, &nByteUsed);

		
		//fopen_s(&fp, "e:\\debug.txt", "a+");
		//_ftprintf(fp, _T("%s\n"), "in while");
		//fclose(fp);

		//��⵱ǰ�Ƿ��д�ӡ����
		if (pPrinterInfo->cJobs != 0)
		{


			//ͨ������EnumJobs()����ö�ٴ�ӡ����

			EnumJobs(printerHandle, 0, pPrinterInfo->cJobs, 2, NULL, 0, (LPDWORD)&nByteNeeded, (LPDWORD)&nReturned);

			pJobInfo = (JOB_INFO_2*)malloc(nByteNeeded);

			ZeroMemory(pJobInfo, nByteNeeded);

			EnumJobs(printerHandle, 0, pPrinterInfo->cJobs, 2, (LPBYTE)pJobInfo, nByteNeeded, (LPDWORD)&nByteUsed, (LPDWORD)&nReturned);


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
			strDOCname.Format("%s", pJobInfo[0].pDocument);

			//��ӡ����
			//strPrintCopies.Format("%d", pJobInfo[0].pDevMode->dmCopies);
			strPrintCopies.Format("%d", pJobInfo[0].pDevMode->dmCopies);

			//��ӡҳ��

			strPrintTotalPages.Format("%d", pJobInfo[0].TotalPages);
			//_ultoa_s(pJobInfo[0].TotalPages, tmpBuf,40, 10);
			//��ӡ������
			strMachinename.Format("%s", pJobInfo[0].pMachineName);

			//��ӡ�û���
			strUsername.Format("%s", pJobInfo[0].pUserName);

			//��ӡ��ɫ

			if (pJobInfo[0].pDevMode->dmColor == DMCOLOR_COLOR)
				strPrintColor = _T("��ɫ");
			else if (pJobInfo[0].pDevMode->dmColor == DMCOLOR_MONOCHROME)
				strPrintColor = _T("�ڰ�");

			//��ӡʱ��

			//strSubmitted.Format("%d-%02d-%02d %02d:%02d:%02d",
			//	pJobInfo[0].Submitted.wYear, pJobInfo[0].Submitted.wMonth, pJobInfo[0].Submitted.wDay,
			//	pJobInfo[0].Submitted.wHour + 8, pJobInfo[0].Submitted.wMinute, pJobInfo[0].Submitted.wSecond);

		 
			markP = 1;
		 

		} //end if //��⵱ǰ�Ƿ��д�ӡ����
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
				//string �����Ƴ��ȵ��ַ������ԡ�\0�� ��Ϊ��β�������ȿɴ� 4G
				//PIP_ADAPTER_INFO�ṹ��ָ��洢����������Ϣ
				PIP_ADAPTER_INFO pIpAdapterInfo = new IP_ADAPTER_INFO();
				//�õ��ṹ���С,����GetAdaptersInfo����
				stSize = sizeof(IP_ADAPTER_INFO);
				//����GetAdaptersInfo����,���pIpAdapterInfoָ�����;����stSize��������һ��������Ҳ��һ�������
				nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);

				if (ERROR_BUFFER_OVERFLOW == nRel)
				{
					//����������ص���ERROR_BUFFER_OVERFLOW
					//��˵��GetAdaptersInfo�������ݵ��ڴ�ռ䲻��,ͬʱ�䴫��stSize,��ʾ��Ҫ�Ŀռ��С
					//��Ҳ��˵��ΪʲôstSize����һ��������Ҳ��һ�������
					//�ͷ�ԭ�����ڴ�ռ�
					delete pIpAdapterInfo;
					//���������ڴ�ռ������洢����������Ϣ
					pIpAdapterInfo = (PIP_ADAPTER_INFO)new BYTE[stSize];
					//�ٴε���GetAdaptersInfo����,���pIpAdapterInfoָ�����
					nRel = GetAdaptersInfo(pIpAdapterInfo, &stSize);
				}
				if (ERROR_SUCCESS == nRel)
				{
					//���������Ϣ
					//�����ж�����,���ͨ��ѭ��ȥ�ж�
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
						//cout << "����IP��ַ���£�" << endl;
						//���������ж�IP,���ͨ��ѭ��ȥ�ж�
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
				//�ͷ��ڴ�ռ�
				if (pIpAdapterInfo)
				{
					delete pIpAdapterInfo;
				}
				//chTime, szBufCPName, strIP.c_str(), strMAC.c_str()
				if (0 == strIP.length()) { strIP = "δ������IP"; }
				if (0 == strMAC.length()) { strIP = "δ������MAC"; }
				//IP��֧��IPV6
				//--------------------------------------------------------------------

				//-----------------------------------------
				//ʱ��
				/*ȡʱ��*/
				//ʹ��ϵͳʱ�䣬��ӡ��ʱ����ʱ��׼ȷ
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
					strcpy_s(chTime, "2055-01-01 01:01:01"); //string ��char[] chTime = "2019-03-26 12:44:08";
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
				//strSQL = W2A(CstrSQL.GetBuffer(0));//unicode����
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
	
	//ofstream OutFile("D:\\test1.txt"); //���ù��캯������txt�ı������Ҵ򿪸��ı�
	//OutFile << argv;  //���ַ�������"This is a Test!"��д��Test.txt�ļ�
	 

	//FILE* fopentest = fopen("D:\\test1.txt", "a");
	//for (int i = 0; i < argc; ++i)
	//{
	//	fprintf(fopentest, "main: %s\n\r", argv[i]);
	//	//OutFile << argv[i] << "\r\n";
	//}
	//fclose(fopentest);
	
	//OutFile.close();

 

	//���������
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