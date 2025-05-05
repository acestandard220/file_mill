#include "includes/pdf_mill.h"

namespace PDF_MILL
{
    uint32_t __get_next_obj_index()
    {
        uint32_t new_obj_index;
        if (!global_data->cur_file_read->available_index.empty())
        {
            new_obj_index = global_data->cur_file_read->available_index.front();
            global_data->cur_file_read->available_index.pop_front();
        }
        else
        {
            new_obj_index = global_data->cur_file_read->num_obj++;
        }
        return new_obj_index;
    }

    void AddPage()
    {
        int new_obj_index = __get_next_obj_index();
        
        global_data->cur_file_read->cPage[new_obj_index] = Page(new_obj_index);

        int new_tot_pages = ++global_data->cur_file_read->root->pages->nPages;
        global_data->cur_file_read->root->pages->mPages.push_back(new_obj_index);

        global_data->cur_file_read->cPage[new_obj_index].media_box = media_box(0, 0, 595.32, 841.92);

        global_data->cur_file_read->cPage[new_obj_index].cProcSet.push_back(PROCSET_PDF);
        global_data->cur_file_read->cPage[new_obj_index].cProcSet.push_back(PROCSET_TEXT);

        global_data->cur_file_read->obj_offsets.resize(global_data->cur_file_read->num_obj);
        global_data->cur_file_read->obj_offsets[new_obj_index] = { 0,0,1 };
    }

    void RemovePage(int page_num)
    {
        auto data = global_data->cur_file_read;
    
        if (page_num > data->root->pages->nPages)
        {
            std::cout << "[ERROR]:[PAGE]:Page number out of bounds....\n";
            return;
        }
    
        uint32_t page_index = data->root->pages->get_obj_index(page_num);
        uint32_t last_page_index = data->root->pages->mPages[data->root->pages->nPages - 1];
    
        if (last_page_index == page_index)
        {
            for (auto it = data->root->pages->mPages.begin(); it < data->root->pages->mPages.end(); ++it)
            {
                if (*it == page_index)
                {
                    data->root->pages->mPages.erase(it);
                    data->root->pages->nPages--;
                    break;
                }
            }
    
            data->cPage.erase(last_page_index);
            data->num_obj--;
        }
        else
        {
            for (auto it = data->root->pages->mPages.begin(); it < data->root->pages->mPages.end(); ++it)
            {
                if (*it == page_index)
                {
                    data->root->pages->mPages.erase(it);
                    data->root->pages->nPages--;
                    //data->root->pages->mPages[data->root->pages->nPages - 1] = page_index;
                    break;
                }
            }
    
            Page pg = data->cPage[last_page_index];
            pg.id = page_index;
            //data->cPage[page_index] = pg;
    
            //data->cPage.erase(last_page_index);
            data->cPage.erase(page_index); //.new.//
            //data->num_obj--;

            data->available_index.push_front(page_index);
        }
    }

    void ChangeFont(int page_num, int tag, base_font font)
    {
        uint32_t index = global_data->cur_file_read->root->pages->get_obj_index(page_num);
        uint32_t font_index = global_data->cur_file_read->cPage[index].rFonts[tag];
        global_data->cur_file_read->cFont[font_index][base_font_param] = font;

        //Currently not editting other font params...
    }

    void AddFont(int page_num, base_font font, sub_type_index sub_type, encoding encodin)
    {
        uint32_t page_index = global_data->cur_file_read->root->pages->get_obj_index(page_num);
        uint32_t new_font_index = __get_next_obj_index();
        int new_font_tag = global_data->cur_file_read->cPage[page_index].highest_font_tag + 1;

        global_data->cur_file_read->cPage[page_index].rFonts[new_font_tag] = new_font_index;

        global_data->cur_file_read->cFont[new_font_index][base_font_param] = font;
        global_data->cur_file_read->cFont[new_font_index][sub_type_param] = sub_type;
        global_data->cur_file_read->cFont[new_font_index][encoding_param] = encodin;

        global_data->cur_file_read->obj_offsets.resize(global_data->cur_file_read->num_obj);
        global_data->cur_file_read->obj_offsets[new_font_index] = { 0,0,1 };
    }

    void ChangePageSize(int page_num, int x, int y)
    {
        auto data = global_data->cur_file_read;
        uint32_t page_index = data->root->pages->get_obj_index(page_num);

        data->cPage[page_index].media_box._w = x;
        data->cPage[page_index].media_box._h = y;
    }

    void UpdateVersion(version_index version)
    {
        global_data->cur_file_read->version = version_string[version];
    }


    int GetNumberOfPages()
    {
        return global_data->cur_file_read->root->pages->nPages;
    }

    uint32_t GetPageObjNumber(int page_number)
    {
        return global_data->cur_file_read->root->pages->get_obj_index(page_number);
    }
    const char** GetPagesNumbers()
    {
        std::vector<const char*> r;
        for (int i = 0; i < global_data->cur_file_read->root->pages->nPages; i++)
        {
            r.push_back(std::to_string(i).c_str());

        }
        return r.data();
    }

    const char** re()
    {
        const char** r = {};
        for (int i = 0; i < global_data->cur_file_read->root->pages->nPages; i++)
        {
            r[i] = std::to_string(i).c_str();

        }
        return r;
    }
}