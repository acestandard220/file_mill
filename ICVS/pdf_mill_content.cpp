#include "includes/pdf_mill_content.h"
#include "includes/pdf_mill.h"


namespace PDF_MILL
{
    void get_tm_data(std::string& line, TextBlock& text_block)
    {
        std::array<int32_t, 6> values;
        std::string x;
        std::stringstream stream(line);

        int i = 0;
        while (std::getline(stream, x, ' '))
        {
            try
            {
                text_block.text_matrix[i++] = std::stoi(x);
            }
            catch (std::invalid_argument& e)
            {
                if (x.find("Tm"))
                {
                    break;
                }
                std::cout << "[WARNING]:[CONTENT]: Invalid Xref Table...\n";
            }
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

        text_block.font_tag = std::stoi(values[0].substr(1));
        text_block.font_size = std::stoi(values[1]);
    }

    void get_tj_data(std::string& line, TextBlock& text_block)
    {
        text_block.text = get_array_values_s(line, 0, ')', '\n')[0];
        text_block.text = get_array_values_i<std::string>(line, 0, ')', '\n')[0];
    }

    void read_content_data(_filedata* filedata, std::ifstream& file, const std::set<uint32_t>& contents)
    {
        std::string line;
        for (auto index : contents)
        {
            Content con;
            con = Content(index);

            read_obj_block(file, line, filedata->obj_offsets[index][BYTE_OFFSET]);

            con.len_stream = get_key_value_i<size_t>(line, std::vector<key_index>{KEY_LENGTH});
            con.filter = filter_map[get_key_value_i<std::string>(line, std::vector<key_index>{KEY_FILTER})];
            if (con.filter == FLATEDECODE)
            {
                ///DECOMPRESS///
            }           

            std::string bt_line;
            break_marker_lines(line, bt_line);
            TextBlock text_block;
            {
                std::string marker;
                {
                    read_line_with(bt_line, marker, TF);
                    get_tf_data(bt_line, text_block);
                }
                {
                    read_line_with(bt_line, marker, TM);
                    get_tm_data(marker, text_block);
                }
                {
                    read_line_with(bt_line, marker, TJ);
                    get_tj_data(marker, text_block);
                }

                con.text_blocks.push_back(text_block);
            }


            GraphicsBlock graphics_block;
            {
                
            }

            filedata->cContent[index] = con;
        }
    }
}
