#include <infra.system/infra.system.hpp>

#include <eosio/crypto.hpp>
#include <eosio/dispatcher.hpp>

#include <cmath>

namespace infra_system {

   system_contract::system_contract( name s, name code, datastream<const char*> ds )
   :native(s,code,ds),
    _producers(get_self(), get_self().value),
    _global(get_self(), get_self().value)
   {
      _gstate  = _global.exists() ? _global.get() : get_default_parameters();
   }

   infrablockchain_global_state system_contract::get_default_parameters() {
      infrablockchain_global_state dp;
      get_blockchain_parameters(dp);
      return dp;
   }

   system_contract::~system_contract() {
      _global.set( _gstate, get_self() );
   }

   void system_contract::setram( uint64_t max_ram_size ) {
      require_auth( get_self() );

      check( _gstate.max_ram_size < max_ram_size, "ram may only be increased" ); /// decreasing ram might result market maker issues
      check( max_ram_size < 1024ll*1024*1024*1024*1024, "ram size is unrealistic" );
      check( max_ram_size > _gstate.total_ram_bytes_reserved, "attempt to set max below reserved" );

      _gstate.max_ram_size = max_ram_size;
   }


   void system_contract::setparams( const eosio::blockchain_parameters& params ) {
      require_auth( get_self() );
      (eosio::blockchain_parameters&)(_gstate) = params;
      check( 3 <= _gstate.max_authority_depth, "max_authority_depth should be at least 3" );
      set_blockchain_parameters( params );
   }

   void system_contract::setpriv( const name& account, uint8_t ispriv ) {
      require_auth( get_self() );
      set_privileged( account, ispriv );
   }

   void system_contract::activate( const eosio::checksum256& feature_digest ) {
      require_auth( get_self() );
      preactivate_feature( feature_digest );
   }

   void system_contract::updtrevision( uint8_t revision ) {
      require_auth( get_self() );
      check( _gstate.revision < 255, "can not increment revision" ); // prevent wrap around
      check( revision == _gstate.revision + 1, "can only increment revision by one" );
      check( revision <= 1, // set upper bound to greatest revision supported in the code
             "specified revision is not yet supported by the code" );
      _gstate.revision = revision;
   }

   /**
    *  Called after a new account is created. This code enforces resource-limits rules
    *  for new accounts as well as new account naming conventions.
    *
    *  Account names containing '.' symbols must have a suffix equal to the name of the creator.
    *  This allows users who own a premium name (shorter than 12 characters with no dots) to be the only ones
    *  who can create accounts with the creator's name as a suffix.
    *
    */
   void native::newaccount( const name&       creator,
                            const name&       newact,
                            ignore<authority> owner,
                            ignore<authority> active ) {

      if( creator != get_self() ) {
         uint64_t tmp = newact.value >> 4;
         bool has_dot = false;

         for( uint32_t i = 0; i < 12; ++i ) {
           has_dot |= !(tmp & 0x1f);
           tmp >>= 5;
         }
         if( has_dot ) { // or is less than 12 characters
            auto suffix = newact.suffix();
            if( suffix == newact ) {
               check( false, "only system account can create accounts having less than 12 characters" );
            } else {
               check( creator == suffix, "only suffix may create this account" );
            }
         }
      }

      set_resource_limits( newact, -1, -1, -1 );
   }

   void native::setabi( const name& acnt, const std::vector<char>& abi ) {
      eosio::multi_index< "abihash"_n, abi_hash >  table(get_self(), get_self().value);
      auto itr = table.find( acnt.value );
      if( itr == table.end() ) {
         table.emplace( acnt, [&]( auto& row ) {
            row.owner = acnt;
            row.hash = eosio::sha256(const_cast<char*>(abi.data()), abi.size());
         });
      } else {
         table.modify( itr, same_payer, [&]( auto& row ) {
            row.hash = eosio::sha256(const_cast<char*>(abi.data()), abi.size());
         });
      }
   }

} /// infra_system
