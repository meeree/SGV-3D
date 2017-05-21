CFLAGS=-ggdb -std=c++11  
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

run : sceneGraph.o base.o main.o graphViewer.o
	g++ sceneGraph.o base.o main.o graphViewer.o -o run $(CFLAGS) $(OPENGL) 

sceneGraph.o : ../src/sceneGraph.cpp ../src/base.cpp
	g++ -c ../src/sceneGraph.cpp $(CFLAGS) 

base.o : ../src/base.cpp
	g++ -c ../src/base.cpp $(CFLAGS)

main.o : ../src/main.cpp ../src/sceneGraph.cpp ../src/graphViewer.cpp
	g++ -c ../src/main.cpp $(CFLAGS) 

graphViewer.o : ../src/graphViewer.cpp
	g++ -c ../src/graphViewer.cpp $(CFLAGS)

clean : 
	rm *.o run
