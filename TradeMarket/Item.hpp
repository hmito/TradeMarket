#ifndef TRADE_ITEM_INC
#define TRADE_ITEM_INC 100
#
#include<string>
#include<unordered_map>
namespace trade {
	using amount_t = int;
	using item_id = unsigned int;
	enum class item_category :unsigned char {
		null=0, goods=1, currency=2, labor=3, bond=4
	};
	enum class education_level :unsigned char{
		none=1, low=2, middle=3, high=4
	};
	constexpr item_id ItemID_Worker = 0;

	struct item_list {
	private:
		struct content {
			item_category Category;
			std::string Name;
			std::string Unit;
		};
	private:
		using container = std::unordered_map<item_id, content>;
	private:
		container Container;
	public:
		static item_category category(item_id ID) {
			//0:null
			//1-999:system id
			//1000-9999:market id
			//10000-:normal id
			if (ID == 0)return item_category::null;
			if (ID < 1000) return item_category::labor;
			if (ID < 2000) return item_category::currency;
			if (ID < 10000) return item_category::bond;
			return item_category::goods;
		}
		static item_id labor_id(education_level Lv) {
			switch (Lv) {
			case education_level::low:
				return 2;
			case education_level::middle:
				return 3;
			case education_level::high:
				return 4;
			default:
				return 1;
			}
		}
	};
}
#
#endif

