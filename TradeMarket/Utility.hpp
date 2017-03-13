#ifndef TRADE_UTILITY_INC
#define TRADE_UTILITY_INC 100
#
namespace trade {
	using amount_t = int;
	using item_id = unsigned int;
	using city_id = unsigned int;

	class item_contents {
		std::string Name;
		std::string Unit;
	};
}
#
#endif