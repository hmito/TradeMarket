#include<iostream>
#include"Market.hpp"
#include"MarketData.hpp"
namespace trade {
	struct null_market_logger :public market_logger_interface {
		amount_t SellPrice=0;
		amount_t SellAmount = 0;
		amount_t BuyPrice = 0;
		amount_t BuyAmount = 0;
		virtual void order_sell( const order& Order)override {}
		virtual void order_buy(const order& Order)override {}
		virtual void contract_sell(const order& Order, amount_t Amount)override {
			SellPrice += Order.price()*Amount;
			SellAmount += Amount;
			std::cout <<"Sell\t"<< Order.id() << "\t" << Amount << "\t" << Order.price() << "\t"<< SellAmount<< "\t" << SellPrice<<std::endl;
		}
		virtual void contract_buy(const order& Order, amount_t Amount)override {
			BuyPrice += Order.price()*Amount;
			BuyAmount += Amount;
			std::cout << "Buy\t" << Order.id() << "\t" << Amount << "\t" << Order.price() << "\t" << BuyAmount << "\t" << BuyPrice << std::endl;
		}
	};
	struct null_merchant: merchant_interface{
		void operator()(item_id ID, amount_t Amount_)override {
		}
	};
}
int main(void) {
	trade::market_data Data;
	trade::market_data::logger Logger(Data);
	trade::qamount_maximize_market_manager Manager;
	trade::market Market1(1);
	trade::null_merchant M;

	Market1.sell(trade::order(100, 10, 21,M,M));
	Market1.sell(trade::order(100, 60, 23, M, M));
	Market1.sell(trade::order(100, 60, 24, M, M));
	Market1.sell(trade::order(101, 80, 24, M, M));
	Market1.sell(trade::order(101, 70, 25, M, M));
	Market1.sell(trade::order(100, 70, 25, M, M));
	Market1.sell(trade::order(100, 5, 26, M, M));
	Market1.sell(trade::order(100, 10, 26, M, M));
	Market1.sell(trade::order(100, 15, 26, M, M));
	Market1.sell(trade::order(100, 20, 27, M, M));
	Market1.sell(trade::order(100, 40, 28, M, M));
	Market1.sell(trade::order(101, 70, 23, M, M));
	Market1.sell(trade::order(100, 50, 29, M, M));
	Market1.sell(trade::order(100, 10, 30, M, M));
	Market1.sell(trade::order(101, 90, 26, M, M));
	Market1.sell(trade::order(101, 20, 27, M, M));
	Market1.sell(trade::order(101, 20, 28, M, M));

	Market1.buy(trade::order(101, 30, 21, M, M));
	Market1.buy(trade::order(101, 60, 22, M, M));
	Market1.buy(trade::order(101, 70, 24, M, M));
	Market1.buy(trade::order(100, 40, 24, M, M));
	Market1.buy(trade::order(100, 40, 25, M, M));
	Market1.buy(trade::order(100, 5, 26, M, M));
	Market1.buy(trade::order(100, 5, 26, M, M));
	Market1.buy(trade::order(100, 4, 27, M, M));
	Market1.buy(trade::order(100, 2, 27, M, M));
	Market1.buy(trade::order(100, 10, 20, M, M));
	Market1.buy(trade::order(100, 30, 21, M, M));
	Market1.buy(trade::order(101, 50, 23, M, M));
	Market1.buy(trade::order(101, 40, 25, M, M));
	Market1.buy(trade::order(100, 1, 22, M, M));
	Market1.buy(trade::order(100, 2, 22, M, M));
	Market1.buy(trade::order(100, 4, 22, M, M));
	Market1.buy(trade::order(100, 43, 22, M, M));
	Market1.buy(trade::order(100, 50, 23, M, M));
	Market1.buy(trade::order(100, 4, 27, M, M));
	Market1.buy(trade::order(100, 10, 29, M, M));


	Market1(Manager, Logger);

	trade::item_id ID = 100;
	auto Info1 = Data.get(ID);
	std::cout << ID << " : " << Info1.sell_amount()<<" ("<<Info1.sell_min_price()<<" - "<<Info1.sell_price() << " - "<<Info1.sell_max_price() << ") >> "<< Info1.contract_amount() << " >> " << Info1.buy_amount() << " (" << Info1.buy_min_price() << " - " << Info1.buy_price() << " - " << Info1.buy_max_price() << ")"<<std::endl;
	ID = 101; 
	Info1 = Data.get(ID);
	std::cout << ID << " : " << Info1.sell_amount() << " (" << Info1.sell_min_price() << " - " << Info1.sell_price() << " - " << Info1.sell_max_price() << ") >> " << Info1.contract_amount() << " >> " << Info1.buy_amount() << " (" << Info1.buy_min_price() << " - " << Info1.buy_price() << " - " << Info1.buy_max_price() << ")" << std::endl;

	system("pause");

	return 0;
}