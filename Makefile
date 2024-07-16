BUILD_DIR=build/

all:
	mkdir -p ${BUILD_DIR}
	cmake -B ${BUILD_DIR}
	cmake --build build -- -j$(shell nproc)

clean:
	rm -rf ${BUILD_DIR}
run: all
	${BUILD_DIR}/examples/echo/ehco_example
