filelines_gen: filelines_gen.cpp find_most_freq.h
	g++ filelines_gen.cpp -o filelines_gen -O2
filelines: filelines.o find_most_freq.o filelines_baseline.o
	g++ filelines.o find_most_freq.o filelines_baseline.o -O2 -o filelines
filelines.o: filelines.cpp find_most_freq.h
	g++ filelines.cpp -c -O2 
find_most_freq.o: find_most_freq.cpp find_most_freq.h
	g++ find_most_freq.cpp -c -O2
filelines_baseline.o: filelines_baseline.cpp filelines_baseline.h
	g++ filelines_baseline.cpp -c -O2