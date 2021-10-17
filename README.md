MySQL Connector for C++
---

这是一个简易的基于MySQL标准客户端开发库的简单辅助工具，提供了类似于Java、C#等高级语言的调用方法。

## 使用方法
只需引用 "mysql_conn_plus.h" 一个文件即可。
### INSERT & UPDATE & DELETE
```
#include "mysql_conn_plus.h"

int main(int argc, char **argv)
{
    // 创建连接对象的实例
    mysql_conn_plus conn(application_config.MYSQL_HOST, application_config.MYSQL_USER, application_config.MYSQL_PASSWD, application_config.MYSQL_DB, application_config.MYSQL_SERV_PORT);
    
    conn.connect(); // 连接数据库
    
    // 执行 INSERT 查询
    long long res = conn.query("INSERT INTO Students ( Name, Sex, ClassNo, Grade ) VALUES ( 'Tom', 1, 1, 3)");
    if(res >= 0){
        printf("Affected %llu rows! \n", res);
    }
    else{
         printf("MySQL error! code: %u , description: %s .\n",
            conn.get_error_code(), conn.get_error_description());
    }
    conn.close();
    
    return 0;
}
```

### SELECT
```
#include "mysql_conn_plus.h"

int main(int argc, char **argv)
{
    // 创建连接对象的实例
    mysql_conn_plus conn(application_config.MYSQL_HOST, application_config.MYSQL_USER, application_config.MYSQL_PASSWD, application_config.MYSQL_DB, application_config.MYSQL_SERV_PORT);
    
    conn.connect(); // 连接数据库
    
    // 执行 SELECT 查询
    long long res = conn.query("SELECT ID, Name, Sex, ClassNo, Grade FROM Students");
    if(res >= 0){
        mysql_result_plus mysql_result(&conn); // 获取查询结果
        
        if (mysql_result.available()) // 结果中包含数据
        {
            while (mysql_result.read_row()) // 循环读取结果
            {
                printf("\nStudent:\n");
                printf("ID: %08u \n", mysql_result["ID"]); // 可以根据字段名称提取
                printf("Namde: %s \n", mysql_result[1]); // 可以根据序号来提取
                ...
            }
        }
    }
    else{
         printf("MySQL error! code: %u , description: %s .\n",
            conn.get_error_code(), conn.get_error_description());
    }
    conn.close();
    
    return 0;
}
```
