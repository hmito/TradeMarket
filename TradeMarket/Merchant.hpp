#ifndef TRADE_MERCHANT_INC
#define TRADE_MERCHANT_INC 100
#
#include"Utility.hpp"
#include"Market.hpp"
#include"Claim.hpp"
namespace trade {
	struct city_officer_interface {
		virtual payment_claim operator()(const trade_content& Trade_) = 0;
	};
	struct market_officer_interface {
		virtual payment_claim operator()(const trade_content& Trade_) = 0;
	};
	class merchant {
	private:
		using order_content = typename market_interface::order_content;
		struct market_recipient : public market_interface::recipient_interface {
		private:
			stock_interface& Stock;
			market_interface& Market;
			city_officer_interface& CityOfficer;
			market_officer_interface& MarketOfficer;
		public:
			bool operator()(trade_claim Claim_)override {
				auto CityTaxClaim = CityOfficer(Claim_.content());
				auto MarketTaxClaim= MarketOfficer(Claim_.content());
				Claim_(Stock);
				CityTaxClaim(Stock);
				MarketTaxClaim(Stock);
			}
		};
	public:
		void buy(item_id ID, amount_t Amount, amount_t Price);
		void sell(item_id ID, amount_t Amount, amount_t Price);
	};
}
#
#endif
