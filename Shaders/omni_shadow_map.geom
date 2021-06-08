#version 330 

// LAYOUTS
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

// UNIFORMS
uniform mat4 lightMatrices[6];

// OUT
out vec4 FragPos;

void main() {

    for(int face = 0; face < 6; ++face) {

        gl_Layer = face;

        // go trhough the triangles inside the face 
        for(int i = 0; i < 3; i++) {

            // gl_in takes the layout triangules in 
            FragPos = gl_in[i].gl_Position;
            gl_Position = lightMatrices[face] * FragPos;
            EmitVertex();

        }

        EndPrimitive();

    }

}