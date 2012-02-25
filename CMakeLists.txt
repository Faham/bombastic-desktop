cmake_minimum_required(VERSION 2.8)

project(bombasticdesktop)

#===============================================================================

set(SRC_DIR ./src/)
set(SRC)
set(INCLUDE_DIR)
set(LIBRARIES)
set(LIB_DIR)

#===============================================================================

set(SRC ${SRC} ${SRC_DIR}main.cpp)
set(SRC ${SRC} ${SRC_DIR}desktop.cpp)
set(SRC ${SRC} ${SRC_DIR}object.cpp)
set(SRC ${SRC} ${SRC_DIR}pile.cpp)
set(INCLUDE_DIR ${INCLUDE_DIR} ./include/)

#===============================================================================

if(EXISTS "/usr/local/lib/OGRE/cmake")
	set(CMAKE_MODULE_PATH "/usr/local/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
elseif(EXISTS "/usr/lib/OGRE/cmake")
	set(CMAKE_MODULE_PATH "/usr/lib/OGRE/cmake/;${CMAKE_MODULE_PATH}")
else ()
	message(SEND_ERROR "Failed to find module path for OGRE.")
endif()

find_package( OGRE REQUIRED )
if (NOT OGRE_FOUND)
	message(SEND_ERROR "Failed to find OGRE.")
endif ()
set(INCLUDE_DIR ${INCLUDE_DIR} ${OGRE_INCLUDE_DIRS})
set(LIBRARIES ${LIBRARIES} ${OGRE_LIBRARIES})

#===============================================================================

find_package( CEGUI REQUIRED )
set(INCLUDE_DIR ${INCLUDE_DIR} ${CEGUI_INCLUDE_DIR})
set(LIBRARIES ${LIBRARIES} ${CEGUI_LIBRARIES})

#===============================================================================

find_package( OIS REQUIRED )
if (NOT OIS_FOUND)
	message(SEND_ERROR "Failed to find OIS.")
endif ()
set(INCLUDE_DIR ${INCLUDE_DIR} ${OIS_INCLUDE_DIR})
set(LIBRARIES ${LIBRARIES} ${OIS_LIBRARIES})
set(LIB_DIR ${LIB_DIR} ${OIS_LIB_DIR})

#===============================================================================

find_package( Bullet REQUIRED )
if (NOT BULLET_FOUND)
	message(SEND_ERROR "Failed to find Bullet.")
endif ()
set(INCLUDE_DIR ${INCLUDE_DIR} ${BULLET_INCLUDE_DIRS})
set(LIBRARIES ${LIBRARIES} ${BULLET_LIBRARIES})

#===============================================================================

set(INCLUDE_DIR ${INCLUDE_DIR} /usr/local/include/OgreBullet/Dynamics /usr/local/include/OgreBullet/Collisions)
set(LIBRARIES ${LIBRARIES} libOgreBulletCol.so libOgreBulletDyn.so libConvexDecomposition.a)

#===============================================================================

add_executable(bombasticdesktop ${SRC})
include_directories(${INCLUDE_DIR})
link_directories(${LIB_DIR})
target_link_libraries(bombasticdesktop ${LIBRARIES})

#===============================================================================
