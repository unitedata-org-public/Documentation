## 凭证上传操作文档

#### 黑名单凭证数据结构
![image](https://github.com/unitedata-org-public/UD-Release/blob/master/ud-eds/1.8.2/images/blacklist.png)

#### 黑名单凭证存储数据表名
* 数据桥接系统Mysql表：blacklist_info

##### 通过API接口上传数据
* 通过数据桥接网关黑名单基础数据上传接口创建数据
* url(本文链接中的localhost:8080请修改为数据桥接系统的主机url)
```
http://localhost:8080/ud-eds/api/blacklist/add
```
* method

```
POST
```
* requestParam

```
[
    {
    "hitTwoHash":"b5cd14dac17836fbaf1175f38d151da7",//姓名+身份证的hash值
    "overdueData":"overdue data"//逾期信息json值       
    }
]
```
* responseDemo

```json
{
"code": "200",
"message": "请求成功",
"data": "success"
}
```

* 注：上传数据会进行手机号校验、身份证校验，未通过校验的数据会被过滤，不予创建。

#### 批量上传工具
通过斑马合约上传工具
* 将数据源导出到明文二要素csv文件，格式为（姓名，身份证,逾期信息）。 示例：

    张三,32098219880703086X,{"overdueMonth":"2018-01-01","overdueDays":"5","overdueQuantity":" 500 RMB"}
    王五,11098219880703086X,{"overdueMonth":"2018-01-01","overdueDays":"5","overdueQuantity":" 500 RMB"}

* 通过密文生成工具，将明文二要素转换为密文文件
	* java -jar test-tool-1.9.2.jar -gu 明文csv文件
    * [数据转换代码](https://github.com/unitedata-org-public/UD-Release/blob/master/ud-eds/1.9.2/test-tool/src/main/java/org/unitedata/consumer/Main.java#L207)
* 将密文文件通过上传工具页面上传
```
http://localhost:8080/ud-eds/blacklist-upload
```
* 支持功能

1. 上传Csv文件---检验数据格式---解析文件---录入数据

* responseDemo

```json
{
"code": "200",
"message": "请求成功",
"data": "上传成功"
}
```

#### 凭证数据生成规则（上链凭证）
1. hit_two_hash（二要素MD5） ：（姓名+身份证）MD5
2. hit_two_random_hash（身份凭证）：（二要素MD5+静态随机码）MD5
3. privacy_hash （逾期凭证）： （（姓名+身份证）MD5+逾期信息+时间戳+静态随机码）MD5 
4. sign_hash （数字签名）： （逾期凭证）私钥签名

注：random_code ：静态随机码，[生成代码请查看](https://github.com/unitedata-org-public/UD-Release/blob/master/ud-eds/1.9.2/test-tool/src/main/java/org/unitedata/consumer/Main.java#L211)

#### 黑名单凭证上链流程
![image](https://github.com/unitedata-org-public/UD-Release/blob/master/ud-eds/1.8.3/images/proof.png)

