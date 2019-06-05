
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
	//返回false则连接失败，返回true则连接成功  
	mysql_options(&m_sqlCon, MYSQL_OPT_COMPRESS, 0);
	if ( !mysql_real_connect(&m_sqlCon, host, user, psw, DB, port, NULL, 0))
		//中间分别是主机，用户名，密码，数据库名，端口号（可以写默认0或者3306等），可以先写成参数再传进去  
	{
		printf("Error connecting to database:%s\n", mysql_error(&m_sqlCon));
		return false;
	}
	else
	{
		printf("连接成功...\n");
		return true;
	}
}

void MySqlConn::user_query(string strSQL)
{
	char query[255];
	//string strsql;
	//strsql = "select * from CarTable where CarNumber=MD5(\"�_Z&";
	//strsql += strSQL.c_str();
	//strsql += "l^_\")";

	//cout << "strsql is:" << strsql.c_str() << endl;

	sprintf_s(query, sizeof(query), "select * from CarTable where CarNumber=\"%s\" ",  strSQL.c_str() );
	//char query[255] = "select * from CarTable where CarNumber = MD5(\"_Z&0005983205l^_\")";
	//::MessageBoxA(NULL, query, query, 0);
	cout <<"query is:" << query << endl;
	//mysql_query(&m_sqlCon, "SET NAMES UTF8"); 
	//mysql_query(&m_sqlCon, "SET NAMES GB2312");//用这个
	mysql_query(&m_sqlCon, "SET NAMES 'Latin1'");
	if (mysql_query(&m_sqlCon, query))
	{

		cout << "查询失败  Fale" << std::endl;
		return;
	}
	cout << "查询成功 OK" << std::endl;
	MYSQL_RES *result;//获得结果集
	result = mysql_store_result(&m_sqlCon);
	if (result) {
		my_ulonglong row_num;
	    int col_num;
		row_num = mysql_num_rows(result);
		col_num = mysql_num_fields(result);
		//std::cout << "共有" << row_num << "条数据，以下为其详细内容：" << std::endl;
		//MYSQL_FIELD *fd;
		//while (fd = mysql_fetch_field(result)) {
		//	std::cout << fd->name << "\t";
		//}
		//std::cout << std::endl;
		cout <<"col_num: "<< col_num <<"    row_num:"<< row_num << std::endl;

		MYSQL_ROW sql_row;
		MYSQL_FIELD *field; 
		if (1 == row_num)
		{
			sql_row = mysql_fetch_row(result); // 获取具体的数据
			field = mysql_fetch_field(result); //获取列名
			//printf("Field %u is %s/n", 0, fields[0]);
			//strcpy(column[i], field->UserName);
			cout << "UserName is： " <<  sql_row[1] << endl;
			cout << "用户名2： "	 <<  sql_row[2] << endl;
			cout << "用户名3： "     <<  sql_row[3] << endl;
		}
		//while (sql_row = mysql_fetch_row(result)) {
		//	for (int i = 0; i < col_num; i++) {
		//		if (sql_row[i] == NULL) std::cout << "NULL\t";
		//		else std::cout << sql_row[i] << "\t";
		//	}
		//	std::cout << std::endl;
		//}
	}
	if (result != NULL)
		mysql_free_result(result); 

}

void MySqlConn::user_insert(string strSQL)
{

	ofstream OutFile("d://Test.txt"); //利用构造函数创建txt文本，并且打开该文本
	OutFile << strSQL.c_str();  //把字符串内容"This is a Test!"，写入Test.txt文件
	OutFile.close();            //关闭Test.txt文件


	char query[10240];
	//strSQL = "insert into PrintDB(Pfilename,PMachinename,PUserName,PpageSize ,PCopies ,Ppage,PColor,Ptime ,Premark) values('读取位置 0x000000000000002E 时发生访问冲突。_百度搜索','\\DESKTOP-V3CQ8LP','1606-KF','A4','1','1','彩色','2019-05-31 15:47:27','')";
	sprintf_s(query, sizeof(query), strSQL.c_str() );

	mysql_query(&m_sqlCon, "SET NAMES GB2312");//用这个

	if (mysql_query(&m_sqlCon, query)) {
		std::cout << "插入数据失败" << std::endl;
		//return;
	}
 
}
 

//释放资源  
void MySqlConn::FreeConnect()
{
	//mysql_free_result(res);  //释放一个结果集合使用的内存。
	mysql_close(&m_sqlCon);	 //关闭一个服务器连接。
}