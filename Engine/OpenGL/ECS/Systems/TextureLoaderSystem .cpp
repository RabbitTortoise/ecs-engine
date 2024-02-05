#include <TextureLoaderSystem.h>
#include <iostream>



// Include STB-image library
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION 
#include <stb_image.h>


TextureLoaderSystem::TextureLoaderSystem(EntityManager* entityManager) : IComponentTypeQuerySystem(entityManager)
{
	
}

TextureLoaderSystem::~TextureLoaderSystem()
{
	
}

void TextureLoaderSystem::Init()
{
	
}

void TextureLoaderSystem::Destroy()
{
	
}

void TextureLoaderSystem::PreUpdate(float deltaTime)
{
	
}

void TextureLoaderSystem::ComponentTypeQueryUpdate(float deltaTime, IEntityStorage* entityTypeStorage)
{


}


int TextureLoaderSystem::LoadTextureFromFile(std::string filePath)
{
    auto it = TextureList.find(filePath);
    if (it == TextureList.end())
    {
        stbi_set_flip_vertically_on_load(true);
        // Load the data for our texture using stb-image stbi_load-function
        int width, height, nrChannels;
        GLubyte* data = stbi_load(filePath.c_str(), &width, &height, &nrChannels, 0);
        // Create texture
        TextureHandles.emplace_back(CreateTexture(width, height, nrChannels, data));

        int index = TextureHandles[TextureHandles.size() - 1];
        TextureList.insert(std::pair(filePath, index));
        return index;
    }
    else
    {
        return it->second;
    }
}

int TextureLoaderSystem::LoadHDRTextureFromFile(std::string filePath)
{
    auto it = HDRTextureList.find(filePath);
    if (it == HDRTextureList.end())
    {
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        float* data = stbi_loadf(filePath.c_str(), &width, &height, &nrComponents, 0);
        HDRTextureHandles.emplace_back(CreateHDRTexture(width, height, nrComponents, data));

        int index = HDRTextureHandles[HDRTextureHandles.size() - 1];
        HDRTextureList.insert(std::pair(filePath, index));
        return index;
    }
    else
    {
        return it->second;
    }
    
}

int TextureLoaderSystem::CreateTexture(int width, int height, int nrChannels, GLubyte* data)
{
    GLuint TextureID;

    // Create texture
    glGenTextures(1, &TextureID);
    
    // Bind it for use
    glBindTexture(GL_TEXTURE_2D, TextureID);
    
    if (nrChannels == 4)
    {
        // set the texture data as RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        
    }
    else if (nrChannels == 1)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
        
    }
    else
    {
        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        
    }


    // set the texture wrapping options to repeat
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // set the texture filtering to nearest (disabled filtering)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    

    stbi_image_free(data);

    return TextureID;
}

int TextureLoaderSystem::CreateHDRTexture(int width, int height, int nrChannels, float* data)
{
    
    unsigned int hdrTexture = 0;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data); // note how we specify the texture's data value to be float

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
    return hdrTexture;
}
