/*
 * Demonstrate how to draw a textured square.
 * 
 * Copyright © 2013 Inori Sakura <inorindesu@gmail.com>
 *
 * This work is free. You can redistribute it and/or modify it under the
 * terms of the Do What The Fuck You Want To Public License, Version 2,
 * as published by Sam Hocevar. See the COPYING file for more details.
 *
 * Texture (texture.png) used in this code is modified from:
 * http://openclipart.org/detail/93199/cuty-cats-by-kib
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <png.h>
#include <GL/glew.h>
#include <GL/glfw3.h>

int min(int a, int b)
{
  return a < b ? a : b;
}

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

void my_read(png_structp readStruct, png_bytep ptr, png_size_t size)
{
  FILE* fp = (FILE*)png_get_io_ptr(readStruct);
  fread(ptr, 1, size, fp);
}

bool power_of_2(int i)
{
  while(true)
    {
      if (i == 1 || i == 2)
        return true;
      if (i % 2 != 0)
        return false;
      i /= 2;
    }
  return false;
}

unsigned char* load_image_new(const char* filename, int* w, int* h)
{
  FILE* fp = fopen(filename, "rb");
  
  if (fp == NULL)
    {
      fprintf(stderr, "ERROR: cannot open texture file '%s'\n", filename);
      return NULL;
    }

  png_structp readStruct;
  png_infop info;
  char header[8];
  unsigned char* buf;

  fread(header, 1, 8, fp);
  if(png_sig_cmp(header, 0, 8) != 0)
    {
      fprintf(stderr, "ERROR: file '%s' is not a PNG file\n", filename);
      fclose(fp);
      return NULL;
    }
  
  // We did not set error pointers or error handling codes (setjmp..)
  // If there're some problems in the PNG file, the application will be
  // killed (by OS).
  readStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  
  info = png_create_info_struct(readStruct);

  png_set_read_fn(readStruct, fp, my_read);
  
  png_set_sig_bytes(readStruct, 8);

  png_read_info(readStruct, info);
  
  *w = png_get_image_width(readStruct, info);
  *h = png_get_image_height(readStruct, info);
  
  if(power_of_2(*w) == false || power_of_2(*h) == false)
    {
      fprintf(stderr, "WARNING: texture have non-power-of-2 dimensions (width or height)\n");
    }
  
  if(png_get_color_type(readStruct, info) != PNG_COLOR_TYPE_RGB_ALPHA
     || png_get_bit_depth(readStruct, info) != 8)
    {
      fprintf(stderr, "WARNING: color type or bit depth not as expected");
      png_destroy_read_struct(&readStruct, &info, NULL);
      return NULL;
    }
  
  buf = malloc(*w * *h * 4);
  unsigned char** rowPointers = malloc(sizeof(unsigned char*) * (*h));
  int i;
  // This causes the last row placed in the start of the buffer, 
  // as OpenGL's assumption.
  // i.e. OpenGL's texture driver assumes upside-down image be sent
  for(i = 0; i < (*h); i++)
    {
      rowPointers[(*h) - i - 1] = &buf[i * (*w) * 4];
    }
  
  png_read_image(readStruct, rowPointers);

  png_destroy_read_struct(&readStruct, &info, NULL);
  fclose(fp);
  free(rowPointers);
  return buf;
}

static const GLfloat vertices[] =
  {
    -1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f
  };

static const GLfloat UV[] =
  {
    0.0f, 1.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 0.0f
  };

static const GLint indices[] = 
  {
    0, 1, 2, 1, 3, 2
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
  int lastW = 0;
  int lastH = 0;
  int curW = 640;
  int curH = 480;
  
  GLFWwindow window = glfwCreateWindow(curW, curH, GLFW_WINDOWED, "Hello gl texture!", NULL);
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

  GLuint UVBufferHandle;
  glGenBuffers(1, &UVBufferHandle);
  glBindBuffer(GL_ARRAY_BUFFER, UVBufferHandle);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UV), UV, GL_STATIC_DRAW);

  GLuint indicesBufferHandle;
  glGenBuffers(1, &indicesBufferHandle);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferHandle);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // load shader
  //
  // vertex shader
  int compilationStatus;
  int vertexShaderHandle = glCreateShader(GL_VERTEX_SHADER);
  char* vertexShaderCode = read_all_bytes("texture.vertex");
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

  // fragment shader
  int fragShaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
  char* fragShaderCode = read_all_bytes("texture.frag");
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
  
  // We should link shaders after compilation
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
  // Tell OpenGL to use linked shader program
  glUseProgram(programHandle);
  GLint vertexPositionIndex = glGetAttribLocation(programHandle, "vertexPosition");
  GLint vertexUVIndex = glGetAttribLocation(programHandle, "vertexUV");
  GLint textureIndex = glGetUniformLocation(programHandle, "myTexture");
  GLint backcolorIndex = glGetUniformLocation(programHandle, "backColor");
  fprintf(stderr, "vertexPositionIndex: %d\nvertexUVIndex: %d\n", vertexPositionIndex, vertexUVIndex);

  // load texture
  int textureW;
  int textureH;
  unsigned char* textureData = load_image_new("texture.png", &textureW, &textureH);
  GLuint textureHandle;
  glGenTextures(1, &textureHandle);
  glBindTexture(GL_TEXTURE_2D, textureHandle);
  /*
   * See http://www.opengl.org/sdk/docs/man/xhtml/glTexImage2D.xml
   * for more information
   */
  glTexImage2D(GL_TEXTURE_2D,
               0,
               GL_RGBA,             /* internal format */
               textureW, textureH,  /* width & height */
               0,                   /* border, must be 0*/
               GL_RGBA,             /* format of the input data*/
               GL_UNSIGNED_BYTE,    /* data type*/
               textureData);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glGenerateMipmap(GL_TEXTURE_2D);
  free(textureData);

  // background color of THE SQUARE!
  glUniform3f(backcolorIndex, 195 / 255.0f, 180 / 255.0f, 218 / 255.0f);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Event processor
  while(true)
    {
      glfwGetWindowSize(window, &curW, &curH);
      if (curW != lastW || curH != lastH)
        {
          int minDimension = min(curW, curH);
          lastW = curW;
          lastH = curH;
          // Such configuration will make the rectangle have 1:1 w:h
          // ratio and be in the center of the screen
          //
          // left and bottom parameter of glViewport specifies the lower left corner of the viewport:
          // After the coordinates are normalized, they will be in (-1~1, -1~1) range. Left and bottom 
          // parameters of glViewport specifies where transformed (-1, -1) should be put on the window.
          //
          // See http://www.opengl.org/sdk/docs/man/xhtml/glViewport.xml
          glViewport((curW - minDimension) / 2, (curH - minDimension) / 2, minDimension, minDimension);
          // fprintf(stderr, "W %d, H %d; minDim %d\n", curW, curH, minDimension);
          // fprintf(stderr, "glViewport(%d, %d, %d, %d)\n", (curW - minDimension) / 2, (curH - minDimension) / 2,
          //        minDimension, minDimension);
        }

      glfwMakeContextCurrent(window);
      
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
      glEnableVertexAttribArray(vertexUVIndex);
      glBindBuffer(GL_ARRAY_BUFFER, UVBufferHandle);
      glVertexAttribPointer(vertexUVIndex, 2, GL_FLOAT,
                            GL_FALSE, 0,
                            NULL);
      
      // use index buffer
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indicesBufferHandle);

      // use designeated texture
      glBindTexture(GL_TEXTURE_2D, textureHandle);
      // glBindTexture will bind texture into texture slot 0
      // so we tell GLSL that sampler should use slot 0
      glUniform1i(textureIndex, 0);

      // Draw the square according to index buffer
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);

      glDisableVertexAttribArray(vertexPositionIndex);
      glDisableVertexAttribArray(vertexUVIndex);

      glFlush();

      glfwSwapBuffers(window);
      
      // Input event check
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
  glDeleteBuffers(1, &UVBufferHandle);

  glfwTerminate();
  return 0;
}
