cmake_minimum_required(VERSION 3.20)
project(client_db)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

set(PostgreSQL_ROOT /opt/homebrew/opt/postgresql@15) # Закоменнтируйте эту строчку! Ее я добавил из-за того, что cmake не может найти библиотеку самой postgresql именно у меня

#add_subdirectory(${PROJECT_SOURCE_DIR}/lib/libpqxx-7.7.5 libpqxx-build)
add_subdirectory(${PROJECT_SOURCE_DIR}/lib/pdb)

add_executable(${PROJECT_NAME} source/main.cpp)
include_directories(${PROJECT_SOURCE_DIR}/lib/pdb)
target_link_libraries(${PROJECT_NAME} PUBLIC pdb)