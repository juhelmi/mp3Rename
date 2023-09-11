#Makefile

all: prepare dependency build_binary

build_binary:
	cmake --build build/ -j

dependency:
	cd build && cmake .. --graphviz=graph.dot && dot -Tpng graph.dot -o graphImage.png

prepare:
	rm -rf build
	mkdir build
	mkdir build/boost_root
