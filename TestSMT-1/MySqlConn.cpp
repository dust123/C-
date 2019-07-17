
#include "stdafx.h"
#include "MySqlConn.h"
#include <fstream>

MySqlConn::MySqlConn()
{
	MySqlConn::a = 0;
	//this->a = 0;
}


MySqlConn::~MySqlConn()
{
	 
}

bool MySqlConn::initConnection()
{

	mysql_init(&m_sqlCon); 
	//����false������ʧ�ܣ�����true�����ӳɹ�  
	mysql_options(&m_sqlCon, MYSQL_OPT_COMPRESS, 0);
	if ( !mysql_real_connect(&m_sqlCon, host, user, psw, DB, port, NULL, 0))
		//�м�ֱ����������û��������룬���ݿ������˿ںţ�����дĬ��0����3306�ȣ���������д�ɲ����ٴ���ȥ  
	{
		printf("Error connecting to database:%s\n", mysql_error(&m_sqlCon));
		return false;
	}
	else
	{
		printf("���ӳɹ�...\n");
		return true;
	}
}

void MySqlConn::user_query(string strMAC)
{
	char query[255]; 

	sprintf_s(query, sizeof(query), "select * from PrintUser where MachineMac=\"%s\" ", strMAC.c_str() );
	mysql_query(&m_sqlCon, "SET NAMES GB2312");

	//FILE *fp;
	//fopen_s(&fp, "e:\\debug.txt", "a+");
	//_ftprintf(fp, _T("%s\n"), query);
	//fclose(fp);
	 
	MYSQL_ROW sql_row;
	MYSQL_RES *result;//��ý����

	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "��ѯʧ��" << std::endl;
		//::MessageBox(NULL, "��ѯʧ��", "qq", 0);
		//fopen_s(&fp, "e:\\debug.txt", "a+");
		//_ftprintf(fp, _T("%s\n"), "��ѯʧ��");
		//fclose(fp);
	}
	else
	{
		std::cout << "��ѯ�ɹ�" << std::endl;
		//::MessageBox(NULL, "��ѯ�ɹ�", "qq", 0);
		//fopen_s(&fp, "e:\\debug.txt", "a+");
		//_ftprintf(fp, _T("%s\n"), "��ѯ�ɹ�");
		//fclose(fp);
	}
	result = mysql_store_result(&m_sqlCon);
	if (result) 
	{
		my_ulonglong  row_num;
		int col_num;
		row_num = mysql_num_rows(result);
		col_num = mysql_num_fields(result);

		//fopen_s(&fp, "e:\\debug.txt", "a+");
		//_ftprintf(fp, _T("if (result) �鵽����ֵΪ��  %d\n"), (int)row_num);
		//fclose(fp);

		if (1 == row_num)
		{

			//fopen_s(&fp, "e:\\debug.txt", "a+");
			//_ftprintf(fp, _T("if (1 == row_num)\n"));
			//fclose(fp);

			sql_row = mysql_fetch_row(result);

			//if (sql_row[1] == NULL) { GetUserPW = ""; }
			//if (sql_row[2] == NULL) { strPCarNumber = ""; }
			//if (sql_row[3] == NULL) { strPUserNickName = ""; }
			if (sql_row[4] == NULL) { strPCarNumber = ""; }
			if (sql_row[5] == NULL) { strPUserNickName = ""; }
			if (sql_row[6] == NULL) { strPUserName = ""; }
			//if (sql_row[7] == NULL) { strPCarNumber = ""; }
			if (sql_row[8] == NULL) { strPprintMark = ""; }
			 
		}
	}
 
	//fopen_s(&fp, "e:\\debug.txt", "a+");
	//_ftprintf(fp, _T("%s\n"), strPCarNumber.c_str());
	//fclose(fp);

	if (result != NULL)
		mysql_free_result(result);

}

void MySqlConn::user_insert(string strSQL)
{

	//ofstream OutFile("d://Test.txt"); //���ù��캯������txt�ı������Ҵ򿪸��ı�
	//OutFile << strSQL.c_str();  //���ַ�������"This is a Test!"��д��Test.txt�ļ�
	//OutFile.close();            //�ر�Test.txt�ļ�


	char query[10240];
	//strSQL = "insert into PrintDB(Pfilename,PMachinename,PUserName,PpageSize ,PCopies ,Ppage,PColor,Ptime ,Premark) values('��ȡλ�� 0x000000000000002E ʱ�������ʳ�ͻ��_�ٶ�����','\\DESKTOP-V3CQ8LP','1606-KF','A4','1','1','��ɫ','2019-05-31 15:47:27','')";
	sprintf_s(query, sizeof(query), strSQL.c_str() );

	mysql_query(&m_sqlCon, "SET NAMES GB2312");//�����

	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "��������ʧ��" << std::endl;
		//return;
	}
 
}
 

//�ͷ���Դ  
void MySqlConn::FreeConnect()
{
	//mysql_free_result(res);  //�ͷ�һ���������ʹ�õ��ڴ档
	mysql_close(&m_sqlCon);	 //�ر�һ�����������ӡ�
}