#pragma once
#include "pdf_mill_document.h"
#include "pdf_mill_obj.h"

namespace PDF_MILL
{
    struct _filedata;

    struct TextBlock
    {
        uint32_t font_size = 0;
        int font_tag = 0;
        std::array<int32_t, 2> text_position;

        std::array<int32_t, 6> text_matrix;
        std::string text = "";
    };

    struct Content : public Object
    {
        Content() = default;
        Content(uint32_t _id)
            :Object(_id, CONTENT)
        {
        }

        size_t len_stream = 0;
        int filter = 0;

        std::vector<TextBlock> text_blocks;
    };

    void read_content_data(_filedata* filedata, std::ifstream& file, const std::set<uint32_t>& contents);
}