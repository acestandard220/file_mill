#pragma once
#include "pdf_mill_core.h"
#include "pdf_mill_obj.h"
#include "pdf_mill_document.h"

namespace PDF_MILL
{
    struct PageCollection;

    struct Root : public Object
    {
        Root();
        Root(uint32_t _id, type_index type);

        PageCollection* pages = nullptr;
    };

    void read_root_obj(_filedata* filedata, std::ifstream& file);

    uint32_t GetRootID(_filedata* filedata);
}