#include"head.h"

int insertTable_fileHouse(char *filename, char *path, char *md5, int fileSize)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";   
    char *user="root";          
    char *password="0928";      
    char query[300]={0}; 
    char *database="CloudDisk"; 
    strcpy(query, "insert into fileHouse (filename, path, md5, fileSize, uploadSize) values('");
    sprintf(query, "%s%s', '%s', '%s', %d, 0)", query, filename, path, md5, fileSize);
    int t,r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("error connecting:%s\n", mysql_error(conn));
        return -1;
    }
    t=mysql_real_query(conn,query, strlen(query));
    if(t)
    {
        printf("error making query:%s\n", mysql_error(conn));
        return -1;
    }
    else
    {
        //printf("insert success\n");
    }
    mysql_close(conn);
    return 0;
}

int insertTable_ConnectInfo(char*user_name)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";   
    char *user="root";          
    char *password="0928";      
    char query[300]={0}; 
    char *database="CloudDisk"; 
    strcpy(query, "insert into userConnectInfo (name) values('");
    sprintf(query, "%s%s%s", query, user_name, "')");
    int t,r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("error connecting:%s\n", mysql_error(conn));
        return -1;
    }
    t=mysql_real_query(conn,query, strlen(query));
    if(t)
    {
        printf("error making query:%s\n", mysql_error(conn));
        return -1;
    }
    else
    {
        //printf("insert success\n");
    }
    mysql_close(conn);
    return 0;
}

int insertTable_Operation(char *user_name, char *operation, int flag)
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";   
    char *user="root";          
    char *password="0928";      
    char query[300]={0}; 
    char *database="CloudDisk"; 
    strcpy(query, "insert into userLog (name, operation, flag) values('");
    sprintf(query, "%s%s', '%s', %d)", query, user_name, operation, flag);
    int t,r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("error connecting:%s\n", mysql_error(conn));
        return -1;
    }
    t=mysql_real_query(conn,query, strlen(query));
    if(t)
    {
        printf("error making query:%s\n", mysql_error(conn));
        return -1;
    }
    else
    {
       // printf("insert success\n");
    }
    mysql_close(conn);
    return 0;
}

int getCiphertext(char *name, char *ciptext) //名字返回盐值
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";
    char *user="root";
    char *password="0928";
    char query[300]={0};
    char *database="CloudDisk";
    strcpy(query, "select * from user where name='");
    sprintf(query, "%s%s%s", query, name, "'");
    int t, r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("error connecting\n");
        return -1;
    }
    //printf("connected\n");

    t=mysql_real_query(conn,query, strlen(query));
    //t=mysql_query(conn,query);
    //printf("t=%d\n", t);
    if(t)
    {
        printf("error\n");
        mysql_close(conn);
        return -1;
    }
    else
    {
        res = mysql_use_result(conn);
        row = mysql_fetch_row(res);
        if(row!=NULL)
        {
            //row=mysql_fetch_row(res);
            memset(name, 0, sizeof(name));
            strcpy(name, row[2]);
            //printf("%s\n", row[2]); //盐值
            strcpy(ciptext, row[3]);
            //printf("%s\n", row[3]);   //密文
            mysql_free_result(res);
            mysql_close(conn);
            return 0;
        }
        else
        {
            printf("no user data\n");
            mysql_free_result(res);
            mysql_close(conn);
            return -1;

        }
    }
}
