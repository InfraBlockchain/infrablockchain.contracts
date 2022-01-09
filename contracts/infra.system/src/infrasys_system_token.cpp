#include <infra.system/infra.system.hpp>

#include <infrablockchain/standard_token.hpp>
#include <infrablockchain/system_token.hpp>

namespace infra_system {

   using namespace eosio;
   using namespace infrablockchain;

   void system_contract::addsystoken( const name& token, const uint32_t weight ) {

      // need 2/3+ block producer's signatures (infrasys@active permission required)
      require_auth( get_self() );

      check( get_token_symbol(token).is_valid(), "invalid token account" );

      std::vector<system_token> new_system_token_list;
      bool already_sys_token = false;

      std::vector<system_token> current_system_tokens;
      get_system_token_list( current_system_tokens );

      for ( const auto& sys_token : current_system_tokens ) {
         if ( sys_token.token_id == token ) {
            already_sys_token = true;
            new_system_token_list.push_back( system_token{token, weight} );
         } else {
            new_system_token_list.push_back( sys_token );
         }
      }

      if ( !already_sys_token ) {
         new_system_token_list.push_back( system_token{token, weight} );
      }

      set_system_token_list( new_system_token_list );
   }

   void system_contract::rmvsystoken( const name& token ) {

      // need 2/3+ block producer's signatures (infrasys@active permission required)
      require_auth( get_self() );

      check( get_token_symbol(token).is_valid(), "invalid token account" );

      std::vector<system_token> new_system_token_list;
      bool removed = false;

      std::vector<system_token> current_system_tokens;
      get_system_token_list( current_system_tokens );

      for ( const auto& sys_token : current_system_tokens ) {
         if ( sys_token.token_id == token ) {
            removed = true;
         } else {
            new_system_token_list.push_back( sys_token );
         }
      }

      check( removed, "token is not currently system token" );

      set_system_token_list( new_system_token_list );
   }

} //namespace infra_system
