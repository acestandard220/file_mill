#include <array>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <set>
#include <iomanip>

#ifdef PDFREAD_EXPORTS
#define ICVS_DLL __declspec(dllexport)
#else
#define ICVS_DLL __declspec(dllimport)
#endif

//#define INDENT for (int i = 0; i < indent_track + 2; i++) { out += " "; }

constexpr const char* INDENT_2 = "  ";
constexpr const char* INDENT_4 = "    ";
constexpr const char* DICT_OPEN = "<<";
constexpr const char* DICT_CLOSE = ">>";

constexpr const char* endline = "\n";


//TODO: Implement custom indent macro...

std::string current_path;
namespace PDFREAD
{
    // Don't touch
    enum media_box_values
    {
        X, Y, W, H
    };

    enum xref_values
    {
        BYTE_OFFSET,
        UID,
        FLAG,
        _xref_values_last
    };

    enum type_index
    {
        CATALOG,
        PAGES,
        PAGE,
        ENDOBJ,
        FONT,
        CONTENT,
        STREAM,
        FONT_DESCRIPTOR,

        _type_index_last // Make sure it's always last
        // NOTE: Don't forget to update array size after additions...
    };

    enum key_index
    {
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
        _key_index_last
    };

    enum key_line_index {
        KL_STREAM,
        KL_XREF,
        KL_STARTXREF,
        KL_TRAILER,
        _key_line_last
    };
    enum label_index
    {
        LABEL_F,

        _label_index_last
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
        _sub_type_index_last
    };

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
        base_font_last
    };
    enum encoding
    {
        MACEXPERTENCODING,
        MACROMANENCODING,
        WINANSIENCODING,
        encoding_last
    };

    enum resource_type
    {
        RESOURCE_FONT = KEY_FONT,
        RESOURCE_PROCSET = KEY_PROCSET,
        _resource_type_last // Make sure it's always last
    };

    enum version_index
    {
        PDF_1_1,
        PDF_1_2,
        PDF_1_4,
        PDF_1_7,
        _version_index_last
    };

    enum font_parameters
    {
        sub_type_param,base_font_param,encoding_param,font_descriptor_param,_font_parameter_last
    };

    enum _font_desc_param
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
        _font_desc_parameter_last
    };
    

    extern std::array<const std::string, _type_index_last> type_string;
    extern std::array<const std::string, base_font_last> base_font_string;
    extern std::array<const std::string, base_font_last> encoding_string;
    extern std::array<const std::string, _key_index_last> key_string;
    extern std::array<const char*, _label_index_last>label_string;
    extern std::array<const char*, _version_index_last>version_string;


    struct root_node;
    struct Page;
    struct Object;
    struct Content;
    struct Font_Descriptor;
    struct Font_File;

    struct _box
    {
        int _x, _y, _w, _h;

        _box() = default;
        _box(int x, int y, int w, int h)
            : _x(x), _y(y), _w(w), _h(h)
        {
        }

        std::array<int, 4> asArray() { return std::array<int, 4>{_x, _y, _w, _h}; }
    };

    struct media_box :public _box
    {

        media_box() = default;

        media_box(int x, int y, int w, int h)
            :_box(x,y,w,h)
        {
        }
    };
    struct font_box :public _box
    {
        font_box() = default;
        font_box(int x, int y, int w, int h)
            :_box(x, y, w, h)
        {

        }
    };

    struct fix_filedata
    {
        size_t file_bytes;
        size_t xref_start;
        size_t trailer_offset = 0;
        size_t num_obj;

        std::vector<std::array<int, _xref_values_last>> obj_offsets;
        bool faulty = false;
    };

    struct read_filedata
    {

        read_filedata() = default;
        size_t file_bytes;
        size_t xref_start;
        size_t xref_start_c;
        size_t num_obj;

        std::string version;
        std::string eof = "%%EOF";

        root_node* root = nullptr;

        std::vector<std::array<int, _xref_values_last>> obj_offsets;
        std::vector<bool> read_objects;//keep track of objects that have been read

        std::unordered_map<uint32_t, Page> cPage;
        std::unordered_map<uint32_t, std::array<int, _font_parameter_last>> cFont;
        std::unordered_map<uint32_t, std::array<int, _font_desc_parameter_last>> cFontDescriptors;
        std::unordered_map<uint32_t, Font_File> cFontFile;
        std::unordered_map<uint32_t, font_box> cFontBox;
        std::unordered_map<uint32_t, Content> cContent;

    };

    struct write_filedata
    {
        write_filedata() = default;

        size_t file_bytes;
        size_t xref_start;
        size_t num_obj;

        uint32_t root_id;

        std::vector<std::array<int, _xref_values_last>> obj_offsets;
        std::array<int, _xref_values_last> __unique_onj_zero__;
    };

    struct global_file_instance
    {
        std::vector<std::shared_ptr<read_filedata>> file_reads;
        std::vector<std::shared_ptr<write_filedata>> file_writes;

        std::shared_ptr<read_filedata> cur_file_read;
        std::shared_ptr<write_filedata> cur_file_write;
        std::shared_ptr<fix_filedata> fix_data;
    };
    struct page_collection
    {
        uint32_t id;
        std::vector<uint32_t> mPages;
        uint32_t nPages = 0;
        media_box* media_box = nullptr;

        const int get_obj_index(int page_num)const { return mPages[page_num-1]; }

    };

    struct Object
    {
    public:
        Object() {};
        Object(uint32_t _id, type_index _type) {
            id = _id;
            type = _type;
        }
        uint32_t id;
        type_index type;
    };

    struct Page : public Object
    {
        Page() = default;

        Page(uint32_t _id)
            :Object(id, PAGE)
        {
        }

        uint32_t parent;
        media_box media_box;
        std::vector<uint32_t> rContents;// reference to its content objects;
        std::unordered_map<int, uint32_t> rFonts;
        std::vector<std::string> cProcSet;

        int highest_font_tag = 1;
    };

   /* struct Font_Descriptor : public Object
    {
        Font_Descriptor() = default;
        Font_Descriptor(uint32_t _id)
            :Object(_id, FONT_DESCRIPTOR)
        {

        }
        int flag;
        font_box font_box;
        int italic_angle;
        int ascent;
        int descent;
        int cap_height;
        int xheight;
        int stemV;
        int stemH;

        uint32_t font_file;
    };*/

    struct Font_File
    {
        size_t length;
        size_t length1;

        std::string stream;
    };

    struct _tf
    {
        int tag;//might change
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

    struct TextBlock
    {
        _tf* Tf;
        _tm* Tm;
        _tj* Tj;
    };

    struct Content : public Object
    {
        Content() = default;
        Content(uint32_t _id)
            :Object(_id, CONTENT)
        {
        }
        size_t stream_length = 0;
        std::vector<TextBlock> BT_ETs;
    };

    struct root_node
    {
        uint32_t id;
        page_collection* pages = nullptr;
    };


    extern "C" ICVS_DLL void Initialize();
    extern "C" ICVS_DLL void ShutDown();
    extern "C" ICVS_DLL void RequestPath(const char* path);
    
    extern "C" void FixXref(std::string& path);
    
    extern "C" ICVS_DLL void AddPage();
    extern "C" ICVS_DLL void UpdateVersion(version_index version);
    
    extern "C" ICVS_DLL void ChangeFont(int page_num, int tag,base_font font);
    extern "C" ICVS_DLL void AddFont(int page_num, base_font font, sub_type_index sub_type = TYPE1, encoding = WINANSIENCODING);

    extern "C" ICVS_DLL void WriteToFile();



}