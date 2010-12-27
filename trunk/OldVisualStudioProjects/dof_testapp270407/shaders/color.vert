
varying float depth;

void main(void)
{
	gl_FrontColor = gl_Color;
	gl_Position = ftransform();
	depth = length(gl_ModelViewMatrix * -gl_Vertex);
} 
