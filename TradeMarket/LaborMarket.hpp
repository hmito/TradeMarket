#ifndef TRADE_LABORMARKET_INC
#define TRADE_LABORMARKET_INC 100
#
#include"Utility.hpp"
#include"Market.hpp"
namespace trade {
	struct labor_market :public market_interface{
	private:
		item_id Currency;
		fastadd_abolute_stock Stock;
	public:
		item_id currency()const override { return Currency; }
		bool order(order_content Content_, recipient_interface& Recipient_)override {
			if (Content_.amount() <= 0)return true;
			if (Content_.is_sell())Sell.emplace_back(Content_, Recipient_);
			else Buy.emplace_back(Content_, Recipient_);
			return false;
		}
		template<typename market_manager_, typename logger_>
		void operator()(market_manager_&& Manager, logger_&& Logger) {
			if (Buy.empty() || Sell.empty())return;

			std::sort(Buy.begin(), Buy.end());
			std::sort(Sell.begin(), Sell.end());

			for (const auto& val : Sell) { Logger.order(val.content()); }
			for (const auto& val : Buy) { Logger.order(val.content()); }

			auto TargetBuyBeg = Buy.begin();
			auto TargetBuyEnd = std::upper_bound(TargetBuyBeg, Buy.end(), *TargetBuyBeg, [](const order_item& v1, const order_item& v2) ->bool {return v1.id() < v2.id(); });
			auto TargetSellBeg = Sell.begin();
			auto TargetSellEnd = std::upper_bound(TargetSellBeg, Sell.end(), *TargetSellBeg, [](const order_item& v1, const order_item& v2) ->bool {return v1.id() < v2.id(); });

			while (TargetBuyBeg != Buy.end() && TargetSellBeg != Sell.end()) {
				//Find Order Range of current ID
				if (TargetBuyBeg->id() < TargetSellBeg->id()) {
					TargetBuyBeg = TargetBuyEnd;
					if (TargetBuyBeg == Buy.end())break;
					TargetBuyEnd = std::upper_bound(TargetBuyBeg, Buy.end(), *TargetBuyBeg, [](const order_item& v1, const order_item& v2) ->bool {return v1.id() < v2.id(); });
					continue;
				} else if (TargetBuyBeg->id() > TargetSellBeg->id()) {
					TargetSellBeg = TargetSellEnd;
					if (TargetSellBeg == Sell.end())break;
					TargetSellEnd = std::upper_bound(TargetSellBeg, Sell.end(), *TargetSellBeg, [](const order_item& v1, const order_item& v2) ->bool {return v1.id() < v2.id(); });
					continue;
				}

				Manager(Currency, Stock, TargetBuyBeg, TargetBuyEnd, TargetSellBeg, TargetSellEnd, Logger);

				TargetBuyBeg = TargetBuyEnd;
				if (TargetBuyBeg == Buy.end())break;
				TargetBuyEnd = std::upper_bound(TargetBuyBeg, Buy.end(), *TargetBuyBeg, [](const order_item& v1, const order_item& v2) ->bool {return v1.id() < v2.id(); });

				TargetSellBeg = TargetSellEnd;
				if (TargetSellBeg == Sell.end())break;
				TargetSellEnd = std::upper_bound(TargetSellBeg, Sell.end(), *TargetSellBeg, [](const order_item& v1, const order_item& v2) ->bool {return v1.id() < v2.id(); });
			}
			Buy.clear();
			Sell.clear();

			Stock.sort();
		}

	};
}
#
#endif
