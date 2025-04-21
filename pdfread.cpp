#include "pdfread.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
int m = 9;

namespace PDFREAD {

    std::array<const std::string, 10> type_string{ "/Catalog", "/Pages", "Page", "endobj" };
    std::unordered_map<char, char> pair = { {'[', ']'}, {'{', '}'}, {'(', ')'} };
    filedata* data;

    int get_length_to(std::string& line, int start, char stop = ' ')
    {
        int x = 0;
        for (int i = 0; i < line.length() - start; i++)
        {
            if (line[i + start] != stop)
            {
                x++;
            }
            else {
                break;
            }
        }
        return x;
    }

    std::vector<int> get_array_objs(std::string& line, int start, char delimiter = ' ')
    {
        std::vector<int> r;
        int objs_start = start + 1;
        int length_to_close = get_length_to(line, objs_start, pair[line[start]]);
        std::string linear_data = line.substr(objs_start, length_to_close);

        std::string value;
        std::stringstream stream(linear_data);

        bool first = true;
        while (std::getline(stream, value, 'R'))
        {
            int index = value[0] != ' ' ? 0:1;
            int len = get_length_to(value, index);
            std::string _v = value.substr(index, len);       
            try
            {
                r.push_back(std::stoi(_v));
            }
            catch (const std::invalid_argument& e)
            {
                std::cout << "Does not contain object index index: Invalid XREF";
                return std::vector<int>();
            }
            catch (const std::out_of_range& e)
            {
                std::cout << "Number is too huge\n";
                return std::vector<int>();
            }
        }
        return r;
    }
    //Read 4 values from an array [x x x x]
    std::vector<int> get_array_values(std::string& line, int start, char delimiter = ' ')
    {
        //0123456789A
        //0 0 900  900
        std::vector<int> r;
        int obj_start = start + 1;
        int length_to_close = get_length_to(line, obj_start, pair[line[start]]);
        std::string linear_data = line.substr(obj_start, length_to_close);

        std::string value;
        std::stringstream stream(linear_data);
        while (std::getline(stream, value,delimiter))
        {
            try
            {
                r.push_back(std::stoi(value));
            }
            catch (std::invalid_argument& e)
            {
                std::cout << "Corrupt XREF...\n";
                return std::vector<int>();
            }
            catch (std::out_of_range& e)
            {
                std::cout << "Corrupt XREF...\n";
                return std::vector<int>();
            }
        }
        return r;
    }
    //todo: think about delimiter param
    std::vector<uint32_t> get_indirect_array_obj(std::string& line, int start, char delimiter = ' ')
    {
        std::vector<uint32_t> r;
        int objs_start = start;
        int len = get_length_to(line, objs_start, '/');
        std::string linear_data = line.substr(objs_start, len);

        std::string value;
        std::stringstream stream(linear_data);
        while (std::getline(stream, value))
        {
            int index = value[0] != ' ' ? 0 : 1;
            int len = get_length_to(value, index);
            std::string _v = value.substr(index, len);

            try
            {
                r.push_back(std::stoi(_v));
            }
            catch(std::invalid_argument& e)
            {
                std::cout << "Corrupt XREF...\n";
                return std::vector<uint32_t>();
            }
            catch (std::out_of_range& e)
            {
                std::cout << "Corrupt XREF...\n";
                return std::vector<uint32_t>();
            }
        }
        return r;
    }

    int has_key(std::string& line, const std::string& key)
    {
        int line_index = line.find(key);
        int search_length = key.length();
        if (line.find(key) != std::string::npos)
        {
            return line_index + search_length; //true
        } else {
            return 0; //false
        }
    }

   
    bool validate_obj_type(std::string& line, type_index type)
    {
        const std::string temp = "/Type ";
        int start = has_key(line, temp);

        int x = get_length_to(line, start);
        std::string sub = line.substr(start, x);
        if (sub.find(type_string[type]) == std::string::npos)
        {
            std::cout << "[ERROR]:[XREF CORRUPT]:: Invalid " << type_string[type] << " Offset...\n";
            return false;
        }

        return true;
    }

    bool end_of_obj(std::string& line)
    {
        return (line.find(type_string[ENDOBJ]) != std::string::npos);
    }

    void read_font_obj(std::ifstream& file)
    {

    }

    void read_page_data(std::ifstream& file)
    {
        std::string line;
        for (auto& page_obj_index : data->root->pages->mPages)
        {
            file.clear();
            file.seekg(data->obj_offsets[page_obj_index][0], std::ios::beg);

            while (std::getline(file, line))
            {
                if (end_of_obj(line)) { break; }
                int start = has_key(line, "/Type ");
                if (start)
                {
                    if (!validate_obj_type(line, PAGE))
                    {
                        return;
                    }
                }

                start = has_key(line, "/MediaBox ");
                if (start)
                {
                    auto g = get_array_values(line, start, ' ');
                    data->cMediaBox[page_obj_index] = media_box (g[0], g[1], g[2], g[3]);
                }

                start = has_key(line, "/Contents ");
                if (start)
                {
                    data->cContent[page_obj_index] = get_indirect_array_obj(line, start);
                    std::cout<<"";
                }
            }
        }
    }

    void read_page_collector(std::ifstream& file)
    {
        file.seekg(data->obj_offsets[data->root->pages->id][0]);
        std::string line;

        while (std::getline(file, line))
        {
            if (end_of_obj(line)) { break; }
            std::string search = "/Type ";
            int start = has_key(line, search);

            if (start)
            {
                if (!validate_obj_type(line, PAGES))
                {
                    return;
                }
            }
            search = "/Count ";
            start = has_key(line, search);
            if (start)
            {
                int x = get_length_to(line, start);
                data->root->pages->nPages = std::stoi(line.substr(start, x));
                std::cout << "Page Count::: " << data->root->pages->nPages << std::endl;
            }
            search = "/Kids ";
            start = has_key(line, search);
            if (start)
            {
                data->root->pages->mPages = get_array_objs(line, start);
               
                if (data->root->pages->mPages.size() == data->root->pages->nPages)
                {
                    //Todo: Page number info does not match...
                    //Todo: This is a ::PDFFix feature to be implemented later...
                    std::cout << "Page number indicated and pages found do not match\n";
                }
                continue;
            }
            std::string subbed = line.substr(start);
        }
    }

    // add breaks when we are certain we dont need anything else;
    void read_root_obj(std::ifstream& file)
    {
        file.clear();
        file.seekg(data->obj_offsets[data->root->id][0]);
        std::string line;

        while (std::getline(file, line))
        {
            if (line.find(type_string[ENDOBJ]) != std::string::npos)
            {
                std::cout << "End of object reached\n";
                break;
            }

            std::string search = "/Type ";
            int start = has_key(line, search);

            if (start)
            {
                if (!validate_obj_type(line, CATALOG))
                {
                    return;
                }
            }

            data->root->pages = new page_collection;
            search = "/Pages ";
            start = has_key(line, search);

            if (start)
            {
                data->root->pages->id = std::stoi(line.substr(start, get_length_to(line, start)));
                std::cout << "Page Collector ID::: " << data->root->pages->id << std::endl;
                break;
            }
        }
    }

    void read_start_xref(std::ifstream* file)
    {
        std::string line;
        size_t global_index = 0;
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
                data->xref_start = value;
                break;
            }
        }
    }

    void read_trailer_info(std::ifstream* file)
    {
        file->seekg(0);
        std::string line;
        while (std::getline(*file, line))
        {
            std::string search = "/Size ";
            int start = has_key(line, search);

            if (start)
            {
                if (file->tellg() < data->xref_start)
                {
                    continue; //skip any /Size attribute before the xref...
                }
                int integer_length = get_length_to(line, start);
                data->num_obj = std::stoi(line.substr(start, integer_length));
                data->obj_offsets.resize(data->num_obj);
                std::cout << "Num_Obj::: " << data->num_obj << std::endl;
                continue;
            }

            search = "/Root ";
            start = has_key(line, search);

            if (start)
            {
                int integer_length = get_length_to(line, start);
                data->root->id = std::stoi(line.substr(start, integer_length));
                std::cout << "ss:: " << data->root->id << std::endl;
                break;
            }
        }
    }

    void read_obj_offsets(std::ifstream& file)
    {
        std::string line;

        file.seekg(data->xref_start);
        std::getline(file, line); //read: xref
        std::getline(file, line); //read: 0-xxx 

        for (int i = 0; i < data->num_obj; i++)
        {
            std::getline(file, line);

            data->obj_offsets[i][0] = std::stoi(line.substr(0, 10));
            data->obj_offsets[i][1] = std::stoi(line.substr(11, 5));
            data->obj_offsets[i][2] = (line.substr(17, 1) == "f") ? 0 : 1; //NOTE: f = 0 && n = 1
        }
    }

    void Initialize()
    {
        std::ifstream file("samplepdf.pdf", std::ios::binary);
        if (!file.is_open())
        {
            std::cout << "Could not open file...\n";
        }
        data = new filedata;

        file.seekg(0, std::ios::end);
        data->file_bytes = file.tellg();
        file.seekg(0, std::ios::beg);

        read_start_xref(&file);

        data->root = new root_node;

        read_trailer_info(&file);
        read_obj_offsets(file);

        read_root_obj(file);
        read_page_collector(file);
        read_page_data(file);
    }

    void Shutdonw()
    {
        // Empty function
    }
}