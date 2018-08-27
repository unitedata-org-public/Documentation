#include "batch_data_contract.hpp"


namespace unitedata {
    
    using eosio::print;

    ///@abi action
    void data_contract::setprvstatus(account_name name, uint8_t status) {
        require_auth(name);
        auto prov_itr = _providers.find(name);
        eosio_assert(prov_itr != _providers.end(), "invalid provider name");

        _providers.modify(prov_itr, 0, [&]( auto& p ) {
            p.status = status;
        });

    }
    
    /*
     退出合约
     */
    ///@abi action
    void data_contract::quitcontract(account_name name) {
        require_auth(name);
        auto prov_itr = _providers.find(name);
        if (prov_itr != _providers.end()) {
            // prov表中删除
            _providers.erase(prov_itr);
            // provsignings 中删除
            provsignings provsigns(_self, name);
            auto provsigns_itr = provsigns.begin();
            while (provsigns_itr != provsigns.end()) {
                provsigns_itr = provsigns.erase(provsigns_itr);
            }
            // dmdsignings 中删除
            auto dmd_itr = _demandsides.begin();
            while (dmd_itr != _demandsides.end()) {
                dmdsignings dmdsigns(_self, dmd_itr->name);
                auto dmd_sign_itr = dmdsigns.find(name);
                if (dmd_sign_itr != dmdsigns.end()) {
                    dmdsigns.erase(dmd_sign_itr);
                }
                ++dmd_itr;
            }
        }
        
        auto dmd_itr = _demandsides.find(name);
        if (dmd_itr != _demandsides.end()) {
            // dmd表中删除
            _demandsides.erase(dmd_itr);
            // dmdsignings 中删除
            dmdsignings dmdsigns(_self, name);
            auto dmdsigns_itr = dmdsigns.begin();
            while (dmdsigns_itr != dmdsigns.end()) {
                dmdsigns_itr = dmdsigns.erase(dmdsigns_itr);
            }
            // provsignings 中删除
            auto prov_itr = _providers.begin();
            while (prov_itr != _providers.end()) {
                provsignings provsigns(_self, prov_itr->name);
                auto prov_sign_itr = provsigns.find(name);
                if (prov_sign_itr != provsigns.end()) {
                    provsigns.erase(prov_sign_itr);
                }
                ++prov_itr;
            }
            
        }
        
    }
    
    // 添加提供方
    ///@abi action
    void data_contract::addprov(account_name name) {
        assert_not_stopped();
        
        require_auth(name);
        acntstats(N(acntstats)).assert_certified(name);

        auto prov_itr = _providers.find(name);
        eosio_assert(prov_itr == _providers.end() , "account already exist");
        _providers.emplace(name, [&]( auto& p ) {
            p.name = name;
            p.create_time = now();
            p.status = 0;
        });
        
        auto dmd_itr = _demandsides.begin();
        uint8_t signing_strategy = get_signing_strategy();
        // 默认主动签约
        while(dmd_itr != _demandsides.end()) {
            //            if (dmd_itr->name == name) {
            //                ++dmd_itr;
            //                continue;
            //            } else {
            _sign(name, dmd_itr->name, 1, name);     // as 1: prov; as 2: dmd。
            if (signing_strategy >= 1) {
                _sign(dmd_itr->name, name, 2, name);
            }
            ++dmd_itr;
            //            }
        }
        
    }
    
    
    ///@abi action
    void data_contract::adddmd(account_name name) {
        assert_not_stopped();
        // 添加需求方
        require_auth(name);
        acntstats(N(acntstats)).assert_certified(name);

        auto dmd_itr = _demandsides.find(name);
        eosio_assert(dmd_itr == _demandsides.end() , "account already exist");
        _demandsides.emplace(name, [&]( auto& d ) {
            d.name = name;
            d.finished_order_count = 0;
            d.canceled_order_count = 0;
            d.create_time = now();
        });
        
        uint8_t signing_strategy = get_signing_strategy();
        auto prov_itr = _providers.begin();
        // 默认主动签约
        while(prov_itr != _providers.end()) {
            //            if (prov_itr->name == name) {
            //                ++prov_itr;
            //                continue;
            //            } else {
            if (signing_strategy == 1) {
                _sign(name, prov_itr->name, 2, name);
            }
            _sign(prov_itr->name, name, 1, name);
            ++prov_itr;
            //            }
        }
    }
    
    ///@abi action
    void data_contract::quitprov(account_name name) {
        require_auth(name);
        
        auto prov_itr = _providers.find(name);
        eosio_assert(prov_itr != _providers.end() , "account doesn't exist");
        // prov表中删除
        _providers.erase(prov_itr);
        // provsignings 中删除
        provsignings provsigns(_self, name);
        auto provsigns_itr = provsigns.begin();
        while (provsigns_itr != provsigns.end()) {
            provsigns_itr = provsigns.erase(provsigns_itr);
        }
        // dmdsignings 中删除
        auto dmd_itr = _demandsides.begin();
        while (dmd_itr != _demandsides.end()) {
            dmdsignings dmdsigns(_self, dmd_itr->name);
            auto dmd_sign_itr = dmdsigns.find(name);
            if (dmd_sign_itr != dmdsigns.end()) {
                dmdsigns.erase(dmd_sign_itr);
            }
            ++dmd_itr;
        }
    }

    ///@abi action
    void data_contract::quitdmd(account_name name) {
        require_auth(name);
        
        auto dmd_itr = _demandsides.find(name);
        eosio_assert(dmd_itr != _demandsides.end() , "account doesn't exist");
        // dmd表中删除
        _demandsides.erase(dmd_itr);
        // dmdsignings 中删除
        dmdsignings dmdsigns(_self, name);
        auto dmdsigns_itr = dmdsigns.begin();
        while (dmdsigns_itr != dmdsigns.end()) {
            dmdsigns_itr = dmdsigns.erase(dmdsigns_itr);
        }
        // provsignings 中删除
        auto prov_itr = _providers.begin();
        while (prov_itr != _providers.end()) {
            provsignings provsigns(_self, prov_itr->name);
            auto prov_sign_itr = provsigns.find(name);
            if (prov_sign_itr != provsigns.end()) {
                provsigns.erase(prov_sign_itr);
            }
            ++prov_itr;
        }
    }
    
    ///@abi action
    void data_contract::signasprov(account_name prov, account_name dmd) {
        assert_not_stopped();
        //        eosio_assert(prov != dmd, "from is same as to");
        require_auth(prov);
        _sign(prov, dmd, 1, prov);
    }

    ///@abi action
    void data_contract::signasdmd(account_name dmd, account_name prov) {
        assert_not_stopped();
        require_auth(dmd);
        //        eosio_assert(prov != dmd, "from is same as to");
        _sign(dmd, prov, 2, dmd);
    }
    
    ///@abi action
    void data_contract::resignasprov(account_name prov, account_name dmd) {
        require_auth(prov);
        //        eosio_assert(prov != dmd, "from is same as to");
        _resign(prov, dmd, 1);
    }
    
    ///@abi action
    void data_contract::resignasdmd(account_name dmd, account_name prov) {
        
        require_auth(dmd);
        //        eosio_assert(prov != dmd, "from is same as to");
        _resign(dmd, prov, 2);
    }
    
    // 签约
    ///@abi action
    void data_contract::_sign(account_name from, account_name to, uint8_t as, account_name ram_payer) {
        
        eosio_assert(as == 1 || as == 2, "invalid as");
        if (as == 1) {    // prov -> dmd
            provsignings existing_prov_signings(_self, from);
            auto to_itr = existing_prov_signings.find(to);
            if (to_itr == existing_prov_signings.end()) {
                existing_prov_signings.emplace(ram_payer, [&]( auto& s ) {
                    //                    s.prov = from;
                    s.dmd = to;
                });
            }
        } else if (as == 2){  // dmd -> prov
            dmdsignings existing_dmd_signings(_self, from);
            auto to_itr = existing_dmd_signings.find(to);
            if (to_itr == existing_dmd_signings.end()) {
                existing_dmd_signings.emplace(ram_payer, [&]( auto& s ) {
                    //                    s.dmd = from;
                    s.prov = to;
                });
            }
        }
        
    }
    
    // 解约
    ///@abi action
    void data_contract::_resign(account_name from, account_name to, uint8_t as) {
        eosio_assert(as == 1 || as == 2, "invalid as");
        if (as == 1) {    // prov -> dmd
            provsignings existing_prov_signings(_self, from);
            auto to_itr = existing_prov_signings.find(to);
            if (to_itr != existing_prov_signings.end()) {
                existing_prov_signings.erase(to_itr);
            }
        } else if (as == 2){  // dmd -> prov
            dmdsignings existing_dmd_signings(_self, from);
            auto to_itr = existing_dmd_signings.find(to);
            if (to_itr != existing_dmd_signings.end()) {
                existing_dmd_signings.erase(to_itr);
            }
        }
        
    }

    // 是否签名了
    ///@abi action
    bool data_contract::_is_signed(account_name from, account_name to, uint8_t as) {
        
        eosio_assert(as == 1 || as == 2, "invalid as");
        if (as == 1) {    // prov -> dmd
            provsignings existing_prov_signings(_self, from);
            auto to_itr = existing_prov_signings.find(to);
            return to_itr != existing_prov_signings.end();
        } else {  // dmd -> prov
            dmdsignings existing_dmd_signings(_self, from);
            auto to_itr = existing_dmd_signings.find(to);
            return to_itr != existing_dmd_signings.end();
        }
        
    }
    
}



