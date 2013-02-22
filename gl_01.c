#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <GL/glew.h>
#include <GL/glfw3.h>

static const GLfloat vertices[] =
  {
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    0.0f, 1.0f, 0.0f
  };

static const GLfloat colors[] = 
  {
    1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f
  };

int main()
{

  if (!glfwInit())
    {
      fprintf( stderr, "Failed to init glfw!\n");
      return -1;
    }

  glfwWindowHint(GLFW_FSAA_SAMPLES, 4);
  glfwWindowHint(GLFW_DEPTH_BITS, 16);
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

  while(true)
    {
      glfwMakeContextCurrent(window);
      
      // OpenGL drawing code
      glClear( GL_COLOR_BUFFER_BIT );

      glEnableClientState(GL_VERTEX_ARRAY);
      glBindBuffer(GL_ARRAY_BUFFER, vertexBufferHandle);
      glVertexPointer(3, GL_FLOAT, 0, (void*)0);

      glEnableClientState(GL_COLOR_ARRAY);
      glBindBuffer(GL_ARRAY_BUFFER, colorBufferHandle);
      glColorPointer(3, GL_FLOAT, 0, (void*)0);

      glDrawArrays(GL_TRIANGLES, 0, 3);

      glFlush();
      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);

      glfwSwapBuffers(window);
      glfwPollEvents();
      if (glfwGetKey(window, GLFW_KEY_ESC) )
        break;
      if (glfwGetWindowParam(window, GLFW_CLOSE_REQUESTED))
        break; 
    }
  
  // cleanup
  glDeleteBuffers(1, &vertexBufferHandle);
  glDeleteBuffers(1, &colorBufferHandle);
  
  glfwTerminate();
  return 0;
}
