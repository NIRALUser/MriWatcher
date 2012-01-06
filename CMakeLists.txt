PROJECT(MRIWatcher)

cmake_minimum_required(VERSION 2.6)

###### FIND ITK PACKAGE #########
FIND_PACKAGE(ITK)

IF(ITK_FOUND)
  INCLUDE(${USE_ITK_FILE})
ELSE(ITK_FOUND)
  MESSAGE("ITK not found. Error !")
ENDIF(ITK_FOUND)

#################################

###### FIND QT PACKAGE ##########

FIND_PACKAGE( Qt REQUIRED )
IF(QT_USE_FILE)
  INCLUDE(${QT_USE_FILE})
  SET(QT_USE_QTXML 1)
ELSE(QT_USE_FILE)
  SET( QT_LIBRARIES   ${QT_QT_LIBRARY})
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
   MriWatcher.cxx
   MriWatcherGUIControls.cxx
   MriWatcherFlow.cxx 
   MriWatcherFrame.cxx
   ImageFrameGUIControls.cxx
   QtWindow2DFloat.cxx
)

IF(WIN32)
SET(MriWatcher_SRCS 
	${MriWatcher_SRCS}
	MriWatcher.rc
)
ENDIF(WIN32)


SET(MriWatcher_MOC_SRCS 
 MriWatcherFrame.h
 ImageFrameGUIControls.h
 QtWindow2DFloat.h
)

IF(QT_WRAP_CPP)
  QT_WRAP_CPP(MriWatcher MriWatcher_SRCS ${MriWatcher_MOC_SRCS})
ENDIF(QT_WRAP_CPP)


SET(MriWatcher_GUI_SRCS 
   MriWatcherGUI.ui
   ImageFrameGUI.ui
   MriWatcherHelpGUI.ui
 )

#####################################

############ Qt Wraping #############

IF(QT_WRAP_UI)
	QT_WRAP_UI(MriWatcher MriWatcher_HDRS  MriWatcher_SRCS  ${MriWatcher_GUI_SRCS} )
ENDIF(QT_WRAP_UI)

#####################################

ADD_DEFINITIONS(-DQT_DLL)
ADD_DEFINITIONS(-DQT_THREAD_SUPPORT)

ADD_EXECUTABLE(MriWatcher ${MriWatcher_SRCS})

TARGET_LINK_LIBRARIES(MriWatcher
  glut
  ITKIO
  ${QT_QT_LIBRARY}
  ${QT_QTMAIN_LIBRARY}
  ${QT_GL_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  ${OPENGL_LIBRARY}
 )







