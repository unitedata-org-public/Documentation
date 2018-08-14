# 数据桥接网关安装手册
### JDK安装
1.	进入orcale官网选择需要下载的版本jdk1.8.0_171
	* 建议版本：jdk-8u171-linux-x64.rpm（建议安装1.8及以上）
	* linux系统用命令行下载、安装和配置jdk1.8的详细步骤https://blog.csdn.net/zhangtxsir/article/details/72685357
2.	安装jdk :rpm -ivh jdk-8u171-linux-x64.rpm
	* 如果下载的是tar.gz包,直接用命令tar -zxvf jdk-8u171-linux-x64.tar.gz解压即可
3.	配置环境变量
	* 执行vi /etc/profile，在文件末尾添加
	* export JAVA_HOME=/home/roo/jdk1.8.0_171  (这里修改为自己的jak安装路径)
	* export PATH=$JAVA_HOME/bin:$PATH
	* export CLASSPATH=.:$JAVA_HOME/lib/dt.jar:$JAVA_HOME/lib/tools.jar
4.	然后执行java -version看看是否配置成功

### Tomcat安装
5.	下载解压tomcat（http://tomcat.apache.org/）建议：Tomcat 8.0
建议下载zip或tar.gz直接解压使用

### Mysql安装 
6.	进入mysql官网选择需要下载的版本
建议版本：Server version: 5.7.20 MySQL Community Server
7.	设置用户名：例如：eds_root
8.	设置密码：例如：eds_pass
9.	设置端口：3306(建议使用默认端口)

### 数据库导入
##### 假设数据库文件目录：/Users/lufeng/Desktop/eds/ud_eds_db.sql
1.	mysql -h localhost -u root -p（进入mysql下面）
2.	create database xxx;(创建数据库)
3.	show databases;(就可看到所有已经存在的数据库，及刚刚创建的数据库xxx)
4.	use xxx;(进入xxx数据库下面)
5.	show tables;(查看xxx数据库下面的所有表,空的)
6.	source  /Users/lufeng/Desktop/eds/ud_eds_db.sql（导入数据库表）
7.	show tables;(查看xxx数据库下面的所有表,就可以看到导入的表了)

### War包部署
##### war包：ud-eds.war 企业内部系统web应用程序
1.	将ud-eds.war放在Tomcat安装目录中的/webapps目录下
2.	运行Tomcat（bin目录下执行：sudo sh startup.sh）
3.	浏览器访问http://localhost:8080/ud-eds/ 测试部署是否成功

### 修改配置文件
1.	停止Tomcat（bin目录下执行：sh shutdown.sh）
2.	打开Tomcat/webapps/ud-eds/WEB-INF/classes目录下的配置文件
3.	修改application.properties文件中private.key用户私钥
4.	修改application-druid.properties文件中：
	* spring.datasource.druid.url(对应本地设置的mysql、数据库名称)
	* spring.datasource.druid.username(对应本地设置的username)
	* spring.datasource.druid.password(对应本地设置的password)
5.	重新运行Tomcat（bin目录下执行：sudo sh startup.sh）
	* 注意：修改项目配置文件.properties,需停止Tomcat。


### 自定义类处理模式
1.	自定义类xxx.java实现IChange接口：
		public interface IChange {
		
		//获取需求方所需数据
		    DataSet change(OutPutDefinitionVo outPutDefinitionVo, Map<String, String> params);
		    //数据获取方式测试接口
		    int validate(TableVo tableVo, OutPutDefinitionVo outPutDefinitionVo, Map<String, String> params);
		}
	* 注：
		* SUCCESS(0,"测试通过!"),
		* FAILURE(1,"测试不通过，无法获取数据!"),
		* DISAGREE(2,"测试不通过，请检查数据是否与标准数据字典对应!");

2.	编译自定义类为xxx.class
3.	停止Tomcat（bin目录下执行：sh shutdown.sh）
4.	将编译成功xxx.class文件放在…/Tomcat/webapps/ud-eds/WEB-INF/classes/org/unitedata/eds/core/change目录之下
5.	重新运行Tomcat（bin目录下执行：sudo sh startup.sh）

