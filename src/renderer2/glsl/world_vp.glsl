/* world_vp.glsl */
#if defined(USE_DEFORM_VERTEXES)
#include "lib/deformVertexes"
#endif // USE_DEFORM_VERTEXES

// vertex attributes
attribute vec4 attr_Position;
attribute vec4 attr_Color;
attribute vec3 attr_Normal;
attribute vec4 attr_TexCoord0; // the diffuse/colormap texture coordinates
#if defined(USE_LIGHT_MAPPING)
	attribute vec4 attr_TexCoord1; // the lightmap texture coordinates
#endif // USE_LIGHT_MAPPING
#if defined(USE_NORMAL_MAPPING)
	attribute vec3 attr_Tangent;
	attribute vec3 attr_Binormal;
#endif // USE_NORMAL_MAPPING

// uniform variables
uniform mat4 u_ModelMatrix;
uniform mat4 u_ModelViewProjectionMatrix;
uniform vec4 u_ColorModulate;
uniform vec4 u_Color;
#if defined(USE_DIFFUSE)
	uniform mat4 u_DiffuseTextureMatrix;
	#if defined(USE_NORMAL_MAPPING)
		uniform vec3 u_ViewOrigin;
		uniform vec3 u_LightDir;
		uniform vec3 u_LightColor;
		#if defined(USE_PARALLAX_MAPPING)
			uniform float u_DepthScale;
		#endif // USE_PARALLAX_MAPPING
	#endif // USE_NORMAL_MAPPING
#endif // USE_DIFFUSE
#if defined(USE_PORTAL_CLIPPING)
	uniform vec4 u_PortalPlane;
#endif // USE_PORTAL_CLIPPING
#if defined(USE_DEFORM_VERTEXES)
	uniform float u_Time;
#endif // USE_DEFORM_VERTEXES

// varying variables
varying vec3 var_Position;
varying vec4 var_Color;
varying vec3 var_Normal;
#if defined(USE_LIGHT_MAPPING)
	varying vec2 var_TexLight;//map
#endif // USE_LIGHT_MAPPING
#if defined(USE_DIFFUSE)
	varying vec2 var_TexDiffuse;
	#if defined(USE_NORMAL_MAPPING)
		varying mat3 var_tangentMatrix;
		varying vec3 var_LightDirW;
varying vec3 var_LightDirT;
varying vec3 var_ViewDirT;          // view direction in tangentspace
		varying vec3 var_ViewDirW;              // view direction in world space
		#if defined(USE_PARALLAX_MAPPING)
///			varying vec3 var_ViewDirT;          // view direction in tangentspace
			varying float var_distanceToCam;    //
		#endif // USE_PARALLAX_MAPPING
	#endif // USE_NORMAL_MAPPING
#endif // USE_DIFFUSE
#if defined(USE_PORTAL_CLIPPING)
	varying float var_BackSide; // in front, or behind, the portalplane
#endif // USE_PORTAL_CLIPPING


void main()
{
	vec4 position = attr_Position;

#if defined(USE_DEFORM_VERTEXES)
	position = DeformPosition2(position, attr_Normal, attr_TexCoord0.st, u_Time);
#endif // USE_DEFORM_VERTEXES

	// transform vertex position into homogenous clip-space
	gl_Position = u_ModelViewProjectionMatrix * position;

	// transform position into world space
	var_Position = (u_ModelMatrix * position).xyz;

	// assign color
	var_Color = attr_Color * u_ColorModulate + u_Color;

#if defined(USE_DIFFUSE)
	// transform diffusemap texcoords
	var_TexDiffuse = (u_DiffuseTextureMatrix * attr_TexCoord0).st;
#endif // USE_DIFFUSE

#if defined(USE_LIGHT_MAPPING)
	// get lightmap texture coordinates
	var_TexLight = attr_TexCoord1.st;
#endif // USE_LIGHT_MAPPING


	// vertex normal in world space
	var_Normal = normalize((u_ModelMatrix * vec4(attr_Normal, 0.0)).xyz);  // 0.0 = direction,  1.0 = position

#if defined(USE_NORMAL_MAPPING)
	// transform tangentspace axis
	vec3 tangent  = normalize((u_ModelMatrix * vec4(attr_Tangent, 0.0)).xyz);
	vec3 binormal = normalize((u_ModelMatrix * vec4(attr_Binormal, 0.0)).xyz);

	mat3 tangentMatrix;

	// world to tangent space
	var_tangentMatrix = mat3(-tangent, -binormal, -var_Normal);
//var_tangentMatrix = transpose(mat3(tangent, binormal, var_Normal)); // this works with parallax, but specular is woot

	// tangent to world space
//var_tangentMatrix = mat3(tangent, binormal, var_Normal);


	var_LightDirW = normalize(-u_LightDir);

	// the viewdirection
	vec3 viewVec = var_Position - u_ViewOrigin;
	var_ViewDirW = normalize(viewVec);

tangentMatrix = transpose(mat3(tangent, binormal, var_Normal));
//tangentMatrix = transpose(var_tangentMatrix);
var_LightDirT = tangentMatrix * var_LightDirW; // i only have a direction. No light position..  hmm
//var_ViewDirT = tangentMatrix * var_ViewDirW;



#if defined(USE_PARALLAX_MAPPING)
	var_distanceToCam = length(viewVec);
//@	tangentMatrix = transpose(mat3(tangent, binormal, var_Normal.xyz)); // already done before^^
	var_ViewDirT = tangentMatrix * viewVec; // do not normalize..
//	var_ViewDirT = var_tangentMatrix * viewVec; // do not normalize..
#endif // USE_PARALLAX_MAPPING
#endif // USE_NORMAL_MAPPING

#if defined(USE_PORTAL_CLIPPING)
	// in front, or behind, the portalplane
	var_BackSide = dot(var_Position.xyz, u_PortalPlane.xyz) - u_PortalPlane.w;
#endif // USE_PORTAL_CLIPPING
}