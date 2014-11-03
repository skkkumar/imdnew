
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(PROJECTNAME kpe)
PROJECT(${PROJECTNAME})

find_package(YARP REQUIRED)
find_package(ICUB REQUIRED)
find_package(ICUBcontrib REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${YARP_MODULE_PATH} ${ICUBCONTRIB_MODULE_PATH})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${YARP_MODULE_PATH} ${ICUB_MODULE_PATH})

find_package(OpenCV)

include(ICUBcontribHelpers)
include(ICUBcontribOptions)
file(GLOB source *.cpp)
file(GLOB header *.h)

source_group("Source Files" FILES ${source})
source_group("Header Files" FILES ${header})

include_directories(${YARP_INCLUDE_DIRS}
                    ${ICUB_INCLUDE_DIRS}
)
add_executable(${PROJECTNAME} ${source} ${header})
target_link_libraries(${PROJECTNAME} icubmod ${YARP_LIBRARIES} ${OpenCV_LIBRARIES})

install(TARGETS ${PROJECTNAME} RUNTIME DESTINATION bin)