set(PROJECT_SOURCES 
	"main.cpp"
	"Math/Matrix4x4.cpp"
	"Math/Vector4.cpp"
	"Math/Matrix4x4.h"
	"Math/Vector4.h"
	"Rendering/Window.h" 
	"Rendering/Window.cpp" 
	"Rendering/Renderer.h" 
	"Rendering/Renderer.cpp" 
	"Rendering/Shader.h" 
	"Rendering/Shader.cpp" "Geometry/Elipse.h" "Geometry/Elipse.cpp" "Rendering/Transform.h" "Rendering/Transform.cpp" "UI/IGui.h" "UI/ElipseGui.h" "UI/ElipseGui.cpp" "Rendering/RGB.h" "Rendering/AdaptiveRendering.h" "Rendering/AdaptiveRendering.cpp")

set(SHADER_FILES 
	"Shaders/vertexShader.vert"
	"Shaders/fragmentShader.frag")
add_executable(CAD ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} ${GLEW_LIBRARIES})
target_link_libraries(${PROJECT_NAME} imgui)

# Copy shaders
foreach (shader_file ${SHADER_FILES})
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/${shader_file}
            ${CMAKE_CURRENT_BINARY_DIR}/${shader_file})

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND glslangValidator ${CMAKE_CURRENT_SOURCE_DIR}/${shader_file})
endforeach (shader_file)
