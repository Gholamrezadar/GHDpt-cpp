#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <cstdio>
#include "utils/renderer.h"
#include <thread>

// Forward declaration of callback
void glfw_error_callback(int error, const char* description);

int main() {
    // Set up error callback
    glfwSetErrorCallback(glfw_error_callback);

    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Set OpenGL version to 3.3 core profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "GHDpt", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        return -1;
    }

    // Set up Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io; // Avoid unused variable warning

    // Set Dear ImGui style
    ImGui::StyleColorsDark();

    // Set up Dear ImGui backend bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    Renderer renderer(800, 600, 4, 4);
    renderer.set_scene_name(SceneName::FLOOR_SPHERE);

    // renderer.render();

    // Scene Selector
    const char* scene_names[] = {
        "Floor Sphere",
        "Three Spheres",
        "Three Spheres 2",
        "Three Spheres 3",
        "FoV",
        "Random",
        "GHD"
    };
    int scene_selector = 0;
    int gui_width = 800;
    int gui_height = 600;
    int gui_samples_per_pixel = 4;
    int gui_max_depth = 4;
    float gui_aperture = 0.1;
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Poll events
        glfwPollEvents();
        
        // Start Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("Settings");
        
        // Render Button
        if (ImGui::Button("Render")) {
            // Handle button press (optional)
            renderer.reset();
        }

        if(renderer.get_current_iteration() < renderer.get_samples_per_pixel()) {
            renderer.set_current_iteration(renderer.get_current_iteration() + 1);
            renderer.render();
        }
        else if(renderer.get_current_iteration() == renderer.get_samples_per_pixel()) {
            std::cout<<"Rendering finished in "<<renderer.get_render_time()<<" seconds"<<std::endl;
            std::cout<<"\n";
            renderer.set_current_iteration(renderer.get_current_iteration() + 1);
        }

        // Render time
        ImGui::Text("Render time: %.3f seconds", renderer.get_render_time());

        // divider
        ImGui::Separator();

        // resolution input
        ImGui::Text("Resolution");
        ImGui::InputInt("Width", &gui_width);
        ImGui::InputInt("Height", &gui_height);
        renderer.set_image_width(gui_width);
        renderer.set_image_height(gui_height);

        // divider
        ImGui::Separator();

        // samples per pixel input
        ImGui::Text("Samples per Pixel");
        ImGui::InputInt("Samples per Pixel", &gui_samples_per_pixel);
        renderer.set_samples_per_pixel(gui_samples_per_pixel);

        // max depth input
        ImGui::Text("Max Depth");
        ImGui::InputInt("Max Depth", &gui_max_depth);
        renderer.set_max_depth(gui_max_depth);

        // divider
        ImGui::Separator();

        // scene selector
        ImGui::Text("Select a Scene");
        if (ImGui::Combo("Scene", &scene_selector, scene_names, IM_ARRAYSIZE(scene_names))) {
            renderer.set_scene_name(static_cast<SceneName>(scene_selector));
        }

        // divider
        ImGui::Separator();

        // aperture input
        ImGui::Text("Aperture");
        ImGui::InputFloat("Aperture", &gui_aperture);
        renderer.set_camera_aperture(gui_aperture);

        ImGui::End();

        ImGui::Begin("Viewport");
        ImVec2 uv0 = ImVec2(0.0f, 1.0f); // Bottom-left
        ImVec2 uv1 = ImVec2(1.0f, 0.0f); // Top-right (flipped vertically)
        ImGui::Image(renderer.get_image().get_imgui_texture_id(), ImVec2(renderer.get_width(), renderer.get_height()), uv0, uv1);
        ImGui::End();

        // Render Dear ImGui
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark gray background
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffers
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "GLFW Error (%d): %s\n", error, description);
}
