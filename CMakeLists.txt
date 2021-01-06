cmake_minimum_required(VERSION 3.1)
project(oscgard VERSION 0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DASIO_STANDALONE)

set(SOURCES
    src/action.cpp   src/action.hpp
    src/args.cpp     src/args.hpp
    src/command.cpp  src/command.hpp
    src/server.cpp   src/server.hpp
    src/settings.cpp src/settings.hpp
    src/system.cpp   src/system.hpp
    src/types.hpp
    src/main.cpp
)

########################
# executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} osc++ pthread)

########################
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES LICENSE.md README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

########################
set(CPACK_STRIP_FILES ON)
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dimitry Ishenko <dimitry (dot) ishenko (at) (gee) mail (dot) com>")

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
