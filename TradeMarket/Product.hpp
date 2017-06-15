#ifndef TRADE_PRODUCT_INC
#define TRADE_PRODUCT_INC 100
#
#include<vector>
#include<iterator>
#include<algorithm>
#include"Utility.hpp"
#include"Stock.hpp"
namespace trade {
	using product_id = unsigned int;

	class product{
	private:
		struct item_element {
			item_id ID;
			double Coef;
		public:
			item_element() :ID(0), Coef(0.) {}
			item_element(item_id ID_, double Coef_) :ID(ID_), Coef(Coef_) {}
		};
		using container = std::vector<item_element>;
		using const_iterator = typename container::const_iterator;
	public:
		struct stock : public stock_interface {
		public:
			using container = std::vector<amount_t>;
			using const_iterator = typename container::const_iterator;
			using product_iterator = typename product::const_iterator;
		private:
			const product* pProduct;
			container LOI;
		public:
			stock(const product& Product_):pProduct(&Product_) {
				LOI.assign(Product_.size(), 0);
			}
		protected:
			bool add(item_id ID, amount_t Amount) override {
				auto Itr= pProduct->find(ID);

				if (Itr == pProduct->end())return true;

				//Labour case
				if (Itr == pProduct->begin()) {
					auto AItr = LOI.begin()+2;
					Itr+=2;
					for (; Itr != pProduct->end(); ++Itr,++AItr) {
						Amount = std::min(Amount, static_cast<amount_t>(*AItr / Itr->Coef));
					}
					if (Amount > 0) {
						AItr = LOI.begin() + 1;
						Itr = pProduct->begin() + 1;

						*AItr += static_cast<amount_t>(Amount*Itr->Coef);

						for (; Itr != pProduct->end(); ++Itr, ++AItr) {
							*AItr -= static_cast<amount_t>(Amount*Itr->Coef);
						}
					}
					return false;
				}

				//Item case
				auto AItr = LOI.begin() + std::distance(pProduct->begin(), Itr);

				if (*AItr + Amount < 0)return true;
				*AItr += Amount;
				return false;
			}
		public:
			amount_t get(item_id ID)const override {
				auto Itr = pProduct->find(ID);
				if (Itr == pProduct->end())return 0;

				//Item case
				auto AItr = LOI.begin() + std::distance(pProduct->begin(), Itr);
				return *AItr;
			}
		};
	private:
		std::string Name;
		container LOI;	//Labour, Output, Input[...]
	private:
		unsigned int size()const { return LOI.size(); }
		const_iterator begin()const { return LOI.begin(); }
		const_iterator end()const { return LOI.end(); }
		const_iterator find(item_id ID)const { return std::find_if(std::begin(LOI), std::end(LOI), [=](const item_element& v)->bool {return v.ID == ID; }); }
	public:
		const std::string& name()const { return Name; }
		item_id labour_id()const { return LOI[0].ID; }
		item_element output() const { return LOI[1]; }
		const_iterator input_begin()const { return LOI.begin() + 2; }
		const_iterator input_end()const { return LOI.end(); }
		unsigned int input_size()const { return LOI.size() - 2; }
		stock make_stock()const { return stock(*this); }
	};
	using product_stock = typename product::stock;
}
#
#endif
