# 数据合约测试命令汇总

```
# master
nodeos --max-irreversible-block-age 9999999 --contracts-console  --enable-stale-production    --producer-name eosio  --plugin eosio::http_plugin    --plugin eosio::chain_api_plugin    --plugin eosio::producer_plugin    --plugin eosio::history_plugin    --plugin eosio::history_api_plugin --delete-all-blocks  --verbose-http-errors


# defaulf wallet pwd: 

# new eos owner key
Private key: 5HtFFQLyNtFcdsQ68Vm55tgNbyvFsXGH2WjjjSpUXBECeuKpA8a
Public key: EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG
# new eos active key
Private key: 5KHouQTDLX2uaAoe6CxYD9c2Jv294tkQhd3c8noR1G3mP5d9dUz

# default pwd

cleos wallet create
cleos wallet import 5HtFFQLyNtFcdsQ68Vm55tgNbyvFsXGH2WjjjSpUXBECeuKpA8a
cleos wallet import 5KHouQTDLX2uaAoe6CxYD9c2Jv294tkQhd3c8noR1G3mP5d9dUz

alias cleos="cleos --wallet-url http://localhost:3000"
cleos wallet unlock --password PW5K8UMij7Hi6eyQkqkpah6WqZiGWyGPN4iwLmw3GcMMxFBM8KWgF

cleos create account eosio alice EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g
# 5K5Dk854rtpfDiyXy4Wo4wrV7rWk7p3xesWWkh3EXKWzz9syC2S
cleos create account eosio bob EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g
cleos create account eosio jack EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g

cleos create account eosio eosio.token EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g

cleos create account eosio dcontract EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g

cleos create account eosio acntstats EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g
cleos set contract acntstats ../certification ../certification/acntstats.wast ../certification/acntstats.abi -p acntstats

cleos set contract dcontract . ./batch_data_contract.wast ./batch_data_contract.abi -p dcontract

cleos push action dcontract init '["alice"]' -p alice
cleos get table dcontract dcontract stats


cleos push action dcontract setprice '[[{"mode":0,"size":30,"price":"10000.0000 UDT"},{"mode":1,"size":1,"price":"1.0000 UDT"}],"memo"]' -p alice
cleos push action acntstats addcertacnt '["acntstats","alice"]' -p acntstats
cleos push action acntstats addcertacnt '["acntstats","bob"]' -p acntstats
cleos push action acntstats addcertacnt '["acntstats","jack"]' -p acntstats

# order creator : alice; providers : bob, jack

cleos create account eosio mortgage EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g
cleos set contract eosio.token ../eosio.token -p eosio.token

cleos push action eosio.token create '[ "mortgage", "1000000000.0000 UDT", 0, 0, 0]' -p eosio.token
cleos push action eosio.token create '[ "eosio", "1000000000.0000 SYS", 0, 0, 0]' -p eosio.token

cleos push action eosio.token issue '[ "alice", "1000.0000 UDT", "" ]' -p mortgage

cleos set account permission alice active '{"threshold": 1,"keys": [{"key": "EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG","weight": 1}],"accounts": [{"permission":{"actor":"dcontract","permission":"eosio.code"},"weight":1}]}' owner -p alice

cleos set account permission dcontract active '{"threshold": 1,"keys": [{"key": "EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG","weight": 1}],"accounts": [{"permission":{"actor":"dcontract","permission":"eosio.code"},"weight":1}]}' owner -p dcontract


cleos push action dcontract adddmd '["alice"]' -p alice
cleos push action dcontract addprov '["bob"]' -p bob
cleos push action dcontract addprov '["jack"]' -p jack

cleos push action dcontract setprvstatus '["jack", 2]' -p jack

cleos get table dcontract bob provsignings
cleos get table dcontract jack provsignings
cleos get table dcontract alice dmdsignings

cleos push action dcontract createorder '[ "alice", 10001, "inqurt parm : 1111",["jack","bob"],{"mode":1,"size":10} ]' -p alice
cleos push action dcontract createorder '[ "alice", 10001, "inqurt parm : 1111",["bob"],{"mode":1,"size":1} ]' -p alice

cleos push action dcontract setprvstatus '["bob",2]' -p bob
cleos push action dcontract createorder '[ "alice", 10001, "inqurt parm : 1111",["bob"],{"mode":1,"size":1} ]' -p alice
cleos push action dcontract createorder '[ "alice", 10002, "inqurt parm : 1111",["bob"],{"mode":0,"size":30} ]' -p alice

cleos get table dcontract bob provorders 
cleos get table dcontract jack provorders 
cleos get table dcontract 10001 odridprovs

cleos push action dcontract commitorder '[ "bob", 10001, "result : 2222" ]' -p bob	
cleos push action dcontract commitorder '[ "bob", 10002, "result : 2222" ]' -p bob	

cleos push action dcontract commitorder '[ "jack", 10001, "result : 3333" ]' -p jack

cleos push action dcontract finishorder '[ "alice", 10001, "bob"]' -p alice
cleos push action dcontract finishorder '[ "alice", 10001, "jack"]' -p alice
cleos get table eosio.token alice accounts 
cleos get table eosio.token bob accounts 
cleos get table eosio.token jack accounts 

cleos get table dcontract p orders 

cleos push action dcontract createorder '[ "alice", 10222, "inqurt parm :222222",["bob","jack"] ]' -p alice

cleos push action dcontract commitorder '[ "bob", 10222, "result : 333333" ]' -p bob	

cleos push action dcontract finishorder '[ "alice", 10222, "bob"]' -p alice
cleos push action dcontract cancelorder '[ "alice", 10222, "jack"]' -p alice

cleos get table eosio.token alice accounts 
cleos get table eosio.token bob accounts 
cleos get table eosio.token jack accounts


```



# test eosio.token API

cleos create account eosio eosio.token EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g
cleos create account eosio alice EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g
cleos create account eosio test EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g
cleos set contract eosio.token /Users/hushi/Software/eos/build/contracts/eosio.token -p eosio.token

cleos push action eosio.token create '[ "eosio", "1000000000.0000 SYS", 0, 0, 0]' -p eosio.token

cleos push action eosio.token create '[ "mortgage", "1000000000.0000 UDT", 0, 0, 0]' -p eosio.token

cleos push action eosio.token issue '[ "alice", "1000.0000 SYS", "" ]' -p eosio

cleos set contract test /Users/hushi/blockchain-workspace/eos-contract/auth /Users/hushi/blockchain-workspace/eos-contract/auth/test_read_table.wast /Users/hushi/blockchain-workspace/eos-contract/auth/test_read_table.abi -p test

cleos push action test testarray '[["alice"]]' -p test
# test eosio.token API
cleos create account eosio udtrade EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG EOS8akjQ6BfbAwZjQco4QJhbpdeZWKqAwFhPQeUoXRiFz8joUd77g
cleos set contract udtrade /Users/hushi/blockchain-workspace/eos-contract/auth /Users/hushi/blockchain-workspace/eos-contract/auth/ud_trade.wast /Users/hushi/blockchain-workspace/eos-contract/auth/ud_trade.abi -p udtrade


cleos set account permission alice active '{"threshold": 1,"keys": [{"key": "EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG","weight": 1}],"accounts": [{"permission":{"actor":"dcontract","permission":"eosio.code"},"weight":1},{"permission":{"actor":"bob","permission":"active"},"weight":1}]}' owner -p alice


# docker环境，数据合约账号
# 合约地址:
Private key: 5K7aNSUsimup68W7ZGgQBH1vThGoKxMiXbc1Skr49cC5qMEARAe
Public key: EOS7pyDLScS4akkyXfy6uaocMtjA7gVxmWGpBozEDEi5zfxj23ti7
cleos system newaccount eosio --transfer dcontract111 EOS7pyDLScS4akkyXfy6uaocMtjA7gVxmWGpBozEDEi5zfxj23ti7 --stake-net "80000.0000 SYS" --stake-cpu "80000.0000 SYS" --buy-ram-kbytes 10000

cleos push action acntstats addcertacnt '["acntstats","acntstats"]' -p acntstats

cleos push action acntstats setsrvurl '["demandacnt11","www.demandacnt11.com"]' -p demandacnt11
cleos push action acntstats applyforcert '["demandacnt11","acntstats","companyname111","www.license.com"]' -p demandacnt11
cleos push action acntstats certify '["acntstats","demandacnt11", 1]' -p acntstats

cleos push action acntstats setsrvurl '["provider1111","www.provider1111.com"]' -p provider1111
cleos push action acntstats applyforcert '["provider1111","acntstats","companyname2221","www.license.com"]' -p provider1111
cleos push action acntstats certify '["acntstats","provider1111", 1]' -p acntstats


cleos set contract dcontract111 /Users/hushi/blockchain-workspace/eos-contract/auth /Users/hushi/blockchain-workspace/eos-contract/auth/inquiry_data_contract.wast /Users/hushi/blockchain-workspace/eos-contract/auth/inquiry_data_contract.abi -p dcontract111


cleos push action eosio setpriv '["dcontract111", 1]' -p eosio@active

# demandacnt11
Private key: 5JVjqSxM8AJjBsqpRPfcnYCQQDmNRhk7ssJd19GQuyR7HF9GM93
Public key: EOS5NASa5XXAaR8teSXsyMULBNZv2carDLuTNoWaX4CLK2GCGqeSA
cleos push action dcontract111 init '["dcontract111"]' -p dcontract111
cleos push action eosio.token issue '[ "demandacnt11", "10000.0000 UDT", "issue" ]' -p mortgage


cleos system newaccount eosio --transfer demandacnt11 EOS5NASa5XXAaR8teSXsyMULBNZv2carDLuTNoWaX4CLK2GCGqeSA --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 10000

cleos push action dcontract111 adddmd '["demandacnt11"]' -p demandacnt11

# provider1111
Private key: 5KTVSUqUyM6w8rY5M2hy1iAvS1ce7UekufYHPNcaPgGMdzzzpoX
Public key: EOS5s1qEmYHfKVsiNbJFUeFVCFovmGk8tuwMvhqJQAk2MTkGjFWVS

cleos system newaccount eosio --transfer provider1111 EOS5s1qEmYHfKVsiNbJFUeFVCFovmGk8tuwMvhqJQAk2MTkGjFWVS --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 10000
cleos push action dcontract111 addprov '["provider1111"]' -p provider1111

cleos push action -x 3000 -j -d dcontract111 createorder '[ "demandacnt11", 10001, "inqurt parm : 1111",["provider1111"] ]' -p demandacnt11

cleos set account permission provider1111 active '{"threshold": 1,"keys": [{"key": "EOS8bfVxP4meXk3DkBqadSwKJ9MYGT8STg1EsGx4ssEujdx7VaZ1p","weight": 1}],"accounts": [{"permission":{"actor":"dcontract112","permission":"eosio.code"},"weight":1}]}' owner -p provider1111


# provider1112
Private key: 5K4xHg7NRtvsJnnVqHtXv1qFRbacTdaaKAEKagKqPST99JXmUbd
Public key: EOS8bfVxP4meXk3DkBqadSwKJ9MYGT8STg1EsGx4ssEujdx7VaZ1p

cleos system newaccount eosio --transfer provider1112 EOS8bfVxP4meXk3DkBqadSwKJ9MYGT8STg1EsGx4ssEujdx7VaZ1p --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 10000

cleos push action acntstats setsrvurl '["provider1112","www.provider1112.com"]' -p provider1112
cleos push action acntstats applyforcert '["provider1112","acntstats","companyname2221","www.license.com"]' -p provider1112
cleos push action acntstats certify '["acntstats","provider1112", 1]' -p acntstats
cleos push action dcontract111 addprov '["provider1112"]' -p provider1112

cleos set account permission provider1112 active '{"threshold": 1,"keys": [{"key": "EOS8bfVxP4meXk3DkBqadSwKJ9MYGT8STg1EsGx4ssEujdx7VaZ1p","weight": 1}],"accounts": [{"permission":{"actor":"dcontract112","permission":"eosio.code"},"weight":1}]}' owner -p provider1112

# provider1113
Private key: 5K48X6G4kN7zaceGAUTWtDubLGRF6pDfMFDrNCcV7nh63h1Y4fn
Public key: EOS8VNMTAZTG1SZQnXa8LiW5QDgiqjPB1PpXgn4ASsBNrJsbuzjrK

cleos system newaccount eosio --transfer provider1113 EOS8VNMTAZTG1SZQnXa8LiW5QDgiqjPB1PpXgn4ASsBNrJsbuzjrK --stake-net "10000.0000 SYS" --stake-cpu "10000.0000 SYS" --buy-ram-kbytes 10000

cleos push action acntstats setsrvurl '["provider1113","www.provider1113.com"]' -p provider1113
cleos push action acntstats applyforcert '["provider1113","acntstats","companyname233321","www.license.com"]' -p provider1113
cleos push action acntstats certify '["acntstats","provider1113", 1]' -p acntstats
cleos push action dcontract111 addprov '["provider1113"]' -p provider1113

cleos create account eosio storage EOS8XAy4haqHrU5CnHFcSY5tdxeXR8dS5xA4JmXeuJgpgu5eHAWUG 
cleos set contract storage /Users/hushi/blockchain-workspace/eos-contract/auth /Users/hushi/blockchain-workspace/eos-contract/auth/storage/storage.wast /Users/hushi/blockchain-workspace/eos-contract/auth/storage/storage.abi -p storage

cleos set account permission provider1113 active '{"threshold": 1,"keys": [{"key": "EOS8bfVxP4meXk3DkBqadSwKJ9MYGT8STg1EsGx4ssEujdx7VaZ1p","weight": 1}],"accounts": [{"permission":{"actor":"dcontract112","permission":"eosio.code"},"weight":1}]}' owner -p provider1113

cleos push action dcontract setname '["测试中文名"]' -p dcontract  

cleos push action d4qilcuiojx5 setmodelinfo '[{"model_used":1,"storage_used":1,"model_jar_address":"testjaraddr","model_jar_hash":"sdjasodasdasd","model_java_code":"testjavacode","data_dictionary_ids":[9,10]}]' -p muhe 
cleos push action d4ze1vyoircz setmodelinfo '[{"model_used":1,"storage_used":1,"model_jar_address":"testjaraddr","model_jar_hash":"sdjasodasdasd","model_java_code":"testjavacode","data_dictionary_ids":[9,10]}]' -p muhe 

# 设置黑名单
cleos push action dlv2sx55gne4 setapi '[{"explain":"","common_request_params":"[{\"columnType\":\"String\",\"comment\":\"合约地址。可在参与合约中查看\",\"name\":\"dataContract\"},{\"columnType\":\"String\",\"comment\":\"需求方账号名称\",\"name\":\"consumer\"},{\"columnType\":\"int\",\"comment\":\"最大响应数(默认为1 可以修改)\",\"name\":\"terminateAfterNSuccess\"}]","conceal_query_request_params":"[{\"columnType\":\"int\",\"comment\":\"匿踪查询隐藏数据位数\",\"name\":\"maskBytes\"},{\"columnType\":\"int\",\"comment\":\"混淆后的最大请求 key 的个数\",\"name\":\"maxRequestKeys\"},{\"columnType\":\"String\",\"comment\":\"匿踪查询字段名称\",\"name\":\"anonymousField\"}]","businesss_request_params":"[{\"columnType\":\"String\",\"comment\":\"姓名\",\"name\":\"name\"},{\"columnType\":\"String\",\"comment\":\"身份证号\",\"name\":\"idno\"},{\"columnType\":\"String\",\"comment\":\"手机号码\",\"name\":\"mobile\"}]","custom_return_results":"","data_dictionary_return_results":"","example":""}]' -p  zx.test1


cleos push action dlv2sx55gne4 setmodelinfo '[{"model_used":1,"storage_used":0,"model_jar_address":"http://cm-web.currency-market.k2.test.wacai.info/file/fileDownload/zx.test1_1532000609670.jar","model_jar_hash":"61DD9B2CA0ED5A6A45A2A734D0BE5358408331E1","model_java_code":"zx.test1_1532000612706.zip","data_dictionary_ids":[10]}]' -p zx.test1

# 设置公债

cleos push action dwk4aezlplhy setapi '[{"explain":"","common_request_params":"{\"dataContract\":\"dwk4aezlplhy\",\"terminateAfterNSuccess\":1}","conceal_query_request_params":"{\"maskBytes\":4,\"maxRequestKeys\":3}","businesss_request_params":"{\"name\":\"lufeng\",\"idno\":\"411524199605278432\",\"mobile\":\"18738121039\"}","custom_return_results":"","data_dictionary_return_results":"","example":""}]' -p  zx.test1

cleos push action dwk4aezlplhy setmodelinfo '[{"model_used":1,"storage_used":0,"model_jar_address":"http://cm-web.currency-market.k2.test.wacai.info/file/fileDownload/zx.test1_1532004543251.jar","model_jar_hash":"6E972B51040893D639149999E67A32EA60DAC131","model_java_code":"zx.test1_1532004549320.zip","data_dictionary_ids":[9]}]' -p zx.test1

[{\"columnType\":\"String\",\"comment\":\"姓名\",\"name\":\"name\"},{\"columnType\":\"String\",\"comment\":\"身份证号\",\"name\":\"idno\"},{\"columnType\":\"String\",\"comment\":\"手机号码\",\"name\":\"mobile\"}]

cleos push action dwk4aezlplhy setapi '[{"explain":"","common_request_params":"[{\"columnType\":\"String\",\"comment\":\"合约地址。可在参与合约中查看\",\"name\":\"dataContract\"},{\"columnType\":\"String\",\"comment\":\"需求方账号名称\",\"name\":\"consumer\"},{\"columnType\":\"int\",\"comment\":\"最大响应数(默认为1 可以修改)\",\"name\":\"terminateAfterNSuccess\"}]","conceal_query_request_params":"[{\"columnType\":\"int\",\"comment\":\"匿踪查询隐藏数据位数\",\"name\":\"maskBytes\"},{\"columnType\":\"int\",\"comment\":\"混淆后的最大请求 key 的个数\",\"name\":\"maxRequestKeys\"},{\"columnType\":\"String\",\"comment\":\"匿踪查询字段名称\",\"name\":\"anonymousField\"}]","businesss_request_params":"[{\"columnType\":\"String\",\"comment\":\"姓名\",\"name\":\"name\"},{\"columnType\":\"String\",\"comment\":\"身份证号\",\"name\":\"idno\"},{\"columnType\":\"String\",\"comment\":\"手机号码\",\"name\":\"mobile\"}]","custom_return_results":"","data_dictionary_return_results":"","example":""}]' -p  zx.test1

cleos set account permission dlv2sx55gne4 active '{"threshold": 1,"keys": [{"key": "EOS5HPsRQPzU684cuhyHTmLK1NxS1dTHeg5fhEShKM3QRniUQvKqz","weight": 1}],"accounts": [{"permission":{"actor":"dlv2sx55gne4","permission":"eosio.code"},"weight":1}]}' owner -p dlv2sx55gne4

cleos set account permission dwk4aezlplhy active '{"threshold": 1,"keys": [{"key": "EOS5HPsRQPzU684cuhyHTmLK1NxS1dTHeg5fhEShKM3QRniUQvKqz","weight": 1}],"accounts": [{"permission":{"actor":"dwk4aezlplhy","permission":"eosio.code"},"weight":1}]}' owner -p dwk4aezlplhy



{
  "expiration": "2018-07-23T07:56:31",
  "ref_block_num": 7412,
  "ref_block_prefix": 3719651921,
  "max_net_usage_words": 0,
  "max_cpu_usage_ms": 0,
  "delay_sec": 0,
  "context_free_actions": [],
  "actions": [{
      "account": "eosio.token",
      "name": "transfer",
      "authorization": [{
          "actor": "zx.test1",
          "permission": "active"
        }
      ],
      "data": "00000021639541ff000000216395419310270000000000000453595300000000046d656d6f"
    }
  ],
  "transaction_extensions": [],
  "signatures": [
    "SIG_K1_K9kJWj4mdGwTiGAQ8S2D5K8azqTF3uQhvkoUhSrydW268rG5AdMAVNLCTjxvkGGUcHG7JkU6rpCpEJA4hrtdsvu9iq8mxM"
  ],
  "context_free_data": []
}


