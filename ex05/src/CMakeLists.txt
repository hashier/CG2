SET(Exercise05_SRC
  Ex05.cpp
  MeshObj.cpp
  ObjLoader.cpp
  Trackball.cpp
)
ADD_EXECUTABLE(ex05 ${Exercise05_SRC})
#TARGET_LINK_LIBRARIES(ex05 ${OpenCV_LIBRARIES} ${OpenGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES})
#TARGET_LINK_LIBRARIES(ex05 ${OpenCV_LIBRARIES} ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES} ${GLEW_LIBRARIES}) # use this line, if 'find OpenGL' doesn't work

TARGET_LINK_LIBRARIES(ex05
    ${OpenCV_LIBRARIES}
    ${OpenGL_LIBRARIES}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${GL_LIBRARY}
    ${OPENGL_LIBRARIES}
    )
