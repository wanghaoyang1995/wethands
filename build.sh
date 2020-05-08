#! /bin/sh

PROJECT_DIR=$(pwd)
BUILD_DIR=${PROJECT_DIR}/build
BUILD_TYPE=${BUILD_TYPE:-debug} # release

mkdir -p ${BUILD_DIR}/${BUILD_TYPE} \
  && cd ${BUILD_DIR}/${BUILD_TYPE} \
  && cmake \
           -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
           -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
           ${PROJECT_DIR} \
  && make $*
