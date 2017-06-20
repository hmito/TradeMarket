#ifndef TRADE_LABORMARKET_INC
#define TRADE_LABORMARKET_INC 100
#
#include"Utility.hpp"
#include"Item.hpp"
#include"Claim.hpp"
#include"Market.hpp"
#include"Stocks.hpp"
namespace trade {
	struct labor_market_interface {
		struct job_seek_card {
		private:
			item_id ID;
			amount_t Amount;
			agent_id AgentID;
		public:
			//Positive Price: sell, Negative Price: buy
			job_seek_card(item_id ID_, amount_t Amount_, agent_id AgentID_)
				: ID(ID_), Amount(Amount_), AgentID(AgentID_) {
			}
			job_seek_card(const job_seek_card&) = default;
			job_seek_card& operator=(const job_seek_card&) = default;
			job_seek_card(job_seek_card&&) = default;
			job_seek_card& operator=(job_seek_card&&) = default;
		public:
			item_id id()const { return ID; }
			agent_id agent()const { return AgentID; }
			amount_t amount()const { return Amount; }
		};
		struct job_offer_card {
		private:
			item_id ID;
			amount_t Amount;
			item_id Currency;
			amount_t Price;
			agent_id AgentID;
		public:
			//Positive Price: sell, Negative Price: buy
			job_offer_card(item_id ID_, amount_t Amount_, amount_t Price_, agent_id AgentID_)
				: ID(ID_), Amount(Amount_), AgentID(AgentID_) {
			}
			job_offer_card(const job_offer_card&) = default;
			job_offer_card& operator=(const job_offer_card&) = default;
			job_offer_card(job_offer_card&&) = default;
			job_offer_card& operator=(job_offer_card&&) = default;
		public:
			item_id id()const { return ID; }
			agent_id agent()const { return AgentID; }
			amount_t amount()const { return Amount; }
			item_id currency()const { return Currency; }
			amount_t price()const { return Price; }
		};
		using recipient_interface = typename market_interface::recipient_interface;
	protected:
		struct job_seek_item {
		private:
			job_seek_card Card;
			recipient_interface* pRecipient;
		public:
			job_seek_item(job_seek_card Card_, recipient_interface& Recipient_)
				: Card(Card_), pRecipient(&Recipient_) {
			}
			job_seek_item(const job_seek_item&) = default;
			job_seek_item& operator=(const job_seek_item&) = default;
			job_seek_item(job_seek_item&&) = default;
			job_seek_item& operator=(job_seek_item&&) = default;
		public:
			const job_seek_card& card()const { return Card; }
			item_id id()const { return Card.id(); }
			amount_t amount()const { return Card.amount(); }
			agent_id agent()const { return Card.agent(); }
			friend bool operator<(const job_seek_item& v1, const job_seek_item& v2) {
				if (v1.id() != v2.id()) return v1.id()< v2.id();
				return v1.agent() < v2.agent();
			}
		};
		struct job_offer_item {
		private:
			job_offer_card Card;
			recipient_interface* pRecipient;
		public:
			job_offer_item(job_offer_card Card_, recipient_interface& Recipient_)
				: Card(Card_), pRecipient(&Recipient_) {
			}
			job_offer_item(const job_offer_item&) = default;
			job_offer_item& operator=(const job_offer_item&) = default;
			job_offer_item(job_offer_item&&) = default;
			job_offer_item& operator=(job_offer_item&&) = default;
		public:
			const job_offer_card& card()const { return Card; }
			item_id id()const { return Card.id(); }
			amount_t amount()const { return Card.amount(); }
			agent_id agent()const { return Card.agent(); }
			item_id currency()const { return Card.currency(); }
			amount_t price()const { return Card.price(); }
			friend bool operator<(const job_offer_item& v1, const job_offer_item& v2) {
				if (v1.id() != v2.id()) return v1.id()< v2.id();
				return v1.price() > v2.price();
			}
		};
	public:
		virtual item_id currency()const = 0;
		virtual bool order(job_seek_card Card_, recipient_interface& Recipient_) = 0;
		virtual bool order(job_offer_card Card_, recipient_interface& Recipient_) = 0;
	};
	struct labor_market: public labor_market_interface {
	private:
		using job_seek_item = typename labor_market_interface::job_seek_item;
		using seeks = std::vector<job_seek_item>;
		using siterator = typename seeks::iterator;
		using job_offer_item = typename labor_market_interface::job_offer_item;
		using offers = std::vector<job_offer_item>;
		using oiterator = typename offers::iterator;
	private:
		item_id Currency;
		fastadd_abolute_stock Stock;
	private:
		seeks Seeks;
		offers Offers;
	public:
		item_id currency()const override { return Currency; }
		bool order(job_seek_card Card_, recipient_interface& Recipient_)override {
			if (Card_.amount() <= 0)return true;
			Seeks.emplace_back(Card_, Recipient_);
			return false;
		}
		bool order(job_offer_card Card_, recipient_interface& Recipient_)override {
			if (Card_.amount() <= 0)return true;
			Offers.emplace_back(Card_, Recipient_);
			return false;
		}
		template<typename market_manager_, typename logger_>
		void operator()(market_manager_&& Manager, logger_&& Logger) {
			if (Seeks.empty() || Offers.empty())return;

			std::sort(Seeks.begin(), Seeks.end());
			std::sort(Offers.begin(), Offers.end());

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
