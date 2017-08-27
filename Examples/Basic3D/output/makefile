GDB=-ggdb 
GPROF=
CFLAGS=-std=c++11 $(GDB) $(GPROF)
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

basic3d : sceneGraph.o base.o basic3d.o logger.o runtimeOptions.o sgv_graphics.o graphics_internal.o camera.o
	g++ sceneGraph.o base.o basic3d.o logger.o runtimeOptions.o graphics_internal.o sgv_graphics.o camera.o -o basic3d $(CFLAGS) $(OPENGL) 

sceneGraph.o : ../../../src/sceneGraph.cpp ../../../src/base.cpp ../../../src/runtimeOptions.cpp
	g++ -c ../../../src/sceneGraph.cpp $(CFLAGS) 

base.o : ../../../src/base.cpp ../../../src/logger.cpp
	g++ -c ../../../src/base.cpp $(CFLAGS) 

basic3d.o : ../basic3d.cpp ../../../src/graphics_internal.cpp ../../../src/runtimeOptions.cpp
	g++ -c ../basic3d.cpp $(CFLAGS) 

logger.o : ../../../src/logger.cpp 
	g++ -c ../../../src/logger.cpp $(CFLAGS)

runtimeOptions.o : ../../../src/runtimeOptions.cpp
	g++ -c ../../../src/runtimeOptions.cpp $(CFLAGS)

graphics_internal.o : ../../../src/graphics_internal.cpp
	g++ -c ../../../src/graphics_internal.cpp $(OPENGL) $(CFLAGS)

sgv_graphics.o : ../../../src/sgv_graphics.cpp ../../../src/graphics_internal.cpp
	g++ -c ../../../src/sgv_graphics.cpp $(OPENGL) $(CFLAGS)

camera.o : ../../../src/camera.cpp ../../../src/base.cpp 
	g++ -c ../../../src/camera.cpp $(CFLAGS)

clean : 
	rm *.o basic3d 
