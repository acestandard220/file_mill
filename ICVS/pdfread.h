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
        TYPE0,
        TYPE1,
        TYPE2,
        _sub_type_index_last
    };

    enum base_font
    {
        HELVETICA,
        ZAPFDINGBATS,

        base_font_last
    };

    enum encoding
    {
        WINANSIENCODING,

        encoding_last
    };

    enum resource_type
    {
        RESOURCE_FONT = KEY_FONT,
        RESOURCE_PROCSET = KEY_PROCSET,
        _resource_type_last // Make sure it's always last
    };

    extern std::array<const std::string, _type_index_last> type_string;
    extern std::array<const std::string, base_font_last> base_font_string;
    extern std::array<const std::string, base_font_last> encoding_string;
    extern std::array<const std::string, _key_index_last> key_string;
    extern std::array<const char*, _label_index_last>label_string;


    struct root_node;
    struct Page;
    struct Object;
    struct Font;
    struct Content;

    struct media_box
    {
        int _x, _y, _w, _h;

        media_box() = default;

        media_box(int x, int y, int w, int h)
            : _x(x), _y(y), _w(w), _h(h)
        { }

        std::array<int, 4> asArray() { return std::array<int, 4>{_x, _y, _w, _h}; }
    };

    struct read_filedata
    {
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
        std::unordered_map<uint32_t, Font> cFont;
        std::unordered_map<uint32_t, Content> cContent;
    };

    struct write_filedata
    {
        size_t file_bytes;
        size_t xref_start;
        size_t num_obj;

        uint32_t root_id;

        std::vector<std::array<int, _xref_values_last>> obj_offsets;
        std::array<int, _xref_values_last> __unique_onj_zero__;
    };
    struct page_collection
    {
        uint32_t id;
        std::vector<uint32_t> mPages;
        uint32_t nPages = 0;
        media_box* media_box = nullptr;
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
            :Object(id,PAGE)
        {
        }

        uint32_t parent;
        media_box media_box;
        std::vector<uint32_t> rContents;// reference to its content objects;
        std::unordered_map<int, uint32_t> rFonts;
        std::vector<std::string> cProcSet;
    };


    struct Font : public Object
    {
        Font() = default;
        Font(uint32_t _id)
            :Object(_id,FONT)
        {
        }

        sub_type_index sub_type;
        base_font base_font;
        encoding encoding;
        //To fiilll...
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

    extern "C" ICVS_DLL void WriteToFile();


    extern "C" ICVS_DLL void RequestPath(const char* path);
    
}
