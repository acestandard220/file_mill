#include "includes/pdf_mill.h"

int m = 9;

namespace PDF_MILL {


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

    int get_length_to_not(std::string& line, char delimiter = ' ')
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
            int index = value[0] != ' ' ? 0 : 1;
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
        if (line.empty() || line.length() - 1 <= 1) { return std::pair<std::string, uint32_t>("", 0); }
        int key_len = get_length_to(line, start, ' ');
        std::string _key = line.substr(start + 1, key_len);

        int value_len = get_length_to(line, start + key_len + 1);
        int _value = std::stoi(line.substr(start + key_len + 1, value_len)); 

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
            if (line[i] == ' ' && line[i + 1] == ' ')
            {
                line.erase(i, 1);
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

    void _introduce_relevant_spacing(std::string& line)
    {
        int last_find = 0;
        int start = line.find("<<", last_find + 2);

        while (start != std::string::npos) //+2->> length of literal
        {
            line.insert(start + 2, 1, ' ');
            last_find = start;
            start = line.find("<<", last_find + 2);
        }
    }

    void dress_string(std::string& line)
    {
        _introduce_relevant_spacing(line);
        _remove_special_chars(line);
        _remove_redun_spaces(line);
    }


    //Read 4 values from an array [x x x x]
    


   
    /// @brief : Read values until delimiter is reached
    /// @param line : string line that contains values
    /// @param start : first values index
    /// @param delimiter : delimiter of whole array in string
    /// @param ele_del : delimiter btn each element
    /// @return : array of extracted uint32_t values
    std::vector<uint32_t> get_array_values_i(std::string& line, int start, char delimiter = ']',char ele_del = ' ')
    {
        //0123456789A
        //0 0 900  900
        std::vector<uint32_t> r;
        int obj_start = start + 1;
        int length_to_close = get_length_to(line, obj_start, delimiter);
        std::string linear_data = line.substr(obj_start, length_to_close);

        std::string value;
        std::stringstream stream(linear_data);
        while (std::getline(stream, value, ele_del))
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

    std::vector<std::string> get_array_values_s(std::string& line, int start, char delimiter = ']', char ele_del = ' ')
    {
        std::vector<std::string> r;
        int obj_start = start + 1;
        int length_to_close = get_length_to(line, obj_start, delimiter);
        std::string linear_data = line.substr(obj_start, length_to_close);

        std::string value;
        std::stringstream stream(linear_data);

        while (std::getline(stream, value, ele_del))
        {
            r.push_back(value);
        }

        return r;
    }

 
    /// @brief 
    /// @param line : string that contains possible values
    /// @param start : values start
    /// @param delimiter : end of values array in string  
    /// @return : extracted uint32_t obj_indexes
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

    template <typename T>
    std::vector<int> get_mapped_values(std::vector<std::string> list, std::unordered_map<std::string, T> map)
    {
        std::vector<int> procsets;
        for (auto& x : list)
        {
            procsets.push_back(map[x]);
        }
        return procsets;
    }

    int has_key(std::string& line, const std::string& key)
    {
        int line_index = line.find(key);
        int search_length = key.length();
        if (line.find(key) != std::string::npos)
        {
            return line_index + search_length; //true
        }
        else {
            return 0; //false
        }
    }

    int has_key(std::string& line, std::vector<std::string> keys)
    {
        for (auto& key : keys)
        {
            int line_index = line.find(key);
            int search_length = key.length();
            if (line.find(key) != std::string::npos)
            {
                return line_index + search_length; //true
            }
            else {
                continue;
            }
        }
        return 0;
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

    void read_font_file_obj(std::ifstream& file, std::set<uint32_t> _obj_index)
    {
        std::string line;
        for (auto& index : _obj_index)
        {
            file.clear();
            file.seekg(global_data->cur_file_read->obj_offsets[index][BYTE_OFFSET]);

            while (std::getline(file, line))
            {
                if (end_of_obj(line)) { break; }

                int start = has_key(line, "/Type ");
                if (start)
                {
                    if (!validate_obj_type(line, FONT_DESCRIPTOR))
                    {
                        return;
                    }
                }

                start = has_key(line, "/Length ");
                if (start)
                {
                    global_data->cur_file_read->cFontFile[index].length = get_array_values_i(line, start, ' ')[0];
                }
                start = has_key(line, "/Length1 ");
                if (start)
                {
                    global_data->cur_file_read->cFontFile[index].length1 = get_array_values_i(line, start, ' ')[0];
                }

                start = has_key(line, "stream");
                if (start)
                {
                    std::string stream;
                    while (std::getline(file, stream, '>'))
                    {
                        global_data->cur_file_read->cFontFile[index].stream = stream;
                    }
                }
            }
        }
    }

    void read_font_desc_obj(std::ifstream& file, std::set<uint32_t>& obj_index)
    {
        std::set<uint32_t> _obj_index;
        std::string line;
        for (auto& index : obj_index)
        {
            file.clear();
            file.seekg(global_data->cur_file_read->obj_offsets[index][BYTE_OFFSET]);

            while (std::getline(file, line))
            {
                if (end_of_obj(line)) { break; }
                
                int start = has_key(line, "/Type ");
                if (start)
                {
                    if (!validate_obj_type(line, FONT_DESCRIPTOR))
                    {
                        return;
                    }
                }

                start = has_key(line, "/Flag ");
                if (start)
                {
                    global_data->cur_file_read->cFontDescriptors[index][flag_param] = get_array_values_i(line, start, ' ')[0];
                }

                start = has_key(line, "/FontBBox ");
                if (start)
                {
                    auto g = get_array_values_i(line, start, ' ');
                    global_data->cur_file_read->cFontBox[index] = font_box(g[0], g[1], g[2], g[3]);
                    global_data->cur_file_read->cFontDescriptors[index][font_bbox_param] = index;
                }

                start = has_key(line, "/ItalicAngle ");
                if (start)
                {
                    global_data->cur_file_read->cFontDescriptors[index][italic_angle_param] = get_array_values_i(line, start, ' ')[0];
                }

                start = has_key(line, "/Ascent ");
                if (start)
                {
                    global_data->cur_file_read->cFontDescriptors[index][ascent_param] = get_array_values_i(line, start, ' ')[0];
                }

                start = has_key(line, "/Descent ");
                if (start)
                {
                    global_data->cur_file_read->cFontDescriptors[index][descent_param] = get_array_values_i(line, start, ' ')[0];
                }
                start = has_key(line, "/CapHeight ");
                if (start)
                {
                    global_data->cur_file_read->cFontDescriptors[index][cap_height_param] = get_array_values_i(line, start, ' ')[0];
                }
                start = has_key(line, "/XHeight ");
                if (start)
                {
                    global_data->cur_file_read->cFontDescriptors[index][xheight_param] = get_array_values_i(line, start, ' ')[0];
                }
                start = has_key(line, "/StemV ");
                if (start)
                {
                    global_data->cur_file_read->cFontDescriptors[index][stemv_param] = get_array_values_i(line, start, ' ')[0];
                }
                start = has_key(line, "/StemH ");
                if (start)
                {
                    global_data->cur_file_read->cFontDescriptors[index][stemh_param] = get_array_values_i(line, start, ' ')[0];
                }
                start = has_key(line, std::vector<std::string>{"/FontFile ","FontFile2 "});
                if (start)
                {
                    global_data->cur_file_read->cFontDescriptors[index][font_file_param] = get_array_values_i(line, start, ' ')[0];
                }
            }

            read_font_file_obj(file, _obj_index);
        }
    }

    void read_font_obj(std::ifstream& file, std::set<uint32_t> _obj_index)
    {
        std::set<uint32_t> font_descs;
        std::string line;
        for (auto& index : _obj_index)
        {
            file.clear();
            file.seekg(global_data->cur_file_read->obj_offsets[index][BYTE_OFFSET]);

            //global_data->cur_file_read->cFont[index] = Font(index);
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

                start = has_key(line, std::vector<std::string>{"/SubType ", "/Subtype"});
                if (start)
                {
                    int len = get_length_to(line, start, ' ');
                    std::string temp_ = line.substr(start, len - 1);
                    if (sub_type_map.find(temp_) != sub_type_map.end())
                    {
                        global_data->cur_file_read->cFont[index][sub_type_param] = sub_type_map[temp_];
                    }
                    else
                    {
                        global_data->cur_file_read->cFont[index][sub_type_param] = TYPE1;
                    }
                }
                start = has_key(line, "/Encoding ");
                if (start)
                {
                    int len = get_length_to(line, start, ' ');
                    global_data->cur_file_read->cFont[index][encoding_param] = encoding_map[line.substr(start, len - 1)];
                }

                start = has_key(line, "/BaseFont ");
                if (start)
                {
                    int len = get_length_to(line, start, ' ');
                    std::string temp_ = (line.substr(start, len - 1));
                    if (base_font_map.find(temp_) != base_font_map.end())
                    {
                        global_data->cur_file_read->cFont[index][base_font_param] = base_font_map[temp_];
                    }
                    else {
                        global_data->cur_file_read->cFont[index][base_font_param] = HELVETICA;
                    }
                }

                global_data->cur_file_read->cFont[index][font_descriptor_param] = 0;
                start = has_key(line, "/FontDescriptor ");
                if (start)
                {
                    int len = get_length_to(line, start, ' ');
                    auto g = get_indirect_array_obj(line, start);
                    global_data->cur_file_read->cFont[index][font_descriptor_param] = g[0];
                    font_descs.insert(g[0]);
                }
            }
        }
        read_font_desc_obj(file,font_descs);
    }

    void read_data_block(std::istream& file, std::string& block, int start, const std::string& delimiter)
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


    void get_tm_data(std::string& line, TextBlock& text_block)
    {
        std::array<int32_t, 6> values;
        std::string x;
        std::stringstream stream(line);
        int i = 0;
        while (std::getline(stream, x, ' '))
        {
            text_block.text_matrix[i++] = std::stoi(x);
        }

        text_block.text_position[0] = text_block.text_matrix[4];
        text_block.text_position[1] = text_block.text_matrix[5];
    }

    void get_tf_data(std::string& line, TextBlock& text_block)
    {
        std::vector<std::string> values;

        std::string x;
        std::stringstream stream(line.substr(1));

        while (std::getline(stream, x, ' '))
        {
            values.push_back(x);
        }

        text_block.font_tag = std::stoi(values[0].substr(2));
        text_block.font_size = std::stoi(values[1]);
    }

    void get_tj_data(std::string& line, TextBlock& text_block)
    {
        text_block.text = get_array_values_s(line, 0, ')', '\n')[0];
    }

    //TODO:Eliminate most of the hard coded string values with map functionalities...
    //TODO:This function needs heavy refactoring...
    void read_content_data(std::ifstream& file, const std::set<uint32_t>& contents)
    {
        for (auto index : contents)
        {
            auto& con = global_data->cur_file_read->cContent[index];
            con = Content(index);
            //Check if the reference updates the actual values...
            std::string content_block;

            read_data_block(file, content_block, global_data->cur_file_read->obj_offsets[index][BYTE_OFFSET], type_string[ENDOBJ]);
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
                //Extract Begin - End Text section
                std::string bt_block;
                std::string _line;
                std::stringstream stream(content_block.substr(start));
                while (std::getline(stream, _line))
                {
                    bt_block += _line;
                    if (_line.find("ET") != std::string::npos) { break; }
                }

                //Get multiple text blocks if any...
                std::set<size_t>positions; positions.insert(0);
                size_t pos = bt_block.find("Tj", 0);
                while (pos != std::string::npos)
                {
                    positions.insert(pos);
                    pos = bt_block.find("Tj", pos + 2);
                }

                std::vector<std::string> _text_blocks;
                for (auto it = positions.begin(); it != positions.end(); ++it)
                {
                    auto it_next = std::next(it);
                    if (it_next == positions.end()) { break; }
                    std::string x = bt_block.substr(*it, *it_next + 2);
                    _text_blocks.push_back(x);
                }

                //Extracting individual textmarker positions
                std::unordered_map<std::string, size_t> marker_positions_map;
                std::set<size_t> marker_positions; marker_positions.insert(0);
                for (auto& text_block : _text_blocks)
                {
                    TextBlock txb;
                    int marker_start = has_key(text_block, "Tf");
                    if (marker_start)
                    {
                        marker_positions.insert(marker_start);
                        marker_positions_map["Tf"] = marker_start;
                    }

                    marker_start = has_key(text_block, "Tm");
                    if (marker_start)
                    {
                        marker_positions.insert(marker_start);
                        marker_positions_map["Tm"] = marker_start;
                    }

                    marker_start = has_key(text_block, "Tj");
                    if (marker_start)
                    {
                        marker_positions.insert(marker_start);
                        marker_positions_map["Tj"] = marker_start;
                    }

                    //Get Tm start and end
                    auto it = marker_positions.find(marker_positions_map["Tm"]);
                    auto T_begin = *std::prev(it);
                    T_begin = T_begin == 0 ? 0 : T_begin + 1;

                    std::string t_string_data = text_block.substr(T_begin, *it - T_begin - 2);
                    get_tm_data(t_string_data, txb);

                    it = marker_positions.find(marker_positions_map["Tf"]);
                    T_begin = *std::prev(it);
                    T_begin = T_begin == 0 ? 0 : T_begin + 2;

                    t_string_data = text_block.substr(T_begin, *it - T_begin - 2);
                    get_tf_data(t_string_data, txb);
                    //global_data->cur_file_read->cPage[page]

                    it = marker_positions.find(marker_positions_map["Tj"]);
                    T_begin = *std::prev(it);
                    T_begin = T_begin == 0 ? 0 : T_begin + 2;
                    t_string_data = text_block.substr(T_begin - 1, *it - T_begin - 2);

                    get_tj_data(t_string_data, txb);

                    con.text_blocks.push_back(txb);
                }

            }
        }
    }

    void read_page_data(std::ifstream& file)
    {
        std::string line;
        std::set<uint32_t> _font_obj;
        std::set<uint32_t> _content_obj;
        for (auto& page_obj_index : global_data->cur_file_read->root->pages->mPages)
        {
            file.clear();
            file.seekg(global_data->cur_file_read->obj_offsets[page_obj_index][BYTE_OFFSET], std::ios::beg);

            global_data->cur_file_read->cPage[page_obj_index] = Page(page_obj_index);

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
                    global_data->cur_file_read->cPage[page_obj_index].media_box = media_box(g[X], g[Y], g[W], g[H]);
                }

                start = has_key(line, "/Contents ");
                if (start)
                {
                    global_data->cur_file_read->cPage[page_obj_index].rContents = get_indirect_array_obj(line, start, '/');
                    for (auto x : global_data->cur_file_read->cPage[page_obj_index].rContents)
                    {
                        _content_obj.insert(x);
                    }
                }

                start = has_key(line, "/Resources ");
                if (start)
                {
                    std::string resource_block;

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
                        if (g.second == 0) { continue; } // -1 is code for empty in this context
                        int font_tag = std::stoi(g.first.substr(1));
                        global_data->cur_file_read->cPage[page_obj_index].highest_font_tag = 
                                (global_data->cur_file_read->cPage[page_obj_index].highest_font_tag < font_tag  )
                                ? font_tag : global_data->cur_file_read->cPage[page_obj_index].highest_font_tag;
                            
                        global_data->cur_file_read->cPage[page_obj_index].rFonts[font_tag] = g.second;
                        _font_obj.insert(g.second);
                    }

                    _start = has_key(resource_block, "/ProcSet ");

                    len = get_length_to(resource_block, _start, ']');
                    res_line = resource_block.substr(_start, len);

                    global_data->cur_file_read->cPage[page_obj_index].cProcSet = get_mapped_values<procset_index>(get_array_values_s(res_line, 0),procset_map);
                }
            }
            global_data->cur_file_read->read_objects[page_obj_index] = true;
        }

        read_font_obj(file, _font_obj);
        read_content_data(file, _content_obj);
    }

    void read_page_collector(std::ifstream& file)
    {
        file.seekg(global_data->cur_file_read->obj_offsets[global_data->cur_file_read->root->pages->id][BYTE_OFFSET]);
        std::string line;

        while (std::getline(file, line))
        {
            if (end_of_obj(line)) { break; }
            std::string search = "/Type ";
            int start = has_key(line, search);

            if (start)
            {
                if (!validate_obj_type(line, type_index::PAGES))
                {
                    return;
                }
            }
            search = "/Count ";
            start = has_key(line, search);
            if (start)
            {
                int x = get_length_to(line, start);
                global_data->cur_file_read->root->pages->nPages = std::stoi(line.substr(start, x));
                std::cout << "Page Count::: " << global_data->cur_file_read->root->pages->nPages << std::endl;
            }
            search = "/Kids ";
            start = has_key(line, search);
            if (start)
            {
                global_data->cur_file_read->root->pages->mPages = get_array_objs(line, start);

                if (global_data->cur_file_read->root->pages->mPages.size() == global_data->cur_file_read->root->pages->nPages)
                {
                    //Todo: Page number info does not match...
                    //Todo: This is a ::PDFFix feature to be implemented later...
                    std::cout << "Page number indicated and pages found do not match\n";
                }
                continue;
            }
        }

        global_data->cur_file_read->read_objects[global_data->cur_file_read->root->pages->id] = true;
    }

    void read_root_obj(std::ifstream& file)
    {
        file.clear();
        file.seekg(global_data->cur_file_read->obj_offsets[global_data->cur_file_read->root->id][BYTE_OFFSET]);
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

            global_data->cur_file_read->root->pages = new page_collection;
            search = "/Pages ";
            start = has_key(line, search);

            if (start)
            {
                global_data->cur_file_read->root->pages->id = std::stoi(line.substr(start, get_length_to(line, start)));
                std::cout << "Page Collector ID::: " << global_data->cur_file_read->root->pages->id << std::endl;
                break;
            }
        }
        global_data->cur_file_read->read_objects[global_data->cur_file_read->root->id] = true;
    }

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
            std::string search = "/Size ";
            int start = has_key(line, search);

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
                global_data->cur_file_read->read_objects.resize(global_data->cur_file_read->num_obj);
                
                std::cout << "Num_Obj::: " << global_data->cur_file_read->num_obj << std::endl;
            }

            search = "/Root ";
            start = has_key(line, search);

            if (start)
            {
                int integer_length = get_length_to(line, start);
                global_data->cur_file_read->root->id = std::stoi(line.substr(start, integer_length));
                std::cout << "ss:: " << global_data->cur_file_read->root->id << std::endl;
                break;
            }
        }
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

        global_data->cur_file_read->root = new root_node;
        read_trailer_info(&file);

        read_obj_offsets(file);
        

        read_root_obj(file);
        read_page_collector(file);
        read_page_data(file);

        file.close();

    }

    void RequestReadPath(const char* path)
    {
        global_data->file_reads.emplace_back(std::make_shared<_filedata>());
        global_data->cur_file_read = global_data->file_reads[0];

        global_data->cur_file_read->current_path = path;
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

  


    

    /// @brief New PDF file method definitions...



}


