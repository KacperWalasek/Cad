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
	 
	"Scene/ISceneElement.h" 
	"Scene/Torus.h" 
	"Scene/Torus.cpp"  
	"interfaces/IRenderable.h" 
	"EventHandlers/SelectedMovement.h" 
	"EventHandlers/SelectedMovement.cpp" 
	"Scene/Scene.h" 
	"Scene/Scene.cpp" 
	"Scene/Point.h" 
	"Scene/Point.cpp" 
	"Scene/Cursor.h" 
	"Scene/Cursor.cpp" 
	"UI/MainMenu.h" 
	"UI/MainMenu.cpp" 
	"Rotator.h" 
	"Rotator.cpp" 
	"EventHandlers/ClickSelection.h" 
	"EventHandlers/ClickSelection.cpp" 
	"Scene/Center.h" 
	"Scene/Center.cpp" 
	 
	"interfaces/ISceneTracker.h" 
	"interfaces/ITransformable.h" 
	"EventHandlers/ObjectFactory.h" 
	"EventHandlers/ObjectFactory.cpp" 
	 
	 
	"Scene/CurveC0.h" 
	"Scene/CurveC0.cpp"  
	"Scene/BezierCurve.h" 
	"Scene/BezierCurve.cpp" 
	"Scene/CurveC2.h" 
	"Scene/CurveC2.cpp" "interfaces/IClickable.h" "interfaces/ICustomMove.h" "Indexer.h" 
	"Scene/InterpolationCurve.h" "Scene/InterpolationCurve.cpp" "Math/CadMath.h" "Math/CadMath.cpp"  
	"Rendering/VariableManager.h"  "CadApp.h" "CadApp.cpp" "Rendering/AnaglyphRenderer.h" 
	"Rendering/AnaglyphRenderer.cpp" "Rendering/AnaglyphCamera.h" "Rendering/AnaglyphCamera.cpp")

set(SHADER_FILES 
	"Shaders/vertexShader.vert"
	"Shaders/fragmentShader.frag"
	"Shaders/ScreenTexture/screenTexture.vert"
	"Shaders/ScreenTexture/texture.frag"
	"Shaders/test.tesc"
	"Shaders/test.tese"
	"Shaders/test.vert"
	"Shaders/DeBoor/deboor.tesc"
	"Shaders/DeBoor/deboor.tese"
	"Shaders/DeBoor/deboor.vert")
add_executable(CAD ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
target_link_libraries(${PROJECT_NAME} glfw)
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
