#include<mysql/mysql.h>
#include<stdio.h>
#include<string.h>

int main(int argc, char *argv[])
{
    if(argc!=2)
    {
        printf("argc error\n");
        return -1;
    }
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    char *server="localhost";
    char *user="root";
    char *password="0928";
    char query[300]={0};
    strcpy(query, "select * from user where name='");
    sprintf(query, "%s%s%s", query, "Lazy", "'");
    int t, r;
    conn=mysql_init(NULL);
    if(!mysql_real_connect(conn, server, user, password, argv[1], 0, NULL, 0))
    {
        printf("error connecting\n");
        return -1;
    }
    printf("connected\n");

    t=mysql_query(conn,query);
    if(t)
    {
        printf("error\n");
    }
    else
    {
        res = mysql_use_result(conn);
        row=mysql_fetch_row(res);
        printf("%s\n", row[2]);
        printf("%s\n", row[3]);
        /* if(res) */
        /* { */
        /*     while((row=mysql_fetch_row(res))!=NULL) */
        /*     { */
        /*         printf("num=%d\n", mysql_num_fields(res)); //列数 */
        /*         for(t=0;t<mysql_num_fields(res);++t) */
        /*         { */
        /*             printf("%s ", row[t]); */
        /*         } */
        /*         printf("\n"); */
        /*     } */
        /* } */
        /* else */
        /* { */
        /*     printf("no data\n"); */
        /* } */
        mysql_free_result(res);
    }
    mysql_close(conn);
    return 0;
}
