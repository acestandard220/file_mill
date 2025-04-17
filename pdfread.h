#include <array>
#include <iostream>
#include <fstream>
#include <type_traits>
#include <unordered_map>
#include <vector>


namespace PDFREAD
{
  struct root_node;
  struct filedata
  {
    size_t file_bytes;
    size_t xref_start;
    size_t num_obj; 
    
    std::vector<std::array<int,3>> obj_offsets;
    root_node* root = nullptr;
  };

  struct page_collection
  {
    uint32_t* mPages;
    uint32_t nPages = 0;

    void GetPage(uint32_t index);//Go to chunks for the content
  };
  
  struct page 
  {
    
  };

  struct chunks
  {
    page* pages;
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
