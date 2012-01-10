PROJECT(MRIWatcher)

cmake_minimum_required(VERSION 2.6)

###### FIND ITK PACKAGE #########
FIND_PACKAGE(ITK)

IF(ITK_FOUND)
  INCLUDE(${USE_ITK_FILE})
ELSE(ITK_FOUND)
  MESSAGE("ITK not found. Error !")
ENDIF(ITK_FOUND)

MESSAGE("USE_ITK_FILE= " ${USE_ITK_FILE})

#################################

###### FIND QT PACKAGE ##########

FIND_PACKAGE(Qt4 REQUIRED)
IF(QT_USE_FILE)
  INCLUDE(${QT_USE_FILE})
  MESSAGE("QT_USE_FILE=" ${QT_USE_FILE})
  SET(QT_USE_QTXML 1)
ELSE(QT_USE_FILE)
  SET( QT_LIBRARIES   ${QT_QT_LIBRARY} )
ENDIF(QT_USE_FILE)

################################

###### FIND GLUT PACKAGE #######

INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/CMake/glutSetup.cmake)  

################################

##### Include directories ######

INCLUDE (${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
INCLUDE (${CMAKE_ROOT}/Modules/FindGLU.cmake)

INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/IO/Qt
  ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/IO/Common
  ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Display/Common
  ${CMAKE_CURRENT_SOURCE_DIR}/Libraries/Display/Window2D
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  )

#################################

###### Include source files #####

IF(WIN32)
 SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows")
ENDIF(WIN32)

SET(MriWatcher_SRCS
  main.cpp
  mriwatchergui.cpp
  mriwatcherframe.cpp
  imageframegui.cpp
  qtwindow2dfloat.cpp
  imageframelayout.cpp
)

IF(WIN32)
SET(MriWatcher_SRCS 
	${MriWatcher_SRCS}
	MriWatcher.rc
)
ENDIF(WIN32)


SET(MriWatcher_MOC_SRCS
  mriwatchergui.h
  mriwatcherframe.h
  imageframegui.h
  qtwindow2dfloat.h
)

SET(MriWatcher_HEADERS
  imageframelayout.h
)

SET(MriWatcher_GUI_SRCS 
  mriwatcherform.ui
  imageframeform.ui
)

SET(MriWatcher_RES
  icon.qrc
)

QT4_WRAP_CPP(MriWatcher_MOC_HEADERS ${MriWatcher_MOC_SRCS})
QT4_WRAP_UI(MriWatcher_FORMS_HEADERS ${MriWatcher_GUI_SRCS})
QT4_ADD_RESOURCES(MriWatcher_RES_RCC ${MriWatcher_RES})


ADD_DEFINITIONS(-DQT_DLL)
ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)
ADD_DEFINITIONS(${QT_DEFINITIONS})
ADD_DEFINITIONS(-DMriWatcher_VERSION="2.1")

ADD_EXECUTABLE(MriWatcher
  ${MriWatcher_SRCS}
  ${MriWatcher_HEADERS}
  ${MriWatcher_MOC_HEADERS}
  ${MriWatcher_FORMS_HEADERS}
  ${MriWatcher_RES_RCC}
)

MESSAGE("ITKIO= " ${ITKIO})

TARGET_LINK_LIBRARIES(MriWatcher
  ${GLUT_glut_LIBRARY}
  ITKIO
  ${QT_LIBRARIES}
  ${QT_QTMAIN_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  ${OPENGL_LIBRARY}
 )



