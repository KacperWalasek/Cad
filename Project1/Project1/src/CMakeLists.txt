set(PROJECT_SOURCES 
	"main.cpp"
	"Rendering/Window.h" 
	"Rendering/Window.cpp" 
	"Rendering/Renderer.h" 
	"Rendering/Renderer.cpp" 
	"Rendering/Shader.h" 
	"Rendering/Shader.cpp" 
	"Rendering/Transform.h" 
	"Rendering/Transform.cpp" 
	"interfaces/IGui.h" 
	"Geometry/TorusGeometry.h" 
	"Rendering/Sampler.h" 
	"Rendering/Mesh.h" 
	"Rendering/Mesh.cpp" 
	"Rendering/Sampler.cpp" 
	"Geometry/TorusGeometry.cpp" 
	"Rendering/Camera.h" 
	"Rendering/Camera.cpp" 
	"EventHandlers/EventHanlderInterfaces.h" 
	"EventHandlers/CameraMovement.h" 
	"EventHandlers/CameraMovement.cpp"  
	"Rendering/SceneObject.h" 
	 
	"Scene/ISceneElement.h" "Scene/Torus.h" "Scene/Torus.cpp"  "interfaces/IRenderable.h" "EventHandlers/SelectedMovement.h" "EventHandlers/SelectedMovement.cpp" "Scene/Scene.h" "Scene/Scene.cpp" "Scene/Point.h" "Scene/Point.cpp" "Scene/Cursor.h" "Scene/Cursor.cpp")

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
