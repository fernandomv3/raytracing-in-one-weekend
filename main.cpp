#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#endif

#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>

#ifdef __EMSCRIPTEN__
#include <GLES3/gl3.h>
#endif

#include <iostream>
#include <memory>
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"

/*hitable* random_scene(){
  int n = 500;
  hitable** list = new hitable*[n+1];
  list[0] = new sphere(vec3(0,-1000,0),1000,new lambertian(vec3(0.5,0.5,0.5)));
  int i = 1;
  for(int a = -11; a < 11; a++){
    for(int b = -11; b < 11; b++){
      float choose_mat = drand48();
      vec3 center(a + 0.9*drand48(),0.2,b+0.9*drand48());
      if((center-vec3(4,0.2,0)).length() > 0.9){
        if (choose_mat < 0.8){ //diffuse
          list[i++] = new sphere(center,0.2,new lambertian(vec3(drand48()*drand48(),drand48()*drand48(),drand48()*drand48())));
        }else if (choose_mat < 0.95){ //metal
          list[i++] = new sphere( center, 0.2,
            new metal(vec3(0.5*(1+drand48()),0.5*(1+drand48()),0.5*(1+drand48())), 0.5*drand48())
          );
        }else{ //glas
          list[i++] = new sphere(center,0.2,new dielectric(1.5));
        }
      }
    }
  }
  list[i++]= new sphere(vec3(0,1,0),1.0,new dielectric(1.5));
  list[i++]= new sphere(vec3(-4,1,0),1.0,new lambertian(vec3(0.4,0.2,0.1)));
  list[i++]= new sphere(vec3(4,1,0),1.0,new metal(vec3(0.7,0.6,0.5),0.0));
  std::cerr << i << std::endl;
  return new hitable_list(list,i);
}*/

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void compileShader(unsigned int shader,const std::string& source){
  unsigned int length = source.length();
  int shaderOk;
  const char* c_source = source.c_str();
  glShaderSource(shader,1,(const GLchar**)&c_source,(GLint*)&length);
  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS,&shaderOk);
  if(!shaderOk){
    int maxLength = 0;
    std::cerr << "Failed to compile:" << std::endl;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
    auto infoLog = std::unique_ptr<char[]>(new char[maxLength]);
    glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);
    std::string ilog = std::string(infoLog.get());
    std::cerr << ilog << std::endl;
    glDeleteShader(shader);
  }
}

void linkProgram(unsigned int program, unsigned int vertexShader, unsigned int fragmentShader){
  int programOk, programValid;
  glAttachShader(program,vertexShader);
  glAttachShader(program,fragmentShader);

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &programOk);
  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &programValid);
  if (!programOk || !programValid) {
    int maxLength = 0;
    std::cerr << "Failed to link shader program:"<< std::endl;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);
    auto infoLog = std::unique_ptr<char[]>(new char[maxLength]);
    glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
    std::string ilog = std::string(infoLog.get());
    std::cerr << ilog << std::endl;
    glDeleteProgram(program);
    program = 0;
  }
  glDetachShader(program,vertexShader);
  glDeleteShader(vertexShader);
  glDetachShader(program,fragmentShader);
  glDeleteShader(fragmentShader);
}


GLFWwindow* window = NULL;
unsigned int timeLoc = 0;

void render(){
  glClear(GL_COLOR_BUFFER_BIT);
  glUniform1f(timeLoc,static_cast<float>(glfwGetTime()));
  glDrawArrays(GL_TRIANGLES,0,3);
  glfwSwapBuffers(window);
  glfwPollEvents();
}

int main(){
  const int nx = 320;
  const int ny = 240;
  const int ns = 100;

  glfwSetErrorCallback([](int error,const char* description){
    std::cerr << "Error: "<< error << " " << description << std::endl;
  });

  //initialize GLFW3
  if(!glfwInit()) exit(EXIT_FAILURE);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  window = glfwCreateWindow(nx, ny, "L3DRE2 Example", NULL, NULL);
  if(!window){
    glfwTerminate();
    #ifdef __EMSCRIPTEN__
    emscripten_force_exit(EXIT_FAILURE);
    #else
    exit(EXIT_FAILURE);
    #endif
  }
  glfwSetKeyCallback(window, key_callback);
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  #ifndef __EMSCRIPTEN__
  if (!gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress)){
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    exit(EXIT_FAILURE);
    
  }
  #endif

  /*vec3 lower_left_corner(-2.0,-1.0,-1.0);
  vec3 horizontal(4.0,0.0,0.0);
  vec3 vertical(0.0,2.0,0.0);
  vec3 origin(0.0,0.0,0.0);
  
  hitable* world = random_scene();

  vec3 lookfrom(15,2,5);
  vec3 lookat(0,0,0);
  float dist_to_focus = (lookfrom - lookat).length();
  float aperture = 0.125;
  camera cam(lookfrom,lookat,vec3(0,1,0),20,float(nx)/float(ny),aperture,dist_to_focus);

  */
  //create VAO, not sure if necessary in OpenGL ES 3.0
  unsigned int vao;
  glGenVertexArrays(1,&vao);
  glBindVertexArray(vao);

  //create buffer with only one triangle that covers all screen
  float points[] = {
    -3.0,-1.0, 3.0,-1.0, 0.0,3.0
  };

  unsigned int buffer;
  glGenBuffers(1,&buffer);
  glBindBuffer(GL_ARRAY_BUFFER,buffer);
  glBufferData(GL_ARRAY_BUFFER,sizeof(points),points,GL_STATIC_DRAW);

  //create shaders & program
  unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
  unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);

  compileShader(vs,R"(#version 300 es
  in lowp vec2 position;
  void main(){
   gl_Position = vec4(position,0.0,1.0);
  })");

  compileShader(fs,R"(#version 300 es
  precision highp float;
  out lowp vec4 color;
  uniform float time;
  uniform lowp vec2 res; 
  float random(vec2 n){
    return fract(sin(dot(n, vec2(20.332,14.23)))*63243.34);
  }
  void main(){
    vec2 uv = gl_FragCoord.xy / res;
    uv = uv * 2.0 -1.0;
    uv.x *= res.x / res.y;
    color = vec4(random(uv*time));
  })");

  unsigned int prog = glCreateProgram();
  linkProgram(prog,vs,fs);
  glUseProgram(prog);

  timeLoc = glGetUniformLocation(prog,"time");
  unsigned int resLoc = glGetUniformLocation(prog,"res");
  glUniform2f(resLoc,nx,ny);

  unsigned int posLoc = glGetAttribLocation(prog,"position");
  glVertexAttribPointer(posLoc,2,GL_FLOAT,GL_FALSE,sizeof(float)*2,0);
  glEnableVertexAttribArray(posLoc);

  glClearColor(1.0,1.0,1.0,1.0);

  #ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(render,0,0);
  #else
  while (!glfwWindowShouldClose(window)){
    render();
  }
  glfwDestroyWindow(window);
  glfwTerminate();
  return EXIT_SUCCESS;
  #endif
}