#pragma once
#include "pdf_mill_core.h"
#include "pdf_mill_alloc.h"

namespace PDF_MILL
{
	struct string
	{
		string() = default;
		private:
			char* str_ptr;
			size_t capacity;
			size_t size;
		public:

		    static string new_string(size_t cap = 0)
		    {
				string str;
				str.str_ptr = string_allocator->alloc(cap);
				str.capacity = cap;
				return str;
			}

			void delete_string()
			{
				string_allocator->dealloc(capacity);
			}

			size_t size()
			{
				return size;
			};

			size_t lenght()
			{
				return size;
			}

			char operator[](const int i) { return str_ptr[i]; }
			bool operator==(const string& other)
			{
				return other.capacity == capacity && memcmp(str_ptr, other.str_ptr, capacity);
			}

			//Don't use for now...Not needed...
			void operator+(const string& other)
			{
				memccpy((void*)(str_ptr + capacity), other.str_ptr, other.capacity);
			}

			char* get() { return str_ptr; }

			static void copy_string(char* src, char* dest, size_t size, size_t dest_off = 0)
			{
				memcpy(dest + dest_off, src, size);
			}

			void append(char* str)
			{
				memcpy()
			}
	};

	StackAllocator* string_allocator = nullptr;
}