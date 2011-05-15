SET(Exercise04_SRC
  Ex04.cpp
  MeshObj.cpp
  ObjLoader.cpp
  Trackball.cpp
)
ADD_EXECUTABLE(ex04 ${Exercise04_SRC})
TARGET_LINK_LIBRARIES(ex04
    ${OpenGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GL_LIBRARY}
    ${OPENGL_LIBRARIES}
    )

