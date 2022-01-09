#include <infra.system/native.hpp>

#include <eosio/check.hpp>

namespace infra_system {

   void native::onerror( ignore<uint128_t>, ignore<std::vector<char>> ) {
      eosio::check( false, "the onerror action cannot be called directly" );
   }

}
