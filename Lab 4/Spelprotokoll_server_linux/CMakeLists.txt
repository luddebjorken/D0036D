PROJECT(LinuxServer)
FILE(GLOB LinuxServer_headers *.h)
FILE(GLOB LinuxServer_sources *.cpp)

find_package (Threads)

SET(files_LinuxServer ${LinuxServer_headers} ${LinuxServer_sources})
SOURCE_GROUP("LinuxServer" FILES ${LinuxServer_LinuxServer})

ADD_EXECUTABLE(LinuxServer ${files_LinuxServer})
target_link_libraries (LinuxServer ${CMAKE_THREAD_LIBS_INIT})
