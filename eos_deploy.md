# eos生产者节点部署文档

这篇介绍centos7上部署eos生产节点的步骤。

## 1. 代码准备和编译
安装git工具。到github拉取eos软件源码。

```
git clone https://github.com/EOSIO/eos.git
```

然后切换到1.2.5的版本。

```
cd eos
git checkout v1.2.5
git submodule update --init --recursive 
```

使用eos自带的自动编译脚本编译。

```
./eosio_build.sh
```

## 2. 配置文件和启动
配置文件包括genesis.json和config.ini。这两个文件我们会准备好。但是仍需要修改个别配置：

* 对于生产节点：需要将```producer-name```和```signature-provider```替换成自己的生产者账号和对应秘钥对。
* 节点的一些功能插件可以按需开启或者关闭。

配置文件中会配置好节点的类型：__1.生产节点。2.非生产节点__。针对不同的节点会给出相应的不同的配置文件。配置文件中同样会列出eos网络的其他节点地址。

启动节点指定配置文件位置即可。

```
$eos_path/build/programs/nodeos/nodeos --genesis-json $genesis_path/genesis.json --config-dir $config_dir_path 
```

启动节点的日志中开始接收区块表示启动节点成功。

如果是生产节点，使用正确的配置文件，开启节点的```producer_plugin```并配置好生产账号的私钥。待节点同步区块完成后，eos上投票达到总发行量的15%之后，就可以开始生产区块了。

为了方便，可以将nodeos放到系统路径中。

```
sudo ./eosio_install.sh
```
这样后面使用nodeos和cleos就不需要输入```$eos_path/```了。


## 3. 准备私钥
当eos软件编译完成之后，eos就为我们提供了一个简易的客户端cleos。可以使用cleos来使用创建私钥。

```
cleos create key
# 示例，不要使用下面这个私钥
Private key: 5JVstSk5TRG17Lsi7fmZnTPB4BcWyjGjdfAwWC7pyK9dCpapZeP
Public key: EOS6wMkcFmqRx54DcnrDN7HNQujn7y4cvXRvYe6gwhA5ARVgjE5Eq
```

如果你决定这个私钥作为你的账号的私钥，请__务必保存好私钥__，并把__公钥__给到我们来创建账号，注意是公钥，私钥自己保管好。

## 4. 进行竞选和投票

竞选和投票目前只能通过cleos的方式，首先创建eos本地钱包导入私钥

### 4.1 创建钱包
```
cleos wallet create
"/opt/eosio/bin/keosd" launched
Creating wallet: default
Save password to use in the future to unlock this wallet.
Without password imported keys will not be retrievable.
"PW5JmXqfW3RbKFRGB1AfggSa7549jcoz9RY976b1xUS1aN5MAyKH5"
```
第一次可能会出现上述情况，eos会启动本地的钱包服务```keosd```，下面一串是钱包的密码，记下来后面解锁钱包使用。

### 4.2 导入私钥
```
# 解锁钱包
cleos wallet unlock --password PW5JmXqfW3RbKFRGB1AfggSa7549jcoz9RY976b1xUS1aN5MAyKH5
# 导入私钥
cleos wallet import 5JVstSk5TRG17Lsi7fmZnTPB4BcWyjGjdfAwWC7pyK9dCpapZeP
```

### 4.3 竞选和投票

```
# 竞选
cleos system regproducer [参选账号] [公钥] [url] [location(选填)]
# 投票
cleos system voteproducer prods [投票人账号] [生产者账号]
# 这里[生产者账号]先就填自己
# 例子：
# 竞选
cleos system regproducer producer11 EOS6L2n8hsvFXaSMuc1dC2ZRpvyktR76k4wftPBe1bmDpu1YNEnNu https://producer11.com/ 
# 投票
cleos system voteproducer prods producer11 producer11

```



## 5. 需要开启的端口
默认情况：

* __9876端口__: 所有类型的节点都要开启。
* __8888端口__: 对外提供API的节点需开启。

以上是默认情况：nodeos的中config.ini可以指定各自端口，对应修改```p2p-listen-endpoint```和```http-server-address```两个参数。

## 6. 配置文件config.ini几个可能需要改动的配置介绍

* p2p-listen-endpoint: 同步区块的通信接口，所有节点都需开启。默认0.0.0.0:9876。
* http-server-address: 对外提供API的接口。默认0.0.0.0:8888。
* p2p-peer-address: 连接的eos节点地址，可以配置复数个。
* producer-name: 如果该节点是生产者节点，需配置生产者账号名称。
* signature-provider: 如果该节点是生产者节点，需配置生产者账号秘钥对。
* blocks-dir: 同步的区块数据文件保存位置。
* plugin: 节点功能的插件。生产节点至少需要开启```eosio::http_plugin```和```eosio::producer_plugin```；api相关的插件有```eosio::chain_api_plugin```、``` eosio::history_plugin```、```eosio::history_api_plugin```，可以按需开启。
* chain-state-db-size-mb: 默认8g，修改为当前机器的最大内存。
* p2p-max-nodes-per-host: 默认是1，适当放大，例如20，以便其他节点连接。
