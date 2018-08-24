#include "batch_data_contract.hpp"



namespace unitedata {
    
    using eosio::print;
    
    ///@abi action
    void data_contract::createorder(account_name creator, uint64_t order_id, string param, vector<account_name>& providers, billing_mode mode) {
        assert_not_stopped();
        require_auth(creator);

        auto dmd_itr = _demandsides.find(creator);
        eosio_assert(dmd_itr != _demandsides.end(), "invalid creator");
        eosio_assert(providers.size() > 0 && providers.size() <= 20 , "invalid providers.size");

        orders dmd_orders(_self, creator);
        auto order_itr = dmd_orders.find(order_id);
        eosio_assert(order_itr == dmd_orders.end(), "invalid order_id");
        // 单一提供方
        eosio_assert(providers.size() == 1, "only single provider supported");
        // 检查提供方重复        
        std::sort(providers.begin(), providers.end());
        for( size_t i = 1; i < providers.size(); ++i ) {
            eosio_assert( providers[i-1] < providers[i], "providers must be unique" );
        }
        // 查询cost
        asset price = asset(-1, S(4, UDT));
        billing_mode matched;
        auto contract_stat = _stats.find(N(stats));
        eosio_assert(contract_stat != _stats.end(), "invalid stats key");
        vector<billing_cfg> supported_modes = contract_stat->billing_configs;
        for (auto sit = supported_modes.begin(); sit != supported_modes.end(); ++sit) {
            if (sit->mode == mode.mode && sit->size == mode.size ) {
                price.amount = sit->price.amount;
                matched.mode = sit->mode;
                matched.size = sit->size;
            }
        }
        eosio_assert(price.amount > 0, "unsupported mode or size");
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
                o.reward = price;
                // todo
                o.last_update_time = now();
                o.mode.mode = matched.mode;
                o.mode.size = matched.size;
                o.creator = creator;

            });
        }
        
        // 转账到合约
        if (set_up_count > 0) {
            asset cost = price;
            dmd_orders.emplace(creator, [&]( auto& o ) {
                o.id = order_id;
                o.creator = creator;
                o.cost = cost;
                o.inquiry_param = param;
                o.crt_time = now();
                o.mode.mode = mode.mode;
                o.mode.size = mode.size;
                // todo
            });
            // todo 如果这个账号手里没有udt会直接报找不到key的错。
            
            const asset eosio_balance = eosio::token( N(eosio.token)).get_balance(creator, eosio::symbol_type(S(4, UDT)).name());
            
            
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

        provorders existing_provorders(_self, prov);
        auto provorder_itr = existing_provorders.find(order_id);
        eosio_assert(provorder_itr != existing_provorders.end(), "invalid provider"); // 提供方在列表中

        orders dmd_orders(_self, provorder_itr->creator);
        auto odr_itr = dmd_orders.find(order_id);
        eosio_assert(odr_itr != dmd_orders.end(), "invalid order id");
        
        // 订单未结束
        if (provorder_itr->mode.mode == BATCH_DURATION) {
            eosio_assert(now() >= provorder_itr->last_update_time + provorder_itr->mode.size * ONE_DAY, "order not finished");
        }
        eosio_assert(result.size() > 0 && result.size() < 512 , "result.size == 0 || result.size > 512");   // 订单未结束

        auto stat_itr = _stats.find(N(stats));
        eosio_assert(stat_itr != _stats.end(), "stats not init");
        // 添加到prov_orders表中
        
        // existing_provorders.modify(provorder_itr, prov, [&]( auto& o ) {
        //     // 修改结果
        //     o.result = result;
        //     o.reward = stat_itr->price;
        //     o.last_update_time = now();
        //     // todo
        // });
        existing_provorders.erase(provorder_itr);       
        dmd_orders.erase(odr_itr);

        // 转账
        eosio::action(
                    eosio::permission_level{ _self, N(active) },
                    N(eosio.token), N(transfer),
                    std::make_tuple(_self, prov, provorder_itr->reward, std::string("transfer reward"))
                ).send();
        
    }
    
   

}



