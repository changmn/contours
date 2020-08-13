#version 330 core

layout (points) in;
layout (line_strip, max_vertices = 4) out;

out vec2 Point;
uniform sampler2D u_grid;
uniform float u_isolevel;
uniform float u_rdx;

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

ivec4 EDGES[16] = ivec4[16]( ivec4( -1, -1, -1, -1 ),   // 0000 -->  0
                             ivec4(  2,  3, -1, -1 ),   // 0001 -->  1
                             ivec4(  1,  2, -1, -1 ),   // 0010 -->  2
                             ivec4(  1,  3, -1, -1 ),   // 0011 -->  3
                             ivec4(  0,  1, -1, -1 ),   // 0100 -->  4
                             ivec4(  3,  0,  1,  2 ),   // 0101 -->  5
                             ivec4(  0,  2, -1, -1 ),   // 0110 -->  6
                             ivec4(  3,  0, -1, -1 ),   // 0111 -->  7
                             ivec4(  3,  0, -1, -1 ),   // 1000 -->  8
                             ivec4(  0,  2, -1, -1 ),   // 1001 -->  9
                             ivec4(  0,  1,  2,  3 ),   // 1010 --> 10
                             ivec4(  0,  1, -1, -1 ),   // 1011 --> 11
                             ivec4(  1,  3, -1, -1 ),   // 1100 --> 12
                             ivec4(  1,  2, -1, -1 ),   // 1101 --> 13
                             ivec4(  2,  3, -1, -1 ),   // 1110 --> 14
                             ivec4( -1, -1, -1, -1 ) ); // 1111 --> 15

void main() {
    vec2 coord = gl_in[0].gl_Position.xy;

    // Get the grid values at each of the four square corners. Values are put into
    // a vec4 like this:
    //
    //     r -- g
    //     |    |
    //     a -- b
    //
    vec4 values = vec4( texture(u_grid, coord                      ).r,
                        texture(u_grid, coord + vec2(u_rdx,     0) ).r,
                        texture(u_grid, coord + vec2(u_rdx, u_rdx) ).r,
                        texture(u_grid, coord + vec2(    0, u_rdx) ).r);

    // Determine the cell type by first coverting each value into a binary digit
    // based on its comparison to the isolevel, then converting the digits into a
    // base-10 integer.
    bvec4 cmp = greaterThanEqual(values, vec4(u_isolevel));
    ivec4 digits = ivec4(cmp) * ivec4(8, 4, 2, 1);
    int type = digits.r + digits.g + digits.b + digits.a;

    // Interpolate based on the isolevel and the corner values that each point sits
    // between. For example, (in the diagram at the top of the page) the point e1
    // sits between corners c0 and c1. We only need to worry about its x-coordinate
    // since its y-coordinate is the same as c0 and c1.
    vec4 v1 = values.rgar;
    vec4 v2 = values.gbba;
    vec4 v = clamp((u_isolevel-v1) / (v2-v1), 0.0, 1.0);

    // Determine the starting points for each possible line.
    vec2 positions[4] = vec2[4]( coord + u_rdx*vec2( v.r, 0 ),
                                 coord + u_rdx*vec2( 1, v.g ),
                                 coord + u_rdx*vec2( v.b, 1 ),
                                 coord + u_rdx*vec2( 0, v.a ) );

    // Find the value at the square's center (mean of the corner values).
    float center = 0.25 * (values.r+values.g+values.b+values.a);

    // Determine if this is a saddle case (case 5 or 10) and whether or not it
    // needs to be flipped. If so, flip it by reversing the bits and clamping
    // between 0 and 15 (inclusive).
    bool flip = (type == 5 || type == 10) && center < u_isolevel;

    type = flip ? ~type & 0x0F : type;

    // Choose which lines need to be drawn based on the cell type.
    ivec4 indices = EDGES[type];

    // Emit line vertices, the number of which depends on the cell type.
    if (indices[0] != -1) {
        Point = 2.0*positions[indices[0]] - 1.0; EmitVertex();
        Point = 2.0*positions[indices[1]] - 1.0; EmitVertex();
        EndPrimitive();
    }
    if (indices[2] != -1) {
        Point = 2.0*positions[indices[2]] - 1.0; EmitVertex();
        Point = 2.0*positions[indices[3]] - 1.0; EmitVertex();
        EndPrimitive();
    }
}
