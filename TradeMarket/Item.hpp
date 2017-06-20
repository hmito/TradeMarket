#ifndef TRADE_ITEM_INC
#define TRADE_ITEM_INC 100
#
#include<string>
#include<vector>
#include<fstream>
#include<hmLib/csv_iterator.hpp>
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

	struct item_info_interface {
		virtual item_category category(item_id ID) = 0;
		virtual item_id labor_id(education_level Lv) = 0;
	};
	struct item_map{
	private:
		struct content {
			item_category Category;
			std::string Name;
			std::string Unit;
		};
	private:
		using container = std::vector<content>;
	private:
		container Container;
	public:
		item_category category(item_id ID) {

		}
		item_id labor_id(education_level Lv) {

		}
		void insert(item_category Category, std::string Name, std::string Unit) {
			Container.emplace_back(Category, Name, Unit);
		}
	public:
		static item_map load(std::string FileName) {
			item_map Map;
			Map.insert(item_category::null, "Null", "");
			std::ifstream Fin(FileName);
			auto Itr = hmLib::icsv_begin(Fin);
		}
	};
}
#
#endif

