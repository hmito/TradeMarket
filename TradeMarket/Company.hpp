#ifndef TRADE_COMPANY_INC
#define TRADE_COMPANY_INC 100
#
#include<string>
#include<vector>
#include<memory>
#include"Product.hpp"
#include"Merchant.hpp"
namespace trade {

	struct market_info_interface {
		std::pair<int, int> minmax_price(item_id ID)const;
		double mean_price(item_id ID)const;
	};
	struct product_info_interface {
		virtual unsigned int size();
		virtual item_id item(unsigned int No);
		virtual unsigned int amount(unsigned int No);
		virtual item_id oitem();
		virtual unsigned int oamount();
		virtual unsigned int max_product(const product_input_t& Input);
	};
	struct info_interface {
		virtual market_info_interface& buy_market() = 0;
		virtual market_info_interface& sell_market() = 0;
		virtual product_info_interface& product(product_id ID) = 0;
		virtual double product_cost();
	};

	struct productAI{
		virtual void operator()(amount_t WorkerNum, product_stock& Stock, info_interface& Info, marchant_interface& Marchant) = 0;
	};
	struct productAI_Type1: public productAI {
	private:
		struct buyAI {
			double sa;
			double pa;
			double pb;
			double ra;
			double prop1;
			double rate1;
			double prop2;
			double rate2;
		public:
			void operator()(amount_t NeedAmount, amount_t Stock, item_id ID, double ProfitRate, market_info_interface& BuyMarketInfo, order_interface& Order) {
				double TStock = sa * NeedAmount;
				double StockRate = std::max(0.0, (TStock - Stock) / (TStock + 1.0));

				double Mean = BuyMarketInfo.mean_price(ID);
				double Max, Min;
				std::tie(Min, Max) = BuyMarketInfo.minmax_price(ID);

				//‘«‚è‚È‚¢Žž‚ÍStockRate‚Íã‚ª‚é ‰¿Ši‚Íã‚°‚é
				//–×‚¯‚Ä‚éŽž‚ÍProfitRate‚Íã‚ª‚é ‰¿Ši‚Íã‚°‚é
				double BuyRateB = pa*(ProfitRate - pb) + ra*StockRate;
				double BuyRate1 = BuyRateB + rate1;
				double BuyRate2 = BuyRateB + rate2;

				double BuyPriceB = (BuyRateB > 0 ? BuyRateB*(Max - Mean) + Mean : (-BuyRateB)*(Min - Mean) + Mean);
				double BuyPrice1 = (BuyRate1 > 0 ? BuyRate1*(Max - Mean) + Mean : (-BuyRate1)*(Min - Mean) + Mean);
				double BuyPrice2 = (BuyRate2 > 0 ? BuyRate2*(Max - Mean) + Mean : (-BuyRate2)*(Min - Mean) + Mean);

				double BuyAmount1 = prop1*(TStock - Stock);
				double BuyAmount2 = prop2*(TStock - Stock);
				double BuyAmountB = TStock - BuyAmount1 - BuyAmount2;

				Order.buy(ID, BuyAmountB, BuyPriceB);
				Order.buy(ID, BuyAmount1, BuyPrice1);
				Order.buy(ID, BuyAmount2, BuyPrice2);
			}
		}BuyAI;
		struct sellAI {
			double sa;
			double pa;
			double pb;
			double ra;
			double prop1;
			double rate1;
			double prop2;
			double rate2;
		public:
			void operator()(amount_t ExpectProduct, amount_t Stock, item_id ID, double ProfitRate, market_info_interface& SellMarketInfo, order_interface& Order) {
				double AStock = sa * ExpectProduct;	//Allowed amount of stack. Over this amount means "too much".
				double StockRate = std::max(0.0, (Stock - AStock) / (AStock + 1.0));

				double Mean = SellMarketInfo.mean_price(ID);
				double Max, Min;
				std::tie(Min, Max) = SellMarketInfo.minmax_price(ID);

				//—]‚Á‚Ä‚éŽž‚ÍStockRate‚Íã‚ª‚é ‰¿Ši‚Í‰º‚°‚é
				//–×‚¯‚Ä‚éŽž‚ÍProfitRate‚Íã‚ª‚é ‰¿Ši‚Í‰º‚°‚é
				double SellRateB = -pa*(ProfitRate - pb) - ra*StockRate;
				double SellRate1 = SellRateB + rate1;
				double SellRate2 = SellRateB + rate2;

				double SellPriceB = (SellRateB > 0 ? SellRateB*(Max - Mean) + Mean : (-SellRateB)*(Min - Mean) + Mean);
				double SellPrice1 = (SellRate1 > 0 ? SellRate1*(Max - Mean) + Mean : (-SellRate1)*(Min - Mean) + Mean);
				double SellPrice2 = (SellRate2 > 0 ? SellRate2*(Max - Mean) + Mean : (-SellRate2)*(Min - Mean) + Mean);

				double SellAmount1 = prop1*Stock;
				double SellAmount2 = prop2*Stock;
				double SellAmountB = Stock - SellAmount1 - SellAmount2;

				Order.sell(ID, SellAmountB, SellPriceB);
				Order.sell(ID, SellAmount1, SellPrice1);
				Order.sell(ID, SellAmount2, SellPrice2);
			}
		}SellAI;
		struct workAI {
			double sa;
			double pa;
			double pb;
			double ra;
			double prop1;
			double rate1;
			double prop2;
			double rate2;
		public:
			void operator()(amount_t WorkerNum, amount_t MaxWorkerNum, double ProfitRate, market_info_interface& BuyMarketInfo, order_interface& Order) {
				double TStock = sa * WorkerNum;
				double StockRate = std::max(0.0, (MaxWorkerNum - TStock) / (TStock + 1.0));

				double Mean = BuyMarketInfo.mean_price(ItemID_Worker);
				double Max, Min;
				std::tie(Min, Max) = BuyMarketInfo.minmax_price(ItemID_Worker);

				//‘«‚è‚È‚¢Žž‚ÍStockRate‚Íã‚ª‚é ‰¿Ši‚Íã‚°‚é
				//–×‚¯‚Ä‚éŽž‚ÍProfitRate‚Íã‚ª‚é ‰¿Ši‚Íã‚°‚é
				double BuyRateB = pa*(ProfitRate - pb) + ra*StockRate;
				double BuyRate1 = BuyRateB + rate1;
				double BuyRate2 = BuyRateB + rate2;

				double BuyPriceB = (BuyRateB > 0 ? BuyRateB*(Max - Mean) + Mean : (-BuyRateB)*(Min - Mean) + Mean);
				double BuyPrice1 = (BuyRate1 > 0 ? BuyRate1*(Max - Mean) + Mean : (-BuyRate1)*(Min - Mean) + Mean);
				double BuyPrice2 = (BuyRate2 > 0 ? BuyRate2*(Max - Mean) + Mean : (-BuyRate2)*(Min - Mean) + Mean);

				double BuyAmount1 = prop1*MaxWorkerNum;
				double BuyAmount2 = prop2*MaxWorkerNum;
				double BuyAmountB = MaxWorkerNum - BuyAmount1 - BuyAmount2;

				Order.buy(ItemID_Worker, BuyAmountB, BuyPriceB);
				Order.buy(ItemID_Worker, BuyAmount1, BuyPrice1);
				Order.buy(ItemID_Worker, BuyAmount2, BuyPrice2);
			}
		}WorkAI;
	public:
		void operator()(amount_t WorkerNum, product_stock& Stock, info_interface& Info, marchant_interface& Marchant)override {
			auto BuyMarketInfo = Info.buy_market();
			auto SellMarketInfo = Info.sell_market();
			auto ProductInfo = Info.product(Stock.id());

			unsigned int BuyPrice = 0;
			for (unsigned int ProductNo = 0; ProductNo < ProductInfo.size(); ++ProductNo) {
				BuyPrice += BuyMarketInfo.mean_price(ProductInfo.item(ProductNo)) * ProductInfo.amount(ProductInfo.item(ProductNo));
			}
			unsigned int SellPrice = SellMarketInfo.mean_price(ProductInfo.oitem())*ProductInfo.oamount();
			double ProductCost = Info.product_cost();

			double ProfitRate = (SellPrice - BuyPrice - ProductCost) / (BuyPrice + ProductCost);

			for (unsigned int ProductNo = 0; ProductNo < ProductInfo.size(); ++ProductNo) {
				BuyAI(WorkerNum * ProductInfo.amount(ProductNo), Stock.Input.at(ProductNo), ProductInfo.item(ProductNo), ProfitRate, BuyMarketInfo, Order);
			}

			SellAI(WorkerNum * ProductInfo.oamount(), Stock.Output, ProductInfo.oitem(), ProfitRate, SellMarketInfo, Order);

			WorkAI(WorkerNum, ProductInfo.max_product(Stock.Input), ProfitRate, BuyMarketInfo, Order);
		}
	};

	struct business {
		product_stock Stock;
		city_id Location;
		city_id TargetCity;
		std::unique_ptr<productAI> ProductAI;
	public:
		merchant Merchant;
	public:
		void operator()(amount_t WorkerNum_, info_interface& Info) {
			ProductAI->operator()(WorkerNum_, Stock, Info, Marchant);
		}
	};
	struct branch {
		city_id City;
		amount_t Transport;
		std::vector<business> Business;

		amount_t Building;
		double BuildingQuality;
		double BuildingDamage;
	};
	struct company {
	private:
		std::string Name;
		std::vector<item_id> Items;
		std::vector<branch> Branchs;
		//std::unique<accountAI> AccountAI;
		//std::unique<branchAI> BranchAI;
		//std::unique<manageAI> BusinessAI;
		//std::unique<logisticsAI> LogisticsAI;
	};
}
#
#endif
