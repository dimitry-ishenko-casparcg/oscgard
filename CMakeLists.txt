cmake_minimum_required(VERSION 3.1)
project(oscgard VERSION 3.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_definitions(-DASIO_STANDALONE)
add_definitions(-DVERSION="${PROJECT_VERSION}")

find_package(Threads REQUIRED)

set(SOURCES
    pgm/args.cpp    pgm/args.hpp
    src/action.cpp  src/action.hpp
    src/actions.cpp src/actions.hpp
    src/main.cpp
    src/server.cpp  src/server.hpp
    src/util.cpp    src/util.hpp
)

include(GNUInstallDirs)

########################
# executable
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} osc++ ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES systemd/oscgard-system.service DESTINATION /lib/systemd/system RENAME oscgard.service)
install(FILES systemd/oscgard-user.service DESTINATION /lib/systemd/user RENAME oscgard.service)

install(FILES LICENSE.md README.md DESTINATION ${CMAKE_INSTALL_DOCDIR})

########################
# package
set(CPACK_STRIP_FILES ON)
set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "OSC Gateway")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dimitry Ishenko <dimitry (dot) ishenko (at) (gee) mail (dot) com>")

set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libosc++ (>= 1.0)")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# source
set(CPACK_SOURCE_GENERATOR TBZ2)
set(CPACK_SOURCE_PACKAGE_FILE_NAME ${PROJECT_NAME}-${PROJECT_VERSION})
set(CPACK_SOURCE_IGNORE_FILES /\.git.*;/build.*;\.user$;/\.vscode.*)

include(CPack)
