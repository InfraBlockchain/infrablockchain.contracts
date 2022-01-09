#include <infra.system/infra.system.hpp>

#include <eosio/system.hpp>
#include <infrablockchain/proof_of_transaction.hpp>

#include <cmath>

namespace infra_system {

   using namespace infrablockchain;
   using eosio::current_time_point;
   using eosio::microseconds;

   double weighted_vote( uint32_t vote ) {
      double weight = int64_t( current_time_point().sec_since_epoch() - vote_weight_timestamp_epoch ) / double( seconds_per_week * 52 );
      return double(vote) * std::pow( 2, weight );
   }

   void system_contract::onblock( ignore<block_header> ) {
      using namespace eosio;

      require_auth(get_self());

      block_timestamp timestamp;
      name producer;
      _ds >> timestamp >> producer;

      /**
       * At startup the initial producer may not be one that is registered / elected
       * and therefore there may be no producer object for them.
       */
      auto prod = _producers.find(producer.value);
      if ( prod != _producers.end() ) {
         _gstate.total_unpaid_blocks++;
         _producers.modify( prod, same_payer, [&](auto& p ) {
            p.unpaid_blocks++;
         });
      }

      /// only update block producers once every minute, block_timestamp is in half seconds
      if( timestamp.slot - _gstate.last_producer_schedule_update.slot > 120 ) {
         update_elected_producers( timestamp );
      }
   }



   void system_contract::update_elected_producers( const block_timestamp& block_time ) {
      _gstate.last_producer_schedule_update = block_time;

      std::vector< std::pair<eosio::producer_key,uint16_t> > top_producers;
      top_producers.reserve( max_elected_block_producer_count );

      uint32_t offset_rank = 0;
      std::vector<tx_vote_stat_for_account> tx_vote_receivers_fetched;
      get_top_transaction_vote_receivers(tx_vote_receivers_fetched, offset_rank, top_tx_vote_receiver_list_fetch_count);
      while (tx_vote_receivers_fetched.size() > 0) {
         for (auto tx_vote_stat_for_account : tx_vote_receivers_fetched) {
            auto producer_itr = _producers.find( tx_vote_stat_for_account.account.value );
            if (producer_itr != _producers.cend()) {
               if ( top_producers.size() < max_elected_block_producer_count && producer_itr->active() && producer_itr->is_trusted_seed ) {
                  top_producers.emplace_back(std::pair<eosio::producer_key, uint16_t>({{producer_itr->owner, producer_itr->producer_key}, producer_itr->location}));

                  if ( top_producers.size() == max_elected_block_producer_count ) {
                     break;
                  }
               }
            }
         }

         if ( top_producers.size() < max_elected_block_producer_count && tx_vote_receivers_fetched.size() == top_tx_vote_receiver_list_fetch_count ) {
            offset_rank += tx_vote_receivers_fetched.size();
            tx_vote_receivers_fetched.clear();
            get_top_transaction_vote_receivers(tx_vote_receivers_fetched, offset_rank, top_tx_vote_receiver_list_fetch_count);
         } else {
            // if the fetched item count is less than requested fetch count, there is no more transaction vote receiver list items on blockchain core,
            // and if max number of block producers are already selected as top producers, then there is no need to fetch additional transaction vote receiver list.
            tx_vote_receivers_fetched.clear();
         }
      }

      if ( top_producers.size() < _gstate.last_producer_schedule_size ) {
         return;
      }

//       if (top_producers.empty()) {
//          return;
//       }

      /// sort by producer name
      std::sort( top_producers.begin(), top_producers.end() );

      std::vector<eosio::producer_key> producers;

      producers.reserve(top_producers.size());
      for( const auto& item : top_producers )
         producers.push_back(item.first);

      auto set_prod_ver = set_proposed_producers( producers );
      if (set_prod_ver.has_value() && set_prod_ver.value() >= 0) {
         _gstate.last_producer_schedule_size = static_cast<decltype(_gstate.last_producer_schedule_size)>( top_producers.size() );
      }
   }

   void system_contract::regproducer( const name& producer, const eosio::public_key& producer_key, const std::string& url, uint16_t location ) {
      check( url.size() < 512, "url too long" );
      check( producer_key != eosio::public_key(), "public key should not be the default value" );
      require_auth( producer );

      auto prod = _producers.find( producer.value );

      if ( prod != _producers.end() ) {
         _producers.modify( prod, producer, [&]( producer_info& info ){
            info.producer_key = producer_key;
            info.is_active    = true;
            info.url          = url;
            info.location     = location;
         });
      } else {
         _producers.emplace( producer, [&]( producer_info& info ){
            info.owner           = producer;
            info.total_votes     = 0;
            info.total_votes_weight = 0;
            info.producer_key    = producer_key;
            info.is_active       = true;
            info.is_trusted_seed = false;
            info.url             = url;
            info.location        = location;
         });
      }
   }

   void system_contract::authproducer( const name& producer ) {
      require_auth( _self );

      auto prod = _producers.find( producer.value );

      check( prod != _producers.end(), "not found registered block producer" );

      _producers.modify( prod, same_payer, [&]( producer_info& info ){
         info.is_trusted_seed = true;
      });
   }

   void system_contract::unregprod( const name& producer ) {
      require_auth( producer );

      const auto& prod = _producers.get( producer.value, "producer not found" );

      _producers.modify( prod, same_payer, [&]( producer_info& info ){
         info.deactivate();
      });
   }

   void system_contract::rmvproducer( const name& producer ) {
      require_auth( get_self() );
      auto prod = _producers.find( producer.value );
      check( prod != _producers.end(), "producer not found" );
      _producers.modify( prod, same_payer, [&](producer_info& info) {
         info.deactivate();
         info.is_trusted_seed = false;
      });
   }

   using namespace eosio;
   using namespace infrablockchain;
   void system_contract::claimrewards( const name& owner ) {
      require_auth(owner);

      const auto& prod = _producers.get( owner.value );
      check( prod.active(), "producer does not have an active key" );
      check( prod.is_trusted_seed, "producer is not trusted seed producer" );

      check( false, "claimrewards not yet supported");
   }

} //namespace infra_system
