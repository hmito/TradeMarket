#ifndef TRADE_MARKET_INC
#define TRADE_MARKET_INC 100
#
#include<utility>
#include<vector>
#include"Utility.hpp"
#include"Stock.hpp"
namespace trade {
	struct market_interface {
	public:
		struct order_content {
		private:
			item_id ID;
			amount_t Amount;
			item_id Currency;
			amount_t Price;
		public:
			//Positive Price: sell, Negative Price: buy
			order_content(item_id ID_, amount_t Amount_, item_id Currency_, amount_t Price_,bool IsSell_)
				: ID(ID_), Amount(Amount_), Currency(Currency_), Price(IsSell_?Price_:-Price_) {
			}
			order_content(const order_content&) = default;
			order_content& operator=(const order_content&) = default;
			order_content(order_content&&) = default;
			order_content& operator=(order_content&&) = default;
		public:
			item_id id()const { return ID; }
			amount_t amount()const { return Amount; }
			item_id currency()const { return Currency; }
			amount_t price()const { return Price>0?Price:-Price; }
			amount_t cmp_price()const { return Price; }
			bool is_sell()const { return Price > 0; }
			void deal(stock_interface& Recipient, stock_interface& Market) {
				if (is_sell()) {
					//Sell
					trade::deal(Recipient, Market, ID, amount());
					trade::deal(Market, Recipient, Currency, price()*amount());
				} else {
					//Buy
					trade::deal(Market, Recipient, ID, amount());
					trade::deal(Recipient, Market, Currency, price()*amount());
				}
				Amount = 0;
			}
		};
		struct recipient_interface {
			virtual bool operator()(order_content Content_, stock_interface& Market_) = 0;
		};
	protected:
		struct order_item {
		private:
			order_content Content;
			recipient_interface* pRecipient;
		public:
			order_item(order_content Content_, recipient_interface& Recipient_)
				: Content(Content_), pRecipient(&Recipient_) {
			}
			order_item(const order_item&) = default;
			order_item& operator=(const order_item&) = default;
			order_item(order_item&&) = default;
			order_item& operator=(order_item&&) = default;
		public:
			const order_content& content()const { return Content; }
			item_id id()const { return Content.id(); }
			amount_t amount()const { return Content.amount(); }
			item_id currency()const { return Content.currency(); }
			amount_t price()const { return Content.price(); }
			bool is_sell()const { return Content.is_sell(); }
			bool operator()(stock_interface& Market_) {
				return (*pRecipient)(Content,Market_);
			}
			bool operator()(stock_interface& Market_, amount_t Amount_) {
				return (*pRecipient)(order_content(Content.id(), Amount_, Content.currency(), Content.price(), Content.is_sell()), Market_);
			}
			friend bool operator<(const order_item& v1, const order_item& v2) {
				if (v1.id() != v2.id()) return v1.id()< v2.id();
				return v1.Content.cmp_price() < v2.Content.cmp_price();
			}
		};
	public:
		virtual item_id currency()const = 0;
		virtual bool order(order_content Content_, recipient_interface& Recipient_) = 0;
	};
	struct market : public market_interface {
	private:
		using order_item = typename market_interface::order_item;
		using orders = std::vector<order_item>;
		using iterator = typename orders::iterator;
	private:
		item_id Currency;
		fastadd_abolute_stock Stock;
	private:
		orders Sell;
		orders Buy;
	public:
		market(item_id Currency_) :Currency(Currency_) {}
	public:
		item_id currency()const override { return Currency; }
		bool order(order_content Content_, recipient_interface& Recipient_)override {
			if (Content_.amount() <= 0)return true;
			if(Content_.is_sell())Sell.emplace_back(Content_, Recipient_);
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
	struct amount_maximize_market_manager {
		template<typename iterator, typename logger>
		void operator()(item_id Currency, stock_interface& Stock, iterator BuyBeg, iterator BuyEnd, iterator SellBeg, iterator SellEnd, logger&& Logger) {
			//Amount Maximize Calculation
			amount_t BuyRemain = 0;
			amount_t SellRemain = 0;
			amount_t Profit = 0;

			iterator BuyItr = BuyBeg;
			iterator SellItr = SellBeg;

			while (BuyItr != BuyEnd && SellItr != SellEnd) {
				amount_t Amount = 0;

				if (BuyRemain>0) {
					if (SellItr->amount() < BuyRemain) {
						Amount = SellItr->amount();
						BuyRemain -= Amount;
						SellRemain = 0;
					} else {
						Amount = BuyRemain;
						BuyRemain = 0;
						SellRemain = SellItr->amount() - Amount;
					}
				} else if (SellRemain>0) {
					if (BuyItr->amount() < SellRemain) {
						Amount = BuyItr->amount();
						BuyRemain = 0;
						SellRemain -= Amount;
					} else {
						Amount = SellRemain;
						BuyRemain = BuyItr->amount() - Amount;
						SellRemain = 0;
					}
				} else {
					if (BuyItr->amount() < SellItr->amount()) {
						Amount = BuyItr->amount();
						BuyRemain = 0;
						SellRemain = SellItr->amount() - Amount;
					} else {
						Amount = SellItr->amount();
						BuyRemain = BuyItr->amount() - Amount;
						SellRemain = 0;
					}
				}

				if (Profit + (BuyItr->price() - SellItr->price())*Amount <= 0) {
					amount_t NewAmount = (Profit / (SellItr->price() - BuyItr->price()));
					BuyRemain += (Amount - NewAmount);
					SellRemain += (Amount - NewAmount);

					Profit += (BuyItr->price() - SellItr->price())*NewAmount;
					break;
				}

				Profit += (BuyItr->price() - SellItr->price())*Amount;

				if (BuyRemain == 0) { ++BuyItr; }
				if (SellRemain == 0) { ++SellItr; }
			}

			for (; BuyBeg != BuyItr; ++BuyBeg) {
				Logger.contract(BuyBeg->content(), BuyBeg->amount());
				(*BuyBeg)(Stock);
			}
			if (BuyItr != BuyEnd && BuyItr->amount() != BuyRemain) {
				Logger.contract(BuyItr->content(), BuyItr->amount() - BuyRemain);
				(*BuyItr)(Stock, BuyItr->amount() - BuyRemain);
			}
			for (; SellBeg != SellItr; ++SellBeg) {
				Logger.contract(SellBeg->content(), SellBeg->amount());
				(*SellBeg)(Stock);
			}
			if (SellItr != SellEnd && SellItr->amount() != SellRemain) {
				Logger.contract(SellItr->content(), SellItr->amount() - SellRemain);
				(*SellItr)(Stock, SellItr->amount() - SellRemain);
			}
		}
	};
	struct qamount_maximize_market_manager {
		template<typename iterator, typename logger>
		void operator()(item_id Currency, stock_interface& Stock, iterator BuyBeg, iterator BuyEnd, iterator SellBeg, iterator SellEnd, logger&& Logger) {
			using order_item = decltype(*BuyBeg);
			//Amount Maximize Calculation
			amount_t BuyRemain = 0;
			amount_t SellRemain = 0;
			amount_t Profit = 0;

			amount_t LastBuyAmount = 0;
			amount_t LastSellAmount = 0;
			amount_t BuyAmount = 0;
			amount_t SellAmount = 0;

			iterator BBeg = BuyBeg;
			if (BBeg == BuyEnd)return;
			iterator BEnd = std::upper_bound(BBeg, BuyEnd, *BBeg);
			iterator SBeg = SellBeg;
			if (SBeg == SellEnd)return;
			iterator SEnd = std::upper_bound(SBeg, SellEnd, *SBeg);

			while (true) {
				BuyAmount = std::accumulate(BBeg, BEnd, 0, [](amount_t val, const order_item& v)->amount_t {return v.amount() + val; });
				amount_t BuyPrice = BBeg->price();
				SellAmount = std::accumulate(SBeg, SEnd, 0, [](amount_t val, const order_item& v)->amount_t {return v.amount() + val; });
				amount_t SellPrice = SBeg->price();


				amount_t Amount = 0;

				if (BuyRemain>0) {
					if (SellAmount < BuyRemain) {
						Amount = SellAmount;
						BuyRemain -= Amount;
						SellRemain = 0;
					} else {
						Amount = BuyRemain;
						BuyRemain = 0;
						SellRemain = SellAmount - Amount;
					}
				} else if (SellRemain>0) {
					if (BuyAmount < SellRemain) {
						Amount = BuyAmount;
						BuyRemain = 0;
						SellRemain -= Amount;
					} else {
						Amount = SellRemain;
						BuyRemain = BuyAmount - Amount;
						SellRemain = 0;
					}
				} else {
					if (BuyAmount < SellAmount) {
						Amount = BuyAmount;
						BuyRemain = 0;
						SellRemain = SellAmount - Amount;
					} else {
						Amount = SellAmount;
						BuyRemain = BuyAmount - Amount;
						SellRemain = 0;
					}
				}

				if (Profit + (BuyPrice - SellPrice)*Amount <= 0) {
					amount_t NewAmount = (Profit / (SellPrice - BuyPrice));
					BuyRemain += (Amount - NewAmount);
					SellRemain += (Amount - NewAmount);

					LastBuyAmount = BuyAmount - BuyRemain;
					LastSellAmount = SellAmount - SellRemain;
					break;
				}

				Profit += (BuyPrice - SellPrice)*Amount;

				if (BuyRemain == 0) {
					BBeg = BEnd;
					if (BBeg != BuyEnd) {
						BEnd = std::upper_bound(BBeg, BuyEnd, *BBeg);
					}
				}
				if (SellRemain == 0) {
					SBeg = SEnd;
					if (SBeg != SellEnd) {
						SEnd = std::upper_bound(SBeg, SellEnd, *SBeg);
					}
				}

				if (BBeg == BuyEnd || SBeg == SellEnd) {
					LastBuyAmount = BuyAmount - BuyRemain;
					LastSellAmount = SellAmount - SellRemain;
					break;
				}
			}

			Profit = 0;
			for (; BuyBeg != BBeg; ++BuyBeg) {
				Profit += BuyBeg->amount()*BuyBeg->price();
				Logger.contract(BuyBeg->content(), BuyBeg->amount());
				(*BuyBeg)(Stock);
			}
			if (BBeg != BuyEnd && BBeg != BEnd) {
				Profit += LastBuyAmount*BBeg->price();
				if (std::next(BBeg) == BEnd) {
					Profit += LastBuyAmount*BBeg->price();
					Logger.contract(BBeg->content(), LastBuyAmount);
					(*BBeg)(Stock, LastBuyAmount);
				} else {
					std::vector<unsigned int> Div(std::distance(BBeg, BEnd), 0);
					int Cnt = 0;
					auto DItr = Div.begin();

					for (auto BItr = BBeg; BItr != BEnd; ++BItr, ++DItr) {
						*DItr = static_cast<amount_t>(BItr->amount()*LastBuyAmount / BuyAmount);
						Cnt += *DItr;
					}

					for (; Cnt < LastBuyAmount; ++Cnt) {
						auto DMaxItr = Div.begin();
						double Val = BBeg->amount() / (*DMaxItr + 1.0);
						DItr = std::next(Div.begin());
						for (auto BItr = std::next(BBeg); BItr != BEnd; ++BItr, ++DItr) {
							if (Val < BItr->amount() / (*DItr + 1.0)) {
								DMaxItr = DItr;
								Val = BItr->amount() / (*DMaxItr + 1.0);
							}
						}
						++(*DMaxItr);
					}

					DItr = Div.begin();
					for (; BBeg != BEnd; ++BBeg, ++DItr) {
						if (*DItr == 0)continue;
						Logger.contract(BBeg->content(), (*DItr));
						(*BBeg)(Stock,*DItr);
					}
				}
			}

			for (; SellBeg != SBeg; ++SellBeg) {
				Profit -= SellBeg->amount()*SellBeg->price();
				Logger.contract(SellBeg->content(), SellBeg->amount());
				(*SellBeg)(Stock);
			}
			if (SBeg != SEnd && SBeg != SellEnd) {
				Profit -= LastSellAmount*SBeg->price();
				if (std::next(SBeg) == SEnd) {
					Profit += LastSellAmount*SBeg->price();
					Logger.contract(SBeg->content(), LastSellAmount);
					(*SBeg)(Stock, LastSellAmount);
				} else {
					std::vector<unsigned int> Div(std::distance(SBeg, SEnd), 0);
					int Cnt = 0;
					auto DItr = Div.begin();

					for (auto SItr = SBeg; SItr != SEnd; ++SItr, ++DItr) {
						*DItr = static_cast<amount_t>(SItr->amount()*LastSellAmount / SellAmount);
						Cnt += *DItr;
					}

					for (; Cnt < LastSellAmount; ++Cnt) {
						auto DMaxItr = Div.begin();
						double Val = SBeg->amount() / (*DMaxItr + 1.0);
						DItr = std::next(Div.begin());
						for (auto SItr = std::next(SBeg); SItr != SEnd; ++SItr, ++DItr) {
							if (Val < SItr->amount() / (*DItr + 1.0)) {
								DMaxItr = DItr;
								Val = SItr->amount() / (*DMaxItr + 1.0);
							}
						}
						++(*DMaxItr);
					}

					DItr = Div.begin();
					for (; SBeg != SEnd; ++SBeg, ++DItr) {
						if (*DItr == 0)continue;
						Logger.contract(SBeg->content(), (*DItr));
						(*SBeg)(Stock, (*DItr));
					}
				}
			}
		}
	};
}
#
#endif
