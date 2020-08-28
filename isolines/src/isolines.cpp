

#include "../include/isolines.hpp"

namespace contours {

IsolineGenerator::IsolineGenerator(GLint Nx, GLint Ny)
    : m_STATE_SIZE((Nx-1) * (Ny-1))
    , m_RDX(1.0f / static_cast<GLfloat>(Nx))
{
    // Create some OpenGL objects.
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    glGenFramebuffers(1, &m_fbo);
    glGenQueries(1, &m_query);
    m_update = glCreateProgram();

    // Create and attach the vertex shader.
    std::ifstream vs_ifs("./glsl/isolines.vert");
    std::string vs_src, line;
    while (getline(vs_ifs, line)) {
        vs_src += line + '\n';
    }
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    GLchar const *vs_str = vs_src.c_str();
    glShaderSource(vs, 1, &vs_str, nullptr);
    glCompileShader(vs);
    glAttachShader(m_update, vs);

    // Create and attach the geometry shader.
    std::ifstream gs_ifs("./glsl/isolines.geom");
    std::string gs_src;
    while (getline(gs_ifs, line)) {
        gs_src += line + '\n';
    }
    GLuint gs = glCreateShader(GL_GEOMETRY_SHADER);
    GLchar const *gs_str = gs_src.c_str();
    glShaderSource(gs, 1, &gs_str, nullptr);
    glCompileShader(gs);
    glAttachShader(m_update, gs);

    // Set the transform feedback varyings and link the program.
    GLchar const *varyings[] = { "Point" };
    glTransformFeedbackVaryings(m_update, 1, varyings, GL_SEPARATE_ATTRIBS );
    glLinkProgram(m_update);

    // Save the uniform locations.
    m_rdx_loc = glGetUniformLocation(m_update, "u_rdx");
    m_iso_loc = glGetUniformLocation(m_update, "u_isolevel");

    // Create a point in the center of each grid cell. These points are converted
    // into isolines in the geometry shader.
    std::vector<GLfloat> data(2 * m_STATE_SIZE);
    GLfloat cell_size_x = 1.0f / static_cast<GLfloat>(Nx);
    GLfloat cell_size_y = 1.0f / static_cast<GLfloat>(Ny);
    for (GLint y = 0, i = 0; y < Ny - 1; ++y) {
        for (GLint x = 0; x < Nx - 1; ++x) {
            data[i++] = (x+0.5f) * cell_size_x;
            data[i++] = (y+0.5f) * cell_size_y;
        }
    }
    // Setup the points vertex buffer and array.
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]),
             data.data(), GL_STATIC_DRAW);
    glBindVertexArray(m_vao);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(data[0]),
             reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

IsolineGenerator::~IsolineGenerator(GLvoid) {
    glDeleteProgram(m_update);
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteQueries(1, &m_query);
}

GLsizei IsolineGenerator::update(GLuint weights, GLuint output, GLfloat isolevel) {
    GLuint n = 0;

    // Tell OpenGL not to rasterize.
    glEnable(GL_RASTERIZER_DISCARD);

    glUseProgram(m_update);
    glUniform1f(m_rdx_loc, m_RDX);
    glUniform1f(m_iso_loc, isolevel);

    // Attach the grid and output textures.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, weights);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, output);
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(0);

    // Keep track of how many primitives were written to m_output.
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, m_query);

    // Capture the lines output by the geometry shader into m_output.
    glBeginTransformFeedback(GL_LINES);
    glDrawArraysInstanced(GL_POINTS, 0, m_STATE_SIZE, 1);
    glEndTransformFeedback();
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
    glBindVertexArray(0);
    glDisable(GL_RASTERIZER_DISCARD);

    // Query the number of primitives written, and return the number of vertices
    // (twice the number of primities, since we're writing GL_LINES).
    glGetQueryObjectuiv(m_query, GL_QUERY_RESULT, &n);
    return n * 2;
}

} // contours
