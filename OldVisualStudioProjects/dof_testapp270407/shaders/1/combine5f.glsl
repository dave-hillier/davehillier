uniform sampler2D Pass0;
uniform sampler2D Pass1;
uniform sampler2D Pass2;
uniform sampler2D Pass3;
uniform sampler2D Scene;

void main(void)
{
    vec4 t0 = texture2D(Pass0, gl_TexCoord[0].st);
    vec4 t1 = texture2D(Pass1, gl_TexCoord[0].st);
    vec4 t2 = texture2D(Pass2, gl_TexCoord[0].st);
    vec4 t3 = texture2D(Pass3, gl_TexCoord[0].st);
    vec4 t4 = texture2D(Scene, gl_TexCoord[0].st);
    gl_FragColor = t0 + t1 + t2 + t3 + t4;
}
