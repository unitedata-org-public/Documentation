# 数据使用方SDK 接入文档

## 引言

数据使用方 SDK 是一个数据服务客户端。

## SDK 实现结构

![未命名文件-6](images/sdk_class.png)

如上图所示，SDK 主要是围绕以上三个类型展开：

类型 DataClient 声明了数据客户端的基本功能。DataClient 使用配置上下文中的描述信息来实现数据处理的业务功能。

类型 DataQueryOptions 声明了数据处理业务功能所需要的所有相关配置信息，它需要依赖 DataClient 类型，并通过 DataClient 类型的实例来得到实例。每一个 DataQueryOptions 类型的实例都对应唯一一个 DataClient 的实例。通过 DataQueryOptions 类型的实例，调用函数 query 来实现数据查询的业务功能。

接口 DataResult 描述了数据处理结果的上下文结构，调用方通过这个类型的实例函数来获取相关的信息。

## 如何使用 SDK

``` java
<dependency>
    <groupId>org.unitedata</groupId>
    <artifactId>ud-data-consumer-sdk</artifactId>
    <version>1.0.0</version>
</dependency>

/**
 * @author baimao
 * @create 2018/8/7
 */
public class DataClientTest {
    
    // 日志组件
    private static final Logger logger = new MyLogger();
    // eos 合约访问账户
    private static final String account = "";
    // eos 合约访问私钥
    private static final String privateKey = "";
    // eos 合约访问地址
    private static final String contractApi = "";
    // eos 合约 id
    private static final String contractId = "";
    // 匿名查询数据
    private static final String anonymousValue = "";
    // 数据查询参数
    private final Map<String, Object> parameters = new HashMap<>();
    
    @Test
    public void testNewQuery(){
        try {
            DataResult result
                    = DataClient.newQuery(account, privateKey, contractApi)
                    .setAnonymousValue(anonymousValue)
                    .setLogger(logger)
                    .query(contractId, parameters);
        } catch (Exception ignored) { }
    }
    
    private static final class MyLogger implements Logger{
        private static final org.slf4j.Logger logger = LoggerFactory.getLogger(MyLogger.class);
        
        @Override
        public void info(String message) {
            logger.info(message);
        }

        @Override
        public void warn(String message) {
            logger.warn(message);
        }

        @Override
        public void debug(String message) {
            logger.debug(message);
        }

        @Override
        public void error(String message) {
            logger.error(message);
        }

        @Override
        public void error(Throwable cause) {
            logger.error(cause.getMessage(), cause);
        }
    }
}
```

## 数据使用方-数据服务 api

数据服务 api 是基于 http[s] 协议的一个数据客户端服务。它进一步封装了数据服务 SDK，并且提供了一组 restful API 风格的简单接口。部署过程请查看[部署文档](consumer-api-install-doc.md)
