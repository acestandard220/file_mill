#include "includes/pdf_mill_root.h"
#include "includes/pdf_mill.h"
#include "includes/pdf_mill_page.h"
#include "includes/pdf_mill_outline.h"

namespace PDF_MILL
{
    uint32_t GetRootID(_filedata* filedata)
    {
        return filedata->root->id;
    }

    Root::Root()
    {

    }

    Root::Root(uint32_t _id, type_index type)
        :Object(_id,type)
    {
    }


    void read_root_obj(_filedata* filedata, std::ifstream& file)
    {
        std::string line;

        read_obj_block(file, line, filedata->obj_offsets[GetRootID(filedata)][BYTE_OFFSET]);

        int status = validate_obj_type(line, CATALOG);
        if (status == PDF_MILL_INVALID)
        {
            return;
        }

        filedata->root->pages = new PageCollection;
        filedata->root->pages->id = get_key_value_i<uint32_t>(line, std::vector<key_index>{ KEY_PAGES });

        uint32_t outline_index = get_key_value_i<uint32_t>(line, std::vector<key_index>{KEY_OUTLINES});
        if(outline_index)
        {
            filedata->root->outline = new Outline(outline_index);
            read_outline_obj(filedata,file);
        }

    }
}
