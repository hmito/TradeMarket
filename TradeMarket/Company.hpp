#ifndef TRADE_COMPANY_INC
#define TRADE_COMPANY_INC 100
#
#include<string>
#include<vector>
#include<memory>
#include"Product.hpp"
namespace trade {
	struct productAI{
		virtual void operator()(amount_t Scale, business& Business, market_interface& Market) = 0;
	};
	struct business {
		product_id ID;
		product_input_t Input;
		amount_t Output;
		amount_t Transport;
		city_id TargetCity;
		std::unique_ptr<productAI> ProductAI;
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
