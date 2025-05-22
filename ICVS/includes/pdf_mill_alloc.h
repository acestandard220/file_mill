#pragma once
#include "pdf_mill_core.h"
#include "pdf_mill_page.h"

namespace PDF_MILL
{
	/**
		* @brief Stack Allocator mainly createed for string creations
		*        Memory is allocated from this pool once for every function that performs string operations
		* 	
	*/
	struct StackAllocator
	{
		private:
			StackAllocator() = default;
			StackAllocator(const StackAllocator& other) {};
			~StackAllocator() {};

			char* base_ptr;
			char* stack_ptr;
			size_t capacity;
			size_t default_alloc_size = 0;
		public:
			static StackAllocator* initialize_allocator(size_t cap,size_t default_alloc)
			{
				StackAllocator* st = new StackAllocator;
				st->base_ptr = (char*)malloc(cap);
				st->stack_ptr = st->base_ptr;
				st->capacity = cap;
				st->default_alloc_size = default_alloc;
				return st;
			}

			char* alloc(size_t size = 0)
			{
				if ((stack_ptr + size) - base_ptr > capacity)
				{
					return nullptr;
				}

				if (!size) { size = default_alloc_size; }

				char* ptr = stack_ptr;
				stack_ptr += size;
				return ptr;
			}

			void dealloc(size_t size)
			{
				stack_ptr = stack_ptr - size;
			}

			void clear()
			{
				stack_ptr = base_ptr;
			}

			char* get_base_ptr() { return base_ptr; }
			size_t get_default_alloc() { return default_alloc_size; }
	};




	struct PageAlloc
	{
		private:
			PageAlloc() = default;
			PageAlloc(const PageAlloc& other) = default;

			Page* base_ptr;
			Page* stack_ptr;

			int capacity;
			int size;
		public:
			static PageAlloc* initialize_allocator(int number)
			{
				PageAlloc* pg = new PageAlloc;
				pg->base_ptr = (Page*) malloc(sizeof(Page) * number);
				pg->stack_ptr = pg->base_ptr;
				pg->capacity = number;
				pg->size = 0;
				return pg;
			}

			Page* alloc(int number)
			{
				Page* ptr;
				ptr = stack_ptr;
				stack_ptr += number;
				size++;
				return ptr;
			}

			void dealloc(int number = 1)
			{
				stack_ptr -= 1;
				size--;
			}

			void clear()
			{
				stack_ptr = base_ptr;
				size = 0;
			}
			
	};
}