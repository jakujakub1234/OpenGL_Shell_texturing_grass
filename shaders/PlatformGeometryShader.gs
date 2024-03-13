#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 144) out;

in vec2 vUV[];
in vec3 vNormal[];
in vec3 vFragPos[];

out vec2 gUV;
out vec3 gNormal;
out vec3 gFragPos;

out float gHeight;

void main() {
    vec4 change = vec4(0.0);

    for (float i = 0.5; i > -0.03; i -= 0.03) {
        gHeight = min(0.98, i*2.0);
        change.y = i * 14.0;

        gl_Position = gl_in[0].gl_Position + change;
        gUV = vUV[0];
        gNormal = vNormal[0];
        gFragPos = vFragPos[0];
        EmitVertex();

        gl_Position = gl_in[1].gl_Position + change;
        gUV = vUV[1];
        gNormal = vNormal[1];
        gFragPos = vFragPos[1];
        EmitVertex();
        
        gl_Position = gl_in[2].gl_Position + change; 
        gUV = vUV[2];
        gNormal = vNormal[2];
        gFragPos = vFragPos[2];
        EmitVertex();

        EndPrimitive();
    } 
}