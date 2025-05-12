#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>

#include "../ICVS/includes/pdf_mill.h"

#include <ft2build.h>
#include FT_FREETYPE_H  

//Not in use don't ask why VS bugs...//
#ifdef PDF_RENDERER_EXPORTS
	#define ICVS_RENDERER_DLL __declspec(dllexport)
#else
	#define ICVS_RENDERER_DLL __declspec(dllexport)
#endif

namespace PDF_RENDERER
{
	struct Character
	{
		uint32_t textureID = 0;
		glm::ivec2 size{ 10 };
		glm::ivec2 bearing{ 5 };
		uint32_t advance;
	};

	struct LineRenderer
	{
		LineRenderer() = default;
		~LineRenderer();

		uint32_t vao;
		uint32_t vbo;
		uint32_t ebo;

		uint32_t program;
		uint32_t vertex_shader;
		uint32_t fragment_shader;

	};

	struct Renderer2D
	{
		Renderer2D() = default;
		~Renderer2D();
		uint32_t vao;
		uint32_t vbo;

		uint32_t program;
		uint32_t vertex_shader;
		uint32_t fragment_shader;

		uint32_t frame_buffer;
		uint32_t frame_texture;


		FT_Library ft;
		FT_Face face;
		std::map<PDF_MILL::base_font, std::map<char, Character>>cCharacters;
		glm::vec2 page_size;
	};

	struct PageVertex
	{
		glm::vec2 size; //TODO: give default size
	};

	extern "C"
	{
		__declspec(dllexport) void InitializeRenderer();
		__declspec(dllexport) void BeginRender(int page_number);
		__declspec(dllexport) void EndRender();
		
		__declspec(dllexport) void RenderPage(int page_number);
		__declspec(dllexport) void RenderBoxOutLines(std::array<float, 4>);

		__declspec(dllexport) void RenderPageText(int page_number);

		__declspec(dllexport) uint32_t GetTextureID() ;
		__declspec(dllexport) glm::vec2 CurrentPageSize() ;
	}
}