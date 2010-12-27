
uniform sampler2D tex0;
uniform sampler2D tex1;

void main(void)
{
	vec4 sharp = texture2D(tex0, gl_TexCoord[0].st);
	vec4 blur  = texture2D(tex1, gl_TexCoord[0].st);
	float range = 5.00;
	float focus = 0.28;
	float blur_ammount = clamp(range * abs(focus - sharp.a), 0.0, 1.0);
	gl_FragColor = mix(sharp, blur, blur_ammount);
}