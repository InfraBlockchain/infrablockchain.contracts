#include <infra.system/infra.system.hpp>

#include <infrablockchain/transaction_fee.hpp>

namespace infra_system {

   using namespace eosio;
   using namespace infrablockchain;

   /**
    *  Set Transaction Fee For Action
    *  @brief set transaction fee for an action. the transaction fee for each code/action is determined by the 2/3+ block producers.
    *  if code == name(0), this sets a transaction fee for the built-in common actions (e.g. InfraBlockchain standard token actions) that every account has.
    *  if code == name(0) and action == name(0), this sets default transaction fee for actions that don't have explicit transaction fee setup.
    *
    *  @param code - account name of contract code
    *  @param action - action name
    *  @param value - transaction fee value
    *  @param feetype - transaction fee type (1: fixed_tx_fee_per_action_type)
    */
   void system_contract::settxfee( const name& code, const name& action, const int32_t value, const uint32_t feetype ) {
      set_trx_fee_for_action( code, action, value, feetype );
   }

   /**
    *  Unset Transaction Fee For Action
    *  @brief delete transaction fee entry for an action (to the unsset status)
    *
    *  @param code - account name of contract code
    *  @param action - action name
    */
   void system_contract::unsettxfee( const name& code, const name& action ) {
      unset_trx_fee_for_action( code, action );
   }

} //namespace infra_system
