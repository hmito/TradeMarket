#include<iostream>
//#include"Company.hpp"
#include"Market.hpp"
#include"MarketData.hpp"
#include"Product.hpp"
#include"Merchant.hpp"
namespace trade {
	//	struct stock{};
	//	struct trader{};
}

namespace trade {
	struct null_market_logger {
		using content = typename market_interface::order_content;
	public:
		amount_t SellPrice=0;
		amount_t SellAmount = 0;
		amount_t BuyPrice = 0;
		amount_t BuyAmount = 0;
		void order( const content& Content_) {
			if (Content_.is_sell()) {
				std::cout << "#Sell";
			} else {
				std::cout << "#Buy";
			}
			std::cout << "\t" << Content_.id() << ":" << Content_.amount()
				<< "\t" << Content_.currency() << ":" << Content_.price()
				<< std::endl;
		}
		void contract(const content& Content_, amount_t Amount_) {
			if (Content_.is_sell()) {
				std::cout << "!Sell";
			} else {
				std::cout << "!Buy";
			}
			std::cout << "\t" << Content_.id() << ":" << Content_.amount()
				<< "\t" << Content_.currency() << ":" << Content_.price()
				<< "\t" <<Amount_<< std::endl;
		}
	};
	struct null_recipient : public market_interface::recipient_interface {
		absolute_stock Stock;
		bool operator()(trade_claim Claim_)override {
			std::cout<<"<<"<< Claim_.give_id() << ":" << Claim_.give_amount()
				<< "\t>>" << Claim_.take_id() << ":" << Claim_.take_amount()
				<< std::endl;
			Claim_(Stock);

			return false;
		}
	};
}

/*
Market‚É
Business‚ÍMarchant‚ðŽ‚Á‚Ä‚¢‚éB
Marchant‚ðMarket‚É—a‚¯‚éŒ`‚Å”­’B
Market‚ÍMarchant‚ð§Œä‚·‚é‚±‚Æ‚Å”­’ˆ—‚ðŠ®—¹‚·‚é
*/
int main(void) {
//	trade::null_market_logger Logger;
	trade::market_data Data;
	trade::market_data::logger Logger(Data);
	trade::qamount_maximize_market_manager Manager;
	trade::market Market1(1);
	trade::null_recipient M1,M2;

	Market1.order(trade::market_interface::order_content(100, 10, 1, 21, true), M1);
	Market1.order(trade::market_interface::order_content(100, 90, 1, 19, true), M1);
	Market1.order(trade::market_interface::order_content(100, 80, 1, 20, true), M1);
	Market1.order(trade::market_interface::order_content(100, 90, 1, 20, true), M1);
	Market1.order(trade::market_interface::order_content(100, 50, 1, 21, true), M1);
	Market1.order(trade::market_interface::order_content(100, 70, 1, 22, true), M1);
	Market1.order(trade::market_interface::order_content(100, 120, 1, 24, true), M1);
	Market1.order(trade::market_interface::order_content(100, 100, 1, 19, false), M2);
	Market1.order(trade::market_interface::order_content(100, 30, 1, 22, false), M2);
	Market1.order(trade::market_interface::order_content(100, 50, 1, 23, false), M2);
	Market1.order(trade::market_interface::order_content(100, 40, 1, 24, false), M2);
	Market1.order(trade::market_interface::order_content(100, 100, 1, 24, false), M2);
	Market1.order(trade::market_interface::order_content(100, 40, 1, 25, false), M2);

	Market1.order(trade::market_interface::order_content(101, 90, 1, 75, true), M2);
	Market1.order(trade::market_interface::order_content(101, 50, 1, 80, true), M2);
	Market1.order(trade::market_interface::order_content(101, 70, 1, 83, true), M2);
	Market1.order(trade::market_interface::order_content(101, 120, 1, 90, true), M2);
	Market1.order(trade::market_interface::order_content(101, 100, 1, 79, false), M1);
	Market1.order(trade::market_interface::order_content(101, 30, 1, 60, false), M1);
	Market1.order(trade::market_interface::order_content(101, 50, 1, 77, false), M1);

	Market1(Manager, Logger);

	system("pause");

	return 0;
}