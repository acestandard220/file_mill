#pragma once
#include "pdf_mill_core_types.h"

namespace PDF_MILL
{
    struct _filedata;

    struct Object
    {
    public:
        Object();
        Object(uint32_t _id, type_index _type) {
            id = _id;
            type = _type;
        }

        uint32_t id;
        type_index type;
    };

    uint32_t __get_next_obj_index(_filedata* filedata);


}