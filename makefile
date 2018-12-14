frame:src/make_index.cpp src/add_index.cpp src/query_index.cpp 
	g++ -o bin/make_index src/make_index.cpp -l pthread
	g++ -o bin/add_index src/add_index.cpp  -l pthread
	g++ -o bin/add_sam src/add_sam.cpp -l pthread
	g++ -o bin/query_index src/query_index.cpp -l pthread
comp_sam:src/comp.h src/comp_sam.cpp
	g++ -o bin/comp_sam  src/comp_sam.cpp -l pthread
clean:
	rm -rf bin/* 
