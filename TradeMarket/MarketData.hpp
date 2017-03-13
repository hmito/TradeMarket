#ifndef TRADE_MARKETDATA_INC
#define TRADE_MARKETDATA_INC 100
#
#include<unordered_map>
#include"Market.hpp"
namespace trade {
	struct market_data {
		using this_type = market_data;
	public:
		struct data_set {
		private:
			amount_t TotalBuy;
			amount_t TotalSell;
			amount_t ContractAmount;
			amount_t MinBuyPrice;
			amount_t MaxBuyPrice;
			amount_t TotalBuyPrice;
			amount_t MinSellPrice;
			amount_t MaxSellPrice;
			amount_t TotalSellPrice;
		public:
			data_set(){ clear(); }
			void clear() {
				TotalBuy = 0;
				TotalSell = 0;
				ContractAmount = 0;
				MinBuyPrice = std::numeric_limits<amount_t>::max();
				MaxBuyPrice = 0;
				MinSellPrice = std::numeric_limits<amount_t>::max();
				MaxSellPrice = 0;
				TotalSellPrice = 0;
				TotalBuyPrice = 0;
			}
			void add_sell_order(amount_t Amount, amount_t Price) {
				TotalSell += Amount;
			}
			void add_buy_order(amount_t Amount, amount_t Price) {
				TotalBuy += Amount;
			}
			void add_sell_contract(amount_t Amount, amount_t Price) {
				ContractAmount += Amount;
				MinSellPrice = std::min(MinSellPrice, Price);
				MaxSellPrice = std::max(MaxSellPrice, Price);
				TotalSellPrice += Amount*Price;
			}
			void add_buy_contract(amount_t Amount, amount_t Price) {
				MinBuyPrice = std::min(MinBuyPrice, Price);
				MaxBuyPrice = std::max(MaxBuyPrice, Price);
				TotalBuyPrice += Amount*Price;
			}
		public:
			amount_t buy_amount() const { return TotalBuy; }
			amount_t sell_amount() const { return TotalSell; }
			amount_t contract_amount() const { return ContractAmount; }
			amount_t buy_min_price() const { return MinBuyPrice; }
			amount_t buy_max_price() const { return MaxBuyPrice; }
			amount_t sell_min_price() const { return MinSellPrice; }
			amount_t sell_max_price() const { return MaxSellPrice; }
			double sell_price()const {return static_cast<double>(TotalSellPrice)/ ContractAmount;}
			double buy_price()const { return static_cast<double>(TotalBuyPrice) / ContractAmount; }
		};
	private:
		std::unordered_map<item_id, data_set> DataSet;
	public:
		struct logger : public market_logger_interface {
		private:
			this_type* This;
		public:
			logger(this_type& This_):This(&This_){}
			void initialize() {
				This->DataSet.clear();
			}
		public:
			void order_sell(const order& Order)override {
				This->DataSet[Order.id()].add_sell_order(Order.amount(), Order.price());
			}
			void order_buy(const order& Order)override {
				This->DataSet[Order.id()].add_buy_order(Order.amount(), Order.price());
			}
			void contract_sell(const order& Order, amount_t Amount)override {
				This->DataSet[Order.id()].add_sell_contract(Amount, Order.price());
			}
			void contract_buy(const order& Order, amount_t Amount) override {
				This->DataSet[Order.id()].add_buy_contract(Amount, Order.price());
			}
		};
	public:
		data_set get(item_id ItemID) {
			return DataSet[ItemID];
		}
	};
}
#
#endif
