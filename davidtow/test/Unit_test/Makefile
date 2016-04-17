
proj_dir = ../..

src_dir = $(proj_dir)/src
include_dir = $(proj_dir)/include
obj_dir = ./objects

all: Runner.o network_structures.o
	g++ -o test $(obj_dir)/*.o -l gtest -l pthread
	
Runner.o:
	g++ -o $(obj_dir)/Runner.o -c ./src/Runner.cpp -I $(include_dir)
	
network_structures.o: 
	g++ -o $(obj_dir)/network_structures.o -c $(src_dir)/network_structures.cpp -I $(include_dir)