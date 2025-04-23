#include <array>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <vector>
#include <sstream>

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
        _type_index_last // Make sure it's always last
        // NOTE: Don't forget to update array size after additions...
    };

    enum resource_type
    {
        RES_FONT,
        RES_PROCSET,
        _resource_type_last // Make sure it's always last
    };

    extern std::array<const std::string, _type_index_last> type_string;

    struct root_node;
    struct Page;
    struct Object;
    struct Font;
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

        std::unordered_map<uint32_t, Page> cPage;
        std::unordered_map<uint32_t, Font> cFont;

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
    };


    struct Font : public Object
    {
        Font() = default;
        Font(uint32_t _id)
            :Object(_id,FONT)
        {
        }

        //To fiilll...
    };

    struct root_node
    {
        uint32_t id;
        page_collection* pages = nullptr;
    };

    void Initialize();
    void ShutDown();
}
