#ifndef TRADE_UTILITY_INC
#define TRADE_UTILITY_INC 100
#
namespace trade {
	using city_id = unsigned int;
	using agent_id = unsigned int;
	enum class education_level : unsigned char {
		none = 0, primary = 1, secondary = 2, tertiary = 3
	};
}
#
#endif
