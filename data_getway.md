# 数链项目服务部署
-------------
##### 数链数据提供包括：数据输出服务、数据桥接服务。当前提供几种部署方式：
1. 由源码打包，然后进行部署
2. 由release包来进行部署
3. 由docker进行部署

# 由源码打包部署
-------
1.  源码的下载地址为：xxx @三板斧补充一下
2. 进入unitedata/scripts目录，执行构建脚本：
	* 若您是在windows系统下，执行
    	
    	````
    	./start.bat;
    	````
	* 若您是在类unix系统下，执行
	
		````
    	./start.sh
    	````
3. 若构建成功，将得到target/release目录，这里面将有bin, conf, lib三个包。

# 通过release包进行部署
----------------
1. 通过以下地址下载发布包：xxxx @三板斧补充一下
2. 解压安装包：tar xzvf xxxx.tar.gz
3. 配置
	* 进入conf目录，为每项服务编辑配置文件。这几样服务的配置基本都是相似的，填入下述配置：
	
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

    #redis连接信息
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
@三板斧补充一下

4. 启动服务
	* 进入bin目录，启动服务。
	* 启动命令格式:
	
		````
	    ./start.sh [eds-server | zebra | producer]  [prod | test]
		````	
		* 其中第一部分：[eds-server | zebra | producer] 为服务名称
		 	* producer： 数据输出服务
			* eds-server： 数据桥接服务
			* zebra： 斑马合约服务插件
		* 第二部分：[prod | test] 为环境名称
			* prod：为正式环境
			* test：为测试环境
		* 下面命令示例中以正式环境prod为例，如果要改成测试环境只需将prod改为test即可
	* 启动数据输出服务命令：
	
		````
		#正式环境
	    ./start.sh producer  prod
	    #测试环境
	    ./start.sh producer  test
		````
	* 启动数据桥接服务命令：
		
		````
		./start.sh eds-server prod
		````
	* 启动斑马合约服务命令：
		
		````
	    ./start.sh zebra prod
		````
	* 一次启动多个服务，服务名称只需用逗号分隔即可，如下命令正式环境启动所有服务
	
		````
	    ./start.sh eds-server,zebra,producer  prod
		````	
	* 启动成功后，可以在logs目录下看到上述几个服务的启动日志。

	* 如果您想关闭相应服务，同理：

		````
    	./shutdown.sh eds-server,zebra
		````
		
# 数据桥接系统插件部署
---------
1. 数据桥接网关插件sdk下载地址为:xxxx @三板斧补充
2. 插件的开发说明（@三板斧 https://github.com/unitedata-org-public/Documentation/blob/master/data_getway.md 这里有一点说明，你补充一下放到这里）
3. 插件的发布：
	* 将开发完的插件编译好后，将IChange实现类的class文件放入lib/biz，重启服务即可生效。


通过docker镜像进行部署
----------------
### 启动步骤 

1. 安装docker。可参考[链接](https://docs.docker-cn.com/engine/installation/)。
2. 使用docker命令拉取镜像`sudo docker pull registry.cn-hangzhou.aliyuncs.com/ud-hushi/eds:v1.9.3`
3. 确定主机的配置文件路径，这里用`[PATH_TO_CONF]`表示，在这一路径下，创建配置文件`zebra.properties`，完整文件格式请参照上文【通过release包进行部署】中的配置文件。
    * 其中必须配置：
        * 用户私钥：`provider.account, provider.key`
        * redis相关：`redis.host, redis.port`
        * jdbc数据库连接信息相关：`spring.datasource.druid.url, spring.datasource.druid.username, spring.datasource.druid.password`
5. 确定主机的日志文件路径，这里用`[PATH_TO_LOGS]`表示。
4. 启动镜像，命令如下。

	```bash
	$ docker run -it -d \ 
	    -v [PATH_TO_LOGS]/logs/:/usr/local/ud-service/logs/ \
	    -v [PATH_TO_CONF]/zebra.properties:/usr/local/ud-service/conf/zebra.properties \
	    -p 8080:8080 -p 8091:8091 \
	    registry.cn-hangzhou.aliyuncs.com/ud-hushi/eds:v1.9.3 eds-server,producer,zebra prod
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

### 镜像内目录介绍

镜像中服务的根目录是`/usr/local/ud-service`，`/usr/local/ud-service`内目录结构如下：

```
+-- bin
|   +-- start.sh
|   +-- shutdown.sh
+-- conf
|   +-- zebra.properties
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

