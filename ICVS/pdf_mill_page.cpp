#include "includes/pdf_mill_page.h"
#include "includes/pdf_mill.h"


namespace PDF_MILL
{
    void read_page_collector(_filedata* filedata, std::ifstream& file)
    {
        std::string line;
        read_obj_block(file, line, filedata->obj_offsets[filedata->root->pages->id][BYTE_OFFSET]);

        if (validate_obj_type(line, type_index::PAGES) == PDF_MILL_INVALID) { return; }

        filedata->root->pages->nPages = get_key_value_i<uint32_t>(line, std::vector<key_index>{ KEY_COUNT });
        std::cout << "Page Count::: " << filedata->root->pages->nPages << std::endl; //OUTTER

        filedata->root->pages->mPages = get_array_objs(line, KEY_KIDS, '[');

    }

    void read_page_data(_filedata* filedata, std::ifstream& file)
    {
        std::set<uint32_t> _font_objs;
        std::set<uint32_t> _content_objs;

        std::string line;

        for (auto& page_obj_index : filedata->root->pages->mPages)
        {
            file.clear();
            file.seekg(filedata->obj_offsets[page_obj_index][BYTE_OFFSET], std::ios::beg);

            read_obj_block(file, line, filedata->obj_offsets[page_obj_index][BYTE_OFFSET]);

            if (validate_obj_type(line, PAGE) == PDF_MILL_INVALID) { return; }

            filedata->cPage[page_obj_index] = Page(page_obj_index);

            auto values = get_array_values_i<float>(line, std::vector<key_index>{KEY_MEDIABOX}, ']');
            filedata->cPage[page_obj_index].media_box = media_box(values[X], values[Y], values[W], values[H]);

            filedata->cPage[page_obj_index].rContents = get_indirect_array_obj(line, std::vector<key_index>{KEY_CONTENTS}, '/');
            for (auto x : filedata->cPage[page_obj_index].rContents)
            {
                _content_objs.insert(x);
            }

            std::string resource_line;
            read_dict_block(line, resource_line, KEY_RESOURCES);

            auto procset_values = get_array_values_i<std::string>(resource_line, std::vector<key_index>{KEY_PROCSET}, ']', ' ');
            filedata->cPage[page_obj_index].cProcSet = get_mapped_values<procset_index>(procset_values, procset_map);

            std::string font_resource_line;
            read_dict_block(resource_line, font_resource_line, KEY_FONT);

                    //Now read individual resources
            int _start = has_key<key_index>(resource_line, KEY_FONT);
            if(_start)
            {
                int len = get_length_to(resource_line, _start, '>');
                std::string res_line = resource_line.substr(_start + 2, len - 2);

                std::string res_data;
                std::stringstream _stream(res_line);
                int index = 0;
                while (std::getline(_stream, res_data, 'R'))
                {
                    index += index == 0 ? 1 : 0;
                    auto values = get_dict_key_value_indirect_obj(res_data, index);

                    if (values.second == 0) { continue; } // 0 is code for empty in this context

                    int font_tag = std::stoi(values.first.substr(1));
                    filedata->cPage[page_obj_index].highest_font_tag =
                        (filedata->cPage[page_obj_index].highest_font_tag < font_tag)
                        ? font_tag : filedata->cPage[page_obj_index].highest_font_tag;

                    filedata->cPage[page_obj_index].rFonts[font_tag] = values.second;
                    _font_objs.insert(values.second);
                }
            }
            
        }
        read_font_obj(filedata, file, _font_objs);
        read_content_data(filedata, file, _content_objs);
    }



    void AddPage(_filedata* filedata)
    {
        int new_obj_index = __get_next_obj_index(filedata);

        filedata->cPage[new_obj_index] = Page(new_obj_index);

        int new_tot_pages = ++filedata->root->pages->nPages;
        filedata->root->pages->mPages.push_back(new_obj_index);

        filedata->cPage[new_obj_index].media_box = media_box(0, 0, 595.32, 841.92);

        filedata->cPage[new_obj_index].cProcSet.push_back(PROCSET_PDF);
        filedata->cPage[new_obj_index].cProcSet.push_back(PROCSET_TEXT);

        filedata->obj_offsets.resize(filedata->num_obj);
        filedata->obj_offsets[new_obj_index] = { 0, 0, 1 };
    }


    void RemovePage(_filedata* filedata, int page_num)
    {
        if (page_num > filedata->root->pages->nPages)
        {
            std::cout << "[ERROR]:[PAGE]:Page number out of bounds....\n";
            return;
        }

        uint32_t page_index = filedata->root->pages->get_obj_index(page_num);
        uint32_t last_page_index = filedata->root->pages->mPages[filedata->root->pages->nPages - 1];

        if (last_page_index == page_index)
        {
            for (auto it = filedata->root->pages->mPages.begin(); it < filedata->root->pages->mPages.end(); ++it)
            {
                if (*it == page_index)
                {
                    filedata->root->pages->mPages.erase(it);
                    filedata->root->pages->nPages--;
                    break;
                }
            }

            filedata->cPage.erase(last_page_index);
            filedata->num_obj--;
        }
        else
        {
            for (auto it = filedata->root->pages->mPages.begin(); it < filedata->root->pages->mPages.end(); ++it)
            {
                if (*it == page_index)
                {
                    filedata->root->pages->mPages.erase(it);
                    filedata->root->pages->nPages--;
                    break;
                }
            }

            Page page = filedata->cPage[last_page_index];
            page.id = page_index;

            filedata->cPage.erase(page_index);
            filedata->available_index.push_front(page_index);
        }
    }


    void ChangePageSize(_filedata* filedata, int page_num, float x, float y)
    {
        uint32_t page_index = filedata->root->pages->get_obj_index(page_num);

        filedata->cPage[page_index].media_box._w = x;
        filedata->cPage[page_index].media_box._h = y;
    }

    int GetNumberOfPages(_filedata* filedata)
    {
        return filedata->root->pages->nPages;
    }

    uint32_t GetPageObjNumber(_filedata* filedata, int page_number)
    {
        return filedata->root->pages->get_obj_index(page_number);
    }
    const char** GetPagesNumbers(_filedata* filedata)
    {
        std::vector<const char*> r;
        for (int i = 0; i < filedata->root->pages->nPages; i++)
        {
            r.push_back(std::to_string(i).c_str());

        }
        return r.data();
    }


    std::array<float, 4> GetPageMediaBox(_filedata* filedata, int page_number)
    {
        uint32_t page_index = filedata->root->pages->get_obj_index(page_number);
        return filedata->cPage[page_index].media_box.asArray();
    }

    std::vector<TextBlock> GetPageTextBlocks(_filedata* filedata, int page_number)
    {
        std::vector<TextBlock> text_blocks;
        uint32_t page_index = filedata->root->pages->get_obj_index(page_number);

        for (auto content_index : filedata->cPage[page_index].rContents)
        {
            auto content = filedata->cContent[content_index];
            for (auto tb : content.text_blocks)
            {
                text_blocks.push_back(tb);
            }
        }
        return text_blocks;
    }

    base_font GetPageBaseFont(_filedata* filedata, int page_number, int font_tag)
    {
        uint32_t page_index = filedata->root->pages->get_obj_index(page_number);
        uint32_t font_index = filedata->cPage[page_index].rFonts[font_tag];

        return (base_font) filedata->cFont[font_index][BASE_FONT_PARAM];
    }
}