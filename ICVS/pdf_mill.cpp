#include "includes/pdf_mill.h"

namespace PDF_MILL
{
    std::array<const char*, _version_index_last>version_string{ 
        "%PDF-1.0",
        "%PDF-1.2",
        "%PDF-1.4",
        "%PDF-1.7" 
    };
    std::array<const std::string, _type_index_last> type_string{
        "/Catalog",
        "/Pages",
        "/Page",
        "endobj",
        "/Font",
        "/Contents",
        "/FontDescriptor",
        "stream"
    };
    std::array<const std::string, _key_index_last> key_string{
        "/Count",
        "/Contents",
        "/Kids",
        "/Pages",
        "/MediaBox",
        "/Length",
        "/Resources",
        "/Font",
        "/ProcSet",
        "/F",
        endline,
        "trailer",
        "/Size",
        "/Root",
        "/BaseFont",
        "/Encoding" };
    std::array<const std::string, _key_line_last> key_line_string = { 
        "stream",
        "xref",
        "startxref",
        "trailer" 
    };
    std::array<const std::string, _resource_type_last> resource_string = { 
        "/Font",
        "/ProcSet" 
    };
    std::array<const char*, _label_index_last>label_string{ "/F" };

    std::unordered_map<char, char> pair = { 
        {'[', ']'}, 
        {'{', '}'}, 
        {'(', ')'} 
    };


    std::array<const std::string, base_font_last> base_font_string = {
        "/Courier",
        "/Courier-Bold",
        "/Courier-BoldOblique",
        "/Courier-Oblique",
        "/Helvetica",
        "/Helvetica-Bold",
        "/Helvetica-BoldOblique",
        "/Helvetica-Oblique",
        "/Symbol",
        "/Times-Roman",
        "/Times-Roman-Bold",
        "/Times-Roman-BoldItalic",
        "/Times-Roman-Italic",
        "/ZapfDingbats"
    };
    std::array<const std::string, _sub_type_index_last> sub_type_string{
        "/Type0",
        "/Type1",
        "/Type2",
        "/Type3",
        "/TrueType",
        "MMType1"
    };
    std::array<const std::string, base_font_last> encoding_string{
        "/MacExpertEncoding",
        "/MacRomanEncoding",
        "/WinAnsiEncoding"
    };

    std::array<const std::string, _procset_index_last> procset_string{
       "/PDF",
       "/Text",
       "/ImageA",
       "/ImageB",
       "/ImageC",
       "/ImageI"
    };

    std::unordered_map<std::string, procset_index>procset_map{
        {"/PDF",   PROCSET_PDF},
        {"/Text",  PROCSET_TEXT},
        {"/ImageA",PROCSET_IMAGE_A},
        {"/ImageB",PROCSET_IMAGE_B},
        {"/ImageC",PROCSET_IMAGE_C},
        {"/ImageI",PROCSET_IMAGE_I},
    };

    std::unordered_map<std::string, base_font> base_font_map{
        {"/Courier", COURIER},
        {"/Courier-Bold", COURIER_BOLD},
        {"/Courier-BoldOblique", COURIER_BOLDOBLIQUE},
        {"/Courier-Oblique", COURIER_OBLIQUE},
        {"/Helvetica", HELVETICA},
        {"/Helvetica-Bold", HELVETICA_BOLD},
        {"/Helvetica-BoldOblique", HELVETICA_BOLDOBLIQUE},
        {"/Helvetica-Oblique", HELVETICA_OBLIQUE},
        {"/Symbol", SYMBOL},
        {"/Times-Roman", TIMES_ROMAN},
        {"/Times-Roman-Bold", TIMES_ROMAN_BOLD},
        {"/Times-Roman-BoldItalic", TIMES_ROMAN_BOLDITALIC},
        {"/Times-Roman-Italic", TIMES_ROMAN_ITALIC},
        {"/ZapfDingbats", ZAPFDINGBATS}
    };

    std::unordered_map<std::string, encoding> encoding_map{
        {"/MacExpertEncoding",MACEXPERTENCODING},
        {"/MacRomanEncoding",MACROMANENCODING},
        {"/WinAnsiEncoding",WINANSIENCODING},
    };

    std::unordered_map<std::string, sub_type_index> sub_type_map{
        {"/Type0",TYPE0},
        {"/Type1",TYPE1},
        {"/Type2",TYPE2},
        {"/Type3",TYPE3},
        {"/TrueType",TRUE_TYPE},
        {"MMType1",MMTYPE1}
    };

    std::array<std::string, 5> text_block_markers{ 
        "Tf",
        "Tm",
        "Tj" 
    };

    global_file_instance* global_data = nullptr;

    void Initialize()
    {
        global_data = new global_file_instance; 
        global_data->fix_data = std::make_shared<fix_filedata>();
    }

}