#include "includes/pdf_mill.h"
#include "includes/pdf_mill_document.h"

int m = 9; //Let him be.

namespace PDF_MILL {    

    void __get_actual_start_xref(std::ifstream& file)
    {
        global_data->fix_data->faulty = true;

        file.clear();
        std::string line;
        while (std::getline(file, line))
        {
            if (line.find("xref") != std::string::npos)
            {
                global_data->fix_data->xref_start = static_cast<size_t>(file.tellg()) - 1 - line.length();
            }
            if (line.find("trailer") != std::string::npos)
            {
                global_data->fix_data->trailer_offset = file.tellg();
            }
            int start = line.find("/Size ");
            if (start != std::string::npos && global_data->fix_data->trailer_offset != 0)
            {
                if (global_data->fix_data->trailer_offset > file.tellg()) { continue; }
                int integer_length = get_length_to(line, start + 6);
                global_data->fix_data->num_obj = std::stoi(line.substr(start + 6, integer_length));
                global_data->fix_data->obj_offsets.resize(global_data->fix_data->num_obj);
                break;
            }
        }

        file.clear();
        file.seekg(0);
        while (std::getline(file, line))
        {
            if (line.find("0 obj") != std::string::npos)
            {
                int len = get_length_to(line, 0, ' ');
                int index = std::stoi(line.substr(0, len));

                global_data->fix_data->obj_offsets[index][BYTE_OFFSET] = static_cast<int>(file.tellg()) - 8;
                global_data->fix_data->obj_offsets[index][UID] = 0;
                global_data->fix_data->obj_offsets[index][FLAG] = 1;
            }
        }
    }

    void read_start_xref(std::ifstream* file)
    {
        std::string line;

        bool found = false;
        while (std::getline(*file, line))
        {
            int line_length = line.length();
            int line_index = line.find("startxref");

            if (line_index != std::string::npos)
            {
                found = true;
                continue;
            }
            if (found)
            {
                int value = std::stoi(line);
                file->seekg(value);

                std::string line;
                std::getline(*file, line);
                if (line.find("xref") != std::string::npos)
                {

                    global_data->cur_file_read->xref_start = value;
                }
                else {
                    std::cout << "[WARNING]:[XREF]: Invalid xref start value...\n";
                    __get_actual_start_xref(*file);
                    global_data->cur_file_read->xref_start = global_data->fix_data->xref_start;
                }
                break;
            }
        }
    }

    void read_trailer_info(std::ifstream* file)
    {
        file->clear();
        file->seekg(0);
        std::string line;
        while (std::getline(*file, line))
        {
            int start = has_key(line, KEY_SIZE);

            if (start)
            {
                if (file->tellg() < global_data->cur_file_read->xref_start)
                {
                    continue; //skip any /Size attribute before the xref...
                }

                int integer_length = get_length_to(line, start);
                global_data->cur_file_read->num_obj = std::stoi(line.substr(start, integer_length));
                global_data->cur_file_read->obj_offsets.resize(global_data->cur_file_read->num_obj);
                global_data->cur_file_read->obj_offsets.resize(global_data->cur_file_read->num_obj);
                
                std::cout << "Num_Obj::: " << global_data->cur_file_read->num_obj << std::endl;
            }

            start = has_key(line, KEY_ROOT);
            if (start)
            {
                int integer_length = get_length_to(line, start);
                //global_data->cur_file_read->root->id = std::stoi(line.substr(start, integer_length));
                uint32_t temp_index = std::stoi(line.substr(start, integer_length));
                global_data->cur_file_read->root = new Root(temp_index, CATALOG);

                std::cout << "ss:: " << GetRootID(global_data->cur_file_read.get()) << std::endl;
                continue;
            }

            start = has_key(line, KEY_INFO);
            if (start)
            {
                int integer_length = get_length_to(line, start);
                uint32_t info_obj_index = std::stoi(line.substr(start, integer_length));
                global_data->cur_file_read->info = new Info(info_obj_index);
            }
        }
    }


    size_t get_largest_offset_difference()
    {
        std::set<size_t> off;
        for (auto x : global_data->cur_file_read->obj_offsets)
        {
            off.insert(x[BYTE_OFFSET]);
        }


        size_t result = 0;
        for (auto it = off.begin(); it != off.end(); it++)
        {
            if (std::next(it) != off.end())
            {
                size_t temp = (*it - *std::next(it));
                result = (temp > result) ? temp : result;
            }
        }

        return result;
    }

    void read_obj_offsets(std::ifstream& file)
    {
        if (global_data->fix_data->faulty)
        {
            global_data->cur_file_read->obj_offsets = global_data->fix_data->obj_offsets;
            return;
        }

        std::string line;

        file.seekg(global_data->cur_file_read->xref_start);
        std::getline(file, line); //read: xref
        std::getline(file, line); //read: 0-xxx 

        for (int i = 0; i < global_data->cur_file_read->num_obj; i++)
        {
            std::getline(file, line);

            try
            {
                global_data->cur_file_read->obj_offsets[i][BYTE_OFFSET] = std::stoi(line.substr(0, 10));
            }
            catch (std::invalid_argument& e)
            {
                std::cout << "[ERROR]:[XREF_OFFSETS]:Invalid offset data\n";
            }
            try
            {
                global_data->cur_file_read->obj_offsets[i][UID] = std::stoi(line.substr(11, 5));
            }
            catch (std::invalid_argument& e)
            {
                std::cout << "[ERROR]:[XREF_OFFSETS]:Invalid offset data\n";
            }
            global_data->cur_file_read->obj_offsets[i][FLAG] = (line.substr(17, 1) == "f") ? 0 : 1; //NOTE: f = 0 && n = 1
        }

        global_data->cur_file_read->largest_obj_size = get_largest_offset_difference();
    }

  

    void ReadToStructure()
    {
        std::ifstream file(global_data->cur_file_read->current_path, std::ios::binary);
        if (!file.is_open())
        {
            std::cout << "Could not open file...\n";
        }
        
        std::getline(file, global_data->cur_file_read->version);
        file.seekg(0, std::ios::end);
        global_data->cur_file_read->file_bytes = file.tellg();
        file.seekg(0, std::ios::beg);
        

        read_start_xref(&file);

        read_trailer_info(&file);

        read_obj_offsets(file);
        
        read_info_obj(global_data->cur_file_read.get(), file);
        read_root_obj(global_data->cur_file_read.get(),file);
        read_page_collector(global_data->cur_file_read.get(),file);
        read_page_data(global_data->cur_file_read.get(),file);

        file.close();
    }

    void RequestReadPath(const char* path)
    {
        global_data->file_reads.emplace_back(std::make_shared<_filedata>());
        global_data->cur_file_read = global_data->file_reads[0];

        global_data->cur_file_read->current_path = path;
    }

    _filedata* PDF_MILL::GetCurFileData()
    {
        return global_data->cur_file_read.get();
    }
    

    std::vector<std::shared_ptr<_filedata>>& GetFileReadArray()
    {
        return global_data->file_reads;
    }

    void ShutDown()
    {
        std::cout << "Shutdown function has beend called\n";
        //global_data->cur_file_read.reset();
    }

    Object add_obj(type_index type)
    {
        uint32_t index = global_data->cur_file_read->num_obj;
        global_data->cur_file_read->num_obj++;

        return Object(index, type);
    }
}


