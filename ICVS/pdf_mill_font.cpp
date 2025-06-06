#include "includes/pdf_mill_font.h"
#include "includes/pdf_mill.h"

namespace PDF_MILL
{
	struct _filedata;

	void read_font_obj(_filedata* filedata, std::ifstream& file, std::set<uint32_t> _obj_index)
	{
		std::set<uint32_t> font_descs;
		std::string line;

		for (auto& index : _obj_index)
		{
			read_obj_block(file, line, filedata->obj_offsets[index][BYTE_OFFSET]);
			if (validate_obj_type(line, FONT) == PDF_MILL_INVALID) { return; }

			filedata->cFont[index][SUB_TYPE_PARAM] = sub_type_map[get_key_value_i<std::string>(line, std::vector<key_index>{KEY_SUBTYPE_b, KEY_SUBTYPE_s})];
			
			filedata->cFont[index][ENCODING_PARAM] = encoding_map[get_key_value_i<std::string>(line, std::vector<key_index>{KEY_ENCODING})];

			filedata->cFont[index][BASE_FONT_PARAM] = base_font_map[get_key_value_i<std::string>(line, std::vector<key_index>{KEY_BASE_FONT})];

			uint32_t font_desc_index = get_indirect_obj(line, std::vector<key_index>{KEY_FONT_DESCRIPTOR}, ' ');
			filedata->cFont[index][FONT_DESCRIPTOR_PARAM] = font_desc_index;
			font_descs.insert(font_desc_index);

		}

		//TODO: Fix get_array_values_i bugs before activating this function
		read_font_desc_obj(filedata, file, font_descs);
	}


	void read_font_desc_obj(_filedata* filedata, std::ifstream& file, std::set<uint32_t>& obj_index)
	{
		std::set<uint32_t> _obj_index;
		std::string line;
		for (auto& index : obj_index)
		{
			read_obj_block(file, line, filedata->obj_offsets[index][BYTE_OFFSET]);
			if (validate_obj_type(line,FONT_DESCRIPTOR) == PDF_MILL_INVALID) { return; }

			FontDescriptor font_descriptor;
			filedata->cFontDescriptors[index] = font_descriptor;
			font_descriptor.flag = get_key_value_i<int>(line, std::vector<key_index>{KEY_FLAGS});

			//font_descriptor.font_box = get_array_values_i<float>(line, std::vector<key_index>{KEY_FONTBBOX}, ']');

			font_descriptor.italic_angle = get_key_value_i<int>(line, std::vector<key_index>{KEY_ITALICANGLE});
			font_descriptor.ascent = get_key_value_i<int>(line, std::vector<key_index>{KEY_ASCENT});
			font_descriptor.descent = get_key_value_i<int>(line, std::vector<key_index>{KEY_DESCENT});
			font_descriptor.cap_height = get_key_value_i<int>(line, std::vector<key_index>{KEY_CAPHEIGHT});

			font_descriptor.xheight = get_key_value_i<int>(line, std::vector<key_index>{KEY_XHEIGHT});
			font_descriptor.stemV = get_key_value_i<int>(line, std::vector<key_index>{KEY_STEMH});
			font_descriptor.stemH = get_key_value_i<int>(line, std::vector<key_index>{KEY_STEMV});

			font_descriptor.font_file = get_indirect_obj(line, std::vector<key_index>{KEY_FONTFILE, KEY_FONTFILE2},' ');


			read_font_file_obj(filedata, file, _obj_index);
		}
	}

	void read_font_file_obj(_filedata* filedata, std::ifstream& file, std::set<uint32_t> obj_index)
	{
		std::string line;
		for (auto& index : obj_index)
		{
			read_obj_block(file, line, filedata->obj_offsets[index][BYTE_OFFSET]);

			FontFile font_file;
			filedata->cFontFile[index] = font_file;
			font_file.length = get_key_value_i<size_t>(line, std::vector<key_index>{KEY_LENGTH});
			font_file.length1 = get_key_value_i<size_t>(line, std::vector<key_index>{KEY_LENGTH1});

			//CALL decompressor//

			font_file.stream;//Unfinished...
			
		}
	}

	void changeFont(_filedata* filedata, int page_number, int tag, base_font font)
	{
		uint32_t index = filedata->root->pages->get_obj_index(page_number);
		uint32_t font_index = filedata->cPage[index].rFonts[tag];

		filedata->cFont[font_index][BASE_FONT_PARAM] = font;

		//Currently not editting other font params...
	}

	void addFont(_filedata* filedata, int page_number, base_font font, sub_type_index sub_type, encoding encodin)
	{
		uint32_t page_index = filedata->root->pages->get_obj_index(page_number);
		uint32_t new_font_index = __get_next_obj_index(filedata);

		int new_font_tag = filedata->cPage[page_index].highest_font_tag + 1;

		filedata->cPage[page_index].rFonts[new_font_tag] = new_font_index;

		filedata->cFont[new_font_index][BASE_FONT_PARAM] = font;
		filedata->cFont[new_font_index][SUB_TYPE_PARAM] = sub_type;
		filedata->cFont[new_font_index][ENCODING_PARAM]  = encodin;

		filedata->obj_offsets.resize(filedata->num_obj);
		filedata->obj_offsets[new_font_index] = { 0, 0, 1 };
	}

}