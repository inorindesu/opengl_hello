cmake_minimum_required(VERSION 2.6)
project(gl_hello)

set(LIBS
  GL
  glfw
  GLEW
  )

set(DATA
  passThrough.vertex
  passThrough.frag
  )

add_executable(gl_01 gl_01.c)
target_link_libraries(gl_01 ${LIBS})

add_executable(gl_01_shader gl_01_shader.c)
target_link_libraries(gl_01_shader ${LIBS})

FOREACH(CPFILE ${DATA})
    add_custom_command(
      TARGET gl_01_shader
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${CPFILE}" "${CMAKE_CURRENT_BINARY_DIR}/${CPFILE}")
ENDFOREACH(CPFILE)
