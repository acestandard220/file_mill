#include "includes/pdf_mill_outline.h"
#include "includes/pdf_mill_document.h"
#include "includes/pdf_mill.h"

namespace PDF_MILL
{
	FIT_R::FIT_R()
		:__fit_param(FIT_FIT_R)
	{

	}
	void read_outline_obj(_filedata* filedata, std::ifstream& file)
	{
		std::string line;
		read_obj_block(file, line, filedata->obj_offsets[filedata->root->outline->id][BYTE_OFFSET]);

		int status = validate_obj_type(line, OUTLINES);
		if (status == PDF_MILL_INVALID)
		{
			return;
		}

		filedata->root->outline->count = get_key_value_i<int>(line, std::vector<key_index>{KEY_COUNT});
		uint32_t first = get_indirect_obj(line, std::vector<key_index>{KEY_FIRST}, ' ');
		uint32_t last = get_indirect_obj(line, std::vector<key_index>{KEY_LAST}, ' ');

		filedata->root->outline->first = read_outline_item(filedata, file, first);
		filedata->root->outline->last = read_outline_item(filedata, file, last);
	}

	__fit_param* get_match_fit(std::string& line, std::vector<key_index> key)
	{
		for (auto x : key)
		{
			int start = has_key<key_index>(line, x);
			if (start)
			{
				switch (x)
				{
					case KEY_FIT:    return new FIT();
					case KEY_FIT_B:  return new FIT_B();

					case KEY_FIT_R:
					{
						FIT_R* fit = new FIT_R;
						auto values = get_array_values_i<float>(line, std::vector<key_index>{ KEY_FIT_R }, '\n', ' ');
						fit->left = values[0];
						fit->bottom = values[1];
						fit->right = values[2];
						fit->top = values[3];
						return fit;
					}

					case KEY_FIT_H:
					{
						FIT_H* fit = new FIT_H;
						auto values = get_array_values_i<float>(line, std::vector<key_index>{ KEY_FIT_H }, '\n', ' ');
						fit->top = values[0];
						return fit;
					}

					case KEY_FIT_BV:
					{
						FIT_BV* fit = new FIT_BV;
						auto values = get_array_values_i<float>(line, std::vector<key_index>{ KEY_FIT_BV }, '\n', ' ');
						fit->left = values[0];
						return fit;
					}

					case KEY_FIT_V:
					{
						FIT_V* fit = new FIT_V;
						auto values = get_array_values_i<float>(line, std::vector<key_index>{ KEY_FIT_V }, '\n', ' ');
						fit->left = values[0];
						return fit;
					}

					case KEY_FIT_BH:
					{
						FIT_BH* fit = new FIT_BH;
						auto values = get_array_values_i<float>(line, std::vector<key_index>{ KEY_FIT_BH }, '\n', ' ');
						fit->top = values[0];
						return fit;
					}
				}
				
			}
		}
	}

	OutlineItem* read_outline_item(_filedata* filedata, std::ifstream& file, uint32_t index)
	{
		std::string line;
		read_obj_block(file, line, filedata->obj_offsets[index][BYTE_OFFSET]);

		OutlineItem* item = new OutlineItem(index);

		uint32_t first_index = get_indirect_obj(line, std::vector<key_index>{KEY_FIRST},' ');
		if (first_index)
		{
			item->first = read_outline_item(filedata, file, first_index);
		}

		uint32_t last_index = get_indirect_obj(line, std::vector<key_index>{KEY_LAST}, ' ');
		if(last_index)
		{
			item->first = read_outline_item(filedata, file, last_index);
		}

		uint32_t next_index = get_indirect_obj(line, std::vector<key_index>{KEY_NEXT}, ' ');
		if (next_index)
		{
			item->next = read_outline_item(filedata, file, next_index);
		}

		item->parent = get_indirect_obj(line, std::vector<key_index>{KEY_PARENT},' ');
		item->prev = get_indirect_obj(line, std::vector<key_index>{KEY_PREVIOUS}, ' ');

		item->title = get_key_value_i<std::string>(line, std::vector<key_index>{KEY_TITLE}, ')','\n');
		item->count = get_key_value_i<int>(line, std::vector<key_index>{KEY_COUNT});

		std::string dest_string = get_key_value_i<std::string>(line, std::vector<key_index>{KEY_DEST},']','\n');
		item->dest = new OutlineDestination(get_key_value_i<uint32_t>(dest_string, 0));

		std::vector<key_index> fit_keys { KEY_FIT, KEY_FIT_H, KEY_FIT_R, KEY_FIT_B, KEY_FIT_V, KEY_FIT_BH, KEY_FIT_BV };
		item->dest->fit_type = get_match_fit(dest_string, fit_keys);		
		return item;
	}
}
