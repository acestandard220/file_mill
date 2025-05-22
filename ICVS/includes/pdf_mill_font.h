#pragma once
#include "pdf_mill_core.h"
#include "pdf_mill_obj.h"

namespace PDF_MILL
{

    struct _filedata;

    enum base_font
    {
        COURIER,
        COURIER_BOLD,
        COURIER_BOLDOBLIQUE,
        COURIER_OBLIQUE,
        HELVETICA,
        HELVETICA_BOLD,
        HELVETICA_BOLDOBLIQUE,
        HELVETICA_OBLIQUE,
        SYMBOL,
        TIMES_ROMAN,
        TIMES_ROMAN_BOLD,
        TIMES_ROMAN_BOLDITALIC,
        TIMES_ROMAN_ITALIC,
        ZAPFDINGBATS,
        _base_font_last_
    };

    enum encoding
    {
        MACEXPERTENCODING,
        MACROMANENCODING,
        WINANSIENCODING,
        _encoding_last_
    };

    enum font_parameters
    {
        SUB_TYPE_PARAM, BASE_FONT_PARAM, ENCODING_PARAM, FONT_DESCRIPTOR_PARAM, _font_parameter_last_
    };

    enum font_desc_param
    {
        flag_param,
        font_bbox_param,
        italic_angle_param,
        ascent_param,
        descent_param,
        cap_height_param,
        xheight_param,
        stemv_param,
        stemh_param,
        font_file_param,
        _font_desc_parameter_last_
    };

    enum sub_type_index
    {
        TYPE0, //Composite fonts
        TYPE1,
        TYPE2,
        TYPE3,
        TRUE_TYPE,
        MMTYPE1,
        //CIDFontType0,
        //CIDFontType2,
        _sub_type_index_last_
    };

    struct FontDescriptor : public Object
    {
        FontDescriptor() = default;
        FontDescriptor(uint32_t _id)
            :Object(_id, FONT_DESCRIPTOR)
        {

        }
        int flag;
        //FontBox font_box;
        int italic_angle;
        int ascent;
        int descent;
        int cap_height;
        int xheight;
        int stemV;
        int stemH;

        uint32_t font_file;
    };

    struct FontFile
    {
        size_t length = 0;
        size_t length1 = 0;

        std::string stream;
    };

    void read_font_obj(_filedata* filedata, std::ifstream& file, std::set<uint32_t> obj_index);
    void read_font_desc_obj(_filedata* filedata, std::ifstream& file, std::set<uint32_t>& obj_index);
    void read_font_file_obj(_filedata* filedata, std::ifstream& file, std::set<uint32_t> obj_index);

    extern "C" void changeFont(_filedata* filedata, int page_number, int tag, base_font font);
    extern "C" void addFont(_filedata* filedata, int page_number, base_font font, sub_type_index sub_type = TYPE1, encoding = WINANSIENCODING);

}