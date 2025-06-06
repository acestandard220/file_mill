#include "renderer.h"

#define TO_INCHES (1/72)

namespace PDF_RENDERER
{
	Renderer2D* renderer_data = nullptr;
	LineRenderer* line_renderer_data = nullptr;

	std::array<std::string, PDF_MILL::_base_font_last_> base_font_path_string
	{ 
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\courier.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\courier_bold.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\courier_bold_oblique.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\courier_oblique.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\helvetica.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\helvetica_bold.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\helvetica_bold_oblique.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\helvetica_oblique.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\test_font.ttf", //Not available symbol.ttf
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\times_roman.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\times_roman_bold.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\times_roman_bold_italic.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\times_roman_italic.ttf",
		"P:\\Projects\\VIM\\ImgConvs\\x64\\Debug\\fonts\\test_font.ttf",//Not available zapfdingbats.ttf
	};

	const char* vertex_source = R"(
		#version 430
        layout(location = 0) in vec2 aPos;
		layout(location = 1) in vec2 aTex;

		out vec2 TexCoord;
		
		uniform mat4 projection;
		
		void main()
		{
			gl_Position = projection * vec4(aPos,0.0,1.0);
			TexCoord = aTex;
		}
	)";

	const char* fragment_source = R"(
		#version 430
		out vec4 frag;

		in vec2 TexCoord;

		uniform sampler2D text;
		uniform int use_alpha_texture;
		
		void main()
		{
			if(use_alpha_texture == 0)
			{
				frag= vec4(1.0f,1.0,1.0f,1.0f);
			}
			else
			{
				frag = vec4(0.0f, 0.0f, 0.0f, texture(text,TexCoord).r);
			}
		}

	)";

	Renderer2D::~Renderer2D()
	{
		glDeleteVertexArrays(1, &renderer_data->vao);
		glDeleteBuffers(1, &renderer_data->vbo);

		glDeleteProgram(renderer_data->program);
	}

	void InitializeFrameBufffer()
	{
		glGenFramebuffers(1, &renderer_data->frame_buffer);
		glBindFramebuffer(GL_FRAMEBUFFER, renderer_data->frame_buffer);

		glGenTextures(1, &renderer_data->frame_texture);
		glBindTexture(GL_TEXTURE_2D, renderer_data->frame_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glBindTexture(GL_TEXTURE_2D, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderer_data->frame_texture, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "Framebuffer is incomplete\n";
			return;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void InitializeCharacters()
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (FT_Init_FreeType(&renderer_data->ft))
		{
			std::cout << "Could not initialize FreeType...\n";
			return;
		}
		for (int i = 0; i < PDF_MILL::_base_font_last_; i++)
		{
			
			if (FT_New_Face(renderer_data->ft, base_font_path_string[i].c_str(), 0, &renderer_data->face))
			{
				std::cout << "Failed to load face...\n";
				return;
			}

			FT_Set_Char_Size(renderer_data->face, 0, 48 * 64, 72, 72);
			//FT_Set_Pixel_Sizes(renderer_data->face, 0, 48);

			for (unsigned char c = 0; c < 128; c++)
			{
				if (FT_Load_Char(renderer_data->face, c, FT_LOAD_RENDER))
				{
					std::cout << "Failed to load glyph for: " << c << std::endl;
					continue;
				}

				uint32_t texture_id;
				glGenTextures(1, &texture_id);
				glBindTexture(GL_TEXTURE_2D, texture_id);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
					renderer_data->face->glyph->bitmap.width,
					renderer_data->face->glyph->bitmap.rows, 0,
					GL_RED, GL_UNSIGNED_BYTE,
					renderer_data->face->glyph->bitmap.buffer
				);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				Character character;
				character.textureID = texture_id;
				character.size = { renderer_data->face->glyph->bitmap.width,renderer_data->face->glyph->bitmap.rows };
				character.bearing = { renderer_data->face->glyph->bitmap_left, renderer_data->face->glyph->bitmap_top };
				character.advance = renderer_data->face->glyph->advance.x;

				renderer_data->cCharacters[(PDF_MILL::base_font)i][c] = character;
			}
		}
	}

	void InitializeRenderer()
	{
		glewInit();

		renderer_data = new Renderer2D;

		InitializeCharacters();
		InitializeFrameBufffer();

		renderer_data->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		renderer_data->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		renderer_data->program = glCreateProgram();

		glShaderSource(renderer_data->vertex_shader, 1, &vertex_source, nullptr);
		glShaderSource(renderer_data->fragment_shader, 1, &fragment_source, nullptr);

		glCompileShader(renderer_data->vertex_shader);
		glCompileShader(renderer_data->fragment_shader);

		glAttachShader(renderer_data->program, renderer_data->vertex_shader);
		glAttachShader(renderer_data->program, renderer_data->fragment_shader);

		glLinkProgram(renderer_data->program);

		glGenVertexArrays(1, &renderer_data->vao);
		glBindVertexArray(renderer_data->vao);

		glGenBuffers(1, &renderer_data->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, renderer_data->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);


		//Line Renderer Initialization
		line_renderer_data = new LineRenderer;
		line_renderer_data->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
		line_renderer_data->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);

		line_renderer_data->program = glCreateProgram();

		glShaderSource(line_renderer_data->vertex_shader, 1, &vertex_source, nullptr);
		glShaderSource(line_renderer_data->fragment_shader, 1, &fragment_source, nullptr);

		glCompileShader(line_renderer_data->vertex_shader);
		glCompileShader(line_renderer_data->fragment_shader);

		glAttachShader(line_renderer_data->program, line_renderer_data->vertex_shader);
		glAttachShader(line_renderer_data->program, line_renderer_data->fragment_shader);

		glLinkProgram(line_renderer_data->program);

		glGenVertexArrays(1, &line_renderer_data->vao);
		glBindVertexArray(line_renderer_data->vao);

		glGenBuffers(1, &line_renderer_data->vbo);
		glBindBuffer(GL_ARRAY_BUFFER, line_renderer_data->vbo);
		glBufferData(GL_ARRAY_BUFFER, 16 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void BeginRender(int page_number)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, renderer_data->frame_buffer);
		glUseProgram(renderer_data->program);

		glm::mat4 projection(1.0f);

		auto mb = PDF_MILL::GetPageMediaBox(PDF_MILL::GetCurFileData(), page_number);
		projection = glm::ortho(0.0f, (float) mb[2], 0.0f, (float) mb[3]);

		float page_aspect = mb[2] / mb[3];

		if (page_aspect > 1.0f)
		{
			projection = glm::ortho(0.0f, (float) mb[2] , 0.0f, (float) mb[2]  / page_aspect);
		}
		else
		{
			projection = glm::ortho(0.0f, (float) mb[3] * page_aspect , 0.0f, (float) mb[3] );
		}

		glUniformMatrix4fv(glGetUniformLocation(renderer_data->program, "projection"), 1, GL_FALSE, &projection[0][0]);
	}

	void EndRender()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//NOTE: Consider if this indirection is needed.
	void _render_page_box(std::array<float,4> data)
	{
		glBindVertexArray(renderer_data->vao);
		glBindBuffer(GL_ARRAY_BUFFER, renderer_data->vbo);

		float vertices[24] = {
			0.0f,    0.0f,    0.0,0.0,
			0.0f,    data[3], 0.0,1.0,
			data[2], 0.0,     1.0,0.0,

			0.0f,    data[3], 0.0,1.0,
			data[2], 0.0,     1.0,0.0,
			data[2], data[3], 1.0,1.0  
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, vertices, GL_DYNAMIC_DRAW);

		glUniform1i(glGetUniformLocation(renderer_data->program, "use_alpha_texture"), 0);

		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void RenderPage(int page_number)
	{
		auto mb = PDF_MILL::GetPageMediaBox(PDF_MILL::GetCurFileData(), page_number);
		_render_page_box(mb);
	}

	void RenderBoxOutLines(std::array<float, 4> data)
	{
		glBindVertexArray(line_renderer_data->vao);
		glBindBuffer(GL_ARRAY_BUFFER, line_renderer_data->vbo);

		float vertices[10] =
		{
			data[0], data[1],
			data[0], data[3],
			data[2], data[3],
			data[2], data[1],
			data[0], data[1]
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 10, vertices, GL_DYNAMIC_DRAW);
		glUseProgram(line_renderer_data->program);

	

		glDrawArrays(GL_LINE_STRIP, 0, 5);

		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void RenderPageText(int page_number)
	{
		std::vector<PDF_MILL::TextBlock> text_blocks = PDF_MILL::GetPageTextBlocks(PDF_MILL::GetCurFileData(), page_number);

		renderer_data->page_size = { PDF_MILL::GetPageMediaBox(PDF_MILL::GetCurFileData(),page_number)[2],PDF_MILL::GetPageMediaBox(PDF_MILL::GetCurFileData(),page_number)[3] };

		for (auto text_block : text_blocks)
		{
			float scale = 1; text_block.font_size / 64;

			auto base_font = PDF_MILL::GetPageBaseFont(PDF_MILL::GetCurFileData(), page_number, text_block.font_tag);
			glm::vec2 font_position = { text_block.text_matrix[4],text_block.text_matrix[5]};

			FT_Set_Char_Size(renderer_data->face, 0, text_block.font_size * 64, 72, 72);
			
			for (auto it = text_block.text.begin(); it < text_block.text.end(); it++)
			{
				glBindVertexArray(renderer_data->vao);
				glBindBuffer(GL_ARRAY_BUFFER, renderer_data->vbo);

				auto character_data = renderer_data->cCharacters[base_font][*it];
				
				float xpos = font_position.x + character_data.bearing.x * scale; 
				float ypos = font_position.y - (character_data.size.y - character_data.bearing.y) * scale ;

				float w = character_data.size.x * scale;
				float h = character_data.size.y * scale;

				float vertices[24] = {
				xpos,     ypos + h, 0.0, 0.0,
				xpos,     ypos,     0.0, 1.0,
				xpos + w, ypos,     1.0, 1.0,

				xpos,     ypos + h, 0.0, 0.0,
				xpos + w, ypos,     1.0, 1.0,
				xpos + w, ypos + h, 1.0, 0.0
				};
				
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 24, vertices, GL_DYNAMIC_DRAW);

				glUseProgram(renderer_data->program);
				glUniform1i(glGetUniformLocation(renderer_data->program, "use_alpha_texture"), 1);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, character_data.textureID);

				glDrawArrays(GL_TRIANGLES, 0, 6);

				glBindVertexArray(0);
				glBindBuffer(GL_ARRAY_BUFFER, 0);

				font_position.x += (character_data.advance >> 6 ) * scale;
			}
		}
	}

	uint32_t GetTextureID()
	{
		return renderer_data->frame_texture;
	}

	glm::vec2 CurrentPageSize()
	{ return renderer_data->page_size; }

	LineRenderer::~LineRenderer()
	{
		glDeleteVertexArrays(1, &line_renderer_data->vao);
		glDeleteBuffers(1, &line_renderer_data->vbo);
		glDeleteBuffers(1, &line_renderer_data->ebo);

		glDeleteProgram(line_renderer_data->program);
	}
}