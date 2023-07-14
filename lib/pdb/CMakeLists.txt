cmake_minimum_required(VERSION 3.20)
project(pdb)

set(SOURCE_FILES pdb.cxx)
set(HEADER_FILES pdb.hxx)

add_library(${PROJECT_NAME} STATIC ${HEADER_FILES} ${SOURCE_FILES})

add_subdirectory(../../lib/libpqxx libpqxx-build)
target_link_libraries(${PROJECT_NAME} pqxx)