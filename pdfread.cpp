#include "pdfread.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
int m = 9;

namespace PDFREAD {

    std::array<const std::string, _type_index_last> type_string{ "/Catalog", "/Pages", "Page", "endobj","/Font","/Contents"};
    
    std::unordered_map<char, char> pair = { {'[', ']'}, {'{', '}'}, {'(', ')'} };
    std::array<const std::string, base_font_last> base_font_string{ "/Helvetica","/ZapfDingbats" };

    extern std::array<const std::string, base_font_last> encoding_string{ "/WinAnsiEncoding" };

    std::unordered_map<std::string, base_font> base_font_map{ {"/Helvetica",HELVETICA},{"ZapfDingbats",ZAPFDINGBATS} };
    std::unordered_map<std::string, encoding> encoding_map{ {"/WinAnsiEncoding",WINANSIENCODING} };
    std::unordered_map<std::string, sub_type_index> sub_type_map{ {"/Type1",TYPE1},{"/Type2",TYPE2} };

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

    int get_length_to_not(std::string& line,char delimiter = ' ')
    {
        int index = 0;
        while (line[index] == delimiter)
        {
            index++;
        }
        return index;
    }

    std::vector<uint32_t> get_array_objs(std::string& line, int start, char delimiter = ' ')
    {
        std::vector<uint32_t> r;
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
                return std::vector<uint32_t>();
            }
            catch (const std::out_of_range& e)
            {
                std::cout << "Number is too huge\n";
                return std::vector<uint32_t>();
            }
        }
        return r;
    }

    std::pair<std::string, uint32_t> get_dict_key_value_indirect_obj(std::string line, int start)
    {
        if (line.empty() || line.length() <= 1) { return std::pair<std::string, uint32_t>(); }
        int key_len = get_length_to(line, start, ' ');
        std::string _key = line.substr(start + 1, key_len);

        int value_len = get_length_to(line, start + key_len + 1);
        int _value = std::stoi(line.substr(start + key_len + 1, value_len)); //ERro found here...

        return std::pair<std::string, uint32_t>(_key, _value);
    }


    void _remove_special_chars(std::string& line)
    {
        for (int i = 0; i < line.length();)
        {
            if (line[i] == '\n')
            {
                //line.erase(i, 1);
                //Spaces are very essential delimiters in this program
                //Erasing \n will merge two lines if the preceding does not have a space infornt of it
                //To solve this we substitute \n for a spce then call remove_redun_spaces() in the dress_string function
                //dress_string will handle removing duplicates if there was already a seperator
                line.replace(i, 1, " ");
            }
            else if (line[i] == '\r')
            {
                //line.erase(i, 1);
                line.replace(i, 1, " ");
            }
            else
            {
                i++;
            }
        }
    }

    void _remove_redun_spaces(std::string& line)
    {        
        int front_c = get_length_to_not(line);
        line = line.substr(front_c);
        for (int i = 0; i < line.length();)
        {
            if (line[i] == ' ' && line[i+1] == ' ')
            {
                line.erase(i,1);
            }
            else if (line[i] == ' ' && line[i + 1] == '\n')
            {
                line.erase(i, 1);
            }
            else
            {
                i++;
            }
        }
    }

    void dress_string(std::string& line)
    {
        _remove_special_chars(line);
        _remove_redun_spaces(line);
    }

 
    //Read 4 values from an array [x x x x]
    std::vector<uint32_t> get_array_values_i(std::string& line, int start, char delimiter = ']')
    {
        //0123456789A
        //0 0 900  900
        std::vector<uint32_t> r;
        int obj_start = start + 1;
        int length_to_close = get_length_to(line, obj_start, delimiter);
        std::string linear_data = line.substr(obj_start, length_to_close);

        std::string value;
        std::stringstream stream(linear_data);
        while (std::getline(stream, value, ' '))
        {
            try
            {
                r.push_back(std::stoi(value));
            }
            catch (std::invalid_argument& e)
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

    std::vector<std::string> get_array_values_s(std::string& line, int start, char delimiter = ']')
    {
        std::vector<std::string> r;
        int obj_start = start + 1;
        int length_to_close = get_length_to(line, obj_start, delimiter);
        std::string linear_data = line.substr(obj_start, length_to_close);

        std::string value;
        std::stringstream stream(linear_data);

        while (std::getline(stream, value, ' '))
        {
            r.push_back(value);
        }
       
        return r;
    }

    //todo: think about delimiter param
    //becareful with reading array due to current delimiter might shoot your self in the leg
    std::vector<uint32_t> get_indirect_array_obj(std::string& line, int start, char delimiter = ' ')
    {
        std::vector<uint32_t> r;
        int objs_start = start;
        int len = get_length_to(line, objs_start, delimiter);
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

    //NOTE: Currently not handling any error for when font is not available in my list.
    void read_font_obj(std::ifstream& file, std::set<uint32_t> _obj_index)
    {
        std::string line;
        for (auto& index : _obj_index)
        {
            file.clear();
            file.seekg(data->obj_offsets[index][BYTE_OFFSET]);

            data->cFont[index] = Font(index);
            while (std::getline(file, line))
            {
                if (end_of_obj(line)) { break; }
                
                int start = has_key(line, "/Type ");
                if (start)
                {
                    if (!validate_obj_type(line, FONT))
                    {
                        return;
                    }
                }

                start = has_key(line, "/SubType ");
                if(start)
                {
                    int len = get_length_to(line, start, ' ');
                    data->cFont[index].sub_type = sub_type_map[line.substr(start, len)];
                }
                start = has_key(line, "/Encoding ");
                if (start)
                {
                    int len = get_length_to(line, start, ' ');
                    data->cFont[index].encoding = encoding_map[line.substr(start, len)];
                }

                start = has_key(line, "/BaseFont ");
                if (start)
                {
                    int len = get_length_to(line, start, ' ');
                    data->cFont[index].base_font = base_font_map[(line.substr(start, len))];
                }
            }

        }
    }

    void read_data_block(std::ifstream& file,std::string& block, int start, const std::string& delimiter)
    {
        file.clear();
        file.seekg(start, std::ios::beg);

        std::string line;
        while (std::getline(file, line))
        {
            //if (!block.empty()) { std::cout << "[WARNING]: This container is not empty. It will affect output.\n"; }
            block += line;
            if (line.find(delimiter) != std::string::npos) { break; }
        }
    }

    //TODO:Eliminate most of the hard coded string values with map functionalities...
    void read_content_data(std::ifstream& file,const std::set<uint32_t>& contents)
    {
        for(auto index : contents)
        {
            auto& con = data->cContent[index];
            con = Content(index);
            //Check if the reference updates the actual values...
            std::string content_block;

            read_data_block(file, content_block, data->obj_offsets[index][BYTE_OFFSET], type_string[ENDOBJ]);
            dress_string(content_block);

            int start = has_key(content_block, "/Length ");
            if (start)
            {
                auto x = get_dict_key_value_indirect_obj(content_block, start - 8);
                con.stream_length = x.second;
            }

            start = has_key(content_block, "BT");
            if (start)
            {
                
            }
        }
    }

    void read_page_data(std::ifstream& file)
    {
        std::string line;
        std::set<uint32_t> _font_obj;
        std::set<uint32_t> _content_obj;
        for (auto& page_obj_index : data->root->pages->mPages)
        {
            file.clear();
            file.seekg(data->obj_offsets[page_obj_index][BYTE_OFFSET], std::ios::beg);

            data->cPage[page_obj_index] = Page(page_obj_index);

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
                    auto g = get_array_values_i(line, start, pair[line[start]]);
                    data->cPage[page_obj_index].media_box = media_box(g[X], g[Y], g[W], g[H]);
                }

                start = has_key(line, "/Contents ");
                if (start)
                {
                    data->cPage[page_obj_index].rContents = get_indirect_array_obj(line, start, '/');
                    for (auto x : data->cPage[page_obj_index].rContents)
                    {
                        _content_obj.insert(x);
                    }
                }

                start = has_key(line, "/Resources ");
                if (start)
                {
                    std::string resource_block;
                    //Pull the entire resource dictionary
                    //Get line was used here and not on pulling specific res. 
                    // because this one could have occupied mutiple lines
                    int open_dict = 0;
                    do
                    {
                        resource_block += line + "\n";
                        for (auto& x : line)
                        {
                            if (x == '<') { open_dict++; }
                            else if (x == '>') { open_dict--; }
                        }

                        if (!open_dict) { break; }

                    } while (std::getline(file, line));
                    dress_string(resource_block);
                    
                    //Now read individual resources
                    int _start = has_key(resource_block, "/Font ");
                    int len = get_length_to(resource_block, _start, '>');
                    std::string res_line = resource_block.substr(_start + 2, len - 2);
                    
                    std::string res_data;
                    std::stringstream _stream(res_line);
                    int index = 0;
                    while (std::getline(_stream, res_data, 'R'))
                    {
                        index += index == 0 ? 1 : 0;
                        auto g = get_dict_key_value_indirect_obj(res_data, index);
                        data->cPage[page_obj_index].rFonts[g.first] = g.second;
                        _font_obj.insert(g.second);
                    }

                    _start = has_key(resource_block, "/ProcSet ");

                    len = get_length_to(resource_block, _start, ']');
                    res_line = resource_block.substr(_start, len  );

                    data->cPage[page_obj_index].cProcSet = get_array_values_s(res_line, 0);
                }
            }

            data->read_objects[page_obj_index] = true;
        }

        read_font_obj(file,_font_obj);
        read_content_data(file, _content_obj);
    }

    void read_page_collector(std::ifstream& file)
    {
        file.seekg(data->obj_offsets[data->root->pages->id][BYTE_OFFSET]);
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
        }

        data->read_objects[data->root->pages->id] = true;
    }

    // add breaks when we are certain we dont need anything else;
    void read_root_obj(std::ifstream& file)
    {
        file.clear();
        file.seekg(data->obj_offsets[data->root->id][BYTE_OFFSET]);
        std::string line;

        while (std::getline(file, line))
        {
            if (end_of_obj(line)) { break; }

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
        data->read_objects[data->root->id] = true;
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
                data->obj_offsets.resize(data->num_obj);
                data->read_objects.resize(data->num_obj);
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

            data->obj_offsets[i][BYTE_OFFSET] = std::stoi(line.substr(0, 10));
            data->obj_offsets[i][UID] = std::stoi(line.substr(11, 5));
            data->obj_offsets[i][FLAG] = (line.substr(17, 1) == "f") ? 0 : 1; //NOTE: f = 0 && n = 1
        }
    }

    void Initialize()
    {

        std::ifstream file("assets/samplepdf.pdf", std::ios::binary);
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