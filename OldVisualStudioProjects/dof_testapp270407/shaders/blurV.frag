uniform sampler2D tex0;

float texScaler = 1.0/128.0;
float texOffset = -0.5/128.0;

vec4 gaussFilter[7];

void main(void)
{
   vec4 color = vec4(0.0,0.0,0.0,0.0);
   vec2 texCoord = gl_TexCoord[0].st;
   
   gaussFilter[0] = vec4(0.0, -3.0, 0.0,  1.0/64.0);
   gaussFilter[1] = vec4(0.0, -2.0, 0.0,  6.0/64.0);
   gaussFilter[2] = vec4(0.0, -1.0, 0.0, 15.0/64.0);
   gaussFilter[3] = vec4(0.0,  0.0, 0.0, 20.0/64.0);
   gaussFilter[4] = vec4(0.0,  1.0, 0.0, 15.0/64.0);
   gaussFilter[5] = vec4(0.0,  2.0, 0.0,  6.0/64.0);
   gaussFilter[6] = vec4(0.0,  3.0, 0.0,  1.0/64.0);
   
   int i;
   for (i=0;i<7;i++)
   {
      color += texture2D(tex0,vec2(texCoord.x + gaussFilter[i].x * texScaler + texOffset,
                                  texCoord.y + gaussFilter[i].y * texScaler + texOffset)) * 
                    gaussFilter[i].w;
   } // End for
   
   color.a *= 2.0;
   
   gl_FragColor = color;
}