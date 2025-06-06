#pragma once
#include "pdf_mill_core.h"
#include "pdf_mill_obj.h"

namespace PDF_MILL
{
	struct _filedata;

	enum MONTHS
	{
		JANUARY,
		FEBUARY,
		MARCH,
		APRIL,
		MAY,
		JUNE,
		JULY,
		AUGUST,
		SEPTEMBER,
		OCTOBER,
		NOVEMBER,
		DECEMBER,
		_month_last_
	};

	struct __date
	{
		uint8_t day = 1;
		uint32_t year = 1970;
		MONTHS month = JANUARY;
		__date() = default;
	};

	struct __time
	{
		uint32_t hour;
		uint32_t minute;
		uint32_t second;

		__time() = default;
	};

	struct __datetime
	{
		__date date;
		__time time;

		__datetime() = default;
		__datetime(std::string datatime_string);
	};

	struct Info : public Object
	{
		Info() = default;
		Info(uint32_t _id)
			:Object(_id, INFO)
		{

		}

		std::string title;
		std::string creator;
		std::string producer;

		__datetime creation_date;
		__datetime modification_time;

		uint32_t creator_year;
		std::string creator_version;
	};

	void read_info_obj(_filedata* filedata, std::ifstream& file);
}