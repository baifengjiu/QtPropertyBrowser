
INCLUDE_DIRECTORIES(
  ${${PROJECT_NAME}_SOURCE_DIR}/src
  )

# setting a common place to put all executable files
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

IF (BUILD_SHARED)
    add_definitions(-DQT_QTPROPERTYBROWSER_IMPORT)
ENDIF()


ADD_SUBDIRECTORY(canvas_typed)
ADD_SUBDIRECTORY(canvas_variant)
ADD_SUBDIRECTORY(decoration)
ADD_SUBDIRECTORY(demo)
ADD_SUBDIRECTORY(extension)
ADD_SUBDIRECTORY(object_controller)
ADD_SUBDIRECTORY(simple)
