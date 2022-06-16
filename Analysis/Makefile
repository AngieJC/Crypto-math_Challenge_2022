# Dont forget to change the gurobi lib paths

all: build

build: 
	g++ -march=native -O3 -pthread -std=c++17 Analysis.cpp -o Analysis \
	 -I/home/chengjunjie/.gurobi/gurobi951/linux64/include -L/home/chengjunjie/.gurobi/gurobi951/linux64/lib -lgurobi_c++ -lgurobi95
# 	 -I/Library/gurobi912/linux64/include -L/Library/gurobi912/mac64/lib -lgurobi_c++ -lgurobi91

clean: 
	rm -f Analysis

