cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

project( covid19 )

set( CMAKE_PREFIX_PATH /usr/local/libtorch )

find_package( Torch REQUIRED )
find_package( PkgConfig REQUIRED )

pkg_check_modules( PYTHON REQUIRED python )
pkg_check_modules( PQXX REQUIRED libpqxx )

include_directories( include )

add_executable( covid19 src/main.cpp src/dataset.cpp src/net.cpp )

target_link_libraries( covid19 ${TORCH_LIBRARIES} ${PYTHON_LIBRARIES} ${PQXX_LIBRARIES} )
