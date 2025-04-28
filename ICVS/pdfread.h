#include <array>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <set>

#ifdef PDFREAD_EXPORTS
    #define ICVS_DLL __declspec(dllexport)
#else
    #define ICVS_DLL __declspec(dllimport)
#endif


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
        _type_index_last // Make sure it's always last
        // NOTE: Don't forget to update array size after additions...
    };


    enum sub_type_index
    {
        TYPE1,
        TYPE2
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
        RES_FONT,
        RES_PROCSET,
        _resource_type_last // Make sure it's always last
    };

    extern std::array<const std::string, _type_index_last> type_string;
    extern std::array<const std::string, base_font_last> base_font_string;
    extern std::array<const std::string, base_font_last> encoding_string;
    


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
    };

    struct filedata
    {
        size_t file_bytes;
        size_t xref_start;
        size_t xref_start_c;
        size_t num_obj;

        root_node* root = nullptr;

        std::vector<std::array<int, _xref_values_last>> obj_offsets;
        std::vector<bool> read_objects;//keep track of objects that have been read

        std::unordered_map<uint32_t, Page> cPage;
        std::unordered_map<uint32_t, Font> cFont;
        std::unordered_map<uint32_t, Content> cContent;

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
        std::unordered_map<std::string, uint32_t> rFonts;
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
        std::string tag;//might change
        uint16_t size;
        const std::string id = "Tf";
    };
    struct _tm
    {
        int32_t a, b, c, d, e, f;
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
    
}
