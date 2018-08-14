#include "inquiry_data_contract.hpp"
#include <eosiolib/dispatcher.hpp>

#include "inquiry_signing.cpp"
#include "inquiry_order.cpp"

namespace unitedata {
    
    data_contract::data_contract( account_name s ) 
        :eosio::contract(s),
        _providers(_self, _self),
        _demandsides(_self, _self),
        _orders(_self, _self),
        _stats(_self,_self)
    {}
    
    
    uint8_t data_contract::get_signing_strategy() {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "stats not init");
        return stat_itr->signing_strategy;
    }
    
    ///@abi aciton
    void data_contract::restart() {
        auto stat_itr = _stats.find(N(stats));
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.stopped = false;
        });
    }

    ///@abi aciton
    void data_contract::stopcontract() {
        auto stat_itr = _stats.find(N(stats));
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.stopped = true;
        });
    }
    
    ///@abi aciton
    void data_contract::init(account_name name) {
        require_auth(name);
        print("init contract stats.");
        auto stat_itr = _stats.find(N(stats));
        if (stat_itr == _stats.end()) {
            // 开始初始化
            print("emplace contract stats.");
            _stats.emplace(name, [&]( auto& s ){
                s.pkey = N(stats);
                s.admin = name;
                s.pay_strategy = 1; 
                s.signing_strategy = 1; // 1 签，2 不签; 提供方加入，默认签需求方。服务方加入按策略配置。
                s.price = asset(1000, S(4,UDT));
            });
        } else {
            print("contract stats already set.");
        }
    }
    
    ///@abi aciton
    void data_contract::setprice(asset price) {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "contract not init");
        require_auth(stat_itr->admin);
        eosio_assert(price.symbol == S(4, UDT), "price only UDT limited");
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.price = price;
        });
    }
    
    ///@abi aciton
    void data_contract::setsigning(uint8_t signing) {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "contract not init");
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.signing_strategy = signing;
        });
    }
    
    ///@abi aciton
    void data_contract::sethidden(uint8_t hidden) {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "contract not init");
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.hidden = hidden >= 1;
        });
    }
    ///@abi aciton
    void data_contract::setmcinfo(mc_info mc) {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "contract not init");
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.multi_party_computation = mc;
        });
    }
    
    ///@abi aciton
    void data_contract::setmodelinfo(model_info model) {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "contract not init");
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.model = model;
        });
    }

    ///@abi aciton
    void data_contract::setname(string name) {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "contract not init");
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.contract_name = name;
        });
    }

    ///@abi aciton
    void data_contract::sethomeimg(string home_image) {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "contract not init");
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.home_image = home_image;
        });
    }

    ///@abi aciton
    void data_contract::setdetailimg(string detail_image) {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "contract not init");
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.detail_image = detail_image;
        });
    }

    ///@abi aciton
    void data_contract::setapi(api_info api) {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "contract not init");
        require_auth(stat_itr->admin);
        _stats.modify(stat_itr, 0, [&]( auto& s ){
            s.api = api;
        });
    }
    
    void data_contract::assert_not_stopped() {
        auto stat_itr = _stats.find(N(stats));
        eosio_assert(!stat_itr->stopped, "contract stopped");
    }
    
} /// eosio.system



EOSIO_ABI( unitedata::data_contract,
        // stats
        (restart)(init)(stopcontract)(setprice)(setsigning)(sethidden)(setmcinfo)(setmodelinfo)(setname)(sethomeimg)(setdetailimg)(setapi)
        // order
        (createorder)(commitorder)(finishorder)(cancelorder)(claimreward)
        // signig
        (addprov)(adddmd)(quitprov)(quitdmd)(signasprov)(signasdmd)(resignasprov)(resignasdmd)(setprvstatus)(quitcontract)
)
