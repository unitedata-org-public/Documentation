#include "inquiry_data_contract.hpp"



namespace unitedata {
    
    using eosio::print;
    
    ///@abi action
    void data_contract::createorder(account_name creator, uint64_t order_id, string param, vector<account_name>& providers) {
        assert_not_stopped();
        require_auth(creator);

        auto dmd_itr = _demandsides.find(creator);
        eosio_assert(dmd_itr != _demandsides.end(), "invalid creator");
        eosio_assert(providers.size() > 0 && providers.size() <= 20 , "invalid providers.size");

        auto order_itr = _orders.find(order_id);
        eosio_assert(order_itr == _orders.end(), "invalid order_id");

        // 检查重复        
        std::sort( providers.begin(), providers.end() );
        for( size_t i = 1; i < providers.size(); ++i ) {
            eosio_assert( providers[i-1] < providers[i], "providers must be unique" );
        }
        // 查询cost
        auto contract_stat = _stats.find(N(stats));
        eosio_assert(contract_stat!=_stats.end(), "invalid stats key");
        // 添加

        
        odridprovs existing_odridprovs(_self, order_id);
        uint16_t set_up_count = 0;

        for( size_t i = 0; i < providers.size(); ++i ) {
            auto prov_itr = _providers.find(providers[i]);
            eosio_assert(prov_itr!=_providers.end(),"invalid provider");
            // eosio_assert(prov_itr->status==2, "providers service is not set up");
            if (prov_itr->status!=2) {
                continue;
            }
            ++set_up_count;
            eosio_assert(_is_signed(providers[i], creator, 1) && _is_signed(creator, providers[i], 2), "not signed");  // 签约
            // 添加到索引表中
            auto odridprov_itr = existing_odridprovs.find(providers[i]);
            if (odridprov_itr == existing_odridprovs.end()) {
                existing_odridprovs.emplace(creator, [&]( auto& p ) {
                    p.name = providers[i];
                });
            }

            provorders existing_provorders(_self, providers[i]);
            
            auto provorder_itr = existing_provorders.find(order_id);
            eosio_assert(provorder_itr == existing_provorders.end(), "Multiple identical providers");
            existing_provorders.emplace(creator, [&]( auto& o ) {
                o.order_id = order_id;
                o.name = providers[i];
                o.reward = asset(0, S(4, UDT));
                o.order_status = 1;
                // todo
            });
        }
        
        // 转账到合约
        if (set_up_count > 0) {
            asset cost = asset(contract_stat->price.amount * set_up_count, contract_stat->price.symbol);
            _orders.emplace(creator, [&]( auto& o ) {
                o.id = order_id;
                o.creator = creator;
                o.cost = cost;
                o.inquiry_param = param;
                o.crt_time = now();
                // todo
            });
            const asset eosio_balance   = eosio::token( N(eosio.token)).get_balance(creator, eosio::symbol_type(S(4, UDT)).name());
            if (eosio_balance.amount >=  cost.amount) {
                eosio::action(
                   eosio::permission_level{ creator, N(active) },
                   N(eosio.token), N(transfer),
                   std::make_tuple(creator, _self, cost, std::string("create order"))
                ).send();
            } else {
                eosio::action(
                   eosio::permission_level{ creator, N(active) },
                   N(mortgage), N(transfer),
                   std::make_tuple(creator, _self, cost, std::string("create order"))
                ).send();
            }
        } else {
            eosio_assert(false, "none of providers set up");
        }


    }
    
    ///@abi action
    void data_contract::commitorder(account_name prov, uint64_t order_id, string result) {
        require_auth(prov);
        
        auto odr_itr = _orders.find(order_id);
        eosio_assert(odr_itr != _orders.end(), "invalid order id");
        
        provorders existing_provorders(_self, prov);
        auto provorder_itr = existing_provorders.find(order_id);
        eosio_assert(provorder_itr != existing_provorders.end(), "invalid provider"); // 提供方在列表中
        eosio_assert(provorder_itr->order_status == 1, "order closed");   // 订单未结束

        eosio_assert(result.size() > 0 && result.size() < 512 , "result.size == 0 || result.size > 512");   // 订单未结束

        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "stats not init");
        // 添加到prov_orders表中
        
        existing_provorders.modify(provorder_itr, prov, [&]( auto& o ) {
            // 修改结果
            o.result = result;
            o.reward = stat_itr->price;
            o.last_update_time = now();
            // todo
        });

        
    }
    
    ///@abi action
    void data_contract::finishorder(account_name creator, uint64_t order_id, account_name prov) {
        require_auth(creator);

        auto odr_itr = _orders.find(order_id);
        eosio_assert(odr_itr != _orders.end(), "invalid order id");
        eosio_assert(odr_itr->creator == creator, "invalid creator");  // creator
        
        provorders existing_provorders(_self, prov);
        auto provorder_itr = existing_provorders.find(order_id);
        eosio_assert(provorder_itr != existing_provorders.end(), "invalid provider"); // 提供方在列表中
        eosio_assert(provorder_itr->order_status == 1, "order closed");   // 订单未结束
        eosio_assert(provorder_itr->result.size() > 0, "result not committed");   // 结果提交才能完成

        // 报酬结算，转账
        auto contract_stat = _stats.find(N(stats));

        
        eosio::action(
            eosio::permission_level{ _self, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple(_self, provorder_itr->name, provorder_itr->reward, std::string("transfer reward"))
        ).send();

        existing_provorders.erase(provorder_itr);

        // 查询索引表
        odridprovs existing_odridprovs(_self, order_id);
        auto order_index_it = existing_odridprovs.find(prov);
        if (order_index_it != existing_odridprovs.end()) {
            existing_odridprovs.erase(order_index_it);
            // 索引表中无数据
            if (existing_odridprovs.begin() == existing_odridprovs.end()) {
                _orders.erase(odr_itr);
            }
        }
    }
    
    ///@abi action
    void data_contract::cancelorder(account_name creator, uint64_t order_id, account_name prov) {
        require_auth(creator);

        auto odr_itr = _orders.find(order_id);
        eosio_assert(odr_itr != _orders.end(), "invalid order id");
        eosio_assert(odr_itr->creator == creator, "invalid creator");  // creator
        
        provorders existing_provorders(_self, prov);
        auto provorder_itr = existing_provorders.find(order_id);
        eosio_assert(provorder_itr != existing_provorders.end(), "invalid provider"); // 提供方在列表中
        eosio_assert(provorder_itr->order_status == 1, "order closed");   // 订单未结束
        eosio_assert(provorder_itr->result.size() == 0, "result committed");   // 结果未提交

        // todo 修改dmds的统计数据
        auto contract_stat = _stats.find(N(stats));
        _orders.modify(odr_itr, 0, [&]( auto& o ) {
            // 取消订单
            o.cost -= contract_stat->price;
        });
      
        eosio::action(
              eosio::permission_level{ _self, N(active) },
              N(eosio.token), N(transfer),
              std::make_tuple(_self, odr_itr->creator, contract_stat->price, std::string("cancel order"))
        ).send();

        existing_provorders.erase(provorder_itr);

        // 查询索引表
        odridprovs existing_odridprovs(_self, order_id);
        auto order_index_it = existing_odridprovs.find(prov);
        if (order_index_it != existing_odridprovs.end()) {
            existing_odridprovs.erase(order_index_it);
            // 索引表中无数据
            if (existing_odridprovs.begin() == existing_odridprovs.end()) {
                _orders.erase(odr_itr);
            }
        }
        
    }
    ///@abi action
    void data_contract::claimreward(account_name prov, uint64_t order_id) {
        require_auth(prov);
        
        auto odr_itr = _orders.find(order_id);
        eosio_assert(odr_itr != _orders.end(), "invalid order id");
        
        provorders existing_provorders(_self, prov);
        auto provorder_itr = existing_provorders.find(order_id);
        eosio_assert(provorder_itr != existing_provorders.end(), "invalid provider"); // 提供方在列表中
        eosio_assert(provorder_itr->order_status == 1, "order closed");   // 订单未结束
        eosio_assert(provorder_itr->last_update_time > 0, "result not committed");       // result committed
        eosio_assert(now() > provorder_itr->last_update_time + 3600, "");   // 结果提交3600s之后
        
        
        // 报酬结算，转账
        auto contract_stat = _stats.find(N(stats));

        
        eosio::action(
            eosio::permission_level{ _self, N(active) },
            N(eosio.token), N(transfer),
            std::make_tuple(_self, prov, contract_stat->price, std::string("claim reward"))
        ).send();

        existing_provorders.erase(provorder_itr);

        // 查询索引表
        odridprovs existing_odridprovs(_self, order_id);
        auto order_index_it = existing_odridprovs.find(prov);
        if (order_index_it != existing_odridprovs.end()) {
            existing_odridprovs.erase(order_index_it);
            // 索引表中无数据
            if (existing_odridprovs.begin() == existing_odridprovs.end()) {
                _orders.erase(odr_itr);
            }
        }
    }
    

}



