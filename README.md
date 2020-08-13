# contours
A GPU-accelerated isoline generator written in C++.

This project provides a class, IsolineGenerator, that generates isolines for a texture of weight values.  The isoline vertices are created in a geometry shader and returned in a vertex buffer using transform feedback.  The isolines can then be rendered as GL_LINEs using a user-provided program.

## Basic Usage:

    GLint Nx = 500; // Grid width
    GLint Ny = 500; // Grid height

    // ** Create a texture of weights as input and a vertex buffer to hold
    //    the result. **

    // Setup the isoline generator.
    contour::IsolineGenerator generator(Nx, Ny);
    
    for (GLfloat isolevel = 0.1f; isolevel <= 0.9f; isolevel += 0.1f) {
        // ** Update the weights texture if you want. The isolines can be
        //    generated in real time for animations. **

        // Generate the isoline.
        GLsizei num_vertices = ms.update(in_texture, out_buffer, isolevel);

        // ** Render the isoline. **
    }
