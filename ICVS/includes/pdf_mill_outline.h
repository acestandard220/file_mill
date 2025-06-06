#pragma once
#include "pdf_mill_core.h"
#include "pdf_mill_obj.h"

namespace PDF_MILL
{
	enum FIT_INDEX
	{
		FIT_FIT,
		FIT_FIT_H,
		FIT_FIT_R,
		FIT_FIT_B,
		FIT_FIT_V,
		FIT_FIT_BH,
		FIT_FIT_BV,
	};

	struct __fit_param
	{
		__fit_param() = default;
		__fit_param(FIT_INDEX id)
			:fit_id(id){ }
		FIT_INDEX fit_id = FIT_FIT;
	};

	struct FIT : public __fit_param
	{
		FIT()
			:__fit_param(FIT_FIT)
		{

		}
	};

	struct FIT_H : public __fit_param
	{
		FIT_H()
			:__fit_param(FIT_FIT_H)
		{

		}
		float top;
	};

	struct FIT_V : public __fit_param
	{
		FIT_V()
			:__fit_param(FIT_FIT_V)
		{

		}
		float left;
	};


	struct FIT_R : public __fit_param
	{
		FIT_R();
		float left;
		float bottom;
		float right;
		float top;
	};

	struct FIT_B : public __fit_param
	{
		FIT_B()
			:__fit_param(FIT_FIT_B)
		{

		}
	};

	struct FIT_BH : public __fit_param
	{
		FIT_BH()
			:__fit_param(FIT_FIT_BH)
		{

		}
		float top;
	};

	struct FIT_BV : public __fit_param
	{
		FIT_BV()
			:__fit_param(FIT_FIT_BV)
		{

		}
		float left;
	};

	struct OutlineDestination
	{
		OutlineDestination() = default;
		OutlineDestination(uint32_t _id)
			:page_index(_id)
		{

		}

		uint32_t page_index;
		__fit_param* fit_type = nullptr;
	};

	struct OutlineItem;
	struct Outline : public Object
	{
		Outline() = default;
		Outline(uint32_t _id)
			:Object(_id, OUTLINES)
		{

		}

		int count = 0;
		OutlineItem* first = nullptr;
		OutlineItem* last = nullptr;
	};

	struct OutlineItem : public Object
	{
		OutlineItem() = default;
		OutlineItem(uint32_t _id)
			:Object(_id, OUTLINES_ITEM)
		{

		}

		int count = 0;
		OutlineItem* first = nullptr;
		OutlineItem* last = nullptr;
		OutlineItem* next = nullptr;

		uint32_t parent = 0;
		uint32_t prev = 0;

		std::string title;
		OutlineDestination* dest;
	};

	void read_outline_obj(_filedata* filedata,std::ifstream& file);
	OutlineItem* read_outline_item(_filedata* filedata, std::ifstream& file, uint32_t index);
	__fit_param* get_match_fit(std::string& line, std::vector<key_index> key);
}
