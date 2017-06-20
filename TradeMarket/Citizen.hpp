#ifndef TRADE_CITIZEN_INC
#define TRADE_CITIZEN_INC 100
#
#include<array>
#include"Utility.hpp"
#include"Item.hpp"
#include"Stock.hpp"
#include"Merchant.hpp"
namespace trade {
	struct citizen {
	private:
		struct stock :public stock_interface {
		protected:
			bool add(item_id ID, amount_t Amount) override {}
			amount_t get(item_id ID)const override {}
		private:
			item_id LaborID;
			amount_t LaborAmount;
			std::array<unsigned int, 3> Amount;
			std::array<unsigned int, 3> Worth;
			double Happiness;
		};
	private:
		stock Stock;
		merchant Merchant;
		education_level EducationLv;
	public:

	};
}
#
#endif
