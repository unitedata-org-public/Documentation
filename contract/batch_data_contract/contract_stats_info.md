## 数据合约——合约信息管理

### 1. Struct
#### 1.1 contract_info 订单信息

name | type | desc
---- | --- | ---
pkey | uint64_t | pk
admin | account_name | 合约管理者
pay_strategy | uint8_t | 支付方式
price | asset | 价格
signing_strategy | uint8_t | 需求方签约方式
hidden | bool | 是否匿踪
contract_name | string | 合约名称
home_image | string | 合约主页图片
detail_image | string | 合约详情图片
multi\_party\_computation | mc_info | 多方计算信息
model | model_info | 合约模型相关信息
api | api_info | api相关信息
stopped | bool | api相关信息

支付方式： 0，提供方获取固定的udt。1，需求方支付固定udt，服务方平均分。其他方式待定。
需求方签约方式： 0，默认不会签约。1，默认自动签约。

#### 1.2 mc_info 多方计算信息

name | type | desc
---- | --- | ---
used | bool | 是 否使用多方计算
input | string[] | 多方计算输入
output | string[] | 多方计算输出
participant_lower_bound | uint16_t | 参与多方计算最少人数

#### 1.3 model_info 模型计算信息

name | type | desc
---- | --- | ---
model_used | bool | 是否使用模型
storage_used | bool | 是否使用存储
model\_jar\_address | string | 模型jar包地址
model\_jar\_hash | string | 模型jar包hash
model\_java\_code | string | java代码地址
data\_dictionary\_ids | uint64[] | 数据字典的id数组


#### 1.4 api_info api信息

name | type | desc
---- | --- | ---
explain | string | 对接说明
common\_request\_params | string | 公共请求参数
conceal\_query\_request\_params | string | 匿踪查询参数
businesss\_request\_params | string | 业务参数
data\_dictionary\_reques\_params | string | 数据字典请求参数
custom\_return\_results | string[] | 自定义返回结果
data\_dictionary\_return\_results | string | 返回结果
example | string | 示例

### 2. Actions

#### 2.1 stopcontract() 停用合约

param_name | type | desc
---- | --- | ---
| |

#### 2.2 setprice() 设置价格

param_name | type | desc
---- | --- | ---
price | asset | 价格，直接收UDT

#### 2.3 setsigning() 设置签约

param_name | type | desc
---- | --- | ---
signing | uint8 | 签约方式：1，需求方默认签约；2，需求方不会默认签约。

#### 2.4 sethidden() 设置是否匿踪

param_name | type | desc
---- | --- | ---
hidden | uint8 | 是否匿踪：0，不匿踪；1，匿踪。

#### 2.5 setmcinfo() 设置多方计算信息

param_name | type | desc
---- | --- | ---
mc | mc_info | 多方计算信息

#### 2.6 setmodelinfo() 设置多方计算信息

param_name | type | desc
---- | --- | ---
model | model_info | 多方计算信息

#### 2.7 setname() 设置合约名称

param_name | type | desc
---- | --- | ---
name | string | 合约名称

#### 2.8 sethomeimg() 设置合约首页图片

param_name | type | desc
---- | --- | ---
home_image | string | 合约首页图片

#### 2.9 setdetailimg() 设置合约详情图片

param_name | type | desc
---- | --- | ---
detail_image | string | 合约详情图片

#### 2.10 setapi() 设置合约api信息

param_name | type | desc
---- | --- | ---
api | api_info | 合约api信息



### 3. Tables
#### 3.1 stats
合约信息

scope | data_type  
---- | ---- 
\_self | contract_info 

