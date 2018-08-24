#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/action.hpp>
#include <eosiolib/contract.hpp>
#include <eosiolib/print.hpp>

#include "../eosio.token/eosio.token.hpp"
#include "../certification/acntstats.hpp"

#include <string>


namespace unitedata {
    // 
    using eosio::indexed_by;
    using eosio::const_mem_fun;
    using eosio::asset;
    using std::string;
    using std::vector;

    static constexpr time ONE_DAY = 3*24*3600;

    enum paying_strategy_enum {
        BATCH_DURATION = 0,
        BATCH_NUMBER = 1
    };

    enum contract_type {
        BATCH_DEFAULT = 0
    };
    
    struct billing_mode {
        uint8_t mode;
        uint32_t size;
    };

    // todo 继承的struct造成的bug
    // struct billing_cfg : public billing_mode {
    //     asset price;

    //     bool operator<(const billing_cfg& that) const { 
    //         return mode != that.mode ? mode < that.mode : size < that.size ; 
    //     }

    //     bool operator==(const billing_cfg& that) const {
    //         return mode == that.mode && size == that.size ;
    //     }
    // };

    struct billing_cfg {
        uint8_t mode;
        uint32_t size;
        asset price;

        bool operator<(const billing_cfg& that) const { 
            return mode != that.mode ? mode < that.mode : size < that.size ; 
        }

        bool operator==(const billing_cfg& that) const {
            return mode == that.mode && size == that.size ;
        }
    };


    struct model_info {  // 模型信息
        bool model_used = false;
        bool storage_used;
        string model_jar_address;
        string model_jar_hash;
        string model_java_code;
        std::vector<uint64_t> data_dictionary_ids;
    };
    
    struct mc_info {   // 多方计算信息
        bool used = false;
        std::vector<string> input;
        std::vector<string> output;
        uint16_t participant_lower_bound;
        
    };

    struct api_info {
        string explain;
        string common_request_params;
        string conceal_query_request_params;
        string businesss_request_params;
        string custom_return_results;
        string data_dictionary_return_results;
        string example;
    };
    
    ///@abi table stats i64
    struct contract_info {
        uint64_t pkey =  N(stats);
        account_name admin;
        std::vector<billing_cfg> billing_configs;   
        // asset price;
        uint8_t signing_strategy; // 1 签，0 不签 ；  提供方加入，默认签需求方。服务方加入按策略配置。
        bool hidden;                      // 是否匿踪
        string contract_name;     // 合约名字
        string home_image;        // 合约首页图片地址
        string detail_image;      // 合约详情图片地址
        
        mc_info multi_party_computation;   // 多方计算
        model_info model;                  // 模型相关
        api_info api;  

        uint8_t type;             // 合约的类型。0，当前默认      

        bool stopped;     
        
        auto primary_key()const { return N(stats); }
    };
    
    
    // order
    ///@abi table orders i64
    struct order {
        uint64_t id;
        account_name creator;
        string inquiry_param;
        asset cost;
        uint64_t crt_time;
        
        billing_mode mode;

        uint64_t get_crttime()const { return crt_time; }
        uint64_t get_creator()const { return creator; }
        
        auto primary_key()const { return id; }
    };
    
    ///@abi table provorders i64
    struct provorder {
        uint64_t order_id;  // pkey
        account_name name;  // scope
        asset reward;
        uint8_t order_status;
        uint64_t last_update_time;
        string result;

        billing_mode mode;
        account_name creator;

//        uint64_t get_order_id()const { return order_id; }
//        uint64_t get_name()const { return name; }
        auto primary_key()const { return order_id; }
    };
    
    ///@abi table odridprovs i64
    struct simple_prov {
//        uint64_t order_id;  // scope 支付的时候作为索引
        account_name name;  // pk
        
        auto primary_key()const { return name; }
        
    };
    // signing
    ///@abi table providers i64
    struct srvprov {
        account_name name;
        uint32_t create_time;
        uint8_t status;

        auto primary_key()const { return name; }
    };
    
    ///@abi table demandsides i64
    struct demand_side {
        account_name name;
        uint32_t finished_order_count;
        uint32_t canceled_order_count;
        uint32_t create_time;
        
        auto primary_key()const { return name; }
    };
    
    ///@abi table provsignings i64
    struct prov_signing {
        account_name dmd;    // 需求方
        
        uint64_t primary_key()const { return dmd; }
    };
    ///@abi table dmdsignings i64
    struct dmd_signing {
        account_name prov;    // 提供方
        
        uint64_t primary_key()const { return prov; }
    };
    
    // stats
    typedef eosio::multi_index<N(stats) , contract_info> contract_stats;
    // signing
    typedef eosio::multi_index<N(providers) , srvprov> providers;
    
    typedef eosio::multi_index<N(demandsides) , demand_side> demandsides;
    
    typedef eosio::multi_index<N(dmdsignings) , dmd_signing> dmdsignings;
    
    typedef eosio::multi_index<N(provsignings) , prov_signing> provsignings;
    
    // order
    typedef eosio::multi_index<N(orders) , order,
        indexed_by<N(bycrttime), const_mem_fun<order, uint64_t, &order::get_crttime>  >
        // indexed_by<N(bycreator), const_mem_fun<order, uint64_t, &order::get_creator>  >
        > orders;
    
    typedef eosio::multi_index<N(provorders) , provorder> provorders;
    
    typedef eosio::multi_index<N(odridprovs) , simple_prov> odridprovs;
    
    class data_contract : public eosio::contract {
        
    private:
        // stats
        contract_stats _stats;
        // signing
        providers _providers;
        
        demandsides _demandsides;
        
        // order
        // orders _orders;
        //
        
        bool stopped = false;
    
    public:
        data_contract(account_name s);
//        ~data_contract();
        // stats
        void stopcontract();

        void init(account_name name);
        
        void setprice(std::vector<billing_cfg> configs);
        
        void setsigning(uint8_t signing);
        
        void sethidden(uint8_t hidden);
        
        void setmcinfo(mc_info mc);
        
        void setmodelinfo(model_info model);

        void setname(string name);

        void sethomeimg(string home_image);

        void setdetailimg(string detail_image);

        void setapi(api_info api);
        
        // order
        void createorder(account_name creator, uint64_t order_id, string param, vector<account_name>& providers, billing_mode mode); // 创建订单
        
        void commitorder(account_name prov, uint64_t order_id, string result);  // 服务结束，获取报酬。

        void finishorder(account_name creator, uint64_t order_id, account_name prov) {
            eosio_assert(false, "action not supported");
        }  
        
        void cancelorder(account_name creator, uint64_t order_id, account_name prov){
            eosio_assert(false, "action not supported");
        }    // 需求方取消订单
        
        void claimreward(account_name prov, uint64_t order_id){
            eosio_assert(false, "action not supported");
        }   // 需求方取消订单
        
        // signing
        void addprov(account_name name);

        void adddmd(account_name name);
        
        void quitprov(account_name name);
        
        void quitdmd(account_name name);
       
        void signasprov(account_name prov, account_name dmd);
        
        void signasdmd(account_name dmd, account_name prov);
        
        void resignasprov(account_name prov, account_name dmd);
        
        void resignasdmd(account_name dmd, account_name prov);
        
        void quitcontract(account_name name);

        void setprvstatus(account_name name, uint8_t status);
        

    private:
        // stats
        // 获取签约策略
        uint8_t get_signing_strategy();
        void assert_not_stopped();
        // 签约
        void _sign(account_name from, account_name to, uint8_t as, account_name ram_payer);
        // 解约
        void _resign(account_name from, account_name to, uint8_t as);
        // 是否签名
        bool _is_signed(account_name from, account_name to, uint8_t as);
        
    };
    
}
