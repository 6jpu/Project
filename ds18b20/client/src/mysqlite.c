/*********************************************************************************
 *      Copyright:    (C) 2023 Kun_<1433729173@qq.com>
 *                    All rights reserved.
 *
 *      Filename:     test.c
 *      Description:  This sqlite file 
 *                 
 *      Version:      2.0.0(2023年04月30日)
 *      Author:       Kun_ <1433729173@qq.com>
 *      ChangeLog:    1, Release initial version on "2023年04月10日 17时25分12秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "pack.h"
#include "mysqlite.h"

int                   rc;
int                   rows;
int                   cols;
char                  table_name[32] = "TEMP";
char                 *buf;
sqlite3_stmt         *pStmt = NULL;
static sqlite3       *s_db = NULL;
char                 *zErrMsg = 0;
char                 *sql;
char                **dbresult;


int db_init(char *db_name)
{

	/* Open database */
	rc = sqlite3_open(db_name, &s_db);	
	if ( rc )  //0:Successful result 
	{
		PARSE_LOG_ERROR("Can't open database: %s\n", sqlite3_errmsg(s_db));
		return -1;
	}
	else 
	{
		PARSE_LOG_INFO("Open database Successfully\n");

	}

	/* Create SQL statement */
	sql=sqlite3_mprintf("DROP TABLE IF EXISTS %s;"
			"CREATE TABLE %s(ID INTEGER PRIMARY KEY, Data BLOB);",table_name,table_name);
	
	/* Execute SQL statement */
	rc = sqlite3_exec(s_db, sql, 0, 0, &zErrMsg);
	if ( rc != SQLITE_OK )
	{
		PARSE_LOG_ERROR("SQL error :%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return -2;
	}
	else
	{
		PARSE_LOG_INFO("Table created Successfully\n");
	}

	return 0;
} 


int db_insert(packet_t pack)
{
	/* insert data  */
	sql=sqlite3_mprintf("INSERT INTO %s(Data) VALUES(?)",table_name); // 插入新值
	rc = sqlite3_prepare(s_db, sql, -1, &pStmt, NULL);
	if (rc != SQLITE_OK) 
	{    
		PARSE_LOG_ERROR("Cannot prepare statement: %s\n", sqlite3_errmsg(s_db));   
		return -2;
	}

	packet_t data_in = pack; // 准备要写入的值
	PARSE_LOG_DEBUG("time:%s,temp:%f,sn:%s\n", data_in.time, data_in.temp, data_in.devsn);

	sqlite3_bind_blob(pStmt, 1, &data_in, sizeof(data_in), SQLITE_STATIC); // 绑定需要写入的值
	rc = sqlite3_step(pStmt);
	if (rc != SQLITE_DONE)
	{
		PARSE_LOG_ERROR("execution failed: %s", sqlite3_errmsg(s_db));
		return -3;
	}

	sqlite3_finalize(pStmt);    

	return 0;

}


int db_select(char *str)
{
	buf = sqlite3_mprintf("select * from %s", table_name);
	sqlite3_get_table(s_db, buf, &dbresult, &rows, NULL, NULL); //查看最后一条数据
	if ( rows <= 0)
	{
		PARSE_LOG_WARN("No data for select in database\n");
		return -1;
	}

	sql=sqlite3_mprintf("SELECT Data from %s WHERE ID = %d;", table_name, rows);

	/* Execute SQL statement */
	rc = sqlite3_prepare_v2(s_db, sql, -1, &pStmt, NULL);
	if ( rc != SQLITE_OK )
	{
		PARSE_LOG_ERROR("Failed to prepare statement\n");		      
		PARSE_LOG_ERROR("Cannot open database: %s\n", sqlite3_errmsg(s_db));
		sqlite3_close(s_db);
		return -3;
	}

	rc = sqlite3_step(pStmt);

	packet_t *pData = ( packet_t*)sqlite3_column_blob(pStmt, 0);

	sprintf(str, "%s,%f,%s", pData->time, pData->temp, pData->devsn);
	PARSE_LOG_DEBUG("%s\n",str);

	sqlite3_finalize(pStmt);   

	return 0;
}


int db_delete(void)
{
	buf =sqlite3_mprintf("select * from %s", table_name);
	sqlite3_get_table(s_db, buf, &dbresult, &rows, NULL, NULL); //查看最后一条数据
	if ( rows <= 0)
	{
		PARSE_LOG_WARN("No data for select in database\n");
		return 0;
	}

	sql=sqlite3_mprintf("DELETE from %s WHERE ID = %d;", table_name, rows);

	/* Execute SQL statement */
	rc = sqlite3_prepare_v2(s_db, sql, -1, &pStmt, NULL);
	if ( rc != SQLITE_OK )
	{
		PARSE_LOG_ERROR("SQL error: %s\n", sqlite3_errmsg(s_db));
		return -1;
	}
	else
	{
		rc = sqlite3_step(pStmt);
		if (rc != SQLITE_DONE)
		{
			PARSE_LOG_ERROR("execution failed: %s", sqlite3_errmsg(s_db));
			return -2;
		}

		sqlite3_finalize(pStmt);
		PARSE_LOG_INFO("Delete data successfully\n");
	}

	return 0;
}

int db_close(void)                 
{
	if( SQLITE_OK != sqlite3_close(s_db) )
	{
		PARSE_LOG_WARN("Error close database: %s\n", zErrMsg );
		sqlite3_free(zErrMsg);

		return -1;
	}   
	PARSE_LOG_INFO("successfully close database\n");
	return 0;
}
