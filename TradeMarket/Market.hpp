#ifndef TRADE_MARKET_INC
#define TRADE_MARKET_INC 100
#
#include<vector>
#include<algorithm>
#include<numeric>
#include"Utility.hpp"
namespace trade {
	using market_id = unsigned int;

	struct merchant_interface {
		virtual void operator()(item_id ID, amount_t Amount_) = 0;
	};

	struct order {
	private:
		item_id ID;
		amount_t Amount;
		amount_t Price;
		merchant_interface* Merchant;
		merchant_interface* Payment;
	public:
		order(item_id ID_, amount_t Amount_, amount_t Price_, merchant_interface& Merchant_, merchant_interface& Payment_) 
		: ID(ID_), Amount(Amount_), Price(Price_), Merchant(&Merchant_), Payment(&Payment_){
		}
		order(const order&) = default;
		order& operator=(const order&) = default;
		order(order&&) = default;
		order& operator=(order&&) = default;
	public:
		item_id id()const { return ID; }
		amount_t amount()const { return Amount; }
		amount_t price()const { return Price; }
		void operator()(bool IsSell, item_id CurrencyID, amount_t Amount_) {
			if (IsSell) {
				(*Merchant)(ID, -Amount_);
				(*Payment)(CurrencyID, Price);
			} else {
				(*Merchant)(ID, Amount_);
				(*Payment)(CurrencyID, -Price);
			}
		}
		void operator()(bool IsSell, item_id CurrencyID) {
			operator()(IsSell, CurrencyID, Amount);
		}
	public:
		struct sell_pred {
			bool operator()(const order& v1, const order& v2) {
				if (v1.ID != v2.ID) return v1.ID < v2.ID;
				return v1.Price < v2.Price;
			}
		};
		struct buy_pred {
			bool operator()(const order& v1, const order& v2) {
				if (v1.ID != v2.ID) return v1.ID < v2.ID;
				return v1.Price > v2.Price;
			}
		};
	};

	struct market_interface {
		virtual void sell(order Order_) = 0;
		virtual void buy(order Order_) = 0;
	};

	struct market_logger_interface {
		virtual void order_sell(const order& Order) = 0;
		virtual void order_buy(const order& Order) = 0;
		virtual void contract_sell(const order& Order, amount_t Amount) = 0;
		virtual void contract_buy(const order& Order, amount_t Amount) = 0;
	};
	struct market : public market_interface {
	private:
		using orders = std::vector<order>;
		using iterator = typename orders::iterator;
	private:
		item_id Currency;
		amount_t Profit;
	private:
		orders Sell;
		orders Buy;
	public:
		market(item_id Currency_) :Currency(Currency_){}
	public:
		void sell(order Order_)override {
			Sell.push_back(std::move(Order_));
		}
		void buy(order Order_)override {
			Buy.push_back(std::move(Order_));
		}
		template<typename market_manager_, typename logger_>
		void operator()(market_manager_&& Manager, logger_&& Logger) {
			if (Buy.empty() || Sell.empty())return;

			std::sort(Buy.begin(), Buy.end(),order::buy_pred());
			std::sort(Sell.begin(), Sell.end(), order::sell_pred());

			for (auto& val : Buy) { Logger.order_buy(val); }
			for (auto& val : Sell) { Logger.order_sell(val); }

			auto TargetBuyBeg = Buy.begin();
			auto TargetBuyEnd = std::upper_bound(TargetBuyBeg, Buy.end(), *TargetBuyBeg, [](const order& v1, const order& v2) ->bool {return v1.id() < v2.id(); });
			auto TargetSellBeg = Sell.begin();
			auto TargetSellEnd = std::upper_bound(TargetSellBeg, Sell.end(), *TargetSellBeg, [](const order& v1, const order& v2) ->bool {return v1.id() < v2.id(); });

			while (TargetBuyBeg != Buy.end() && TargetSellBeg != Sell.end()) {
				//Find Order Range of current ID
				if (TargetBuyBeg->id() < TargetSellBeg->id()) {
					TargetBuyBeg = TargetBuyEnd;
					if (TargetBuyBeg == Buy.end())break;
					TargetBuyEnd = std::upper_bound(TargetBuyBeg, Buy.end(), *TargetBuyBeg, [](const order& v1, const order& v2) ->bool {return v1.id() < v2.id(); });
					continue;
				}
				else if (TargetBuyBeg->id() > TargetSellBeg->id()) {
					TargetSellBeg = TargetSellEnd;
					if (TargetSellBeg == Sell.end())break;
					TargetSellEnd = std::upper_bound(TargetSellBeg, Sell.end(), *TargetSellBeg, [](const order& v1, const order& v2) ->bool {return v1.id() < v2.id(); });
					continue;
				}

				Profit += Manager(Currency, TargetBuyBeg, TargetBuyEnd, TargetSellBeg, TargetSellEnd, Logger);

				TargetBuyBeg = TargetBuyEnd;
				if (TargetBuyBeg == Buy.end())break;
				TargetBuyEnd = std::upper_bound(TargetBuyBeg, Buy.end(), *TargetBuyBeg, [](const order& v1, const order& v2) ->bool {return v1.id() < v2.id(); });

				TargetSellBeg = TargetSellEnd;
				if (TargetSellBeg == Sell.end())break;
				TargetSellEnd = std::upper_bound(TargetSellBeg, Sell.end(), *TargetSellBeg, [](const order& v1, const order& v2) ->bool {return v1.id() < v2.id(); });
			}
			Buy.clear();
			Sell.clear();
		}
	};
	struct amount_maximize_market_manager {
		template<typename iterator, typename logger>
		amount_t operator()(item_id Currency, iterator BuyBeg, iterator BuyEnd, iterator SellBeg, iterator SellEnd, logger&& Logger) {
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
				(*BuyBeg)(false, Currency);
				Logger.contract_buy(*BuyBeg, BuyBeg->amount());
			}
			if (BuyItr != BuyEnd && BuyItr->amount() != BuyRemain) {
				(*BuyItr)(false, Currency, BuyItr->amount() - BuyRemain);
				Logger.contract_buy(*BuyItr, BuyItr->amount() - BuyRemain);
			}
			for (; SellBeg != SellItr; ++SellBeg) {
				(*SellBeg)(true, Currency);
				Logger.contract_sell(*SellBeg, SellBeg->amount());
			}
			if (SellItr != SellEnd && SellItr->amount() != SellRemain) {
				(*SellItr)(true, Currency, SellItr->amount() - SellRemain);
				Logger.contract_sell(*SellItr, SellItr->amount() - SellRemain);
			}
			
			return Profit;
		}
	};
	struct qamount_maximize_market_manager {
		template<typename iterator, typename logger>
		amount_t operator()(item_id Currency, iterator BuyBeg, iterator BuyEnd, iterator SellBeg, iterator SellEnd, logger&& Logger) {
			//Amount Maximize Calculation
			amount_t BuyRemain = 0;
			amount_t SellRemain = 0;
			amount_t Profit = 0;

			amount_t LastBuyAmount = 0;
			amount_t LastSellAmount = 0;
			amount_t BuyAmount = 0;
			amount_t SellAmount = 0;

			iterator BBeg = BuyBeg;
			if (BBeg == BuyEnd)return Profit;
			iterator BEnd = std::upper_bound(BBeg,BuyEnd,*BBeg,order::buy_pred());
			iterator SBeg = SellBeg;
			if (SBeg == SellEnd)return Profit;
			iterator SEnd = std::upper_bound(SBeg, SellEnd, *SBeg, order::sell_pred());

			while (true) {
				BuyAmount = std::accumulate(BBeg, BEnd, 0, [](amount_t val, const order& v)->amount_t{return v.amount() + val; });
				amount_t BuyPrice = BBeg->price();
				SellAmount = std::accumulate(SBeg, SEnd, 0, [](amount_t val, const order& v)->amount_t {return v.amount() + val; });
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
						BEnd = std::upper_bound(BBeg, BuyEnd, *BBeg, order::buy_pred());
					}
				}
				if (SellRemain == 0) {
					SBeg = SEnd;
					if (SBeg!= SellEnd) {
						SEnd = std::upper_bound(SBeg, SellEnd, *SBeg, order::sell_pred());
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
				(*BuyBeg)(false, Currency);
				Logger.contract_buy(*BuyBeg, BuyBeg->amount());
			}
			if(BBeg != BuyEnd && BBeg!=BEnd){
				Profit += LastBuyAmount*BBeg->price();
				if (std::next(BBeg) == BEnd) {
					Profit += LastBuyAmount*BBeg->price();
					(*BBeg)(false, Currency, LastBuyAmount);
					Logger.contract_buy(*BBeg, LastBuyAmount);
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
						(*BBeg)(false, Currency, (*DItr));
						Logger.contract_buy(*BBeg, (*DItr));
					}
				}
			}

			for (; SellBeg != SBeg; ++SellBeg) {
				Profit -= SellBeg->amount()*SellBeg->price();
				(*SellBeg)(true, Currency);
				Logger.contract_sell(*SellBeg, SellBeg->amount());
			}
			if (SBeg != SEnd && SBeg != SellEnd){
				Profit -= LastSellAmount*SBeg->price();
				if (std::next(SBeg) == SEnd) {
					Profit += LastSellAmount*SBeg->price();
					(*SBeg)(true, Currency, LastSellAmount);
					Logger.contract_sell(*SBeg, LastSellAmount);
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
						(*SBeg)(true, Currency, (*DItr));
						Logger.contract_sell(*SBeg, (*DItr));
					}
				}
			}	

			return Profit;
		}
	};
}
#
#endif
