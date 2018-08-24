## 数据合约的签约管理
 
### 1. Struct
#### 1.1 srvprov 服务方

name | type | desc
---- | --- | ---
name | account_name | 账户名(pk)
create_time | uint32_t | 参与合约时间
status | uint8_t | 服务配置状态


#### 1.2 demand_side 需求方

name | type | desc
---- | --- | ---
name | account_name | 账户名(pk)
finished_order_count | uint32_t | 完成订单数量
canceled_order_count | uint32_t | 取消订单数量
create_time | uint32_t | 参与合约时间


#### 1.3 prov_signing 提供方签约表（提供方->需求方）

name | type | desc
---- | --- | ---
dmd | account_name | 签约需求方（pk）


#### 1.3 dmd_signing 需求方签约表（需求方->提供方）

name | type | desc
---- | --- | ---
prov | account_name | 签约提供方（pk）


	
### 2. Actions
#### 2.1 addprov() 添加提供方 

param_name | type | desc
---- | --- | ---
name | account_name | 提供方账户

__require_auth : name__

#### 2.2 adddmd() 添加需求方 


param_name | type | desc 
---- | --- | ---
name | account_name | 需求方账户

__require_auth : name__
  
#### 2.3 quitprov() 去除提供方

param_name | type | desc
---- | --- | ---
name | account_name | 提供方账户

__require_auth : name__

#### 2.4 quitdmd() 去除需求方

param_name | type | desc
---- | --- | ---
name | account_name | 提供方账户

__require_auth : name__

#### 2.5 signasprov() 作为提供方主动签约

param_name | type | desc
---- | --- | ---
prov | account_name | 提供方账户
dmd | account_name | 需求方账户

__require_auth : prov__

#### 2.6 signasdmd() 作为需求方主动签约

param_name | type | desc
---- | --- | ---
dmd | account_name | 需求方账户
prov | account_name | 提供方账户

__require_auth : dmd__

#### 2.7 resignasprov() 作为提供方主动签约

param_name | type | desc
---- | --- | ---
prov | account_name | 提供方账户
dmd | account_name | 需求方账户

__require_auth : prov__

#### 2.8 resignasdmd() 作为需求方主动签约

param_name | type | desc
---- | --- | ---
dmd | account_name | 需求方账户
prov | account_name | 提供方账户

__require_auth : dmd__

#### 2.9 quitcontract() 退出合约（同时退出需求方和提供方） 

param_name | type | desc
---- | --- | ---
name | account_name | 账户

__require_auth : name__

#### 2.10 setprvstatus() 提供方设置服务配置状态 

param_name | type | desc
---- | --- | ---
name | account_name | 提供方
status | uint8 | 提供方服务配置状态

__require_auth : name__

### 3. Tables

#### 3.1 providers
服务提供方表格

scope | data_type  
---- | ---- 
_self |srvprov

#### 3.2 demandsides 
服务需求方表格

scope | data_type  
---- | ---- 
_self | demand\_side

#### 3.3 signings 

scope | data_type  
---- | ---- 
_self | signing\_info

