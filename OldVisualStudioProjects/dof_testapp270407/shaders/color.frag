
varying float depth;

void main(void)
{
	float far = 400.0; // TODO make uniform
	
	gl_FragColor = vec4(gl_Color.r,gl_Color.g,gl_Color.b,depth/far);
} 
