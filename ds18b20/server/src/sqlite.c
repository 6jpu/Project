/*********************************************************************************
 *      Copyright:  (C) 2023 Kun_<1433729173@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file test sqlite
 *                 
 *        Version:  1.0.0(2023年04月18日)
 *         Author:  Kun_ <1433729173@qq.com>
 *      ChangeLog:  1, Release initial version on "2023年04月10日 17时25分12秒"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <sqlite3.h>
#include <stdlib.h>
#include <string.h>


int            rows,cols;
char         **dbresult;
sqlite3_stmt  *pStmt = NULL;
sqlite3       *db;
char          *zErrMsg = 0;
int            rc;
char          *sql;

typedef struct{
     char       report_time[64];
     float      temp;
     char       sn[64];
}myData;



int sqlite_init()
{

    /* Open database */
	rc = sqlite3_open("temp_data.db", &db);	
	if ( rc )  //0:Successful result 
	{
		fprintf(stderr,"Can't open database: %s\n", sqlite3_errmsg(db));
		return -1;
	}
    else 
	{
		fprintf(stderr, "Open database Successfully\n");

	}

	/* Create SQL statement */
	const char *sql = "DROP TABLE IF EXISTS TEMP;" 
                      "CREATE TABLE TEMP(ID INTEGER PRIMARY KEY, Data BLOB);";
    
	/* Execute SQL statement */
    rc = sqlite3_exec(db, sql, 0, 0, &zErrMsg);
    if ( rc != SQLITE_OK )
    {
        fprintf(stderr, "SQL error :%s\n", zErrMsg);
        sqlite3_free(zErrMsg);
		return -2;
    }
    else
    {
        fprintf(stderr, "Table created Successfully\n");
    }

    sqlite3_close(db);
    return 0;
} 


int sqlite_insert(myData data)
{

    /* Open database */
    rc = sqlite3_open("temp_data.db",&db);
    if ( rc )  //0:Successful result 
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return -1;
    }
    else
    {
        fprintf(stderr, "Open database for insert Successfully\n");

    }

	/* insert data  */
    const char *newSql = "INSERT INTO TEMP(Data) VALUES(?)"; // 插入新值
    rc = sqlite3_prepare(db, newSql, -1, &pStmt, NULL);
    if (rc != SQLITE_OK) 
	{    
        fprintf(stderr, "Cannot prepare statement: %s\n", sqlite3_errmsg(db));   
        return -2;
    }

    myData data_in = data; // 准备要写入的值
	printf ("time:%s,temp:%f,sn:%s\n", data_in.report_time, data_in.temp, data_in.sn);

    sqlite3_bind_blob(pStmt, 1, &data_in, sizeof(data_in), SQLITE_STATIC); // 绑定需要写入的值
    rc = sqlite3_step(pStmt);
    if (rc != SQLITE_DONE)
    {
        printf("execution failed: %s", sqlite3_errmsg(db));
		return -3;
    }

    sqlite3_finalize(pStmt);    
    sqlite3_close(db);

    return 0;

}


int sqlite_select(char *str)
{
   /* Open database */
   rc = sqlite3_open("temp_data.db", &db);
   if ( rc )
   {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return -1;
   }
   else
   {
      fprintf(stderr, "Opened database for SELECT successfully\n");
   }

   /* Create SQL statement */
   sqlite3_get_table(db, "select * from TEMP", &dbresult, &rows, NULL, NULL); //查看最后一条数据
   if ( rows <= 0)
   {
	   printf ("No data for select in database\n");
	   return -2;
   }
   
   sql=sqlite3_mprintf("SELECT Data from TEMP WHERE ID = %d;", rows);
    
   /* Execute SQL statement */
   rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, NULL);
   if ( rc != SQLITE_OK )
   {
	    fprintf(stderr, "Failed to prepare statement\n");		      
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
	    sqlite3_close(db);
	    return -3;
   }

   rc = sqlite3_step(pStmt);

   myData *pData = ( myData*)sqlite3_column_blob(pStmt, 0);

   sprintf(str, "%s,%f,%s", pData->report_time, pData->temp, pData->sn);
   printf("%s\n",str);

   sqlite3_finalize(pStmt);   
   sqlite3_close(db);

   return 0;
}


int sqlite_delete()
{
   /* Open database */
   rc = sqlite3_open("temp_data.db", &db);
   if ( rc )
   {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return -1;
   }
   else
   {
      fprintf(stderr, "Opened database for DELETE successfully\n");
   }

   /* Create merged SQL statement */
   sqlite3_get_table(db, "select * from TEMP", &dbresult, &rows, NULL, NULL); //删除最后一条数据
   if ( rows <= 0)
   {
	   printf ("No data for delete\n");
	   return -2;
   }
   sql=sqlite3_mprintf("DELETE from TEMP where ID = %d;", rows);
   
   /* Execute SQL statement */
   rc = sqlite3_prepare_v2(db, sql, -1, &pStmt, NULL);
   if ( rc != SQLITE_OK )
   {
      fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
   }
   else
   {
      rc = sqlite3_step(pStmt);
	  if (rc != SQLITE_DONE)
	  {
		  printf("execution failed: %s", sqlite3_errmsg(db));
		  return -3;
	  }
	
      sqlite3_finalize(pStmt);
      fprintf(stdout, "Delete data successfully\n");
    }

   sqlite3_close(db);

   return 0;
}
