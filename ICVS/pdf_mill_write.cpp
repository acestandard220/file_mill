#include "includes/pdf_mill.h"


namespace PDF_MILL
{

    int indent_track = 0;//TODO: Make static in function below...
    void indent(std::string& out) { for (int i = 0; i < indent_track + 2; i++) { out += " "; }; }//Confirm whether to make inline...

    inline void write_line(std::ofstream& file, std::string& line)
    {
        file << line;
        line.clear();
    }

    void create_obj_start_line(std::string& out, uint32_t obj_index)
    {
        out += std::to_string(obj_index) + " 0 obj";

        out += key_string[KEY_ENDLINE_S] + DICT_OPEN + endline;

    }

    void create_obj_end_line(std::string& out)
    {
        out += ">>\n" + type_string[ENDOBJ] + endline;
    }

    void create_obj_type_line(std::string& out, type_index type)
    {
        indent(out);
        out += "/Type " + type_string[type] + endline;
    }

    void create_obj_sub_type_line(std::string& out, sub_type_index type)
    {
        indent(out);
        out += "/SubType " + sub_type_string[type] + endline;
    }

    void create_direct_key_value_line(std::string& out, key_index key, std::string value)
    {
        indent(out);
        out += key_string[key] + " " + value + endline;
    }

    //TODO: Add maps for trailing strings "0 R"eg.
    //TODO: implement reading something like this and thenn get other functions to do some more work later
    //      read kids and counts in the same way ... 1. extract array full length then individual functions
    std::string create_indirect_obj_value_array_string(const std::vector<uint32_t>& value)
    {
        std::string out;
        const std::string _z = " 0 R";
        for (auto& x : value)
        {
            out += std::to_string(x) + _z + " ";//adding spaces to last elements becareful
        }
        return out;
    }

    std::string create_indirect_obj_value_array_string(uint32_t value)
    {
        std::vector<uint32_t>v_value{ value };
        return create_indirect_obj_value_array_string(v_value);
    }


    void create_key_array_line(std::string& out, key_index key, const std::string& value_string, const char& array_type = '[')
    {
        indent(out);
        out += key_string[key] + " " + array_type + value_string + pair[array_type] + "\n";
    }

    std::string create_array_string(const std::string& value_string, const char array_type = '[')
    {
        std::string out;
        out += array_type + value_string + pair[array_type];
        return out;
    }

    void create_key_indirect_obj_value_ref_line(std::string& out, key_index key, const std::string& value_string)
    {
        indent(out);
        out += key_string[key] + " " + value_string + endline;
    }

    void create_key_label_indirect_obj_value_ref_line(std::string& out, label_index key, int label_count, const std::string& value_string)
    {
        indent(out);
        out += label_string[key] + std::to_string(label_count) + std::string(" ") + value_string + endline;
    }

    std::string create_media_box_value_string(media_box& box)
    {
        std::string out = "";
        for (auto x : box.asArray())
        {
            out += std::to_string(x) + " ";
        }
        return out;
    }

    void insert_char(std::string& out, char c, int n)
    {
        for (int i = 0; i < n; i++) { out += c; }
    }

    template <typename T>
    std::string create_value_array_string(std::vector<T> values)
    {
        std::string out = "";

        if constexpr (std::is_arithmetic_v<T>)
        {
            for (auto& x : values)
            {
                out += std::to_string(x) + " ";
            }
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            for (auto& x : values)
            {
                out += x + " ";
            }
        }

        return out;
    }


    void create_custom_line(std::string& out, const std::string& value, int indent = 0)
    {
        for (int i = 0; i < indent; i++) { out += " "; }
        out += value + endline;
    }

    void create_dict_key_opening_line(std::string& out, key_index type, int indent_override = 0)
    {
        indent_track += 2 + indent_override;
        for (int i = 0; i < indent_track; i++) { out += " "; }
        out += key_string[type] + " " + DICT_OPEN + endline;
    }

    void create_dict_close_line(std::string& out)
    {
        for (int i = 0; i < indent_track; i++) { out += " "; }
        out += DICT_CLOSE;
        out += endline;
        indent_track -= (indent_track == 0) ? 0 : 2;
    }

    void create_key_line(std::string& out, key_line_index key)
    {
        out += key_line_string[key] + endline;
    }

    void write_root_obj(std::ofstream& file)
    {
        int offset = file.tellp();

        global_data->cur_file_write->root_id = global_data->cur_file_read->root->id;
        global_data->cur_file_write->obj_offsets[global_data->cur_file_read->root->id] = {
            offset,
            global_data->cur_file_read->obj_offsets[global_data->cur_file_read->root->id][UID],
            global_data->cur_file_read->obj_offsets[global_data->cur_file_read->root->id][FLAG]
        };
        global_data->cur_file_write->obj_offsets[0] = { 0,65535,0 };


        std::string line;
        create_obj_start_line(line, global_data->cur_file_read->root->id);

        create_obj_type_line(line, type_index::CATALOG);

        std::string temp = create_indirect_obj_value_array_string(global_data->cur_file_read->root->pages->id);
        create_key_indirect_obj_value_ref_line(line, key_index::KEY_PAGES, temp);

        create_obj_end_line(line);
        write_line(file, line);
    }

    void write_page_collector(std::ofstream& file)
    {
        int offset = file.tellp();

        global_data->cur_file_write->obj_offsets[global_data->cur_file_read->root->pages->id] = { offset,global_data->cur_file_read->obj_offsets[global_data->cur_file_read->root->pages->id][UID],global_data->cur_file_read->obj_offsets[global_data->cur_file_read->root->pages->id][FLAG] };

        std::string line;
        create_obj_start_line(line, global_data->cur_file_read->root->pages->id);

        create_obj_type_line(line, type_index::PAGES);

        std::string temp = create_indirect_obj_value_array_string(global_data->cur_file_read->root->pages->mPages);
        create_key_array_line(line, key_index::KEY_KIDS, temp);

        create_direct_key_value_line(line, KEY_COUNT, std::to_string(global_data->cur_file_read->root->pages->nPages));

        create_obj_end_line(line);
        write_line(file, line);
    }

    void write_page_obj(std::ofstream& file)
    {
        int offset = 0;
        for (auto& x : global_data->cur_file_read->cPage)
        {
            offset = file.tellp();
            auto& page_data = x.second;
            global_data->cur_file_write->obj_offsets[page_data.id] = {
                offset,
                global_data->cur_file_read->obj_offsets[page_data.id][UID],
                global_data->cur_file_read->obj_offsets[page_data.id][FLAG]
            };
            std::string line;
            create_obj_start_line(line, page_data.id);
            create_obj_type_line(line, type_index::PAGE);

            create_key_array_line(line, key_index::KEY_MEDIABOX, create_media_box_value_string(page_data.media_box));
            if (!page_data.rContents.empty())
            {
                create_key_indirect_obj_value_ref_line(line, KEY_CONTENTS, create_indirect_obj_value_array_string(page_data.rContents));
            }
            //Resources
            create_dict_key_opening_line(line, KEY_RESOURCES);
            {
                //Font 
                if (!page_data.rFonts.empty())
                {
                    create_dict_key_opening_line(line, KEY_FONT);

                    for (auto& y : page_data.rFonts)
                    {
                        std::string temp = create_indirect_obj_value_array_string(y.second);

                        create_key_label_indirect_obj_value_ref_line(line, LABEL_F, y.first, temp);
                    }

                    create_dict_close_line(line);
                }

                //ProcSet 
                {
                    std::vector<std::string>cv;
                    for (auto z : page_data.cProcSet)
                    {
                        cv.push_back(procset_string[z]);
                    }
                    create_key_array_line(line, KEY_PROCSET, create_value_array_string<std::string>(cv));
                }

            }
            create_dict_close_line(line);

            create_obj_end_line(line);
            write_line(file, line);
        }
    }

    void write_font_obj(std::ofstream& file)
    {
        int offset = 0;
        for (auto& x : global_data->cur_file_read->cFont)
        {
            offset = file.tellp();
            auto& obj_index = x.first;
            auto& font_data = x.second;
            global_data->cur_file_write->obj_offsets[obj_index] = {
                offset,
                global_data->cur_file_read->obj_offsets[obj_index][UID],
                global_data->cur_file_read->obj_offsets[obj_index][FLAG]
            };
            std::string line;

            create_obj_start_line(line, obj_index);
            create_obj_type_line(line, type_index::FONT);
            create_obj_sub_type_line(line, static_cast<sub_type_index>(font_data[sub_type_param]));

            create_direct_key_value_line(line, KEY_BASE_FONT, base_font_string[font_data[base_font_param]]);
            create_direct_key_value_line(line, KEY_ENCODING, encoding_string[font_data[encoding_param]]);

            create_obj_end_line(line);
            write_line(file, line);
        }
    }

    void write_content_obj(std::ofstream& file)
    {
        int offset = 0;
        for (auto& x : global_data->cur_file_read->cContent)
        {
            offset = file.tellp();
            auto& content_data = x.second;
            global_data->cur_file_write->obj_offsets[content_data.id] = {
                offset,
                global_data->cur_file_read->obj_offsets[content_data.id][UID],
                global_data->cur_file_read->obj_offsets[content_data.id][FLAG]
            };
            std::string line;
            create_obj_start_line(line, content_data.id);

            create_direct_key_value_line(line, KEY_LENGTH, std::to_string(content_data.stream_length));

            create_custom_line(line, DICT_CLOSE);

            create_custom_line(line, type_string[STREAM]);
            {
                if (!x.second.text_blocks.empty())
                {
                    create_custom_line(line, "BT");
                    for (auto& y : content_data.text_blocks)
                    {
                        create_custom_line(line, label_string[LABEL_F] + std::to_string(y.font_tag) + " " + std::to_string(y.font_size) + " " + "Tf");
                        create_custom_line(line, (create_value_array_string<int32_t>(std::vector<int32_t>{y.text_matrix[0], y.text_matrix[1], y.text_matrix[2],
                            y.text_matrix[3], y.text_matrix[4], y.text_matrix[5]}) + "Tm"));
                        create_custom_line(line, create_array_string(y.text, '(') + "Tj");
                    }
                    create_custom_line(line, "ET");
                }
            }
            create_custom_line(line, "endstream");
            create_custom_line(line, "endobj");
            write_line(file, line);

        }
    }

    void write_xref_table(std::ofstream& file)
    {
        size_t offset = file.tellp();
        std::string line;
        create_key_line(line, KL_XREF);
        create_custom_line(line, "0 " + std::to_string(global_data->cur_file_read->num_obj));

        for (auto x : global_data->cur_file_write->obj_offsets)
        {

            std::string _byteoffset = "0000000000 ";
            std::string int_value = std::to_string(x[BYTE_OFFSET]);
            int start_pos = _byteoffset.length() - int_value.length() - 1;
            for (int i = 0; i < int_value.length(); i++)
            {
                _byteoffset[i + start_pos] = int_value[i];
            }

            std::string _uid = "00000 ";
            int_value = std::to_string(x[UID]);

            start_pos = _uid.length() - int_value.length() - 1;
            for (int i = 0; i < int_value.length(); i++)
            {
                _uid[i + start_pos] = int_value[i];
            }

            std::string _flag = (x[FLAG]) ? "n" : "f";

            std::string xref_table_line = _byteoffset + _uid + _flag;

            create_custom_line(line, xref_table_line);
        }
        create_key_line(line, KL_TRAILER);
        create_custom_line(line, DICT_OPEN);
        {
            create_key_indirect_obj_value_ref_line(line, KEY_ROOT, create_indirect_obj_value_array_string(global_data->cur_file_read->root->id));
            create_direct_key_value_line(line, KEY_SIZE, std::to_string(global_data->cur_file_read->num_obj));
        }
        create_custom_line(line, DICT_CLOSE);
        create_key_line(line, KL_STARTXREF);
        create_custom_line(line, std::to_string(offset));
        write_line(file, line);
    }

    void WriteToFile()
    {
        std::ofstream file(global_data->cur_file_write->write_path, std::ios::binary);
        if (!file.is_open())
        {
            std::cout << "Could not write to file...\n";
            return;
        }

        global_data->cur_file_write->obj_offsets.resize(global_data->cur_file_read->num_obj);

        write_line(file, global_data->cur_file_read->version);
        write_root_obj(file);
        write_page_collector(file);
        write_page_obj(file);
        write_font_obj(file);
        write_content_obj(file);
        write_xref_table(file);
        write_line(file, global_data->cur_file_read->eof);

        file.close();
    }

    void RequestWritePath(std::string path)
    {
        global_data->file_writes.emplace_back(std::make_shared<write_filedata>());
        global_data->cur_file_write = global_data->file_writes[0];

        global_data->cur_file_write->write_path = path;
    }
}