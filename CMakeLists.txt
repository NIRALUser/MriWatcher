project(MRIWatcher)

cmake_minimum_required(VERSION 2.8)

###### FIND ITK PACKAGE #########
find_package(ITK)

if(ITK_FOUND)
  include(${ITK_USE_FILE})
else(ITK_FOUND)
  message("ITK not found. Error !")
endif(ITK_FOUND)

message("USE_ITK_FILE= " ${ITK_USE_FILE})

#################################

###### FIND QT PACKAGE ##########

find_package(Qt4 REQUIRED)
if(QT_USE_FILE)
  include(${QT_USE_FILE})
  message("QT_USE_FILE=" ${QT_USE_FILE})
  set(QT_USE_QTXML 1)
else(QT_USE_FILE)
  set( QT_LIBRARIES   ${QT_QT_LIBRARY} )
endif(QT_USE_FILE)

################################

###### FIND GLUT PACKAGE #######

include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/glutSetup.cmake)

################################

##### Include directories ######

include(${CMAKE_ROOT}/Modules/FindOpenGL.cmake)
include(${CMAKE_ROOT}/Modules/FindGLU.cmake)

include_directories(
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

if(WIN32)
 set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows")
endif(WIN32)

set(MriWatcher_SRCS
  main.cpp
  mriwatcherapp.cpp
  mriwatchergui.cpp
  mriwatcherframe.cpp
  imageframegui.cpp
  qtwindow2dfloat.cpp
  imageframelayout.cpp
)

if(WIN32)
set(MriWatcher_SRCS
	${MriWatcher_SRCS}
	MriWatcher.rc
)
endif(WIN32)


set(MriWatcher_MOC_SRCS
  mriwatcherapp.h
  mriwatchergui.h
  mriwatcherframe.h
  imageframegui.h
  qtwindow2dfloat.h
)

set(MriWatcher_HEADERS
  imageframelayout.h
)

set(MriWatcher_GUI_SRCS
  mriwatcherform.ui
  imageframeform.ui
)

set(MriWatcher_RES
  icon.qrc
)

qt4_wrap_cpp(MriWatcher_MOC_HEADERS ${MriWatcher_MOC_SRCS})
qt4_wrap_ui(MriWatcher_FORMS_HEADERS ${MriWatcher_GUI_SRCS})
qt4_add_resources(MriWatcher_RES_RCC ${MriWatcher_RES})


add_definitions(-DQT_DLL)
add_definitions(-DQT_THREAD_SUPPORT)
add_definitions(${QT_DEFINITIONS})
add_definitions(-DMriWatcher_VERSION="2.3")

option(APPLE_BUNDLE "APPLE_BUNDLE" OFF)

if(APPLE_BUNDLE)

set(MACOSX_BUNDLE_ICON_FILE icons.icns)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/images/icons.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

set(MriWatcher_SRCS ${MriWatcher_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/images/icons.icns)

add_executable(MriWatcher
  MACOSX_BUNDLE
  ${MriWatcher_SRCS}
  ${MriWatcher_HEADERS}
  ${MriWatcher_MOC_HEADERS}
  ${MriWatcher_FORMS_HEADERS}
  ${MriWatcher_RES_RCC}
)

set_target_properties(MriWatcher PROPERTIES
               MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in)

set(plugin_dest_dir MriWatcher.app/Contents/MacOS)
set(qtconf_dest_dir MriWatcher.app/Contents/Resources)
set(APPS "\${CMAKE_INSTALL_PREFIX}/MriWatcher.app")

install(TARGETS MriWatcher
  BUNDLE DESTINATION . COMPONENT Runtime
  RUNTIME DESTINATION bin COMPONENT Runtime)

install(CODE "
  file(WRITE \"\${CMAKE_INSTALL_PREFIX}/${qtconf_dest_dir}/qt.conf\" \"\")
  " COMPONENT Runtime)

set(DIRS ${QT_LIBRARY_DIRS})

install(CODE "file(GLOB_RECURSE QTPLUGINS\"\${CMAKE_INSTALL_PREFIX}
  /${plugin_dest_dir}/plugins/*${CMAKE_SHARED_LIBRARY_SUFFIX}\")
  include(BundleUtilities)
  fixup_bundle(\"${APPS}\" \"\${QTPLUGINS}\" \"${DIRS}\")
  " COMPONENT Runtime)

set(CPACK_BINARY_DRAGNDROP ON)
include(CPack)

else(APPLE_BUNDLE)

add_executable(MriWatcher
  ${MriWatcher_SRCS}
  ${MriWatcher_HEADERS}
  ${MriWatcher_MOC_HEADERS}
  ${MriWatcher_FORMS_HEADERS}
  ${MriWatcher_RES_RCC}
)
install( TARGETS MriWatcher RUNTIME DESTINATION bin )
endif(APPLE_BUNDLE)


target_link_libraries(MriWatcher
  ${GLUT_glut_LIBRARY}
  ${ITK_LIBRARIES}
  ${QT_LIBRARIES}
  ${QT_QTMAIN_LIBRARY}
  ${QT_QTOPENGL_LIBRARY}
  ${OPENGL_glu_LIBRARY}
  ${OPENGL_LIBRARY}
)






