#include "pdfread.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
int m =9;

namespace PDFREAD{

  filedata* data;

  int get_length_to(std::string& line, int start , char stop = ' ')
  {
    int x = 0;
    for(int i = 0;i < line.length() - start; i++)
    {
      if(line[i + start] != stop)
      {
        x++;
      }
      else {
        break;  
      
      }
    }
    return x;
  }

  //TODO:
  //Seperate finding string keys into fnuctions 
  //Create uint32_ start variable
  //Tests
/*  bool validate_obj_type()
  {
     int x = get_length_to(line, line_index + search_length);
     if(line.substr(line_index + search_length, x) != "/Pages")
     {

     }
  }
  */

  void read_page_collector(std::ifstream& file)
  {
    file.seekg(data->obj_offsets[data->root->pages->id][0]);
    std::string line;
    while(std::getline(file,line))
    {
      std::string search = "/Type ";
      int search_length = search.length();
      int line_index = line.find(search);
      int line_length = line.length(); 

      if(line_index != std::string::npos)
      {
        int x = get_length_to(line, line_index + search_length);
        if(line.substr(line_index + search_length, x) != "/Pages")
        {
          std::cout<<"[ERROR]:[XREF CORRUPT]:: Invalid Page Collector Offset...\n";
          return;
        }
      }

      search = "/Count ";
      search_length = search.length();
      line_index = line.find(search);
      line_length = line.length();

      if(line_index != std::string::npos)
      {
        int x = get_length_to(line, line_index + search_length);
        data->root->pages->nPages = std::stoi(line.substr(line_index + search_length,x));
        data->root->pages->mPages = new uint32_t[data->root->pages->nPages];
      }
      search = "/Kids ";
      search_length = search.length();
      line_index = line.find(search);
      line_length = line.length();

      //Read kids
   }
  }
  // add breaks when we are certain we dont need anything else;
  void read_root_obj(std::ifstream& file)
  {
    
    file.seekg(data->obj_offsets[data->root->id][0]);
    std::string line;
    while(std::getline(file,line))
    {
      std::string search = "/Type ";
      int search_length = search.length();
      int line_index = line.find(search);
      int line_length = line.length();

      if(line_index != std::string::npos)
      {
        int x = 0;
        for(int i = 0;i < line_length - line_index - search_length; i++)
        {
          if(line[i + line_index +search_length] != ' ')
          {
            x++;
          }
        }
        if(line.substr(line_index + search_length,x) != "/Catalog")
        {
          std::cout<<"Corrupt XREF table, please repair pdf...\n";
          return ;
        }
      }

      data->root->pages = new page_collection;
      search = "/Pages ";
      search_length = search.length();
      line_index = line.find(search);
      line_length = line.length();

      if(line_index != std::string::npos)
      {  
        data->root->pages->id = std::stoi(line.substr(line_index + search_length,get_length_to(line,line_index + search_length, ' ')));
        continue;
      }
    }
  }
  //todo: Integrate With read_trailer_info
  void read_start_xref(std::ifstream* file)
  {
    std::string line;
    size_t global_index = 0;
    bool found = false;
    while(std::getline(*file,line))
    {
      int line_length = line.length();
      int line_index = line.find("startxref");
     
      if(line_index != std::string::npos)
      {
        found = true;     
        continue;
      }
      if(found)
      {
        int value = std::stoi(line);
        data->xref_start = value;
        std::cout<<"XREF::: "<<value<<std::endl;
        break;
      }
    }
  }

  void read_trailer_info(std::ifstream* file)
  {
    file->seekg(0);
    std::string line; 
    while(std::getline(*file,line))
    { 
      std::string search = "/Size ";  
      int search_length = search.length();
      int line_index = line.find(search);
      int line_length = line.length();
      
      if(line_index != std::string::npos)
      {  
         if(file->tellg() < data->xref_start)
         {
             continue; //skip any /Size attribute before the xref...
         }
         int integer_length = 0;
         for(int i = 0; i < line_length - line_index - search_length; i++)
         {
           if(line[i + line_index + search_length] != ' ')
           {
             integer_length++;
           }
         }
         data->num_obj = std::stoi(line.substr(line_index + search_length,integer_length));
         data->obj_offsets.resize(data->num_obj);
         //std::cout<<"Num_Obj::: "<<data->num_obj<<std::endl;
         continue;
      }
      
      search = "/Root ";
      line_length = line.length();
      line_index = line.find(search);

      search_length = search.length();

      if(line_index != std::string::npos)
      {
        int integer_length = 0;
        for(int i = 0; i < line_length - line_index - search_length; i++)
        {
          if(line[i + line_index + search_length] != ' ')
          {
            integer_length++;
          }
        }
        data->root->id = std::stoi(line.substr(line_index + search_length, integer_length));
        //std::cout<<"ss:: "<<data->root->id<<std::endl;
        break;
      }
    }
  }
  
  void read_obj_offsets(std::ifstream& file)
  {
    std::string line;

    file.seekg(data->xref_start); 
    std::getline(file,line);//read: xref
    std::getline(file,line);//read: 0-xxx 

    for(int i = 0; i< data->num_obj;i++)
    {  
      std::getline(file,line);

      data->obj_offsets[i][0] = std::stoi(line.substr(0,10));
      data->obj_offsets[i][1] = std::stoi(line.substr(11,5));
      data->obj_offsets[i][2] = (line.substr(17, 1) == "f") ? 0 : 1; //NOTE: f = 0 && n = 1
    }
  }

  void Initialize()
  {

    std::ifstream file("samplepdf2.pdf",std::ios::binary);
    if(!file.is_open())
    {
      std::cout<<"Could not open file...\n";
    }
    data = new filedata;

    file.seekg(0,std::ios::end);
    data->file_bytes = file.tellg();
    file.seekg(0,std::ios::beg);

    read_start_xref(&file);

    data->root = new root_node;
      
    read_trailer_info(&file);
    read_obj_offsets(file);

    read_root_obj(file);
  }
 
  void Shutdonw()
  {

  }
}
