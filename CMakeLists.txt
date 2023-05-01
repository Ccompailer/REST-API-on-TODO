cmake_minimum_required(VERSION 3.23)
project(newProject)

set(CMAKE_CXX_STANDARD 23)

find_package(BZip2 REQUIRED)
find_package(expat CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Poco REQUIRED COMPONENTS Net)

add_executable(newProject TodoSereverApp.cpp TodoServerApp.h MimeTypes.cpp MimeTypes.cpp MimeTypes.cpp MimeTypes.h)
target_link_libraries(newProject PRIVATE BZip2::BZip2)
target_link_libraries(newProject PRIVATE expat::expat)
target_link_libraries(newProject PRIVATE ZLIB::ZLIB)
target_link_libraries(newProject PRIVATE Poco::Net)