CFLAGS=-ggdb -std=c++11  
OPENGL=-L/usr/local/lib -lGLEW -lGLU -lm -lglfw3 -lrt -lm -ldl -lXrandr -lXinerama -lXi -lXcursor -lXrender -lGL -lm -lpthread -ldl -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lXxf86vm -lXext -lX11 -lpthread -lxcb -lXau -lXdmcp

run : main.o sceneGraph.o graphics.o
	g++ main.o sceneGraph.o graphics.o -o run $(CFLAGS) $(OPENGL) 

main.o : ../src2/main.cpp ../src2/sceneGraph.cpp ../src2/graphics.cpp
	g++ -c ../src2/main.cpp $(CFLAGS) 

sceneGraph.o : ../src2/sceneGraph.cpp
	g++ -c ../src2/sceneGraph.cpp $(CFLAGS) 

graphics.o : ../src2/sceneGraph.cpp ../src2/graphics.cpp
	g++ -c ../src2/graphics.cpp $(CFLAGS)

clean : 
	rm *.o run
