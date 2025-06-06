#pragma once
#include "zlib.h"
#include <fstream>

#include "pdf_mill_core_types.h"

namespace PDF_MILL
{
	enum marker_index
	{
		TF,
		TJ,
		TM,
		TD,

		W,
		RG,
		rg,
		re,
		_marker_index_last_
	};
	struct __marker;
	extern std::unordered_map<char, char> pair2;

	extern std::array<const std::string, _key_index_last_> key_string;
	extern std::array<const std::string, _type_index_last_> type_string;
	extern std::array<const std::string, _label_index_last_> label_string;
	extern std::array<const std::string, _marker_index_last_> marker_string;
	extern std::array<const std::string, _procset_index_last_> procset_string;
	extern std::array<const std::string, _key_line_last_> key_line_string;


	bool end_of_obj(std::string& line);
	int validate_obj_type(std::string& line, type_index type);
	
	int get_length_to(std::string& line, int start, char stop = ' ');
	int get_length_to_not(std::string& line, char delimiter = ' ');
	
	template <typename T>
	inline const std::string* get_typeid_string_arr();

	template <>
	inline const std::string* get_typeid_string_arr<std::string>()
	{
		return nullptr;
	}

	template <>
	inline const std::string* get_typeid_string_arr<key_index>()
	{
		return key_string.data();
	}

	template <>
	inline const std::string* get_typeid_string_arr<type_index>()
	{
		return type_string.data();
	}

	template <>
	inline const std::string* get_typeid_string_arr<key_line_index>()
	{
		return key_line_string.data();
	}

	template <>
	inline const std::string* get_typeid_string_arr<procset_index>()
	{
		return procset_string.data();
	}

	template <>
	inline const std::string* get_typeid_string_arr<label_index>()
	{
		return label_string.data();
	}

	template <>
	inline const std::string* get_typeid_string_arr<marker_index>()
	{
		return marker_string.data();
	}

	template <typename T>
	size_t has_key(std::string& line, T key)
	{
		const std::string* arr_ptr = get_typeid_string_arr<T>();

		std::string str_value = arr_ptr[(int)key];
		int line_index = line.find(str_value);
		int search_length = str_value.length();
		if (line.find(str_value) != std::string::npos)
		{
			return (line[line_index + search_length] == ' ') ? line_index + search_length + 1 : line_index + search_length; //true
		}
		else
		{
			return 0;
		}
	}

	template <typename T>
	size_t has_key(std::string& line, std::vector<T> keys)
	{
		const std::string* arr_ptr = get_typeid_string_arr<T>();

		for (auto& key : keys)
		{
			std::string str_value = arr_ptr[(int)key];
			int line_index = line.find(str_value);
			int search_length = str_value.length();
			if (line.find(str_value) != std::string::npos)
			{
				return (line[line_index + search_length] == ' ') ? line_index + search_length + 1 : line_index + search_length; //true
			}
			else
			{
				continue;
			}
		}
		return 0;
	}

	void _remove_special_chars(std::string& line);
	void _remove_redun_spaces(std::string& line);
	void _introduce_relevant_spacing(std::string& line);

	void dress_string(std::string& line);

	std::pair<std::string, uint32_t> get_dict_key_value_indirect_obj(std::string line, int start);
	
	std::vector<uint32_t> get_array_objs(std::string& line, int start, char delimiter = ' ');
	std::vector<uint32_t> get_array_objs(std::string& line, key_index key, char delimiter);

	template <typename T>
	std::vector<T> get_array_values_i(std::string& line, int start, char delimiter, char ele_del)
	{
		std::vector<T> r;

		int index = (line[start] == pair2[delimiter]) ? 1 : 0;

		int obj_start = start + index;
		int length_to_close = get_length_to(line, obj_start, delimiter);
		std::string linear_data = line.substr(obj_start, length_to_close);

		std::string value;
		std::stringstream stream(linear_data);
		while (std::getline(stream, value, ele_del))
		{
			if (value.empty()) { continue; }

			if constexpr (std::is_floating_point_v<T>)
			{
				try 
				{
					r.push_back(std::stof(value));
				}
				catch (std::invalid_argument& e)
				{
					std::cout << "Corrupt XREF...\n";
					return std::vector<T>();
				}
				catch (std::out_of_range& e)
				{
					std::cout << "Corrupt XREF...\n";
					return std::vector<T>();
				}
			}
			else if constexpr (std::is_integral_v<T>)
			{
				try
				{
					r.push_back(std::stoi(value));
				}
				catch (std::invalid_argument& e)
				{
					std::cout << "Corrupt XREF...\n";
					return std::vector<T>();
				}
				catch (std::out_of_range& e)
				{
					std::cout << "Corrupt XREF...\n";
					return std::vector<T>();
				}
			}
			else
			{
				r.push_back(value);
			}
		}


		return r;
	}
	template <typename T>
	std::vector<T> get_array_values_i(std::string& line, std::vector<key_index> type, char delimiter, char ele_del = ' ')
	{
		int start = has_key<key_index>(line, type);
		return get_array_values_i<T>(line, start, delimiter, ele_del);
	}

	template <typename T>
	T get_key_value_i(std::string& line, int start)
	{
		return get_array_values_i<T>(line, start, ' ', ' ')[0];
	}

	template <typename T>
	T get_key_value_i(std::string& line, std::vector<key_index> type, char delimiter = ' ', char ele_del = ' ')
	{
		return get_array_values_i<T>(line, type, delimiter, ele_del)[0];
	}


	std::vector<std::string> get_array_values_s(std::string& line, int start, char delimiter = ']', char ele_del = ' ');
	std::vector<std::string> get_array_values_s(std::string& line, key_index start, char delimiter = ']', char ele_del = ' ');

	std::vector<uint32_t> get_indirect_array_obj(std::string& line, int start, char delimiter = ' ');
	std::vector<uint32_t> get_indirect_array_obj(std::string& line, label_index label, char delimiter);
	std::vector<uint32_t> get_indirect_array_obj(std::string& line, std::vector<key_index> key, char delimiter);

	uint32_t get_indirect_obj(std::string& line, std::vector<key_index> key, char delimiter);
	uint32_t get_indirect_obj(std::string& line, label_index key, char delimiter);
	

	//std::vector<uint32_t> get_indirect_array_obj_multiple(std::string& line, label_index label, char delimiter);

	template <typename T>
	inline std::vector<int> get_mapped_values(std::vector<std::string> list, std::unordered_map<std::string, T> map)
	{
		std::vector<int> procsets;
		for (auto& x : list)
		{
			procsets.push_back(map[x]);
		}
		return procsets;
	}

	void zlib_decode_deflate(std::ifstream& file, size_t off, std::string& out);

	void read_data_block(std::string& line, std::string& block, int start, const std::string& delimiter);
	void read_data_block(std::string& line, std::string& block, key_line_index key, key_line_index key_end);

	void break_marker_lines(std::string& line, std::string& string);

	template <typename T>
	void read_line_with(std::string& line, std::string& block, T key)
	{
		std::stringstream stream(line);
		while (std::getline(stream, block))
		{
			size_t size = has_key<T>(block, key);
			if (size) { break; }
		}		
	}

	template <typename T>
	void read_line_with(std::string& line, std::string& block, std::vector<T> key)
	{
		std::stringstream stream(line);
		while (std::getline(stream, block))
		{
			size_t size = has_key<T>(block, key);
			if (size) { break; }
		}
	}

	void read_obj_block(std::istream& file, std::string& block, int start);
	void read_dict_block(std::string& file, std::string& block,key_index key);

}