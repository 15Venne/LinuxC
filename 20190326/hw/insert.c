#include<mysql/mysql.h>
#include<stdio.h>
#include<string.h>

int main(int argc, char *argv[])
{
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server = "localhost";
    char *user = "root";
    char *password = "0928";
    char *database = "hw20190326";
    char query[200] = {0};
    sprintf(query, "insert into lesson_choose (S_ID, T_ID) values (%d, %d)", atoi(argv[1]), atoi(argv[2]));
    int t,r;
    conn = mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
    {
        printf("mysql_real_connect error to database:%s\n", mysql_error(conn));
    }
    else
    {
        printf("connect success\n");
    }
    t = mysql_query(conn, query);
    if(t)
    {
        printf("error making query:%s\n", mysql_error(conn));
    }
    else
    {
        printf("insert success\n");
    }
    mysql_close(conn);
    return 0;
}
