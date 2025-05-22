#pragma once
#include "pdf_mill_core.h"
#include "pdf_mill_obj.h"
#include "pdf_mill_box.h"
#include "pdf_mill_font.h"
#include "pdf_mill_content.h"

namespace PDF_MILL
{
    
    struct PageCollection
    {
        uint32_t id;
        uint32_t nPages = 0;

        std::vector<uint32_t> mPages;
        media_box* media_box = nullptr;

        const int get_obj_index(int page_num)const { return mPages[page_num - 1]; }
    };

    struct Page : public Object
    {
        Page() {};

        Page(uint32_t _id)
            :Object(_id, PAGE)
        {
        }

        uint32_t parent;
        media_box media_box;

        std::vector<uint32_t> rContents;
        std::vector<int> cProcSet;

        std::unordered_map<int, uint32_t> rFonts;

        int highest_font_tag = 1;
    };


    void read_page_collector(_filedata* filedata, std::ifstream& file);
    void read_page_data(_filedata* filedata, std::ifstream& file);

    extern "C" ICVS_DLL void AddPage(_filedata* filedata);
    extern "C" ICVS_DLL void RemovePage(_filedata* filedata, int page_number);
    extern "C" ICVS_DLL void ChangePageSize(_filedata* filedata, int page_number, float x, float y);

    extern "C" ICVS_DLL int GetNumberOfPages(_filedata* filedata);
    extern "C" ICVS_DLL uint32_t GetPageObjNumber(_filedata* filedata, int page_number);
    
    extern "C" ICVS_DLL const char** GetPagesNumbers(_filedata* filedata);


    extern "C" ICVS_DLL std::array<float, 4> GetPageMediaBox(_filedata* filedata, int page_number);
    extern "C" ICVS_DLL std::vector<TextBlock> GetPageTextBlocks(_filedata* filedata, int page_number);
    extern "C" ICVS_DLL base_font GetPageBaseFont(_filedata* filedata, int page_number, int font_tag);
}