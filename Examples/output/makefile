CFLAGS=-std=c++11  
GDB=-ggdb 
GPROF=
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

flower : sceneGraph.o base.o animated_polar_flower.o logger.o runtimeOptions.o graphics.o
	g++ sceneGraph.o base.o animated_polar_flower.o logger.o runtimeOptions.o graphics.o -o flower $(CFLAGS) $(GPROF) $(GDB) $(OPENGL) 

sceneGraph.o : ../../src/sceneGraph.cpp ../../src/base.cpp ../../src/runtimeOptions.cpp
	g++ -c ../../src/sceneGraph.cpp $(CFLAGS) $(GPROF) $(GDB) 

base.o : ../../src/base.cpp ../../src/logger.cpp
	g++ -c ../../src/base.cpp $(CFLAGS) $(GPROF) $(GDB) 

animated_polar_flower.o : ../animated_polar_flower.cpp ../../src/graphics.cpp ../../src/runtimeOptions.cpp
	g++ -c ../animated_polar_flower.cpp $(CFLAGS) $(GPROF) $(GDB) 

logger.o : ../../src/logger.cpp 
	g++ -c ../../src/logger.cpp $(CFLAGS) $(GPROF) $(GDB)

runtimeOptions.o : ../../src/runtimeOptions.cpp
	g++ -c ../../src/runtimeOptions.cpp $(CFLAGS) $(GPROF) $(GDB)

graphics.o : ../../src/graphics.cpp
	g++ -c ../../src/graphics.cpp $(OPENGL) $(CFLAGS) $(GPROF) $(GDB)

clean : 
	rm *.o flower
