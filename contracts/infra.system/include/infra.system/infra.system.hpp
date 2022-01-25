#pragma once

#include <eosio/asset.hpp>
#include <eosio/binary_extension.hpp>
#include <eosio/privileged.hpp>
#include <eosio/producer_schedule.hpp>
#include <eosio/singleton.hpp>
#include <eosio/system.hpp>
#include <eosio/time.hpp>

#include <infra.system/native.hpp>

#include <string>

namespace infra_system {

   using eosio::asset;
   using eosio::block_timestamp;
   using eosio::check;
   using eosio::contract;
   using eosio::const_mem_fun;
   using eosio::datastream;
   using eosio::indexed_by;
   using eosio::name;
   using eosio::same_payer;
   using eosio::symbol;
   using eosio::symbol_code;
   using eosio::time_point;
   using eosio::time_point_sec;
   using eosio::unsigned_int;

   template<typename E, typename F>
   static inline auto has_field( F flags, E field )
   -> std::enable_if_t< std::is_integral_v<F> && std::is_unsigned_v<F> &&
                        std::is_enum_v<E> && std::is_same_v< F, std::underlying_type_t<E> >, bool>
   {
      return ( (flags & static_cast<F>(field)) != 0 );
   }

   template<typename E, typename F>
   static inline auto set_field( F flags, E field, bool value = true )
   -> std::enable_if_t< std::is_integral_v<F> && std::is_unsigned_v<F> &&
                        std::is_enum_v<E> && std::is_same_v< F, std::underlying_type_t<E> >, F >
   {
      if( value )
         return ( flags | static_cast<F>(field) );
      else
         return ( flags & ~static_cast<F>(field) );
   }

   static constexpr uint32_t seconds_per_year      = 52 * 7 * 24 * 3600;
   static constexpr uint32_t seconds_per_day       = 24 * 3600;
   static constexpr uint32_t seconds_per_hour      = 3600;
   static constexpr int64_t  useconds_per_year     = int64_t(seconds_per_year) * 1000'000ll;
   static constexpr int64_t  useconds_per_day      = int64_t(seconds_per_day) * 1000'000ll;
   static constexpr int64_t  useconds_per_hour     = int64_t(seconds_per_hour) * 1000'000ll;
   static constexpr uint32_t blocks_per_day        = 2 * seconds_per_day; // half seconds per day

   static constexpr const uint32_t seconds_per_week = 24 * 3600 * 7;
   static constexpr const int64_t  vote_weight_timestamp_epoch = 1609459200ll;  // 01/01/2021 @ 12:00am (UTC) (seconds)

   static constexpr const uint32_t top_tx_vote_receiver_list_fetch_count = 30;
   static constexpr const uint32_t min_elected_block_producer_count = 7;  // initial bp count, adjusted by system contract upgrade by bp consensus
   static constexpr const uint32_t max_elected_block_producer_count = 21; // max elected bp count, adjusted by system contract upgrade by bp consensus


   // Defines new global state parameters.
   struct [[eosio::table("global"), eosio::contract("infra.system")]] infrablockchain_global_state : eosio::blockchain_parameters {
      uint64_t free_ram()const { return max_ram_size - total_ram_bytes_reserved; }

      uint8_t              revision = 0; ///< used to track version updates in the future.
      uint64_t             max_ram_size = 64ll*1024 * 1024 * 1024;
      uint64_t             total_ram_bytes_reserved = 0;
      int64_t              total_ram_stake = 0;

      block_timestamp      last_producer_schedule_update;
      uint32_t             total_unpaid_blocks = 0; /// all blocks which have been produced but not paid
      uint16_t             last_producer_schedule_size = 0;
      uint64_t             reserved_1 = 0;
      uint64_t             reserved_2 = 0;

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE_DERIVED( infrablockchain_global_state, eosio::blockchain_parameters,
                                (revision)(max_ram_size)(total_ram_bytes_reserved)(total_ram_stake)
                                (last_producer_schedule_update)
                                (total_unpaid_blocks)(last_producer_schedule_size)(reserved_1)(reserved_2) )
   };

   /*
   inline eosio::block_signing_authority convert_to_block_signing_authority( const eosio::public_key& producer_key ) {
      return eosio::block_signing_authority_v0{ .threshold = 1, .keys = {{producer_key, 1}} };
   }*/

   // Defines `producer_info` structure to be stored in `producer_info` table, added after version 1.0
   struct [[eosio::table, eosio::contract("infra.system")]] producer_info {
      name                                                     owner;
      double                                                   total_votes = 0;
      double                                                   total_votes_weight = 0;
      eosio::public_key                                        producer_key; /// a packed public key object
      bool                                                     is_active = true;
      bool                                                     is_trusted_seed = false; // authorized flag for permissioned setup
      std::string                                              url;
      uint32_t                                                 unpaid_blocks = 0;
      time_point                                               last_claim_time;
      uint16_t                                                 location = 0;
      // eosio::binary_extension<eosio::block_signing_authority>  producer_authority;

      uint64_t primary_key()const { return owner.value;                             }
      double   by_votes()const    { return is_active ? -total_votes : total_votes;  }
      bool     active()const      { return is_active;                               }
      void     deactivate()       { producer_key = public_key(); /*producer_authority.reset();*/ is_active = false; }

      /*
      eosio::block_signing_authority get_producer_authority()const {
         if( producer_authority.has_value() ) {
            bool zero_threshold = std::visit( [](auto&& auth ) -> bool {
               return (auth.threshold == 0);
            }, *producer_authority );
            // zero_threshold could be true despite the validation done in regproducer2 because the v1.9.0 eosio.system
            // contract has a bug which may have modified the producer table such that the producer_authority field
            // contains a default constructed eosio::block_signing_authority (which has a 0 threshold and so is invalid).
            if( !zero_threshold ) return *producer_authority;
         }
         return convert_to_block_signing_authority( producer_key );
      }
       */

      // explicit serialization macro is not necessary, used here only to improve compilation time
      EOSLIB_SERIALIZE( producer_info, (owner)(total_votes)(total_votes_weight)(producer_key)(is_active)(is_trusted_seed)
         (url)(unpaid_blocks)(last_claim_time)(location) /*(producer_authority)*/ )
   };

   typedef eosio::multi_index< "producers"_n, producer_info,
                               indexed_by<"prototalvote"_n, const_mem_fun<producer_info, double, &producer_info::by_votes>  >
                             > producers_table;

   typedef eosio::singleton< "global"_n, infrablockchain_global_state >   global_state_singleton;

   /**
    * The `infra.system` smart contract is provided by Blockchain Labs as a sample system contract, and it defines the structures and actions needed for blockchain's core functionality.
    *
    * Just like in the `eosio.bios` sample contract implementation, there are a few actions which are not implemented at the contract level (`newaccount`, `updateauth`, `deleteauth`, `linkauth`, `unlinkauth`, `canceldelay`, `onerror`, `setabi`, `setcode`), they are just declared in the contract so they will show in the contract's ABI and users will be able to push those actions to the chain via the account holding the `infra.system` contract, but the implementation is at the InfraBlockchain core level. They are referred to as InfraBlockchain native actions.
    *
    */
   class [[eosio::contract("infra.system")]] system_contract : public native {

      private:
         producers_table          _producers;
         global_state_singleton   _global;
         infrablockchain_global_state     _gstate;

      public:
         static constexpr eosio::name active_permission{"active"_n};
         static constexpr eosio::name null_account{"infra.null"_n};

         system_contract( name s, name code, datastream<const char*> ds );
         ~system_contract();

         /**
          * On block action. This special action is triggered when a block is applied by the given producer
          * and cannot be generated from any other source. It is used to pay producers and calculate
          * missed blocks of other producers. Producer pay is deposited into the producer's stake
          * balance and can be withdrawn over time. If blocknum is the start of a new round this may
          * update the active producer config from the producer votes.
          *
          * @param header - the block header produced.
          */
         [[eosio::action]]
         void onblock( ignore<block_header> header );

         /**
          * The activate action, activates a protocol feature
          *
          * @param feature_digest - hash of the protocol feature to activate.
          */
         [[eosio::action]]
         void activate( const eosio::checksum256& feature_digest );

         // functions defined in voting.cpp

         /**
          * Register producer action, indicates that a particular account wishes to become a producer,
          * this action will create a `producer_config` and a `producer_info` object for `producer` scope
          * in producers tables.
          *
          * @param producer - account registering to be a producer candidate,
          * @param producer_key - the public key of the block producer, this is the key used by block producer to sign blocks,
          * @param url - the url of the block producer, normally the url of the block producer presentation website,
          * @param location - is the country code as defined in the ISO 3166, https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
          *
          * @pre Producer to register is an account
          * @pre Authority of producer to register
          */
         [[eosio::action]]
         void regproducer( const name& producer, const public_key& producer_key, const std::string& url, uint16_t location );

//         /**
//          * Register producer action, indicates that a particular account wishes to become a producer,
//          * this action will create a `producer_config` and a `producer_info` object for `producer` scope
//          * in producers tables.
//          *
//          * @param producer - account registering to be a producer candidate,
//          * @param producer_authority - the weighted threshold multisig block signing authority of the block producer used to sign blocks,
//          * @param url - the url of the block producer, normally the url of the block producer presentation website,
//          * @param location - is the country code as defined in the ISO 3166, https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
//          *
//          * @pre Producer to register is an account
//          * @pre Authority of producer to register
//          */
//         [[eosio::action]]
//         void regproducer2( const name& producer, const eosio::block_signing_authority& producer_authority, const std::string& url, uint16_t location );

         /**
          * Unregister producer action, deactivates the block producer with account name `producer`.
          *
          * Deactivate the block producer with account name `producer`.
          * @param producer - the block producer account to unregister.
          */
         [[eosio::action]]
         void unregprod( const name& producer );

         /**
          *
          * @param producer
          */
         [[eosio::action]]
         void authproducer( const name& producer );

         /**
          * Remove producer action, deactivates a producer by name, if not found asserts.
          * @param producer - the producer account to deactivate.
          */
         [[eosio::action]]
         void rmvproducer( const name& producer );

         /**
          * Set ram action sets the ram supply.
          * @param max_ram_size - the amount of ram supply to set.
          */
         [[eosio::action]]
         void setram( uint64_t max_ram_size );

         /**
          * Set the blockchain parameters. By tunning these parameters a degree of
          * customization can be achieved.
          * @param params - New blockchain parameters to set.
          */
         [[eosio::action]]
         void setparams( const eosio::blockchain_parameters& params );

         /**
          * Claim rewards action, claims block producing and vote rewards.
          * @param owner - producer account claiming per-block and per-vote rewards.
          */
         [[eosio::action]]
         void claimrewards( const name& owner );

         /**
          * Set privilege status for an account. Allows to set privilege status for an account (turn it on/off).
          * @param account - the account to set the privileged status for.
          * @param is_priv - 0 for false, > 0 for true.
          */
         [[eosio::action]]
         void setpriv( const name& account, uint8_t is_priv );

         /**
          * Update revision action, updates the current revision.
          * @param revision - it has to be incremented by 1 compared with current revision.
          *
          * @pre Current revision can not be higher than 254, and has to be smaller
          * than or equal 1 (“set upper bound to greatest revision supported in the code”).
          */
         [[eosio::action]]
         void updtrevision( uint8_t revision );

         // System Token Management (infrasys.system_token.cpp)

         /**
          *
          * @param token
          * @param weight
          */
         [[eosio::action]]
         void addsystoken( const name& token, const uint32_t weight );

         /**
          *
          * @param token
          */
         [[eosio::action]]
         void rmvsystoken( const name& token );


         // Transaction Fee Management (infrasys.transaction_fee.cpp)

         /**
          *
          * @param code
          * @param action
          * @param value
          * @param feetype
          */
         [[eosio::action]]
         void settxfee( const name& code, const name& action, const int32_t value, const uint32_t feetype );

         /**
          *
          * @param code
          * @param action
          */
         [[eosio::action]]
         void unsettxfee( const name& code, const name& action );


         using activate_action = eosio::action_wrapper<"activate"_n, &system_contract::activate>;
         using regproducer_action = eosio::action_wrapper<"regproducer"_n, &system_contract::regproducer>;
//         using regproducer2_action = eosio::action_wrapper<"regproducer2"_n, &system_contract::regproducer2>;
         using unregprod_action = eosio::action_wrapper<"unregprod"_n, &system_contract::unregprod>;
         using authproducer_action = eosio::action_wrapper<"authproducer"_n, &system_contract::authproducer>;
         using setram_action = eosio::action_wrapper<"setram"_n, &system_contract::setram>;
         using claimrewards_action = eosio::action_wrapper<"claimrewards"_n, &system_contract::claimrewards>;
         using rmvproducer_action = eosio::action_wrapper<"rmvproducer"_n, &system_contract::rmvproducer>;
         using setpriv_action = eosio::action_wrapper<"setpriv"_n, &system_contract::setpriv>;
         using setparams_action = eosio::action_wrapper<"setparams"_n, &system_contract::setparams>;
         using updtrevision_action = eosio::action_wrapper<"updtrevision"_n, &system_contract::updtrevision>;

         using addsystoken_action = eosio::action_wrapper<"addsystoken"_n, &system_contract::addsystoken>;
         using rmvsystoken_action = eosio::action_wrapper<"rmvsystoken"_n, &system_contract::rmvsystoken>;
         using settxfee_action = eosio::action_wrapper<"settxfee"_n, &system_contract::settxfee>;
         using unsettxfee_action = eosio::action_wrapper<"unsettxfee"_n, &system_contract::unsettxfee>;

      private:
         // Implementation details:

         static infrablockchain_global_state get_default_parameters();

         void update_elected_producers( const block_timestamp& block_time );

//         // defined in voting.cpp
//         void register_producer( const name& producer, const eosio::block_signing_authority& producer_authority, const std::string& url, uint16_t location );
//         void update_elected_producers( const block_timestamp& timestamp );
//         void update_votes( const name& voter, const name& proxy, const std::vector<name>& producers, bool voting );
//         void propagate_weight_change( const voter_info& voter );
//         double update_producer_votepay_share( const producers_table2::const_iterator& prod_itr,
//                                               const time_point& ct,
//                                               double shares_rate, bool reset_to_zero = false );
//         double update_total_votepay_share( const time_point& ct,
//                                            double additional_shares_delta = 0.0, double shares_rate_delta = 0.0 );

   };

}
