CXX=g++
CXXFLAGS=-lglfw

shellTexturing: src/engine_core/main.cpp src/engine_core/ShaderManager.cpp src/engine_core/HeightsGenerator.cpp src/engine_core/Terrain.cpp src/engine_core/Skybox.cpp src/engine_core/GLCore.cpp src/utils/glad.c
	$(CXX) -o shellTexturing src/engine_core/main.cpp src/engine_core/ShaderManager.cpp src/engine_core/HeightsGenerator.cpp src/engine_core/Terrain.cpp src/engine_core/Skybox.cpp src/engine_core/GLCore.cpp src/utils/glad.c $(CXXFLAGS) 

ShaderManager.o: src/engine_core/ShaderManager.cpp
	$(CXX) -o ShaderManager.o src/engine_core/ShaderManager.cpp src/utils/glad.c $(CXXFLAGS) 

HeightsGenerator.o: src/engine_core/HeightsGenerator.cpp
	$(CXX) -o Terrain.o src/engine_core/HeightsGenerator.cpp $(CXXFLAGS) 

Terrain.o: src/engine_core/Terrain.cpp
	$(CXX) -o Terrain.o src/engine_core/Terrain.cpp src/utils/glad.c $(CXXFLAGS) 

Skybox.o: src/engine_core/Skybox.cpp
	$(CXX) -o Terrain.o src/engine_core/Skybox.cpp src/utils/glad.c $(CXXFLAGS) 

GLCore.o: src/engine_core/GLCore.cpp
	$(CXX) -o GLCore.o src/engine_core/GLCore.cpp src/utils/glad.c $(CXXFLAGS) 

.PHONY: clean

clean:
	rm shellTexturing