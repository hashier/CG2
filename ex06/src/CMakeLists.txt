SET(Exercise06_SRC
  Ex06.cpp
  MeshObj.cpp
  ObjLoader.cpp
  Trackball.cpp
)
ADD_EXECUTABLE(ex06 ${Exercise06_SRC})
TARGET_LINK_LIBRARIES(ex06
    ${OpenCV_LIBRARIES}
    ${OpenGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GL_LIBRARY}
    ${OPENGL_LIBRARIES}
    )
