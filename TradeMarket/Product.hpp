#ifndef TRADE_TRADEMARKET_INC
#define TRADE_TRADEMARKET_INC 100
#
#include<vector>
#include<array>
#include<string>
#include<unordered_map>
#include<memory>
#include<algorithm>
#include"Utility.hpp"
#include"Market.hpp"
#include"Stock.hpp"
namespace trade {
	using product_id = unsigned int;

	class product{
	private:
		static constexpr unsigned int MaxInputSize = 3;
	public:
		static constexpr unsigned int max_input_size() { return MaxInputSize; }
		static constexpr unsigned int max_size() { return max_input_size() + 2; }
	private:
		std::string Name;
		std::array<item_id, MaxInputSize + 2> ID;		//Lab,Out,In...
		std::array<amount_t, MaxInputSize + 2> Coef;	//Lab,Out,In...
	public:
		bool operator()(stock& Stock, amount_t Worker) {
			amount_t Num = Worker;

			for (unsigned int i = 0; i < MaxInputSize; ++i) {
				if (InputCoef[i] > 0.0) Num = std::min(Num, static_cast<amount_t>(Input[i] / InputCoef[i]));
			}

			for (unsigned int i = 0; i < MaxInputSize; ++i) {
				if (InputCoef[i] > 0.0) Input[i] -= Num*InputCoef[i];
				if (Input[i] < 0)Input[i] = 0.0;
			}

			Output += OutputCoef*Num;

			return false;
		}
	};
	struct product_stock : public stock_interface {
	public:
		using container = std::array<amount_t, product::max_size()>;
		using const_iterator = typename container::const_iterator;
	private:
		product* pProduct;
		container Container;
	protected:
		bool add(item_id ID, amount_t Amount) override {
			auto No = pProduct->find_no(ID);
			if (No < 0)return true;
			Container.at(No) += Amount;
			return false;
		}
	public:
		amount_t get(item_id ID)const override {
			auto No = pProduct->find_no(ID);
			if (No < 0)return 0;
			return Container.at(No);
		}
	};
	struct product_holder_interface {
		virtual const product& get_product(product_id)const = 0;
	};
	class product_stock : public stock_interface {
	public:
		product* Ptr;
		product::input_t Input;
		amount_t Output;
	private:
		void work(amount_t Worker) {
			(*Ptr)(Input, Output, Worker);
		}
	protected:
		bool add(item_id ID, amount_t Amount)override {
			if (ID == ItemID_Worker) {
				work(Amount);
			}

			if (Ptr->OutputID == ID) {
				Output += Amount;
				return false;
			}

			for (unsigned int i = 0; i < product::MaxInputSize; ++i) {
				if (Ptr->InputID[i] == ID) {
					Input[i] += Amount;
					return false;
				}
			}
			return true;
		}
	public:
		const amount_t get(item_id ID)const override {
			if (Ptr->OutputID == ID) {
				return Output;
			}

			for (unsigned int i = 0; i < product::MaxInputSize; ++i) {
				if (Ptr->InputID[i] == ID) {
					return Input[i];
				}
			}

			return 0;
		}
	};
}
#
#endif
