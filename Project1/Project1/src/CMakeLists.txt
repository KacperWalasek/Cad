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
	 
	 
	"interfaces/ISceneElement.h" 
	"Geometry/Torus.h" 
	"Geometry/Torus.cpp"  
	"interfaces/IRenderable.h" 
	"EventHandlers/SelectedMovement.h" 
	"EventHandlers/SelectedMovement.cpp" 
	"Scene/Scene.h" 
	"Scene/Scene.cpp" 
	"Geometry/Point.h" 
	"Geometry/Point.cpp" 
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
	 
	 
	"Geometry/Curves/CurveC0.h" 
	"Geometry/Curves/CurveC0.cpp"  
	"Geometry/Curves/BezierCurve.h" 
	"Geometry/Curves/BezierCurve.cpp" 
	"Geometry/Curves/CurveC2.h" 
	"Geometry/Curves/CurveC2.cpp" 
	"interfaces/IClickable.h" 
	"interfaces/ICustomMove.h" 
	"Indexer.h" 
	"Geometry/Curves/InterpolationCurve.h" 
	"Geometry/Curves/InterpolationCurve.cpp" 
	"Math/CadMath.h" 
	"Math/CadMath.cpp"  
	"Rendering/VariableManager.h"  
	"CadApp.h" 
	"CadApp.cpp" 
	"Rendering/Anaglyph/AnaglyphRenderer.h" 
	"Rendering/Anaglyph/AnaglyphRenderer.cpp" 
	"Rendering/Anaglyph/AnaglyphCamera.h" 
	"Rendering/Anaglyph/AnaglyphCamera.cpp" "Geometry/Surfaces/SurfaceC0.h" "Geometry/Surfaces/SurfaceC0.cpp" "Geometry/Surfaces/SurfaceCreator.h" "Geometry/Surfaces/SurfaceCreator.cpp" "interfaces/ISceneModifier.h"  "interfaces/ISelfControl.h" "interfaces/IPointOwner.h")

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
	"Shaders/DeBoor/deboor.vert"
	"Shaders/Surface/surfaceC0.tesc"
	"Shaders/Surface/surfaceC0.tese"
	"Shaders/Surface/surfaceC0.vert"
	"Shaders/Surface/surfaceChain.tesc"
	"Shaders/Surface/surfaceChain.tese"
)
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
