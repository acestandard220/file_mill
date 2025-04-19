#include <array>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <vector>


namespace PDFREAD
{
  enum type_index
  {
    CATALOG,
    PAGES,
    PAGE,
    ENDOBJ,
    FONT,
    //NOTE: Don't forget to update  array size after additions...
  };

  extern std::array<const std::string, 10> type_string;

  struct root_node;
  struct page;

  struct media_box
  {
      int x, y, w, h;
  };

  struct filedata
  {
    size_t file_bytes;
    size_t xref_start;
    size_t xref_start_c;
    size_t num_obj; 
    
    root_node* root = nullptr;
    
    std::vector<std::array<int,3>> obj_offsets;
    std::vector<media_box> cMediaBox;
    std::vector<page*> cPage;
  };

  struct page_collection
  {
    uint32_t id;
    std::vector<int> mPages;
    uint32_t nPages = 0;
    media_box* media_box = nullptr;
  };
  
  struct page 
  {
    uint32_t id;
    uint32_t parent;
    uint32_t mMediaBbox;
    uint32_t* mContents;
  };

  struct root_node
  {
    uint32_t id;
    page_collection* pages = nullptr; 
  };
  
  struct obj 
  {
    uint32_t id;
  };
  
 
  void Initialize();

  
  void ShutDown();
}
