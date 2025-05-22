#pragma once
#include "pdf_mill_core.h"

namespace PDF_MILL
{

    enum xref_values
    {
        BYTE_OFFSET,
        UID,
        FLAG,
        _xref_values_last_
    };

    enum type_index //1
    {
        CATALOG,
        PAGES,
        PAGE,
        ENDOBJ,
        FONT,
        CONTENT,
        FONT_DESCRIPTOR,
        STREAM,
        FONT_FILE,
        _type_index_last_ 
    };

    enum key_index //2
    {
        KEY_TYPE,
        KEY_COUNT,
        KEY_CONTENTS,
        KEY_KIDS,
        KEY_PAGES,
        KEY_MEDIABOX,
        KEY_LENGTH,
        KEY_RESOURCES,
        KEY_FONT,
        KEY_PROCSET,
        KEY_F_LABEL,
        KEY_ENDLINE_S,
        KEY_TRAILER,
        KEY_SIZE,
        KEY_ROOT,
        KEY_BASE_FONT,
        KEY_ENCODING,
        KEY_SUBTYPE_s,
        KEY_SUBTYPE_b,
        KEY_FONT_DESCRIPTOR,
        KEY_FLAGS,
        KEY_FONTBBOX,
        KEY_ITALICANGLE,
        KEY_ASCENT,
        KEY_DESCENT,
        KEY_CAPHEIGHT,
        KEY_XHEIGHT,
        KEY_STEMV,
        KEY_STEMH,
        KEY_FONTFILE,
        KEY_FONTFILE2,
        KEY_LENGTH1,
        KEY_FILTER,
        _key_index_last_
    };

    enum key_line_index //3
    {
        KL_STREAM,
        KL_XREF,
        KL_STARTXREF,
        KL_TRAILER,
        KL_BT,
        KL_ET,
        _key_line_last_
    };

    enum procset_index //4
    {
        PROCSET_PDF,
        PROCSET_TEXT,
        PROCSET_IMAGE_A,
        PROCSET_IMAGE_B,
        PROCSET_IMAGE_C,
        PROCSET_IMAGE_I,
        _procset_index_last_
    };

    enum label_index //5
    {
        LABEL_F,
        _label_index_last_
    };

    enum typeid_index
    {
        type_index_index,
        key_index_index,
        key_line_index_index,
        procset_index_index,
        label_index_index
    };
}