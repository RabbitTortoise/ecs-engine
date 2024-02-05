#version 400 core
out vec4 FragColor;

// Vertex Shader Output
in VS_OUT {
    vec4 FragPos;
    vec2 TexCoords;
    vec3 T;
    vec3 B;
    vec3 N;
} fs_in;

// Camera position
uniform vec4 camPos;

// Material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

// Lights
uniform vec3 lightPositions[2];
uniform vec3 lightColors[2];

// Options
uniform float textureRepeat;
uniform int IBLEnabled;

// Constants
const float PI = 3.14159265359;

// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   
// ----------------------------------------------------------------------------

void main()
{		

    mat3 TBN = mat3(fs_in.T, fs_in.B, fs_in.N);

    vec2 uv = fract(fs_in.TexCoords * textureRepeat);

    vec3 albedo     = pow(texture(albedoMap, uv).rgb, vec3(2.2));
    float metallic  = texture(metallicMap, uv).r;
    float roughness = texture(roughnessMap, uv).r;
    float ao        = texture(aoMap, uv).r;

    // obtain normal from normal map in range [0,1]
    vec3 normal = texture(normalMap, uv).rgb;
    //normal.g = 1.0 - normal.g;

    // transform normal vector to range [-1,1]
    normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
    vec3 N = normalize(TBN * normal);
    vec3 V = normalize(camPos - fs_in.FragPos).xyz;
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);

    //Directional and point light calculations
    for(int i = 0; i < 2; ++i) 
    {
        vec3 L = vec3(0.0);
		vec3 H = vec3(0.0);
		vec3 radiance = vec3(0.0);
        
        if(i == 0)
        {
                L = normalize(-lightPositions[i].xyz);
				H = normalize(V + L);
				radiance = lightColors[i].rgb;
        }
        else
        {
            // calculate per-light radiance
            L = normalize(lightPositions[i] - fs_in.FragPos.xyz);
            H = normalize(V + L);
            float distance = length(lightPositions[i] - fs_in.FragPos.xyz);
            float attenuation = 1.0 / (distance * distance);
            radiance = lightColors[i] * attenuation;
        }

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }

    //Ambient lighting calculations
    vec3 ambient = vec3(0.0);
    if(IBLEnabled > 0)
    {
        // ambient lighting (we now use IBL as the ambient term)
        vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

        vec3 kS = F;
        vec3 kD = 1.0 - kS;
        kD *= 1.0 - metallic;	  
    
        vec3 irradiance = texture(irradianceMap, N).rgb;
        vec3 diffuse      = irradiance * albedo;

         // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
        const float MAX_REFLECTION_LOD = 4.0;
        vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
        vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
        vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

        ambient = (kD * diffuse + specular) * ao;

    }
    else
    {
        ambient = vec3(0.03) * albedo * ao;
    }

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color, 1.0);
}