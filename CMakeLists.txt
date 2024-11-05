cmake_minimum_required(VERSION 3.10)

project(
  lightrays
  VERSION 0.0.1
  LANGUAGES C)

set(CMAKE_INSTALL_PREFIX "/usr")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(SOURCES "${SRC_DIR}/main.c")
set(HEADERS 
    "${SRC_DIR}/materials.h"
    "${SRC_DIR}/objects.h"
    "${SRC_DIR}/rays.h"
    "${SRC_DIR}/scene.h"
)
set(MODELS_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/share/lightrays_models")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

find_package(SDL2 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS} ${SRC_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2)

target_link_libraries(${PROJECT_NAME} PRIVATE m)

#target_compile_definitions(${PROJECT_NAME} PRIVATE "MODELS_PATH=\"${MODELS_INSTALL_DIR}\"")
install(DIRECTORY ${PROJECT_SOURCE_DIR}/models/ DESTINATION ${MODELS_INSTALL_DIR})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
