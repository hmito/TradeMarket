#ifndef TRADE_STOCKS_INC
#define TRADE_STOCKS_INC 100
#
#include<algorithm>
#include<vector>
#include<unordered_map>
#include"Stock.hpp"
namespace trade {
	struct stock :public stock_interface {
	public:
		using stock_item = std::pair<item_id, amount_t>;
		using container = std::unordered_map<item_id, amount_t>;
		using const_iterator = container::const_iterator;
	private:
		container Stock;
	protected:
		bool add(item_id ID, amount_t Amount) override {
			auto Itr = Stock.find(ID);

			if (Itr != Stock.end()) {
				if (Amount + Itr->second < 0)return true;
				Itr->second += Amount;
			} else {
				if (Amount < 0)return true;
				Stock.insert(stock_item(ID, Amount));
			}

			return false;
		}
	public:
		amount_t get(item_id ID)const override {
			auto Itr = Stock.find(ID);
			if (Itr == Stock.end())return 0;
			return Itr->second;
		}
	public:
		const_iterator begin()const { return Stock.begin(); }
		const_iterator end()const { return Stock.end(); }
		const_iterator cbegin()const { return Stock.cbegin(); }
		const_iterator cend()const { return Stock.cend(); }
	};
	struct absolute_stock :public stock_interface {
	public:
		using stock_item = std::pair<item_id, amount_t>;
		using container = std::unordered_map<item_id, amount_t>;
		using const_iterator = container::const_iterator;
	private:
		container Stock;
	protected:
		bool add(item_id ID, amount_t Amount) override {
			auto Itr = Stock.find(ID);

			if (Itr != Stock.end()) {
				Itr->second += Amount;
			} else {
				Stock.insert(stock_item(ID, Amount));
			}

			return false;
		}
	public:
		amount_t get(item_id ID)const override {
			auto Itr = Stock.find(ID);
			if (Itr == Stock.end())return 0;
			return Itr->second;
		}
	public:
		const_iterator begin()const { return Stock.begin(); }
		const_iterator end()const { return Stock.end(); }
		const_iterator cbegin()const { return Stock.cbegin(); }
		const_iterator cend()const { return Stock.cend(); }
	};
	struct fastadd_abolute_stock :public stock_interface {
	public:
		using stock_item = std::pair<item_id, amount_t>;
		using container = std::vector<stock_item>;
		using const_iterator = container::const_iterator;
	private:
		container Stock;
	protected:
		bool add(item_id ID, amount_t Amount) override {
			Stock.emplace_back(ID, Amount);
			return false;
		}
	public:
		amount_t get(item_id ID)const override {
			return std::accumulate(Stock.begin(), Stock.end(), 0, [=](amount_t Amount, const stock_item& Item)->amount_t {return Item.first == ID ? Item.second + Amount : Amount; });
		}
	public:
		const_iterator begin()const { return Stock.begin(); }
		const_iterator end()const { return Stock.end(); }
		const_iterator cbegin()const { return Stock.cbegin(); }
		const_iterator cend()const { return Stock.cend(); }
		void sort() {
			std::sort(Stock.begin(), Stock.end(), [](const stock_item& v1, const stock_item& v2)->bool {return v1.first < v2.first; });
			auto Out = Stock.begin();
			for (auto Lower = Stock.begin(), Upper = Stock.begin(); Lower != Stock.end(); Lower = Upper) {
				amount_t Sum = 0;
				while (Upper != Stock.end() && Lower->first == Upper->first) {
					Sum += Upper->second;
					++Upper;
				}

				(*Out++) = stock_item(Lower->first, Sum);
			}
			Stock.erase(Out, Stock.end());
		}
		void deal(stock_interface& To) {
			std::sort(Stock.begin(), Stock.end(), [](const stock_item& v1, const stock_item& v2)->bool {return v1.first < v2.first; });

			for (auto Lower = Stock.begin(), Upper = Stock.begin(); Lower != Stock.end(); Lower = Upper) {
				amount_t Sum = 0;
				while (Upper != Stock.end() && Lower->first == Upper->first) {
					Sum += Upper->second;
					++Upper;
				}

				dealable Deal;
				Deal(To, Lower->first, Sum);
			}

			Stock.clear();
		}
	};
}
#
#endif
