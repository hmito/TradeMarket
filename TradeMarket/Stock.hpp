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
	protected:
		struct dealable {
			void operator()(stock_interface& To, item_id ID, amount_t Amount) {To.add(ID, Amount);}
		};
	protected:
		virtual bool add(item_id ID, amount_t Amount) = 0;
		virtual amount_t get(item_id ID)const = 0;
	public:
		const amount_t operator[](item_id ID)const { return get(ID); }
	public:
		friend bool deal(stock_interface& From, stock_interface& To, item_id ID, amount_t Amount) {
			if (From.add(ID, -Amount))return true;
			if (To.add(ID, Amount)) {
				From.add(ID, Amount);
				return true;
			}
			return false;
		}
	};

}
#
#endif
