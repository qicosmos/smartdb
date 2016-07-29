cmake_minimum_required(VERSION 2.8)
project(smartdb)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -std=c++14")
#aux_source_directory(. SRC_LIST)
#add_executable(${PROJECT_NAME} ${SRC_LIST})

SET(PG_BASE "/usr")
SET(SOURCE_FILES
        main.cpp   
        )

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
INCLUDE_DIRECTORIES("${PG_BASE}/include/postgresql"
#                   "${PG_BASE}/include/postgresql/9.3/server/catalog"
					"include"
                   )
LINK_DIRECTORIES("${PG_BASE}/lib/postgresql")
TARGET_LINK_LIBRARIES(${PROJECT_NAME} "${PG_BASE}/lib/libpq.so")
