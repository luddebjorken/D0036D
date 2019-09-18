PROJECT(Client)
FILE(GLOB Client_headers *.h)
FILE(GLOB Client_sources *.cpp)

find_package (Threads)

SET(files_Client ${Client_headers} ${Client_sources})
SOURCE_GROUP("Client" FILES ${Client_Client})

ADD_EXECUTABLE(Client ${files_Client})
target_link_libraries (Client ${CMAKE_THREAD_LIBS_INIT})
