#ifndef TRADE_MERCHANT_INC
#define TRADE_MERCHANT_INC 100
#
#include"Market.hpp"
namespace trade {
	struct city_officer_interface {
		using order_content = typename market_interface::order_content;
		payment_claim operator()(const order_content& Order_);
	};
	struct market_officer_interface {
		using order_content = typename market_interface::order_content;
		payment_claim operator()(const order_content& Order_);
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
			bool operator()(order_content Content_, stock_interface& Market_)override {
				auto CityTax = CityOfficer(Content_);
				auto MarketTax = MarketOfficer(Content_);
				CityTax.deal(Stock);
				MarketTax.deal(Stock);
				Content_.deal(Stock, Market_);
			}
		};
	private:
	public:
		void buy();
		void sell();
	};
}
#
#endif
