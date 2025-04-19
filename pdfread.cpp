#include "pdfread.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
int m =9;

namespace PDFREAD{

  filedata* data;

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
  }
 
  void Shutdonw()
  {

  }
}
