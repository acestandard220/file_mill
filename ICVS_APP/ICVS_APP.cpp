#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_opengl3.h"
#include "imgui/imgui_impl_glfw.h"

#include <iostream>

#include "../ICVS/includes/pdf_mill.h"
#include "../ICVS_RENDERER/renderer.h"

int win_width = 1280;
int win_height = 1024;

bool show_ = true;

//TODO: Add JSON data reading
//TODO: Read these values from file

const char** r{};

struct current_file
{
    PDF_MILL::_filedata* file = nullptr;

    char file_path_buffer[256] = "samplepdf.pdf";
    char write_path[256] = "outputpdf.pdf";
    std::string file_name_buffer = "No file loaded.";
    
    int page_number = 1;
    int remove_page = 4;
    int number_of_pages = 0;

    int file_index = 0;
};
current_file* _file = nullptr;


void render_()
{
    static bool dockspaceOpen = true;
    static bool opt_fullscreen = true;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
    ImGui::PopStyleVar(2);

    // Submit the DockSpace
    ImGuiIO& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    bool demo = true;
    ImGui::Begin("Inspector");
    ImGui::Text("%s", _file->file_name_buffer.c_str());
    if (ImGui::InputText("Path From", _file->file_path_buffer, 256, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Open File", ImVec2(440, 20)))
    {
        PDF_MILL::ShutDown();
        PDF_MILL::RequestReadPath(_file->file_path_buffer);
        PDF_MILL::ReadToStructure();

        _file->file = PDF_MILL::GetCurFileData();

        std::string file_p = _file->file_path_buffer;
        uint32_t index = (uint32_t)file_p.find(".");
        _file->file_name_buffer = "FILE::: " + file_p.substr(0, index);

       _file->number_of_pages = PDF_MILL::GetNumberOfPages(_file->file);
       _file->file_index = 1;
    }

    if (ImGui::Button("Add Page", ImVec2(440, 20)))
    {
        auto ptr = _file->file;
        if (!ptr)
        {
            ImGui::OpenPopup("No files are currently open");
            ImGui::BeginPopupModal("Message Box", NULL, ImGuiWindowFlags_AlwaysAutoResize);
            std::cout << "No files are currently open\n";
        }
        else
        {
            PDF_MILL::AddPage(ptr);
        }
    }

    if (ImGui::InputInt("Page", &_file->remove_page) || ImGui::Button("Remove Page", ImVec2(440, 20)))
    {
        PDF_MILL::RemovePage(PDF_MILL::GetCurFileData(), _file->remove_page);
    }

    if (ImGui::Button("Add Font", ImVec2(440, 20)))
    {
        //PDF_MILL::AddFont(2, PDF_MILL::COURIER_BOLD);
    }

    if (ImGui::InputText("Path To", _file->write_path, 256, ImGuiInputTextFlags_EnterReturnsTrue) || ImGui::Button("Write To File", ImVec2(440, 20)))
    {
        PDF_MILL::RequestWritePath(_file->write_path);
        PDF_MILL::WriteToFile();
    }

    if(_file->file_index)
    {
        ImGui::SliderInt("Page Number", &_file->page_number, 1, PDF_MILL::GetNumberOfPages(PDF_MILL::GetCurFileData()));
    }

    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0,0 });

    ImGui::Begin("ScenePort");

    ImGui::Image(
        PDF_RENDERER::GetTextureID(), ImVec2(1280, 1024), ImVec2(0, 1), ImVec2(1, 0)
        );

    ImGui::End();

    ImGui::Begin("FILE DATA");

    ImGui::End();

    ImGui::PopStyleVar();

    ImGui::ShowDemoWindow(&show_);
    
    
    ImGui::End(); // End DockSpace
    
}

int main()
{
    glfwInit();
    GLFWwindow* window = glfwCreateWindow(win_width, win_height, "ICVS", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glewInit();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init("#version 130");


    bool show_demo_window = false;

    _file = new current_file;
    
    PDF_MILL::Initialize();
    PDF_RENDERER::InitializeRenderer();    

    glViewport(0, 0, 1280, 1024);
    while (!glfwWindowShouldClose(window))
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.1f, 0.3f, 0.2f, 1.0f);

        render_();
        
        if (_file->file_index)
        {
            PDF_RENDERER::BeginRender(_file->page_number);
            PDF_RENDERER::RenderPage(_file->page_number);
            PDF_RENDERER::RenderPageText(_file->page_number);
            PDF_RENDERER::EndRender();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}
