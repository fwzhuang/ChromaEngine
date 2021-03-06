#version 450 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

// UNIFORMS
#include "util/uniformBufferCamera.glsl"

// VOXELS
#include "util/voxelUniforms.glsl"

in VS_OUT{
	vec3 worldPositionGeom;
	vec3 normalGeom;
	vec2 texCoordsGeom;
	vec4 posLightSpaceGeom;
} vs_in[];

out GS_OUT{
	vec3 worldPositionFrag;
	vec3 normalFrag;
	vec2 texCoordsFrag;
	vec4 posLightSpaceFrag;
} gs_out;

// Declare Vec3 array to write to
vec4[3] voxelPoints;

void main()
{
	// Select the greatest axis of the normal to project against
	vec3 faceNormal = abs(vs_in[0].normalGeom + vs_in[1].normalGeom + vs_in[2].normalGeom);
	uint maxi = faceNormal[1] > faceNormal[0] ? 1 : 0;
	maxi = faceNormal[2] > faceNormal[maxi] ? 2 : maxi;

	// project onto dominant axis
	for(uint i = 0; i < 3 ; ++i)
	{
		// World Space -> Voxel Grid Space : 
		voxelPoints[i].xyz = (vs_in[i].worldPositionGeom.xyz - u_VoxelGridCentroid) * (1.0 / u_VoxelGridSize);

		// Project onto dominant axis
		if(maxi == 0)
		{
			voxelPoints[i] = voxelPoints[i].zyxw;
		}
		else if(maxi == 1)
		{
			voxelPoints[i] = voxelPoints[i].xzyw;
		}
	}

    // Expand the triangle to fake Conservative Rasterization
    vec2 side0N = normalize(voxelPoints[1].xy - voxelPoints[0].xy);
    vec2 side1N = normalize(voxelPoints[2].xy - voxelPoints[1].xy);
    vec2 side2N = normalize(voxelPoints[0].xy - voxelPoints[2].xy);
    voxelPoints[0].xy += normalize(side2N - side0N);
    voxelPoints[1].xy += normalize(side0N - side1N);
    voxelPoints[2].xy += normalize(side1N - side2N);


	for(uint j = 0 ; j < 3; ++j)
	{
		// Voxel grid space -> Clip space
		voxelPoints[j].xy *= vec2(1.0 / u_VoxelGridResolution);
		voxelPoints[j].zw = vec2(1.0);

		// Normal, UV, WorldPos
		gs_out.normalFrag        = vs_in[j].normalGeom;
		gs_out.texCoordsFrag     = vs_in[j].texCoordsGeom;
		gs_out.worldPositionFrag = vs_in[j].worldPositionGeom;
		gs_out.posLightSpaceFrag = vs_in[j].posLightSpaceGeom;
		
		// emit vert
		gl_Position = voxelPoints[j];
		EmitVertex();
	}

	// Create Primitive
	EndPrimitive();
}