all:
	mkdir -p build
	cd build; cmake -DCMAKE_BUILD_TYPE="Debug" ..; make -j3 && make test ARGS="-V"

clean:
	rm -rf build
