CFLAGS=-ggdb -std=c++11  
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

run : main.o sceneGraph.o graphics.o
	g++ main.o sceneGraph.o graphics.o -o run $(CFLAGS) $(OPENGL) 

main.o : ../src/main.cpp ../src/sceneGraph.cpp ../src/graphics.cpp
	g++ -c ../src/main.cpp $(CFLAGS) 

sceneGraph.o : ../src/sceneGraph.cpp
	g++ -c ../src/sceneGraph.cpp $(CFLAGS) 

graphics.o : ../src/sceneGraph.cpp
	g++ -c ../src/graphics.cpp $(CFLAGS)

clean : 
	rm *.o run
