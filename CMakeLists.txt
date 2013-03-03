cmake_minimum_required(VERSION 2.6)
project(gl_hello)

set(LIBS
  GL
  glfw
  GLEW
  png15
  )

set(DATA
  passThrough.vertex
  passThrough.frag
  texture.vertex
  texture.frag
  grayTexture.frag
  texture.png
  Trollface.png
  )

add_executable(gl_01 gl_01.c)
target_link_libraries(gl_01 ${LIBS})

add_executable(gl_01_shader gl_01_shader.c)
target_link_libraries(gl_01_shader ${LIBS})

add_executable(gl_texture gl_texture.c)
target_link_libraries(gl_texture ${LIBS})

add_executable(gl_texture_grayscale gl_texture_grayscale.c)
target_link_libraries(gl_texture_grayscale ${LIBS})

set(shader_copier)
FOREACH(CPFILE ${DATA})
    add_custom_command(
      OUTPUT ${CPFILE}
      COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_CURRENT_SOURCE_DIR}/${CPFILE}" "${CMAKE_CURRENT_BINARY_DIR}/${CPFILE}"
      DEPENDS ${CPFILE})
    list(APPEND shader_copier ${CPFILE})
ENDFOREACH(CPFILE)

add_custom_target(
  copy_shaders ALL
  DEPENDS ${shader_copier})

