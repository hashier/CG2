SET(Exercise09_SRC
  Ex09.cpp
  Shader.cpp
  Material.cpp
  MeshObj.cpp
  ObjLoader.cpp
  Trackball.cpp
)
ADD_EXECUTABLE(ex09 ${Exercise09_SRC})
TARGET_LINK_LIBRARIES(ex09
    ${OpenCV_LIBRARIES}
    ${OpenGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GL_LIBRARY}
    ${OPENGL_LIBRARIES}
    )
