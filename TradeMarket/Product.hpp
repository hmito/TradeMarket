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
namespace trade {
	using product_id = unsigned int;
	constexpr unsigned int PRODUCT_INPUT_NUM = 3;

	class product{
	public:
		std::string Name;
		using input_t = std::array<amount_t, PRODUCT_INPUT_NUM>;
		std::array<item_id, PRODUCT_INPUT_NUM> InputID;
		item_id OutputID;
		std::array<double, PRODUCT_INPUT_NUM> InputCoef;
	public:
		double operator()(input_t& Input) {
			amount_t Num = std::numeric_limits<amount_t>::max();

			for (unsigned int i = 0; i < PRODUCT_INPUT_NUM; ++i) {
				if (InputCoef[i] > 0.0) Num = std::min(Num, static_cast<amount_t>(Input[i] / InputCoef[i]));
			}

			for (unsigned int i = 0; i < PRODUCT_INPUT_NUM; ++i) {
				if (InputCoef[i] > 0.0) Input[i] -= Num*InputCoef[i];
				if (Input[i] < 0)Input[i] = 0.0;
			}

			return Num;
		}
	};
	using product_input_t = typename product::input_t;
	struct product_holder_interface {
		virtual const product& get_product(product_id)const = 0;
	};
}
#
#endif
