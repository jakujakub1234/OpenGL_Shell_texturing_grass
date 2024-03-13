#version 330 core
out vec4 FragColor;

in vec2 gUV;
in vec3 gNormal;
in vec3 gFragPos;

in float gHeight;

uniform float time;

// https://www.shadertoy.com/view/4tXyWN
float hash( uvec2 x )
{
    uvec2 q = 1103515245U * ( (x>>1U) ^ (x.yx   ) );
    uint  n = 1103515245U * ( (q.x  ) ^ (q.y>>3U) );
    return float(n) * (1.0/float(0xffffffffU));
}

void main()
{	
    vec2 p = vec2(gUV * 5000.0);
    float f = hash(uvec2(p));

    vec2 local_coord = fract(p) * 2.0 - 1.0;

    local_coord.x += sin(time);
    local_coord.y += cos(time);

    if ((length(local_coord) > 2.1 * (f - gHeight) && gHeight > 0.0) || f < gHeight) {
        discard;
    }
    
    vec3 col = vec3(0.0, max(0.01, gHeight), 0.0);

    vec3 lightPos = vec3(100.0, 1000.0, 100.0);
    vec3 lightColor = vec3(0.9, 0.9, 0.9);

    float ambientStrength = 0.3;
    vec3 ambient = ambientStrength * lightColor;
  	
    vec3 norm = normalize(gNormal); //TODO maybe not normalize
    vec3 lightDir = normalize(lightPos - gFragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;
            
    vec3 result = (ambient + diffuse) * col;

    FragColor = vec4(result, 1.0);
}