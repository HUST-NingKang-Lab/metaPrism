#frame:src/make_index.cpp src/add_index.cpp src/query_index.cpp 
#	g++ -o bin/make_index src/make_index.cpp -l pthread
#	g++ -o bin/add_index src/add_index.cpp  -l pthread
#	g++ -o bin/add_sam src/add_sam.cpp -l pthread
#	g++ -o bin/query_index src/query_index.cpp -l pthread
cpu:cpu-src/make_index.cpp cpu-src/add_index.cpp cpu-src/query_index.cpp
	g++ -o bin/make_index cpu-src/make_index.cpp -l pthread 
	g++ -o bin/add_index cpu-src/add_index.cpp  -l pthread
	g++ -o bin/add_sam cpu-src/add_sam.cpp -l pthread
	g++ -o bin/query_index cpu-src/query_index.cpp -l pthread
gpu:gpu-code/query_index.cpp gpu-code/add_index.cpp gpu-code/make_index.cpp
	nvcc -x cu -o bin/gpu_query_index gpu-code/query_index.cpp -std=C++11

comp_sam:cpu-src/comp.h cpu-src/comp_sam.cpp
	g++ -o bin/comp_sam  cpu-src/comp_sam.cpp -l pthread
clean:
	rm -rf bin/* 
