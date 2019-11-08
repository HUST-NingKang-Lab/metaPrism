tool:src/make_index.cpp src/add_index.cpp src/query_index.cpp
	g++ -o bin/make_index cpu_src/make_index.cpp -l pthread
	g++ -o bin/add_index cpu_src/add_index.cpp  -l pthread
	g++ -o bin/add_sam cpu_src/add_sam.cpp -l pthread
cpu:
	g++ -g -std=c++11 -o bin/cpu_query cpu_src/query_index.cpp -l pthread
gpu:src/query_index.cpp
	nvcc -x cu -std=c++11 -o bin/gpu_query src/query.cu
debug:src/query_index.cpp
	nvcc -x cu -g -std=c++11 -o bin/gpu_query_debug src/query.cu
comp_sam:src/comp.h src/comp_sam.cpp
	g++ -o bin/comp_sam  src/comp_sam.cpp -l pthread
clean:
	rm -rf bin/* 
