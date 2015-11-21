#version 330 core

uniform float time;
uniform sampler2D depthTex;

in vec3 interpolatedNormal;
in vec2 st;
in vec4 depthCoord;
in vec3 viewPos;

out vec4 color;

uniform float radius = 1.5;
uniform float distanceThreshold = 1.5;

const int sample_count = 16;
const vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
                                vec2( -0.94201624,  -0.39906216 ),
                                vec2(  0.94558609,  -0.76890725 ),
                                vec2( -0.094184101, -0.92938870 ),
                                vec2(  0.34495938,   0.29387760 ),
                                vec2( -0.91588581,   0.45771432 ),
                                vec2( -0.81544232,  -0.87912464 ),
                                vec2( -0.38277543,   0.27676845 ),
                                vec2(  0.97484398,   0.75648379 ),
                                vec2(  0.44323325,  -0.97511554 ),
                                vec2(  0.53742981,  -0.47373420 ),
                                vec2( -0.26496911,  -0.41893023 ),
                                vec2(  0.79197514,   0.19090188 ),
                                vec2( -0.24188840,   0.99706507 ),
                                vec2( -0.81409955,   0.91437590 ),
                                vec2(  0.19984126,   0.78641367 ),
                                vec2(  0.14383161,  -0.14100790 )
                               );

vec3 calcPos(vec2 coord, float depth) {
	return vec3(coord, depth);
}


void main() {
     
	 float occlusion = 0.0f;

	for (int i = 0; i < sample_count; i++) {
		vec2 sampleTexCoord = st + (poisson16[i] * radius);
		float sampleDepth = texture(depthTex, depthCoord.xy).r;
		vec3 samplePos = calcPos(st, sampleDepth * 2 - 1);
		vec3 sampleDir = normalize(samplePos - viewPos);

		// calculate the angle between sampleDir and the fragments normal
		// clamp the values between 0-1 we do not want contribution form negative angles
		float dotProd = max(dot(normalize(interpolatedNormal), sampleDir), 0);

		// calculate the distance between the fragment and the samplePos
		float dist = length(samplePos - viewPos);
		
		// radius should be the distance threshold
		float distFunc = 1.0 - smoothstep(distanceThreshold, distanceThreshold * 2, dist);
		occlusion += (distFunc * dotProd);
	}

	occlusion = 1.0 - (occlusion / sample_count);

	 vec4 texcolor = vec4(1.0, 1.0, 1.0, 1.0);
     vec3 nNormal = normalize(interpolatedNormal);
     float diffuse = max(0.0, nNormal.z);
     color = texcolor * occlusion;
	 //color = texcolor * diffuse;
}

