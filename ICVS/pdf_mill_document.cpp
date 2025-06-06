#include "includes/pdf_mill_document.h"
#include "includes/pdf_mill.h"


namespace PDF_MILL
{
	bool end_of_obj(std::string& line)
	{
		return (line.find(type_string[ENDOBJ]) != std::string::npos);
	}

    int get_length_to(std::string& line, int start, char stop )
    {
        int len = 0;
        for (int i = 0; i < line.length() - start; i++)
        {
            if (line[i + start] != stop)
            {
                len++;
            }
            else {
                break;
            }
        }
        return len;
    }

    int get_length_to_not(std::string& line, char delimiter)
    {
        int index = 0;
        while (line[index] == delimiter)
        {
            index++;
        }
        return index;
    }


    std::vector<uint32_t> get_array_objs(std::string& line, int start, char delimiter)
    {
        std::vector<uint32_t> r;

        int index = (line[start] == delimiter) ? 1 : 0;
        int objs_start = start + index;
        int length_to_close = get_length_to(line, objs_start, pair[delimiter]);
        std::string linear_data = line.substr(objs_start, length_to_close);

        std::string _line;
        std::stringstream stream(linear_data);

        while (std::getline(stream, _line, 'R'))
        {
            int index = _line[0] != ' ' ? 0 : 1;
            int len = get_length_to(_line, index);
            std::string value = _line.substr(index, len);

            try
            {
                r.push_back(std::stoi(value));
            }
            catch (const std::invalid_argument& e)
            {
                std::cout << "Does not contain object index index: Invalid XREF";
                return r;
            }
            catch (const std::out_of_range& e)
            {
                std::cout << "Number is too huge\n";
                return r;
            }
        }
        return r;
    }

    std::vector<uint32_t> get_array_objs(std::string& line, key_index key, char delimiter)
    {
        size_t start = has_key<key_index>(line, key);
        return get_array_objs(line, start, delimiter);
    }

    std::pair<std::string, uint32_t> get_dict_key_value_indirect_obj(std::string line, int start)
    {
        if (line.empty() || line.length() - 1 <= 1) { return std::pair<std::string, uint32_t>("", 0); }

        int key_len = get_length_to(line, start, ' ');
        std::string key = line.substr(start + 1, key_len);

        int value_len = get_length_to(line, start + key_len + 1);
        int value = 0;

        try
        {
            value = std::stoi(line.substr(start + key_len + 1, value_len));
        }
        catch (std::invalid_argument& e)
        {
            std::cout << "Invalid STOI Argument...\n";
        }

        return std::pair<std::string, uint32_t>(key, value);
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
        int start = line.find("<<", last_find );

        while (start != std::string::npos) //+2->> length of literal
        {
            line.insert(start + 2, 1, ' ');
            line.insert(start, 1, ' ');
            last_find = start + 1;
            start = line.find("<<", last_find + 3);
        }

        last_find = 0;
        start = line.find(">>", last_find + 2);

        while (start != std::string::npos) //+2->> length of literal
        {
            line.insert(start + 2, 1, ' ');
            line.insert(start, 1, ' ');
            last_find = start;
            start = line.find(">>", last_find + 3);
        }

        last_find = 0;
        start = line.find("/", last_find + 1);

        while (start != std::string::npos) //+2->> length of literal
        {
            line.insert(start, 1, ' ');
            last_find = start;
            start = line.find("/", last_find + 2);
        }

        last_find = 0;
        start = line.find("[", last_find + 1);

        while (start != std::string::npos) //+2->> length of literal
        {
            line.insert(start, 1, ' ');
            last_find = start + 1;
            start = line.find("[", last_find + 2);
        }

    }

    void dress_string(std::string& line)
    {
        _introduce_relevant_spacing(line);
        _remove_special_chars(line);
        _remove_redun_spaces(line);
    }


    /// @brief : Read values until delimiter is reached
    /// @param line : string line that contains values
    /// @param start : first values index
    /// @param delimiter : delimiter of whole array in string
    /// @param ele_del : delimiter btn each element
    /// @return : array of extracted uint32_t values
    




    std::vector<std::string> get_array_values_s(std::string& line, int start, char delimiter, char ele_del )
    {
        std::vector<std::string> r;

        int index = (line[start] == pair2[delimiter]) ? 1 : 0;

        int obj_start = start + index;
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

    std::vector<std::string> get_array_values_s(std::string& line, key_index key, char delimiter, char ele_del)
    {
        size_t start = has_key<key_index>(line, key);
        return get_array_values_s(line, start, delimiter, ele_del);
    }

    /// @brief 
/// @param line : string that contains possible values
/// @param start : values start
/// @param delimiter : end of values array in string  
/// @return : extracted uint32_t obj_indexes
    std::vector<uint32_t> get_indirect_array_obj(std::string& line, int start, char delimiter)
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

    std::vector<uint32_t> get_indirect_array_obj(std::string& line, std::vector<key_index> key, char delimiter)
    {
        size_t start = has_key<key_index>(line, key);
        return (start != 0) ? get_indirect_array_obj(line, start, delimiter) : std::vector<uint32_t>();
    }

    std::vector<uint32_t> get_indirect_array_obj(std::string& line, label_index label, char delimiter)
    {
        size_t start = has_key<label_index>(line, label);
        return (start) ? get_indirect_array_obj(line, start, delimiter) : std::vector<uint32_t>();
    }

    uint32_t get_indirect_obj(std::string& line, std::vector<key_index> key, char delimiter)
    {
        size_t start = has_key<key_index>(line, key);
        return (start) ? get_indirect_array_obj(line, start, delimiter)[0] : 0;
    }

    uint32_t get_indirect_obj(std::string& line, label_index label, char delimiter)
    {
        size_t start = has_key<label_index>(line, label);
        return (start) ? get_indirect_array_obj(line, start, delimiter)[0] : 0;
    }

 /*   std::vector<uint32_t> get_indirect_array_obj_multiple(std::string& line, label_index label, char delimiter)
    {
        std::vector<uint32_t> returns;
        std::string label_string_temp = label_string[label];
        label_string_temp += " ";

        std::string _line = line;
        size_t start = has_key(_line, label_string_temp);
        while(start)
        {
            returns.push_back(get_indirect_array_obj(_line, start, delimiter)[0]);
            _line = _line.substr(start);
            start = has_key(_line, label_string_temp);
        }
        returns;
    }*/

    int validate_obj_type(std::string& line, type_index type)
    {
        const std::string temp = "/Type ";
        int start = has_key<key_index>(line, KEY_TYPE);
        if (!start) { return PDF_MILL_INVALID; }

        int x = get_length_to(line, start);
        std::string sub = line.substr(start, type_string[type].length());

        if (sub.find(type_string[type]) == std::string::npos)
        {
            std::cout << "[ERROR]:[XREF CORRUPT]:: Invalid " << type_string[type] << " Offset...\n";
            return PDF_MILL_INVALID;
        }
        return PDF_MILL_VALID;
    }

    void zlib_decode_deflate(std::ifstream& file,size_t off, std::string& out)
    {
       /*

        std::string d_string;
        std::string l;
        file.seekg(off);
        while (std::getline(file, l))
        {
            int start = has_key<std::string>(l, "stream");
            if (start)
            {
                while (std::getline(file, l))
                {
                    if (l.find("endstream") != std::string::npos)
                    {
                        break;
                    }
                    d_string += l;
                }break;
            }
        }

       z_stream stream;
       stream.zalloc = Z_NULL;
       stream.zfree = Z_NULL;
       stream.opaque = Z_NULL;

       uLongf d_size = d_string.size() * 4;
       std::vector<char> buffer(d_size);
       int result = uncompress(reinterpret_cast<Byte*>(buffer.data()),   &d_size,
                               reinterpret_cast<const Byte*>(d_string.data()),  d_string.size()
                              );
       while (result == Z_BUF_ERROR) {
           d_size *= 2;
           buffer.resize(d_size);

           result = uncompress(reinterpret_cast<Byte*>(buffer.data()), &d_size,
               reinterpret_cast<const Byte*>(d_string.data()), d_string.size()
           );

       }

       if (result != Z_OK)
       {
           return;
       }

       out.assign(buffer.data(), d_size);
       return;

        for (int mode : {-MAX_WBITS, MAX_WBITS, 16 + MAX_WBITS}) {
            z_stream stream{};
            stream.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(d_string.data()));
            stream.avail_in = d_string.size();

            if (inflateInit2(&stream, mode) != Z_OK)
                continue;

            std::vector<char> buffer(4096);
            out.clear();

            int ret;
            do {
                stream.next_out = reinterpret_cast<Bytef*>(buffer.data());
                stream.avail_out = buffer.size();

                ret = inflate(&stream, Z_NO_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END) {
                    inflateEnd(&stream);
                    break;
                }

                out.append(buffer.data(), buffer.size() - stream.avail_out);
            } while (ret != Z_STREAM_END);

            if (ret == Z_STREAM_END) {
                inflateEnd(&stream);
                return;
            }
        }*/

        return; // All modes failed
    }

    void break_marker_lines(std::string& line, std::string& strips)
    {
        std::string block;
        read_data_block(line, block, KL_BT, KL_ET);


        std::string _line;
        std::stringstream stream(block);
        while (std::getline(stream, _line, ' '))
        {
            strips += _line + " ";
            for (int i = 0; i < _marker_index_last_; i++)
            {
                if (_line.find(marker_string[i]) != std::string::npos)
                {
                    strips += "\n";
                    break;
                }
                else
                {
                    continue;
                }
            }

        }
    }


    //Does not dress_string
    void read_data_block(std::string& line, std::string& block, int start, const std::string& delimiter)
    {
     
        std::string _line;
        std::stringstream stream(line);
        while (std::getline(stream, _line, ' '))
        {
            block += _line + " ";
            if (_line.find(delimiter) != std::string::npos) { break; }
        }
    }

    void read_data_block(std::string& line, std::string& block, key_line_index key, key_line_index key_end)
    {
        size_t start = has_key<key_line_index>(line, key);
        std::string temp = line.substr(start);
        read_data_block(temp, block, start, key_line_string[key_end]);
    }

    void read_obj_block(std::istream& file, std::string& block, int start)
    {
        file.clear();
        file.seekg(start, std::ios::beg);

        block.clear();

        std::string line;
        while (std::getline(file, line))
        {
            block += line;

            if (end_of_obj(line)) { break; }
        }

        dress_string(block);
    }

    void read_dict_block(std::string& line, std::string& block, key_index key)
    {
        size_t start = has_key<key_index>(line,key);

        std::string _line;
        _line = line.substr(start);

        std::stringstream file(_line);

        int open_dict = 0;
        do
        {
            for (auto& x : _line)
            {
                if (x == '<') { open_dict++; }
                else if (x == '>') { open_dict--; }
                block += x;
                if (!open_dict) { break; }
            }
            if (!open_dict) { break; }

        } while (std::getline(file, _line));

        if (open_dict) { return; }
        dress_string(block);
    }

}