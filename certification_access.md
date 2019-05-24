
# 数链授权存证接入文档
------------

## 业务说明

授权存证应用于数据供应商合规提供数据的场景，当业务方向数据提供方请求数据的时候，需要将用户授权的凭据传递给提供方，提供方验证此凭据后方可提供数据，同时此凭据保存入到数链中，确保提供方的合规风险得到隔离。

## 前提

使用授权存证与业务查询前，业务方需要就具体业务，将对应用户协议信息（协议原文，协议md5，业务方，提供方，业务id，业务说明等）信息提交到链上。见https://github.com/unitedata-org-public/UD-Release/blob/master/ud-proxy/api.md#%E7%AE%80%E5%8D%95%E5%AD%98%E8%AF%81%E5%8D%8F%E8%AE%AE%E5%A4%87%E6%A1%88

## 接入方式

授权存证有两种接入方式：无SDK接入；通过SDK接入。


## 无SDK接入——业务方

**存证构建**

业务方需要自行构建存证数据结构，先填入userAgent, protocalDigest, directAuthorizer, provider, bizId, bizName,timestamp，其中timestamp是当前创建日期，其余数据（protocalDigest,directAuthorizer, provider, bizId, bizName）要和已上链协议保持一致。完整的存证结构

    public class Certification {
    
        /**
         * 用户信息
         */
        private Map<String, Object> userAgent;
    
        /**
         * 协议摘要
         */
        private String protocalDigest;
    
        /**
         * 数据提供方
         */
        private String provider;
    
        /**
         * 业务ID
         */
        private String bizId;
    
        /**
         * 业务说明
         */
        private String bizName;
    
        /**
         * 创建时间
         */
        private long timestamp;
    
        /**
         * 签名方式，可以为空，默认即采用EOS签名方式
         */
        private String encryptType;
    
        /**
         * 签名用的公钥，便于提供方验签
         */
        private String publicKey;
        /**
         * 签名人，即业务方
         */
        private String directAuthorizer;
    
        /**
         * 业务方签名
         */
        private String directSignature;
    
        /**
         * 间接授权人，可以为空
         */
        private String indirectAuthorizer;
    
        /**
         * 间接授权人签名，可以为空
         */
        private String indirectSignature;
    
    
        /**
         * 获取存证的摘要数据
         */
        private String getDigest() {
            StringBuilder sb = new StringBuilder();
            sb.append(JSON.toJSONString(this.userAgent));
            sb.append(protocalDigest);
            sb.append(directAuthorizer);
            sb.append(provider);
            sb.append(bizId);
            sb.append(bizName);
            sb.append(Long.valueOf(timestamp));
            byte[] digest = HashUtils.md5(sb.toString().getBytes(Charset.forName("UTF-8")));
            String result =  Hex.encodeHexString(digest);//二进制转HEX，请自行调用apache相关接口或自行实现
            return result;
        }
    }

**填充签名**

对存证的digest执行签名，代码：

     Certification cert = buildCert();//存证创建
     String digest = cert.getDigest();//获取存证摘要
     cert.setDirectSignature(CryptoUtils.doEosSignature(digest, "[私钥]"));
     cert.setPublicKey("[公钥]");

签名代码见后文。

**业务方将此存证放入查询，发送给数据提供方。**

## 无SDK接入——提供方

**将查询中的存证提取出来，存证结构参考上文**

**验证存证中的协议摘要、bizid是否与当前业务符合等**

**验证存证签名，摘要获取见上文，验证方式调用签名的验签方法verifyMsg**

**调用数链公共服务，将存证存储到数链，参考https://github.com/unitedata-org-public/UD-Release/blob/master/ud-proxy/api.md#%E7%AE%80%E5%8D%95%E5%AD%98%E8%AF%81**

## 附：签名代码

签名需要依赖一个数链专用签名库：

       <dependency>
            <groupId>org.unitedata</groupId>
            <artifactId>ud-crypto</artifactId>
            <version>1.0.0</version>
        </dependency>

只需如下调用即可获得签名：
       
       String sig = CryptoUtils.doEosSignature("[原文]", "[私钥]")

验证签名：
       
       boolean verified = CryptoUtils.verifyMsg("签名者公钥", "[签名]", "[原文]")


## 通过sdk接入-业务方

**依赖存证sdk**

请联系相关人员获取到sdk，并在项目中依赖此sdk

**进行存证创建**

只要调用ConsumerFacade进行调用即可。示例：

    private static CertificationConfiguration configuration;
    public static void example(){
        if(configuration == null) {
            configuration = ConsumerFacade.consumeConfig("[业务方账户]", "[私钥]", "[公钥]");
        }

        Map<String, Object> userInfo = new HashMap<>();//用户信息
        userInfo.put("name","张三");
        userInfo.put("idno","110108199303333333");
        String protocalMd5 = "0x111111111";//协议MD5，请参考 协议上链流程中的MD5
        Certification certification = ConsumerFacade.buildCertification(userInfo, protocalMd5, configuration.getAccount(), "数据提供方账户", "业务id"
        ,"业务说明", configuration);
    }
    

**将存证发送给提供方**

这一步要由需求方自己完成，将得到的存证对象和查询一起发送给提供方，提供方会分析出存证并进行处理。


## 通过sdk接入-提供方

**依赖存证sdk**

请联系相关人员获取到sdk，并在项目中依赖此sdk

**从业务方查询中提取出存证**

这一步由提供方自行实现，存证数据结构见上文或Certification类

**创建配置，并处理存证对象**

    private static CertificationConfiguration configuration;
    public static void example(Certification certification){
        if(configuration == null) {
            configuration = ProviderFacade.providerConfig("[提供方账户]", "[提供方私钥]", "[协议摘要]","[业务ID]","[存证合约地址]", Envs.PRODUCTION);
        }
        try{
            ProviderFacade.handleCertificationAsync(certification, configuration);
        }
        catch (CertificationValidationFailedException ex){
            //..存证信息验证失败处理逻辑
        }
    }
    
* ProviderFacade.providerConfig最后一个参数是环境值，可选环境见Envs类
* 存证处理逻辑包括：验证存证、异步上链。存证验证失败后会抛出异常，请提供方处理此异常
* handleCertificationAsync内部会在根据提供方填入的信息验证完存证后，异步地存储上链。
