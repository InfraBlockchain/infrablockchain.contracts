#include <sys.tokenabi/sys.tokenabi.hpp>

namespace infrablockchain { namespace contracts {

   using namespace eosio;
   using std::string;

   void tokenabi::settokenmeta( const symbol& /*sym*/, const string& /*url*/, const string& /*desc*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, settokenmeta" );
   }

   void tokenabi::issue( const name& /*t*/, const name& /*to*/, const asset& /*qty*/, const string& /*tag*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, issue" );
   }

   void tokenabi::transfer( const name& /*t*/, const name& /*from*/, const name& /*to*/, const asset& /*qty*/, const string& /*tag*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, transfer" );
   }

   void tokenabi::txfee( const name& /*t*/, const name& /*payer*/, const asset& /*fee*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, txfee" );
   }

   void tokenabi::redeem( const asset& /*qty*/, const string& /*tag*/ ) {
      check( false, "InfraBlockchain core provides built-in standard token operation, redeem" );
   }

} } /// namespace infrablockchain::contracts
