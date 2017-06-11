#ifndef TRADE_OLDMARKET_INC
#define TRADE_OLDMARKET_INC 100
#
#include<vector>
#include<algorithm>
#include<numeric>
#include"Utility.hpp"
#include"Stock.hpp"
namespace trade {
	using market_id = unsigned int;
	using agent_id= unsigned int;


	struct order_logger_interface {
		virtual void order_sell(const order_content& Order) = 0;
		virtual void order_buy(const order_content& Order) = 0;
		virtual void contract_sell(const order_content& Order, amount_t Amount) = 0;
		virtual void contract_buy(const order_content& Order, amount_t Amount) = 0;
	};

	//council tax officer
	struct order_officer_interface {
		virtual item_id currency()const = 0;
		virtual amount_t estimate(city_id From, market_id To, agent_id By, const order& Order)const = 0;
		virtual void operator()(city_id From, market_id To, agent_id By, const order& Order, amount_t Amount, stock_interface& Stock) = 0;
	};
	struct council_officer {

	};
	struct market_officer {

	};
	struct marchant_interface {
		virtual void sell(const order_content& Order) = 0;
		virtual amount_t sell_estimate(const order_content& Order) = 0;
		virtual void buy(const order_content& Order) = 0;
	};
	//Proxy of order with tax management
	struct marchant : public marchant_interface, public stock_interface::manager {
		using my_type = marchant;
	private:
		struct order_recipient : public order_recipient_interface {
		private:
			my_type& Ref;
		public:
			void sell(order_content Content_, amount_t Result_) override {
				Ref.contract_sell(Content_, Result_);
			}
			void buy(order_content Content_, amount_t Result_) override {
				Ref.contract_buy(Content_, Result_);
			}
		}Recipient;
	private:
		council_officer& CouncilOfficer;
		market_officer& SellMarketOfficer;
		market_interface& SellMarket;
		market_interface& CurMarket;
		market_interface& BuyMarket;
		stock_interface& Stock;
	private:
		void contract_sell(const order_content& Order, amount_t Amount) {
			Logger.contract_sell(Order, Amount);
			if (Amount > 0) {
				stock_interface::manager::add(Order.id(), -Amount, Stock);
				stock_interface::manager::add(Order.currency(), Order.price()*Amount, Stock);
			}
		}
		void contract_buy(const order_content& Order, amount_t Amount) {
			Logger.contract_buy(Order, Amount);
			if (Amount > 0) {
				stock_interface::manager::add(Order.id(), Amount, Stock);
				stock_interface::manager::add(Order.currency(), -Order.price()*Amount, Stock);
			}
		}
	public:
		void sell(const order_content& Order) {
			Logger.order_sell(Order);
			SellMarket.sell(Order, Recipient);
		}
		void buy(const order_content& Order) {
			Logger.order_buy(Order);
			BuyMarket.buy(Order, Recipient);
		}
	};



}
#
#endif
