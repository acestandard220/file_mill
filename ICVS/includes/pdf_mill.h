#pragma once


#include "pdf_mill_core_types.h"
#include "pdf_mill_obj.h"
#include "pdf_mill_root.h"
#include "pdf_mill_page.h"
#include "pdf_mill_font.h"
#include "pdf_mill_box.h"



constexpr const char* INDENT_2 = "  ";
constexpr const char* INDENT_4 = "    ";
constexpr const char* DICT_OPEN = "<<";
constexpr const char* DICT_CLOSE = ">>";

constexpr const char* endline = "\n";



namespace PDF_MILL
{
    // Don't touch
    enum media_box_values
    {
        X, Y, W, H
    };

    enum resource_type
    {
        RESOURCE_FONT = KEY_FONT,
        RESOURCE_PROCSET = KEY_PROCSET,
        _resource_type_last_
    };

    enum version_index
    {
        PDF_1_1,
        PDF_1_2,
        PDF_1_4,
        PDF_1_7,
        _version_index_last_
    };

    enum filter_index
    {
        UNKNOWN,
        FLATEDECODE,

        _filter_index_last_
    };




    extern std::array<const std::string, _filter_index_last_> filter_string;
    extern std::unordered_map<std::string, filter_index> filter_map;

    extern std::array<const std::string, _base_font_last_> base_font_string;
    extern std::array<const std::string, _base_font_last_> encoding_string;

    extern std::array<const std::string, _sub_type_index_last_> sub_type_string;
 


    extern std::array<const char*, _version_index_last_>version_string;
    extern std::array<const std::string, _resource_type_last_> resource_string;
    
    extern std::unordered_map<std::string, base_font> base_font_map;
    extern std::unordered_map<std::string, encoding> encoding_map;
    extern std::unordered_map<std::string, sub_type_index> sub_type_map;



    /////////
    


    extern std::unordered_map<char, char> pair;


    extern std::unordered_map<std::string, procset_index>procset_map;




    struct FixFileData
    {
        FixFileData();
        size_t file_bytes = 0;
        size_t xref_start = 0;
        size_t trailer_offset = 0;
        size_t num_obj = 0;

        std::vector<std::array<int, _xref_values_last_>> obj_offsets;
        bool faulty = false;
    };


    struct _filedata
    {
        _filedata();
        size_t file_bytes = 0;;
        size_t xref_start = 0;
        size_t num_obj = 0;
        size_t largest_obj_size = 0;

        std::string version = "";
        std::string eof = "%%EOF";

        Root* root = nullptr;

        std::vector<std::array<int, _xref_values_last_>> obj_offsets;
        std::vector<bool> read_objects; //keep track of objects that have been read

        std::unordered_map<uint32_t, Page> cPage;
        std::unordered_map<uint32_t, std::array<int, _font_parameter_last_>> cFont;
        std::unordered_map<uint32_t, FontDescriptor> cFontDescriptors;
        std::unordered_map<uint32_t, FontFile> cFontFile;
        std::unordered_map<uint32_t, FontBox> cFontBox;
        std::unordered_map<uint32_t, Content> cContent;

        std::string current_path;
        std::list<uint32_t> available_index;
    };

    struct WriteFileData
    {
        WriteFileData();

        size_t file_bytes;
        size_t xref_start;
        size_t num_obj;

        uint32_t root_id;

        std::vector<std::array<int, _xref_values_last_>> obj_offsets;
        std::array<int, _xref_values_last_> __unique_onj_zero__;

        std::string write_path;
    };

    struct NewFileData
    {
        NewFileData() = default;

        size_t file_bytes;
        size_t xref_start;
        size_t xref_start_c;
        size_t num_obj;

        std::string version = "";
        std::string eof = "%%EOF";

        Root* root = nullptr;

        std::vector<std::array<int, _xref_values_last_>> obj_offsets;

        std::unordered_map<uint32_t, Page> cPage;
        std::unordered_map<uint32_t, std::array<int, _font_parameter_last_>> cFont;
        std::unordered_map<uint32_t, std::array<int, _font_desc_parameter_last_>> cFontDescriptors; 
        std::unordered_map<uint32_t, FontFile> cFontFile;
        std::unordered_map<uint32_t, FontBox> cFontBox;
        std::unordered_map<uint32_t, Content> cContent;

    };

    struct GlobalContext
    {
        std::vector<std::shared_ptr<_filedata>> file_reads;
        std::vector<std::shared_ptr<WriteFileData>> file_writes;
        std::vector<std::shared_ptr<NewFileData>> new_writes;

        std::shared_ptr<_filedata> cur_file_read;
        std::shared_ptr<WriteFileData> cur_file_write;
        std::shared_ptr<FixFileData> fix_data;
        std::shared_ptr<NewFileData> cur_new_write;
    };

    struct _tf
    {
        int tag;
        uint16_t size;
        const std::string id = "Tf";

        inline std::string stringify()
        {
            return label_string[LABEL_F] + std::to_string(tag) + " " + std::to_string(size) + " " + id;
        }
    };

    struct _tm
    {
        int32_t a, b, c, d, e, f;
        std::vector<int32_t> asArray() const { return std::vector<int32_t>{a, b, c, d, e, f}; }
    };

    struct _tj
    {
        std::string text = "";
    };

    extern GlobalContext* global_data;

    extern "C" ICVS_DLL void Initialize();

    extern "C" ICVS_DLL void ReadToStructure();
    extern "C" ICVS_DLL void ShutDown();
    extern "C" ICVS_DLL void RequestReadPath(const char* path);
    
    extern "C" ICVS_DLL _filedata* GetFileData();

    extern "C" ICVS_DLL void UpdateVersion(version_index version);
    
    extern "C" ICVS_DLL void WriteToFile();
    extern "C" ICVS_DLL void RequestWritePath(std::string path);
    
    extern "C" ICVS_DLL void WriteNewPDF();


}

//ERRORS in pdf
//When the pdf page has no content references.
// 
//...

//BUG: Some pages repeat themselves during excessive removal and additions...