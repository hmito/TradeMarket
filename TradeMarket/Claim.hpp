#ifndef TRADE_CLAIM_INC
#define TRADE_CLAIM_INC 100
#
#include"Stock.hpp"
namespace trade {
	struct claim_interface {
		virtual operator bool ()const = 0;
		virtual bool operator()(stock_interface& Target) = 0;
	};
	struct payment_content {
	private:
		item_id ID;
		amount_t Amount;
	public:
		payment_content() :ID(0), Amount(0) {}
		payment_content(item_id ID_, amount_t Amount_, bool IsCraimerPayment_) :ID(ID_), Amount(IsCraimerPayment_?-Amount_: Amount_) {}
	public:
		item_id id()const { return ID; }
		amount_t amount()const { return std::abs(Amount); }
		bool is_claimer_payment()const { return Amount < 0; }
	};
	struct payment_claim :public claim_interface {
	private:
		using this_type = payment_claim;
	private:
		payment_content Content;
		stock_interface* pClaimer;
	public:
		payment_claim()noexcept : Content(), pClaimer(nullptr) {}
		payment_claim(stock_interface& Claimer_, payment_content Content_)
			: pClaimer(&Claimer_)
			, Content(Content_) {
		}
		payment_claim(const this_type&) = delete;
		this_type& operator=(const this_type&) = delete;
		payment_claim(this_type&&) = default;
		this_type& operator=(this_type&&) = default;
	public:
		item_id id()const { return Content.id(); }
		amount_t amount()const { return Content.amount(); }
		const payment_content& content()const { return Content; }
	public:
		operator bool()const override{return pClaimer!=nullptr;}
		bool operator()(stock_interface& Target)override {
			if (!*this)return true;

			if (Content.is_claimer_payment()) {
				if (deal(*pClaimer, Target, Content.id(), Content.amount()))return true;
			} else {
				if (deal(Target, *pClaimer, Content.id(), Content.amount()))return true;
			}
			pClaimer = nullptr;
			return false;
		}
	};
	struct trade_content {
	private:
		item_id GiveID;
		amount_t GiveAmount;
		item_id TakeID;
		amount_t TakeAmount;
	public:
		trade_content() :GiveID(0), GiveAmount(0), TakeID(0), TakeAmount(0) {}
		trade_content(item_id GiveID_, amount_t GiveAmount_, item_id TakeID_, amount_t TakeAmount_) 
			:GiveID(GiveID_), GiveAmount(GiveAmount_) , TakeID(TakeID_), TakeAmount(TakeAmount_) {}
	public:
		item_id give_id()const { return GiveID; }
		amount_t give_amount()const { return GiveAmount; }
		item_id take_id()const { return TakeID; }
		amount_t take_amount()const { return TakeAmount; }
	};
	struct trade_claim :public claim_interface {
	private:
		using this_type = trade_claim;
	private:
		trade_content Content;
		stock_interface* pClaimer;
	public:
		trade_claim()noexcept : Content(), pClaimer(nullptr) {}
		trade_claim(stock_interface& Claimer_, trade_content Content_)
			: pClaimer(&Claimer_)
			, Content(Content_) {
		}
		trade_claim(const this_type&) = delete;
		this_type& operator=(const this_type&) = delete;
		trade_claim(this_type&&) = default;
		this_type& operator=(this_type&&) = default;
	public:
		item_id give_id()const { return Content.give_id(); }
		amount_t give_amount()const { return Content.give_amount(); }
		item_id take_id()const { return Content.take_id(); }
		amount_t take_amount()const { return Content.take_amount(); }
		const trade_content& content()const { return Content; }
	public:
		operator bool()const override { return pClaimer != nullptr; }
		bool operator()(stock_interface& Target)override {
			if (!*this)return true;

			if (deal(Target, *pClaimer, take_id(), take_amount()))return true;

			if (deal(*pClaimer, Target, give_id(), give_amount())){
				deal(*pClaimer, Target, take_id(), take_amount());
				return true;
			}
			pClaimer = nullptr;
			return false;
		}
	};

}
#
#endif
