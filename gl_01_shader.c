/*
 * Demonstrate how to draw a triangle with VBO and GLSL.
 * 
 * Copyright © 2013 Inori Sakura <inorindesu@gmail.com>
 *
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GL/glfw3.h>

char* read_all_bytes(char* path)
{
  FILE* fp = fopen(path, "r");
  if (fp == NULL)
    return NULL;
  
  fseek(fp, 0, SEEK_END);
  long fileSize = ftell(fp);
  fseek(fp, 0, SEEK_SET);
  
  char* buffer = malloc(fileSize + 1);
  int readCount = fread(buffer, 1, fileSize, fp);
  buffer[fileSize] = '\0';
  fclose(fp);
  return buffer;
}

void show_gl_shader_compilation_error(GLuint shaderHandle)
{
  int errorLogLength;
  glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &errorLogLength);
  char* buffer = malloc(errorLogLength + 1);
  glGetShaderInfoLog(shaderHandle, errorLogLength + 1, NULL, buffer);
  fprintf(stderr, buffer);
  free(buffer);
}

void show_gl_linking_error(GLuint programHandle)
{
  int errorLogLength;
  glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &errorLogLength);
  char* buffer = malloc(errorLogLength + 1);
  glGetProgramInfoLog(programHandle, errorLogLength + 1, NULL, buffer);
  fprintf(stderr, buffer);
  free(buffer);
}

static const GLfloat vertices[] =
  {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
  };

static const GLfloat colors[] = 
  {
    1.0f, 0.75f, 0.75f,
    0.75f, 0.75f, 1.0f,
    0.75f, 1.0f, 0.75f
  };

int main()
{

  if (!glfwInit())
    {
      fprintf( stderr, "Failed to init glfw!\n");
      return -1;
    }

  glfwWindowHint(GLFW_FSAA_SAMPLES, 4);
  // so sad that nouveau driver cannot provide OpenGL 3.3..
  glfwWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);
  //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  
  GLFWwindow window = glfwCreateWindow(300, 300, GLFW_WINDOWED, "Hello world!", NULL);
  if (window == NULL)
    {
      glfwTerminate();
      fprintf( stderr, "Failed to open a window!\n");
      fprintf( stderr, glfwErrorString(glfwGetError()));
      fprintf( stderr, "\n");
      return -1;
    }
  
  /* obtain the OpenGL context of the newly-created window*/
  glfwMakeContextCurrent(window);

  glewExperimental = true;
  if( glewInit() != GLEW_OK)
    {
      fprintf( stderr, "GLEW init failed!");
      return -1;
    }

  if (GLEW_VERSION_2_1)
    {
      printf("GL 2.1: supported\n");
    }

  // ???
  if (GLEW_VERSION_3_3)
    {
      printf("GL 3.3: supported\n");
    }

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

  GLuint vertexBufferHandle;
  glGenBuffers(1, &vertexBufferHandle);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint colorBufferHandle;
  glGenBuffers(1, &colorBufferHandle);
  glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);

  // load shader
  //
  // vertex shader
  int compilationStatus;
  int vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
  char* vertexShaderCode = read_all_bytes("passThrough.vertex");
  glShaderSource(vertexShaderHandle, 1, (const GLchar**)&vertexShaderCode, NULL);
  glCompileShader(vertexShaderHandle);
  free(vertexShaderCode);
  glGetShaderiv(vertexShaderHandle, GL_COMPILE_STATUS, &compilationStatus);
  if (compilationStatus != GL_TRUE)
    {
      fprintf(stderr, "ERROR: while compiling vertex shader.");
      show_gl_shader_compilation_error(vertexShaderHandle);
      return -1;
    }

  int fragShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
  char* fragShaderCode = read_all_bytes("passThrough.frag");
  glShaderSource(fragShaderHandle, 1, (const GLchar**)&fragShaderCode, NULL);
  glCompileShader(fragShaderHandle);
  free(fragShaderCode);
  glGetShaderiv(fragShaderHandle, GL_COMPILE_STATUS, &compilationStatus);
  if (compilationStatus != GL_TRUE)
    {
      fprintf(stderr, "ERROR: while compiling fragment shader\n");
      show_gl_shader_compilation_error(fragShaderHandle);
      return -1;
    }

  int programHandle = glCreateProgram();
  glAttachShader(programHandle, vertexShaderHandle);
  glAttachShader(programHandle, fragShaderHandle);
  glLinkProgram(programHandle);
  glGetProgramiv(programHandle, GL_LINK_STATUS, &compilationStatus);
  if (compilationStatus != GL_TRUE)
    {
      fprintf(stderr, "ERROR: while linking shader\n");
      show_gl_linking_error(programHandle);
      return -1;
    }
  glUseProgram(programHandle);
  GLint vertexPositionIndex = glGetAttribLocation(programHandle, "vertexPosition");
  GLint vertexColorIndex = glGetAttribLocation(programHandle, "vertexColor");
  fprintf(stderr, "vertexPositionIndex: %d\nvertexColorIndex: %d\n", vertexPositionIndex, vertexColorIndex);

  while(true)
    {
      glfwMakeContextCurrent(window);
      
      // OpenGL drawing code
      glClear( GL_COLOR_BUFFER_BIT );

      // Original opengl-tutorial.org tutorial uses 0 here
      // I don't feel it right
      glEnableVertexAttribArray(vertexPositionIndex);
      glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
      glVertexAttribPointer(vertexPositionIndex, 3, GL_FLOAT,
                            GL_FALSE, 0,
                            NULL);
                            
      // Original opengl-tutorial.org tutorial uses 1 here
      // I don't feel it right
      glEnableVertexAttribArray(vertexColorIndex);
      glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
      glVertexAttribPointer(vertexColorIndex, 3, GL_FLOAT,
                            GL_FALSE, 0,
                            NULL);

      glDrawArrays(GL_TRIANGLES, 0, 3);

      glDisableVertexAttribArray(vertexPositionIndex);
      glDisableVertexAttribArray(vertexColorIndex);

      glFlush();

      glfwSwapBuffers(window);
      glfwPollEvents();
      if (glfwGetKey(window, GLFW_KEY_ESC) )
        break;
      if (glfwGetWindowParam(window, GLFW_CLOSE_REQUESTED))
        break; 
    }
  
  // cleanup
  //
  // shader cleanup
  glUseProgram(0);
  glDetachShader(programHandle, vertexShaderHandle);
  glDeleteShader(vertexShaderHandle);
  glDetachShader(programHandle, fragShaderHandle);
  glDeleteShader(fragShaderHandle);
  glDeleteProgram(programHandle);

  // VBO cleanup
  glDeleteBuffers(1, &vertexBufferHandle);
  glDeleteBuffers(1, &colorBufferHandle);

  glfwTerminate();
  return 0;
}
