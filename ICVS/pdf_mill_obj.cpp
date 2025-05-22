#include "includes/pdf_mill_obj.h"
#include "includes/pdf_mill.h"


namespace PDF_MILL
{
    Object::Object()
    {

    }

    uint32_t __get_next_obj_index(_filedata* filedata)
    {
        uint32_t new_obj_index;
        if (!filedata->available_index.empty())
        {
            new_obj_index = filedata->available_index.front();
            filedata->available_index.pop_front();
        }
        else
        {
            new_obj_index = filedata->num_obj++;
        }
        return new_obj_index;
    }
}