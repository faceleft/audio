BUILD_DIR=build/

CC=clang
CXX=clang++

all:
	mkdir -p ${BUILD_DIR}
	CC=${CC} CXX=${CXX} cmake -B ${BUILD_DIR} -G Ninja
	cmake --build build -- -j$(shell nproc)
clean:
	rm -rf ${BUILD_DIR}
run: all
	${BUILD_DIR}/examples/echo/ehco_example
