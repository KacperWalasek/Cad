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
	"Rendering/Anaglyph/AnaglyphCamera.cpp" "Geometry/Surfaces/SurfaceC0.h" "Geometry/Surfaces/SurfaceC0.cpp" 
	"Geometry/Surfaces/SurfaceCreator.h" "Geometry/Surfaces/SurfaceCreator.cpp" "interfaces/ISceneModifier.h"  
	"interfaces/ISelfControl.h" "interfaces/IPointOwner.h" "Geometry/Surfaces/SurfaceC2.h"
	"Geometry/Surfaces/SurfaceC2.cpp" "UI/FileLoader.h" "UI/FileLoader.cpp" "interfaces/ISerializable.h"
	"EventHandlers/Collapse.h" "EventHandlers/Collapse.cpp" "Geometry/Surfaces/GregoryPatch.h" 
	"Geometry/Surfaces/GregoryPatch.cpp"   "interfaces/IUVSurface.h" "Geometry/Intersection.h" "Geometry/Intersection.cpp" "UI/SceneVector.h" "UI/SceneVector.cpp" "Geometry/BrokenLine.h" "Geometry/BrokenLine.cpp" "EventHandlers/RectSelection.h" "EventHandlers/RectSelection.cpp" "EventHandlers/CameraMainPositions.h" "EventHandlers/CameraMainPositions.cpp" "simulations/MillingMachine/MillingMachineSimulation.h" "simulations/MillingMachine/MillingMachineSimulation.cpp" "Simulations/Simulator.h" "Simulations/Simulator.cpp" "Simulations/ISimulation.h" "Simulations/MillingMachine/MillingPath.h" "Simulations/MillingMachine/MillingPath.cpp" "Simulations/MillingMachine/Cutter.h" "Simulations/MillingMachine/Cutter.cpp" "Simulations/MillingMachine/MaterialCube.h" "Simulations/MillingMachine/MaterialCube.cpp" "Rendering/ProceduralTexture.h" "Rendering/TextureRenderer.h" "Rendering/TextureRenderer.cpp" "Simulations/MillingMachine/MillingHeightMapRenderer.h" "Simulations/MillingMachine/MillingHeightMapRenderer.cpp")

set(SHADER_FILES 
	"Shaders/vertexShader.vert"
	"Shaders/fragmentShader.frag"
	"Shaders/torus.vert"
	"Shaders/uv.frag"
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
	"Shaders/Surface/surfaceC2.tesc"
	"Shaders/Surface/surfaceC2.tese"
	"Shaders/Surface/surfaceChain.tesc"
	"Shaders/Surface/surfaceChain.tese"
	"Shaders/Surface/gregory.tesc"
	"Shaders/Surface/gregory.tese"
	"Shaders/2dShader.vert"
	"Shaders/Phong/phong.vert"
	"Shaders/Phong/phong.frag"
	"Shaders/texture.vert"
	"Shaders/texture.frag"
	"Shaders/baseColor.frag"
	"Shaders/HeightMap/heightMap.tese"
	"Shaders/HeightMap/heightMap.tesc"
	"Shaders/HeightMap/heightMap.vert"
	"Shaders/HeightMap/heightMap.frag"
	"Shaders/HeightMap/wall.tese"
	"Shaders/HeightMap/wall.tesc"
	"Shaders/MillingPath/millingPath.vert"
	"Shaders/MillingPath/millingPath.frag"
	"Shaders/MillingPath/millingPathCirc.vert"
	"Shaders/MillingPath/millingPathCirc.frag"
	
)
add_executable(CAD ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} OpenGL::GL)
target_link_libraries(${PROJECT_NAME} GLEW::GLEW)
target_link_libraries(${PROJECT_NAME} glfw)
target_link_libraries(${PROJECT_NAME} imgui)
target_link_libraries(${PROJECT_NAME} tinyfiledialogs)
target_link_libraries(${PROJECT_NAME} nlohmann_json nlohmann_json::nlohmann_json)

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
