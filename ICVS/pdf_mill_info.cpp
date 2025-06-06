#include "includes/pdf_mill_info.h"
#include "includes/pdf_mill.h"


namespace PDF_MILL
{

	__datetime::__datetime(std::string datetime_string)
	{
		date.year = std::stoi(datetime_string.substr(2, 4));
		date.month = (MONTHS)std::stoi(datetime_string.substr(6, 2));
		date.day = std::stoi(datetime_string.substr(8, 2));

		time.hour = std::stoi(datetime_string.substr(10, 2));
		time.minute = std::stoi(datetime_string.substr(12, 2));
		time.second = std::stoi(datetime_string.substr(14, 2));
	}

	void read_info_obj(_filedata* filedata, std::ifstream& file)
	{
		if (!filedata->info) { return; }
		std::string line;
		read_obj_block(file, line, filedata->info->id);

		filedata->info->title = get_key_value_i<std::string>(line, std::vector<key_index>{KEY_INFO}, ')', '\n');
		filedata->info->producer = get_key_value_i<std::string>(line, std::vector<key_index>{KEY_PRODUCER}, ')', '\n');
		filedata->info->creator = get_key_value_i<std::string>(line, std::vector<key_index>{KEY_CREATOR}, ')', '\n');
		filedata->info->creation_date = __datetime(get_key_value_i<std::string>(line, std::vector<key_index>{KEY_CREATION_DATE}, ')', '\n'));
		filedata->info->modification_time = __datetime(get_key_value_i<std::string>(line, std::vector<key_index>{KEY_MODIFICATION_DATE}, ')', '\n'));
	}
}
