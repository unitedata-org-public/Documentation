## 斑马-黑名单匿踪查询合约实施指南

### 斑马合约
  「斑马」合约，由多家头部金融机构参与共同建立，在分布式的数链网络上运行，通过区块链技术和匿踪技术实现了一个安全合规、匿踪脱敏的数据服务环境，使查询方可以隐藏查询的目标客户身份信息，同时又能得到想要的信息。「斑马」合约建立了公平合理的利益分配机制，贡献多的参与方将获得更多的权利。「斑马」合约可以帮助金融机构获得真实新鲜的风险数据，直接降低坏账。

### 合约首页
1. 斑马合约首页:[https://preview.unitedata.link/exchange/blacklist](https://preview.unitedata.link/exchange/blacklist) 

### 参与合约
1. 前提：加入合约需要质押5万链积分；
2. 注册数链账号，参与「斑马」合约。
3. 参与合约:[https://preview.unitedata.link/exchange/contractDetail/22](https://preview.unitedata.link/exchange/contractDetail/22)
4. 参与合约后可以在 [https://preview.unitedata.link/exchange/blacklist](https://preview.unitedata.link/exchange/blacklist) 查看相关信息
![image](http://git.caimi-inc.com/UniteData/manager/uploads/32962d96b72424f4f9b5185ca30fcec6/image.png)


### 提供方部署相关服务
#### 数据桥接服务及凭证上传
1. 此服务的功能为内部数据输出到外部的中转站，是部署在企业内部的系统，无需外部访问接口；
2. 在黑名单业务中，此服务会保存上传的凭证及原始信息，以及返回给查询方的数据备份，同时服务中还包含了自动上传凭证及同步凭证状态的功能。
3. 使用说明：
	* 下载web服务包（请下载1.8.2版本）：[https://github.com/unitedata-org-public/UD-Release/tree/master/ud-eds](https://github.com/unitedata-org-public/UD-Release/tree/master/ud-eds)
	* 部署服务（部署方法请参考下载页说明）
	* 通过api插入数据，或使用数链凭证上传工具进行上传，数据格式及详细信息请参考：[https://github.com/unitedata-org-public/UD-Release/blob/master/ud-eds/1.8.2/PROOF.md](https://github.com/unitedata-org-public/UD-Release/blob/master/ud-eds/1.8.2/PROOF.md)
	* 系统会自动生成凭证，并且把凭证上传到链上，并且从链上同步数据状态，更新至blacklist_info表。

#### 数据输出服务
1. 此服务的功能是接受查询方的请求，并按合给模型返回数据，此服务需要访问数据桥接服务，以及需要开放对外访问端口；
3. 使用说明：
	* 下载系统(请下载1.8版本):[https://github.com/unitedata-org-public/UD-Release/tree/master/ud-data-producer](https://github.com/unitedata-org-public/UD-Release/tree/master/ud-data-producer)
	* 部署服务（部署方法请参考下载页说明）；
	* 此服务通常不需要特别处理，保持服务正常运行即可。


### 黑名单数据查询
1. 下载sdk(请下载1.8版本):[https://github.com/unitedata-org-public/UD-Release/tree/master/ud-data-consumer](https://github.com/unitedata-org-public/UD-Release/tree/master/ud-data-consumer)
2. 相关说明请参照下载页说明
3. 示例代码

````
/**
 * 数据查询示例代码
 */
public class DataQueryProtocolTest {

	//message 服务 dns 地址（示例代码中可不传，有默认处理）
    private static final String messageServiceHost = "https://preview.unitedata.link/ud-message";
    //token 服务 dns 地址（示例代码中可不传，有默认处理）
    private static final String tokenServiceHost = "https://preview.unitedata.link/wallet/wallet-proxy/ud-wallet/v1";
    //eosApi 服务 dns 地址
    private static final String eosHost = "http://preview.unitedata.link/v1";
    //需求方账户
    private static final String account = "name";
    //需求方私钥
    private static final String privateKey = "xxx";
    合约地址
    private static final String contractId = "ud.blacklist";
    //交易ID 为空即可
    private static final String transactionId = "";
    //姓名
    private String name = "张三";
    //身份证号
    private String idNumber = "4115241345678843x";

     @Test
    public void simpleCreditQuery() {
        //数据查询协议--确定数据查询的方式、过程、结果。
        DataQueryProtocol protocol =DataQueryClient
                    .newProtocol(account, privateKey, tokenServiceHost, messageServiceHost)
                    .setContractUri(eosHost);    //tokenServiceHost、messageServiceHost可不传，有默认处理
        //通过二要素进行数据查询
        Object data = protocol.creditQuery(contractId, transactionId, name, idNumber, continuePriceQuery, System.currentTimeMillis());
        try {
            Logger.defaultLogger().info(JsonUtils.toString(data));
        } catch (JsonProcessingException e) {
            throw new RuntimeException(e);
        }
    }
}
````


### 特别说明
* 以上涉及preview.unitedata.link域名的链接均为测试链地址，正式链请将改为：www.unitedata.link
