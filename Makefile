CMAKE_GENERATOR = Ninja
BUILD_DIR = build


.PHONY: all gen build clean cleanall


all:
	-@[ -d ${BUILD_DIR} ] || mkdir ${BUILD_DIR}
	-@[ -f ${BUILD_DIR}/CMakeCache.txt ] || { ${MAKE} gen ;}
	${MAKE} build


gen:
	@cmake -Wno-dev -S . -B ${BUILD_DIR} -G ${CMAKE_GENERATOR}


build:
	@cmake --build ${BUILD_DIR}


clean:
	@cmake --build ${BUILD_DIR} --target clean


cleanall:
	@echo "Removing ALL files in the build directory (restart)"
	-@rm -rf ${BUILD_DIR}/*
