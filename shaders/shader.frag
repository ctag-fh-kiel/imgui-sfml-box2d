// check https://thebookofshaders.com/ for more fun

// taken from https://www.shadertoy.com/view/tdGfRd and modified

uniform float iTime;
uniform vec2 iResolution;
uniform float iAlpha;

vec2 hash(vec2 x)
{
    const vec2 k = vec2(0.3183099, 0.3678794);
    x = x*k + k.yx;
    return -1.0 + 2.0*fract(16.0 * k*fract(x.x*x.y*(x.x+x.y)));
}

float noise(in vec2 p)
{
    vec2 i = floor(p);
    vec2 f = fract(p);

    vec2 u = f*f*(3.0-2.0*f);

    return mix(mix(dot(hash(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),
    dot(hash(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),
    mix(dot(hash(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),
    dot(hash(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);
}

// -----------------------------------------------

void main(void)
{
    vec2 dir = vec2(-1, 1) * 20.0;
    vec2 coord = gl_FragCoord.xy;
    vec2 scale = vec2(5.0, 5.0);
    vec2 q = (ceil(coord / scale) + ceil(dir * (iTime + 60.0))) * scale;
    vec2 p = q / (iResolution.xy * 10.0);
    p = fract(p);

    vec2 uv = p*vec2(iResolution.x/iResolution.y, 1.0);

    float f = 0.0;

    uv *= 10.0;
    mat2 m = mat2(1.6, 1.2, -1.2, 1.6);
    f  = 0.5000*noise(uv); uv = m*uv;
    f += 0.2500*noise(uv); uv = m*uv;
    f += 0.1250*noise(uv); uv = m*uv;
    f += 0.0625*noise(uv); uv = m*uv;


    f = (f + 0.1) * 1.0 + 0.1;


    gl_FragColor = vec4(f, f, f, iAlpha);
}