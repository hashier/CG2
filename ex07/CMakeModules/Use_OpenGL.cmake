
# 
# This convenience include finds if OpenGL is installed
# and set the appropriate libs, incdirs, flags etc. 

# @author Jan Woetzel <jw -at- mip.informatik.uni-kiel.de> (07/2003)
##
# -----------------------------------------------------
# USAGE: 
#   just include Use_wxWindows.cmake 
#   in your projects CMakeLists.txt
# INCLUDE( ${CMAKE_MODULE_PATH}/Use_OpenGL.cmake)
##
# ------------------------------------------------------------------
# @author Jan Woetzel <http://www.mip.informatik.uni-kiel.de/~jw> (07/2003)
##
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS ``AS IS''
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR
# ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ------------------------------------------------------------------

FIND_PACKAGE( OpenGL )

IF(OPENGL_FOUND)
  
  IF(OPENGL_LIBRARIES)
    LINK_LIBRARIES(${OPENGL_LIBRARIES})
  ENDIF(OPENGL_LIBRARIES)
  
  IF(OPENGL_INCLUDE_DIR)
    INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
  ENDIF(OPENGL_INCLUDE_DIR)

  IF(OPENGL_gl_LIBRARY)
    # path to OpenGL library
    LINK_DIRECTORIES(${OPENGL_gl_LIBRARY})
  ENDIF(OPENGL_gl_LIBRARY)

ELSE(OPENGL_FOUND)
  MESSAGE(SEND_ERROR "Use_OpenGL.cmake: OpenGL not found!")
ENDIF(OPENGL_FOUND)
