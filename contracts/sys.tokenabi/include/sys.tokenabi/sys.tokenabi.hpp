#pragma once

#include <eosio/asset.hpp>
#include <eosio/eosio.hpp>
#include <eosio/symbol.hpp>

#include <string>

namespace infrablockchain { namespace contracts {

   using namespace eosio;
   using std::string;

   /**
    * Every account on InfraBlockchain can process built-in standard token actions (settokenmeta, issue, transfer, redeem)
    * without custom smart contract code deployed to an account.
    * This system contract (sys.tokenabi) provide standard application binary interface(abi) for standard token actions.
    * Client tools such as infra-cli encoding InfraBlockchain transactions could refer 'sys.tokenabi' contract account
    * to retrieve abi information for standard token actions.
    */
   class [[eosio::contract("sys.tokenabi")]] tokenabi : public contract {
   public:
      using contract::contract;

      /**
       * Set token meta info
       * @param sym - token symbol (precision, symbol name)
       * @param url - web site url providing token information managed by token issuer
       * @param desc - token description
       */
      [[eosio::action]]
      void settokenmeta( const symbol& sym, const string& url, const string& desc );

      /**
       * Issue new token
       * @param to - account name receiving the issued tokens
       * @param qty - token quantity (amount, symbol) to issue
       * @param memo - user memo string to identity a specific issue action (application-specific purpose)
       */
      [[eosio::action]]
      void issue( const name& to, const asset& quantity, const string& memo );

      /**
       * Transfer token
       * @param from - account name sending tokens
       * @param to - account name receiving tokens
       * @param qty - token quantity (amount, symbol) to transfer
       * @param memo - user memo string to identity a specific transfer action (application-specific purpose)
       */
      [[eosio::action]]
      void transfer( const name& from, const name& to, const asset& quantity, const string& memo );

      /**
       * Transaction fee payment
       * if current token account is selected as a system token,
       * 'txfee' actions are generated from InfraBlockchain core after processing actions on a submitted transaction
       * @param payer - account name paying transaction fee
       * @param fee - token quantity (amount, symbol) being charged as transaction fee
       */
      [[eosio::action]]
      void txfee( const name& payer, const asset& fee );

      /**
       * Retire(burn) token
       * only token issuer can retire(burn) its own token
       * @param qty token quantity (amount, symbol) to redeem
       * @param memo user memo string to identity a specific redeem action (application-specific purpose)
       */
      [[eosio::action]]
      void retire( const asset& quantity, const string& memo );
   };

} } /// namespace infrablockchain::contracts
