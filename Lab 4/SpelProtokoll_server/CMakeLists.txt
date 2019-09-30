PROJECT(Server)
FILE(GLOB Server_headers *.h)
FILE(GLOB Server_sources *.cpp)

find_package (Threads)

SET(files_Server ${Server_headers} ${Server_sources})
SOURCE_GROUP("Server" FILES ${Server_Server})

ADD_EXECUTABLE(Server ${files_Server})
target_link_libraries (Server ${CMAKE_THREAD_LIBS_INIT})
