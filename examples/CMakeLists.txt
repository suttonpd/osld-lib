#include fftw3 directories
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")
find_package(FFTWS REQUIRED)
include_directories(${FFTWS_INCLUDE_DIRS})



set(LIBRARIES osld m ${FFTWS_LIBRARIES})


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(hl_example hl_example.c)
target_link_libraries(hl_example ${LIBRARIES})

add_executable(ll_example ll_example.c)
target_link_libraries(ll_example ${LIBRARIES})

add_executable(synch_test synch_test.c)
target_link_libraries(synch_test ${LIBRARIES})









