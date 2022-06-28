#include <sys.tokenabi/sys.tokenabi.hpp>

namespace infrablockchain { namespace contracts {

   using namespace eosio;
   using std::string;

   void tokenabi::settokenmeta( const symbol& /*sym*/, const string& /*url*/, const string& /*desc*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, settokenmeta" );
   }

   void tokenabi::issue( const name& /*to*/, const asset& /*quantity*/, const string& /*memo*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, issue" );
   }

   void tokenabi::transfer( const name& /*from*/, const name& /*to*/, const asset& /*quantity*/, const string& /*memo*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, transfer" );
   }

   void tokenabi::txfee( const name& /*payer*/, const asset& /*fee*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, txfee" );
   }

   void tokenabi::retire( const asset& /*quantity*/, const string& /*memo*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, redeem" );
   }

} } /// namespace infrablockchain::contracts
