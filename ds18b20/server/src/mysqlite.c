/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  mysqlite.c
 *    Description:  This mysqlite file 
 *                 
 *        Version:  2.0.0(2023年04月30日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月10日 17时25分12秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"
#include "mysqlite.h"

int                   rows;
int                   cols;
char                  table_name[32] = "RPI007";
char                 *buf;
sqlite3_stmt         *pStmt = NULL;
static sqlite3       *s_db = NULL;
char                 *zErrMsg = 0;
char                 *sql;
char                **dbresult;


int db_init(char *db_name)
{
	int    rv;
	/* Open database */
	rv = sqlite3_open(db_name, &s_db);	
	if ( rv )  //0:Successful result 
	{
		PARSE_LOG_ERROR("Can't open database: %s\n", sqlite3_errmsg(s_db));
		return -1;
	}
	else 
	{
		PARSE_LOG_INFO("Open database Successfully\n");

	}

	sqlite3_exec(s_db, "pragma synchronous = OFF; ", NULL, NULL, NULL);
	sqlite3_exec(s_db, "pragma auto_vacuum = 2 ; ", NULL, NULL, NULL);

	return 0;
} 


int db_insert(packet_t pack)
{
	int    rv = 0;
	packet_t data_in = pack; // 准备要写入的值
	strcpy(table_name, data_in.devsn);


	/* Create table by devsn if not exists */
	/* Create SQL statement */
	sql=sqlite3_mprintf("CREATE TABLE IF NOT EXISTS %s( Data BLOB);",table_name);

	/* Execute SQL statement */
	rv = sqlite3_exec(s_db, sql, 0, 0, &zErrMsg);
	if ( rv != SQLITE_OK )
	{
		PARSE_LOG_ERROR("SQL error :%s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return -1;
	}
	else
	{
		PARSE_LOG_INFO("Table created Successfully\n");
	}


	/* insert data  */
	sql=sqlite3_mprintf("INSERT INTO %s(Data) VALUES(?)",table_name); // 插入新值
	rv = sqlite3_prepare(s_db, sql, -1, &pStmt, NULL);
	if (rv != SQLITE_OK) 
	{    
		PARSE_LOG_ERROR("Cannot prepare statement: %s\n", sqlite3_errmsg(s_db));   
		rv = -2;
		goto OUT;
	}

	PARSE_LOG_DEBUG("time:%s,temp:%f,sn:%s\n", data_in.time, data_in.temp, data_in.devsn);

	sqlite3_bind_blob(pStmt, 1, &data_in, sizeof(data_in), SQLITE_STATIC); // 绑定需要写入的值
	rv = sqlite3_step(pStmt);
	if (rv != SQLITE_DONE)
	{
		PARSE_LOG_ERROR("execution failed: %s", sqlite3_errmsg(s_db));
		rv = -3;
		goto OUT;
	}

	rv = 0;

OUT:
	sqlite3_finalize(pStmt);    
	return rv;

}


int db_select(char *str, int *bytes)
{
	int    rv = 0;

	sql=sqlite3_mprintf("SELECT Data from %s limit 0,1;", table_name);

	/* Execute SQL statement */
	rv = sqlite3_prepare_v2(s_db, sql, -1, &pStmt, NULL);
	if ( rv != SQLITE_OK )
	{
		PARSE_LOG_ERROR("Failed to prepare statement\n");		      
		PARSE_LOG_ERROR("Cannot open database: %s\n", sqlite3_errmsg(s_db));
		sqlite3_close(s_db);
		rv = -2;
		goto OUT;
	}

	rv = sqlite3_step(pStmt);

	packet_t *pData = ( packet_t*)sqlite3_column_blob(pStmt, 0);
	*bytes = sqlite3_column_bytes(pStmt, 0);

	sprintf(str, "%s,%f,%s", pData->time, pData->temp, pData->devsn);
	PARSE_LOG_DEBUG("%s\n",str);

	rv = 0;

OUT:
	sqlite3_finalize(pStmt);   
	return rv;
}


int db_delete(void)
{
	int    rv;

	sql=sqlite3_mprintf("DELETE from %s limit 0,1;", table_name);

	/* Execute SQL statement */
	rv = sqlite3_prepare_v2(s_db, sql, -1, &pStmt, NULL);
	if ( rv != SQLITE_OK )
	{
		PARSE_LOG_ERROR("SQL error: %s\n", sqlite3_errmsg(s_db));
		rv = -1;
		goto OUT;
	}
	else
	{
		rv = sqlite3_step(pStmt);
		if (rv != SQLITE_DONE)
		{
			PARSE_LOG_ERROR("execution failed: %s", sqlite3_errmsg(s_db));
			rv = -2;
			goto OUT;
		}

		PARSE_LOG_INFO("Delete data successfully\n");
	}

	rv = 0;

	/*  Vacuum the database */
	sqlite3_exec(s_db, "VACUUM;", NULL, 0, NULL);

OUT:
	sqlite3_finalize(pStmt);
	return rv;
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
