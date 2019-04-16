# 数链项目服务部署
-------------
##### 数链数据提供包括：数据输出服务、数据桥接服务。当前提供几种部署方式：
1. 由源码打包，然后进行部署
2. 由release包来进行部署
3. 由docker进行部署

# 由源码打包部署
-------
1.  源码的下载地址为：https://github.com/unitedata-org/UniteData
2. 进入unitedata/scripts目录，执行构建脚本：
	* 若您是在windows系统下，执行
    	
    	````
    	./start.bat;
    	````
	* 若您是在类unix系统下，执行
	
		````
    	./start.sh
    	````
3. 若构建成功，将得到target/release目录，这里面将有bin, conf, lib三个包, 其中lib包含dependency 和 biz两个包。

# 通过release包进行部署
----------------
1. 通过以下地址下载发布包：xxxx @三板斧补充一下
2. 解压安装包：tar xzvf xxxx.tar.gz
3. 配置
	* 进入conf目录，为每项服务编辑配置文件。ud.properties里填入下述必选配置，主要是数链账户信息、数据库信息、redis信息：
	
	````
    #数链账户名
    provider.account=xxx
    #数链私钥
    provider.key=xxx

    #jdbc数据库连接信息
    spring.datasource.druid.url=xxx
    spring.datasource.druid.username=xxxx
    spring.datasource.druid.password=xxxx
    spring.datasource.druid.driver-class-name=com.mysql.jdbc.Driver

    #redis连接信息。
    redis.host=localhost
    redis.port=6379
    redis.password=
    redis.timeout=3000
    redis.pool.max-active=8
    redis.pool.max-wait=-1
    redis.pool.max-idle=8
    redis.pool.min-idle=0
    ````
4. Mysql初始化

https://github.com/unitedata-org-public/UD-Release/blob/master/ud-eds/1.9.3/ud_eds_db.sql

4. 启动服务
	* 进入bin目录，启动服务。
	* 启动命令格式:
	
		````
	    ./start.sh [servername]  [prod | test] [port]
		````	
		* 其中第一部分：[servername] 为服务名称
		 	* producer： 数据输出服务
			* eds-server： 数据桥接服务
			* zebra： 斑马合约服务插件
			* eds-cert:  存证服务插件
		* 第二部分：[prod | test] 为环境名称
			* prod：为正式环境
			* test：为测试环境
		* 第三部分：[port] 为端口号
		* 下面命令示例中以正式环境prod为例，如果要改成测试环境只需将prod改为test即可
	* 启动数据输出服务命令：
	
		````
		#正式环境
	    ./start.sh producer  prod 8091
	    #测试环境
	    ./start.sh producer  test 8091
		````
	* 启动数据桥接服务命令：
		
		````
		./start.sh eds-server prod 8080
		````
	* 启动斑马合约服务命令：
		
		````
	    ./start.sh zebra prod 9999
		````

	* 启动成功后，可以在logs目录下看到上述几个服务的启动日志。

	* 如果您想关闭相应服务，只需指定要关闭的服务名称，可用逗号分隔，下述示例关闭eds服务和斑马合约插件服务：

		````
    	./shutdown.sh eds-server,zebra
		````
	* 如果eds和提供方服务部署在不同的服务器上，则启动提供方服务前，需要在配置ud.properties中加入对应的eds地址，例如：
		
		````
	    eds.api=http://yourserver/ud-eds/
		````
	 否则提供方会默认以http://localhost:8080/ud-eds 来访问eds
	 
	 * 登录EDS的时候会出现login界面，初始用户名：admin 初始密码：123456
	 * EDS的端口务必只对内网可见！务必只对内网可见！务必只对内网可见！
		
# 数据桥接系统插件部署
---------
1. 下载源码eds-core包：https://github.com/unitedata-org/UniteData/tree/master/eds/eds-core
2. 插件开发：依赖eds-core包，实现IChange类来实现具体的数据访问。具体示例可参见SqlChange.java和ExampleChange.java.

        * Eds作为数据桥接网关，自身不存储实际业务数据，当通过eds将数据输出时，它会通过IChange类请求真实的业务数据。它是一层需求方和真实数据之间的代理层。
        * 实现validate接口：validate接口用于验证通过请求参数（params参数）所取得的结果集是否和配置的一致（table参数），在界面上点击“验证”时该接口会调用。
        * 实现change接口：change接口的作用是传递请求参数（params参数），并向业务数据进行请求并得到业务数据。
        * IChange与示例实现地址：https://github.com/unitedata-org/UniteData/tree/master/eds/eds-core/src/main/java/org/unitedata/eds/core/change.
        
3. 插件的发布：
	* 将开发完的插件编译好后，将IChange实现类对应的jar包或class包放到发布包/lib/biz下面，这样运行时扩展类加载器将可以加载到你的插件。


通过docker镜像进行部署
----------------
### 启动步骤 

1. 安装docker。可参考[链接](https://docs.docker-cn.com/engine/installation/)。
2. 使用docker命令拉取镜像`sudo docker pull registry.cn-hangzhou.aliyuncs.com/ud-hushi/eds:v1.9.3`
3. 确定主机的配置文件路径，这里用`[PATH_TO_CONF]`表示，在这一路径下，创建配置文件`ud.properties`，完整文件格式请参照上文【通过release包进行部署】中的配置文件。
    * 其中必须配置：
        * 用户私钥：`provider.account, provider.key`
        * jdbc数据库连接信息相关：`spring.datasource.druid.url, spring.datasource.druid.username, spring.datasource.druid.password`
5. 确定主机的日志文件路径，这里用`[PATH_TO_LOGS]`表示。
4. 启动镜像，命令如下。以下示例为启动eds-server。对于zebra，producer，启动方式相同。

	```bash
	$ docker run -it -d \ 
	    -v [PATH_TO_LOGS]/logs/:/usr/local/ud-service/logs/ \
	    -v [PATH_TO_CONF]/ud.properties:/usr/local/ud-service/conf/ud.properties \
	    --network host \
	    registry.cn-hangzhou.aliyuncs.com/ud-hushi/eds:v1.9.3 eds-server -s prod -po 8080
	```

#### 注意： 
* `[PATH_TO_LOGS]`，`[PATH_TO_CONF]`为主机路径，而非docker内路径
* 启动后可以在`[PATH_TO_LOGS]/logs/`下查看各服务的日志，是否启动成功。
* `-d`参数后台启动docker。
* `-p 8080:8080 -p 8091:8091`，是容器内端口和主机端口之间的映射关系。
* docker入口就是上述的start.sh，具体使用参考上面的介绍。`eds-server,producer,zebra`是启动的服务，`prod`是指定生产环境，如要改成测试环境将`prod`改为`test`。


### 启动后如何进入docker

容器启动后，使用命令`docker container ls`，可以查看当前运行中的容器。可以看到类似如下：

```
CONTAINER ID        IMAGE                                                   COMMAND                  CREATED             STATUS              PORTS                                            NAMES
25de5fec6e78        registry.cn-hangzhou.aliyuncs.com/ud-hushi/eds:v1.9.3   "/bin/bash entry.sh …"   24 seconds ago      Up 21 seconds       0.0.0.0:8080->8080/tcp, 0.0.0.0:8091->8091/tcp   upbeat_matsumoto
```

使用命令`docker exec -it [CONTAINER ID] /bin/bash` 可以进入容器。

### 分离部署eds和提供方服务

如果eds和提供方服务部署在不同的服务器上，则启动提供方服务前，需要在配置ud.properties中加入对应的eds地址，例如：
		
```
eds.api=http://yourserver/ud-eds/
```
 否则提供方会默认以http://localhost:8080/ud-eds 来访问eds		

### 镜像内目录介绍

镜像中服务的根目录是`/usr/local/ud-service`，`/usr/local/ud-service`内目录结构如下：

```
+-- bin
|   +-- start.sh
|   +-- shutdown.sh
+-- conf
|   +-- ud.properties
+-- lib
|   +-- eds-server-1.9.3.jar
|   +-- ud-data-producer-server-1.9.3.jar
|   +-- ...
|   +-- dependency
|       +-- ...
+-- logs
    +-- eds-server
    |   +-- app.log
    +-- producer-server
        +-- app.log
```

部署后的检验
----------------
### 提供方部署检验 

1. 通过浏览器访问提供方服务的配置读取api：/health_check/config，确保所有的配置都如期配置。 注意，出于安全性原因，私钥并不会被显示，而是以公钥形式展示。
2. 通过浏览器访问提供方服务的健康检测api：/health_check/，确保各个组件都显示"OK"。
