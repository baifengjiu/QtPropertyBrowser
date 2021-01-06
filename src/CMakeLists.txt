# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_DEBUG_POSTFIX "D")

set(_SRCS
  private/enummapedit.cpp
  private/lineeditwithbutton.cpp
  private/titlebutton.cpp
  custombrowser.cpp
  qtbuttonpropertybrowser.cpp
  qteditorfactory.cpp
  qtgroupboxpropertybrowser.cpp
  qtpropertybrowser.cpp
  qtpropertybrowserutils.cpp
  qtpropertymanager.cpp
  qttreepropertybrowser.cpp
  qtvariantproperty.cpp
  )

file(GLOB_RECURSE _IMPL_HDRS *.h)
file(GLOB _PUBLIC_HDRS Qt*)

set(_UI_FORMS
  )

set(_RESOURCES
  qtpropertybrowser.qrc
  )
  
QT5_WRAP_UI(_UI_SRCS ${_UI_FORMS})
QT5_ADD_RESOURCES(_QRC_SRCS ${_RESOURCES})

set(TARGET_NAME ${PROJECT_NAME})

IF(BUILD_SHARED)
    set(BUILD_TYPE SHARED)
    add_definitions(-DQT_QTPROPERTYBROWSER_EXPORT)
ELSE()
    set(BUILD_TYPE STATIC)
ENDIF()

add_library(${TARGET_NAME} ${BUILD_TYPE}
  ${_SRCS}
  ${_UI_SRCS}
  ${_QRC_SRCS}
  ${_IMPL_HDRS}
  )

target_link_libraries(${TARGET_NAME} Qt5::Widgets)

######################### Installation Stuff ###########################
include(GenerateExportHeader)
generate_export_header(${TARGET_NAME})

install(TARGETS ${TARGET_NAME}
  EXPORT ${TARGET_NAME}Targets
  RUNTIME DESTINATION ${INSTALL_BIN_DIR}
  LIBRARY DESTINATION ${INSTALL_LIB_DIR}
  ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
  INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
)

install(
  FILES
    ${_PUBLIC_HDRS}
    ${_IMPL_HDRS}
  DESTINATION
    ${INSTALL_INCLUDE_DIR}
  COMPONENT
    Devel
)

