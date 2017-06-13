#ifndef TRADE_STOCK_INC
#define TRADE_STOCK_INC 100
#
#include<algorithm>
#include<vector>
#include<unordered_map>
#include<utility>
#include<numeric>
#include"Utility.hpp"
namespace trade {
	struct stock_interface {
		friend bool deal(stock_interface& From, stock_interface& To, item_id ID, amount_t Amount);
	protected:
		struct dealable {
			void operator()(stock_interface& To, item_id ID, amount_t Amount) {To.add(ID, Amount);}
		};
	protected:
		virtual bool add(item_id ID, amount_t Amount) = 0;
	public:
		virtual amount_t get(item_id ID)const = 0;
	public:
		const amount_t operator[](item_id ID)const { return get(ID); }
	public:
		struct manager {
		protected:
			bool add(item_id ID, amount_t Amount, stock_interface& Stock) {
				return Stock.add(ID, Amount);
			}
		};
	};
	inline bool deal(stock_interface& From, stock_interface& To, item_id ID, amount_t Amount) {
		if(From.add(ID, -Amount))return true;
		if (To.add(ID, Amount)) {
			From.add(ID, Amount);
			return true;
		}
		return false;
	}
	struct claim_interface {
		virtual bool deal(stock_interface& Target) = 0;
	};
	struct payment_claim:public claim_interface {
	private:
		stock_interface& Claimer;
		item_id ReqID;
		amount_t ReqAmount;
	public:
		payment_claim(stock_interface& Claimer_, item_id ReqID_, amount_t ReqAmount_)
			: Claimer(Claimer_)
			, ReqID(ReqID_)
			, ReqAmount(ReqAmount_) {
		}
	public:
		item_id id()const { return ReqID; }
		amount_t amount()const { return ReqAmount; }
		bool deal(stock_interface& Target)override {
			if (ReqAmount > 0) {
				return trade::deal(Target, Claimer, ReqID, ReqAmount);
			} else if(ReqAmount<0){
				return trade::deal(Claimer, Target, ReqID, -ReqAmount);
			}
		}
	};
}
#
#endif
