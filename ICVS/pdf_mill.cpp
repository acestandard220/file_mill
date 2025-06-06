#include "includes/pdf_mill.h"

namespace PDF_MILL
{
    std::array<const std::string, _marker_index_last_> marker_string {
        "Tf",
        "Tj",
        "Tm",
        "Td",
        "w",
        "RG",
        "rg",
        "re"
    };

    std::array<const std::string, _filter_index_last_> filter_string
    {
         "__unknown__",
         "/FlateDecode"
    };

    std::unordered_map<std::string, filter_index> filter_map
    {
        {"/FlateDecode", FLATEDECODE}
    };

    std::array<const char*, _version_index_last_>version_string{ 
        "%PDF-1.0",
        "%PDF-1.2",
        "%PDF-1.4",
        "%PDF-1.7" 
    };
   
    std::array<const std::string, _resource_type_last_> resource_string = { 
        "/Font",
        "/ProcSet" 
    };
  
    std::array<const std::string, _type_index_last_> type_string{
       "/Catalog",
       "/Pages",
       "/Page",
       "endobj",
       "/Font",
       "/Contents",
       "/FontDescriptor",
       "stream",
       "/FontFile",
       "/Outlines",
       "OutlineItem",
       "/Info"
    };

    std::array<const std::string, _key_index_last_> PDF_MILL::key_string{
        "/Type",
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
        "\n",
        "trailer",
        "/Size",
        "/Root",
        "/BaseFont",
        "/Encoding",
        "/SubType",
        "/Subtype",
        "/FontDescriptor",
        "/Flags",
        "/FontBBox",
        "/ItalicAngle",
        "/Ascent",
        "/Descent",
        "/CapHeight",
        "/XHeight",
        "/StemV",
        "/StemH",
        "/FontFile",
        "/FontFile2",
        "/Length1",
        "/Filter",
        "/Outlines",
        "__OutlineItem__",
        "/Title",
        "/Next",
        "/Prev",
        "/First",
        "/Last",
        "/Parent",
        "/Dest",
        "/Fit",
        "/FitH",
        "/FitR",
        "/FitB",
        "/FitV",
        "/FitBH",
        "/FitBV",
        "/Info",
        "/Producer",
        "/Creator",
        "/CreationDate",
        "/ModDate"
    };

    std::array<const std::string, _key_line_last_> key_line_string{
        "stream",
        "xref",
        "startxref",
        "trailer",
        "BT",
        "ET"
    };

    std::array<const std::string, _label_index_last_>label_string{
        "/F"
    };

    std::unordered_map<char, char> pair{
        {'[', ']'},
        {'{', '}'},
        {'(', ')'}
    };

    std::unordered_map<char, char> pair2 {
       {']', '['},
       {'}', '{'},
       {')', '('}
    };


    std::array<const std::string, _procset_index_last_> procset_string{
        "/PDF",
        "/Text",
        "/ImageA",
        "/ImageB",
        "/ImageC",
        "/ImageI"
    };

    std::unordered_map<std::string, procset_index>procset_map{
        {"/PDF",    PROCSET_PDF},
        {"/Text",   PROCSET_TEXT},
        {"/ImageA", PROCSET_IMAGE_A},
        {"/ImageB", PROCSET_IMAGE_B},
        {"/ImageC", PROCSET_IMAGE_C},
        {"/ImageI", PROCSET_IMAGE_I},
    };

    std::array<const std::string, _base_font_last_> base_font_string = {
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
    std::array<const std::string, _sub_type_index_last_> sub_type_string{
        "/Type0",
        "/Type1",
        "/Type2",
        "/Type3",
        "/TrueType",
        "MMType1"
    };
    std::array<const std::string, _base_font_last_> encoding_string{
        "/MacExpertEncoding",
        "/MacRomanEncoding",
        "/WinAnsiEncoding"
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

    GlobalContext* global_data = nullptr;

    void Initialize()
    {
        global_data = new GlobalContext;
        global_data->fix_data = std::make_shared<FixFileData>();
    }

    _filedata::_filedata()
    {
    }

    FixFileData::FixFileData()
    {
    }

    WriteFileData::WriteFileData()
    {
    }

}