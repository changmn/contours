#ifndef __ISOLINES_HPP
#define __ISOLINES_HPP

// Silence OpenGL deprecation warnings on OSX.
#ifdef __APPLE__
#ifndef GL_SILENCE_DEPRECATION
#define GL_SILENCE_DEPRECATION
#endif
#ifndef GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
#endif
#endif // __APPLE__

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <glm/glm.hpp>

#if defined( __linux__ )
	#include <GL/gl.h>
#elif defined( __APPLE__ )
	#include <OpenGL/gl3.h>
#endif

// Isoline Configurations:
//
//  c0---e0---c1
//   |        |
//  e3        e1
//   |        |
//  c3---e2---c2
//
//    +--------+     +--------+     +--------+     +--------+
//    |        |     |        |     |        |     |        |
//    |        |     ".       |     |       ."     """"""""""
//    |        |     | ".     |     |     ." |     |        |
//    +--------+     @---".---+     +---."---@     @--------@
//      Case 0         Case 1         Case 2         Case 3
//
//    +---".---@     +---."---@     +----:---@     +---."---@
//    |     ". |     | ."     |     |    :   |     | ."     |
//    |       ".     ."      ."     |    :   |     ."       |
//    |        |     |     ." |     |    :   |     |        |
//    +--------+     @---."---+     +----:---@     @--------@
//      Case 4         Case 5         Case 6         Case 7
//
//    @---."---+     @----:---+     @---".---+     @---".---+
//    | ."     |     |    :   |     |     ". |     |     ". |
//    ."       |     |    :   |     ".      ".     |       ".
//    |        |     |    :   |     | ".     |     |        |
//    +--------+     @----:---+     +---".---@     @--------@
//      Case 8         Case 9         Case 10        Case 11
//
//    @--------@     @--------@     @--------@     @--------@
//    |        |     |        |     |        |     |        |
//    """"""""""     |       ."     ".       |     |        |
//    |        |     |     ." |     | ".     |     |        |
//    +--------+     @---."---+     +---".---@     @--------@
//      Case 12        Case 13        Case 14        Case 15
//
// Degenerate Cases:
//
//    0 == 15
//    1 == 14
//    2 == 13
//    3 == 12
//    4 == 11
//    5 == 10
//    6 ==  9
//    7 ==  8
//

namespace contour {

class IsolineGenerator {
public:
    IsolineGenerator(GLint, GLint);
   ~IsolineGenerator(GLvoid);

    GLsizei update(GLuint, GLuint, GLfloat);

private:
    GLint   const m_STATE_SIZE;
    GLfloat const m_RDX;

    GLuint        m_update;
    GLuint        m_query;
    GLuint        m_vao;
    GLuint        m_vbo;
    GLuint        m_fbo;
    GLuint        m_rdx_loc;
    GLuint        m_iso_loc;
};

} // contour

#endif // __ISOLINES_HPP
