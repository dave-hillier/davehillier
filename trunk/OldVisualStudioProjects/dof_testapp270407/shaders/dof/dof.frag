// "Depth of Field" demo for Ogre
// Copyright (C) 2006  Christian Lindequist Larsen
//
// This code is in the public domain. You may do whatever you want with it.

#define NUM_TAPS 12						// number of taps the shader will use


uniform sampler2D scene;				// full resolution image
uniform sampler2D blur;					// downsampled and blurred image

vec2 poisson[NUM_TAPS];					// containts poisson-distributed positions on the unit circle

vec2 maxCoC = vec2(5.0, 10.0);			// maximum circle of confusion (CoC) radius
										// and diameter in pixels
//uniform vec2 pixelSizeScene;			// pixel size of full resolution image
//uniform vec2 pixelSizeBlur;			// pixel size of downsampled and blurred image
vec2 pixelSizeScene = vec2(1.0/1024.0, 1.0/1024.0);			// pixel size of full resolution image
vec2 pixelSizeBlur = vec2(1.0/256.0, 1.0/256.0);			// pixel size of downsampled and blurred image

float radiusScale = 1.4;				// scale factor for minimum CoC size on low r0es. image

void main()
{

	poisson[ 0] = vec2( 0.00,  0.00);
	poisson[ 1] = vec2( 0.07, -0.45);
	poisson[ 2] = vec2(-0.15, -0.33);
	poisson[ 3] = vec2( 0.35, -0.32);
	poisson[ 4] = vec2(-0.39, -0.26);
	poisson[ 5] = vec2( 0.10, -0.23);
	poisson[ 6] = vec2( 0.36, -0.12);
	poisson[ 7] = vec2(-0.31, -0.01);
	poisson[ 8] = vec2(-0.38,  0.22);
	poisson[ 9] = vec2( 0.36,  0.23);
	poisson[10] = vec2(-0.13,  0.29);
	poisson[11] = vec2( 0.14,  0.41);

	// Get depth of center tap and convert it into blur radius in pixels
	float centerDepth = texture2D(scene, gl_TexCoord[0].st).a;
	float discRadiusScene = abs(centerDepth * maxCoC.y - maxCoC.x);
	float discRadiusBlur = discRadiusScene * radiusScale; // radius on low res. image

	vec4 sum = vec4(0.0);

	for (int i = 0; i < NUM_TAPS; ++i)
	{
		// compute texture coordinates
		vec2 coordScene = gl_TexCoord[0].st + (pixelSizeScene * poisson[i] * discRadiusScene);
		vec2 coordBlur = gl_TexCoord[0].st + (pixelSizeBlur * poisson[i] * discRadiusBlur);
	
		// fetch taps and depth
		vec4 tapScene = texture2D(scene, coordScene);
		float tapDepth = texture2D(scene, coordScene).a;
		vec4 tapBlur = texture2D(blur, coordBlur);
		
		// mix low and high res. taps based on tap blurriness
		float blurAmount = abs(tapDepth * 2.0 - 1.0); // put blurriness into [0, 1]
		vec4 tap = mix(tapScene, tapBlur, blurAmount);
	
		// "smart" blur ignores taps that are closer than the center tap and in focus
		float factor = (tapDepth >= centerDepth) ? 1.0 : abs(tapDepth * 2.0 - 1.0);
	
		// accumulate
		sum.rgb += tap.rgb * factor;
		sum.a += factor;
	}

	gl_FragColor = (sum / sum.a);
}
