BUILD_DIR=build

all:
	meson compile -C ${BUILD_DIR}

setup:
	mkdir -p ${BUILD_DIR}
	meson setup ${BUILD_DIR} --buildtype=debug 

clean:
	rm -rf ${BUILD_DIR}
	
run: all
	${BUILD_DIR}/echo_example
