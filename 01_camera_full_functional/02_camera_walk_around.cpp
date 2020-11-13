/*
"3D Camera / glm::lookAt (position, target, up)". Just the basics about model matrix
In this example both Model (M), View(V)  and Projection (P) Matrices --> MVP.
We manipulate only the camera (Cube)
Use keys : WASDQZ to manipulate (Translate Up,Left,Down,Right,Far,Near)
Dependencies:
GLM, GL3W and GLFW3 to start GL.
Based on LearnOpenGL
*/

#include <iostream>

#include <GL/gl3w.h> // here: we need compile gl3w.c - utils dir
#include <GLFW/glfw3.h>
#include "gl_utils.h" // parser for shader source files

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

static int width = 800, height = 600;
float angle = 0.0f;
float radius = 5.0f;
float camx = 0.0; 
float camy = 0.0;
float camz = radius;

glm::mat4 view = glm::mat4(1.0);


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
     
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        angle -= 5.0f;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        angle += 5.0f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        camy += 0.1f;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        camy -= 0.1f;
    camx = sin(glm::radians(angle)) * radius;
    camz = cos(glm::radians(angle)) * radius;

}

void framebuffer_size_callback(GLFWwindow* window, int _width, int _height)
{
    glViewport(0, 0, _width, _height);
}


int main( )
{
  GLFWwindow* g_window;

  if (!glfwInit())
    return -1;
#ifdef APPLE
  glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
  glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );
  glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
  glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  g_window = glfwCreateWindow(width,height , "3D Transform", NULL, NULL);
  if (!g_window)
    {
      glfwTerminate();
      return -1;
    }

  glfwMakeContextCurrent( g_window );
  int screenWidth, screenHeight;
  glfwGetFramebufferSize(g_window, &screenWidth, &screenHeight);
  glfwSetFramebufferSizeCallback(g_window, framebuffer_size_callback);

  if (gl3wInit()) {
      std::cout << "failed to initialize OpenGL\n" << std::endl;
      return -1;
  }

  if (!gl3wIsSupported(3, 2)) {
      std::cout << "OpenGL 3.2 not supported\n" << std::endl;
      return -1;
  }


  glViewport( 0, 0, screenWidth, screenHeight );
  // important for 3d correct rendering
  glEnable(GL_DEPTH_TEST);
  

  char vertex_shader[1024 * 256];
  char fragment_shader[1024 * 256];
  parse_file_into_str( "transf_vs.glsl", vertex_shader, 1024 * 256 );
  parse_file_into_str( "transf_fs.glsl", fragment_shader, 1024 * 256 );

  GLuint vs = glCreateShader(GL_VERTEX_SHADER);
  const GLchar *p = (const GLchar *)vertex_shader;
  glShaderSource(vs, 1, &p, NULL);
  glCompileShader(vs);

  GLint result;
  GLchar infoLog[512];
  glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
  if (!result)
    {
      glGetShaderInfoLog(vs, sizeof(infoLog), NULL, infoLog);
      std::cout << "Error! Vertex shader failed to compile. " << infoLog << std::endl;
    }

  GLint fs = glCreateShader(GL_FRAGMENT_SHADER);
  p = (const GLchar *)fragment_shader;
  glShaderSource( fs, 1, &p, NULL );
  glCompileShader(fs);

  glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
  if (!result)
    {
      glGetShaderInfoLog(fs, sizeof(infoLog), NULL, infoLog);
      std::cout << "Error! Fragment shader failed to compile. " << infoLog << std::endl;
    }

  GLint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vs);
  glAttachShader(shaderProgram, fs);
  glLinkProgram(shaderProgram);

  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
  if (!result)
    {
      glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
      std::cout << "Error! Shader program linker failure. " << infoLog  << std::endl;
    }
  glDeleteShader(vs);
  glDeleteShader(fs);


  float vertices[] = {
      // positions          // colors           
       0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 1.0f,    
       0.5f, -0.5f, 0.5f,   1.0f, 0.0f, 1.0f, 
      -0.5f, -0.5f, 0.5f,   0.0f, 0.0f, 1.0f,   
      -0.5f,  0.5f, 0.5f,   1.0f, 1.0f, 0.0f,   

      // positions          // colors           
       0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f,
       0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f,
      -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f
  };
  unsigned int index[] = {
      3, 1, 0,  // near / front
      1, 3, 2,
      5, 7, 4, // far / near
      7, 5, 6,   
      5, 0, 1, // right
      0, 5, 4,
      7, 0, 4,  // top
      0, 7, 3,
      6, 1, 2, // bottom
      1, 6, 5,
      6, 3, 7,
      3, 6, 2

  };

  GLuint VBO, VAO, ibo;
  glGenVertexArrays( 1, &VAO );
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &ibo);

  // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
  glBindVertexArray(VAO);

  glBindBuffer( GL_ARRAY_BUFFER, VBO );
  glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

  // Position attribute
  glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), ( GLvoid * ) 0 );
  glEnableVertexAttribArray( 0 );
  // Color attribute
  glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
  glEnableVertexAttribArray( 1 );

  glBindBuffer(GL_ARRAY_BUFFER, 0); // First unbind vbo - Don't unbind ibo before VAO
  glBindVertexArray(0); // Unbind VAO

  glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

  glUseProgram(shaderProgram);

  // create transformations
  glm::mat4 model(1.0f);
  unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
  model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
  glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

  // copy matrix to GPU (once)
  unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
 
  // copy matrix to GPU (once)
  glm::mat4 projection(1.0);

  // ortho planes: left, right, bottom, up, near, far
  // projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 100.0f);

  // FOV, W/H, near, far
  projection = glm::perspective(glm::radians(45.0f), 
                                (float)screenWidth / (float)screenHeight, 
                                0.1f, 
                                100.0f);                     
  unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");
  glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

  // rendering loop
  while ( !glfwWindowShouldClose( g_window ) )
    {
     

      // camera/view transformation
      view = glm::mat4(1.0f);
      view = glm::lookAt(glm::vec3(camx, camy, camz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

      glfwPollEvents( );
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glBindVertexArray( VAO );
     // glDrawArrays( GL_TRIANGLES, 0, 6 );
     // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
      glDrawElements(GL_TRIANGLES, sizeof(index), GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
      // Swap the screen buffers
      glfwSwapBuffers( g_window );
      processInput(g_window);
    }

  // Properly de-allocate all resources once they've outlived their purpose
  glDeleteVertexArrays( 1, &VAO );
  glDeleteBuffers( 1, &VBO );
  glDeleteBuffers(1, &ibo);

  // Terminate GLFW, clearing any resources allocated by GLFW.
  glfwTerminate( );

  return EXIT_SUCCESS;
}
