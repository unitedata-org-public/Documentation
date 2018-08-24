## 批量查询数据合约——订单信息管理

### 1. Struct
#### 1.1 order 订单信息

name | type | desc
---- | --- | ---
id | uint64_t | id(pk)
creator | account_name | 订单创建者（scope）
inquiry_param | string | 查询参数摘要
cost | asset | 总花费
crt_time | uint64_t | 创建时间
mode| billing_mode| 计费方式


#### 1.2 provorder 订单服务方信息

name | type | desc
---- | --- | ---
order_id | uint64_t | 订单id(pk)
name | account_name | 服务方账号（scope）
reward | asset | 报酬
order_status | uint8_t | 订单完成状态
last_update_time | uint64_t | 订单完成状态
result | string | 结果摘要
mode| billing_mode| 计费方式
creator| account_name |创建人


#### 1.3 billing_mode 计费模式

name | type | desc
---- | --- | ---
mode | uint8_t | 计费模式；0，按时间。1，按次数。
size | uint32_t | 大小。mode为0时表示天数，为1时表示次数。



### 2. Actions
#### 2.1 createorder() 需求方创建订单

param_name | type | desc
---- | --- | ---
creator | account_name | 订单创建者
order_id | uint64_t | 订单id
param | string | 订单查询参数
providers | account_name[] | 提供者数组
mode | billing_mode | 计费方式


__require_auth : creator__

#### 2.2 commitorder() 数据提供方提交结果

param_name | type | desc
---- | --- | ---
prov | account_name | 提供方账号
order_id | uint64_t | 订单id
result | string | 订单查询结果

__require_auth : prov__

