GDB=-ggdb 
GPROF=
CFLAGS=-std=c++11 $(GDB) $(GPROF)
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

flower : sceneGraph.o base.o animated_polar_flower.o logger.o runtimeOptions.o sgv_graphics.o graphics_internal.o
	g++ sceneGraph.o base.o animated_polar_flower.o logger.o runtimeOptions.o graphics_internal.o sgv_graphics.o -o flower $(CFLAGS) $(OPENGL) 

sceneGraph.o : ../../src/sceneGraph.cpp ../../src/base.cpp ../../src/runtimeOptions.cpp
	g++ -c ../../src/sceneGraph.cpp $(CFLAGS) 

base.o : ../../src/base.cpp ../../src/logger.cpp
	g++ -c ../../src/base.cpp $(CFLAGS) 

animated_polar_flower.o : ../animated_polar_flower.cpp ../../src/graphics_internal.cpp ../../src/runtimeOptions.cpp
	g++ -c ../animated_polar_flower.cpp $(CFLAGS) 

logger.o : ../../src/logger.cpp 
	g++ -c ../../src/logger.cpp $(CFLAGS)

runtimeOptions.o : ../../src/runtimeOptions.cpp
	g++ -c ../../src/runtimeOptions.cpp $(CFLAGS)

graphics_internal.o : ../../src/graphics_internal.cpp
	g++ -c ../../src/graphics_internal.cpp $(OPENGL) $(CFLAGS)

sgv_graphics.o : ../../src/sgv_graphics.cpp ../../src/graphics_internal.cpp
	g++ -c ../../src/sgv_graphics.cpp $(OPENGL) $(CFLAGS)

clean : 
	rm *.o flower
