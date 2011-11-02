SET(Exercise10_SRC
  Ex10.cpp
  Shader.cpp
  Material.cpp
  MeshObj.cpp
  ObjLoader.cpp
  Trackball.cpp
  Timer.cpp
  Path.cpp
)
ADD_EXECUTABLE(ex10 ${Exercise10_SRC})
TARGET_LINK_LIBRARIES(ex10 ${OpenCV_LIBRARIES} ${OpenGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})