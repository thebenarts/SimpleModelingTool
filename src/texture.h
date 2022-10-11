#pragma once

#include <glad/glad.h>

// to be used as GL_TEXTURE_2D
class Texture2D
{
public:
	// holds the ID assigned by OpenGL
	unsigned int ID;
	// texture dimensions
	unsigned int Width, Height;
	// texture format
	unsigned int Internal_Format;	// format of texture object
	unsigned int Image_Format;		// format of loaded image
	// texture configuration
	unsigned int Wrap_S;			// wrapping mode on S axis
	unsigned int Wrap_T;			// wrapping mode on T axis
	unsigned int Filter_Min;		// filtering mode if texture pixels < screen pixels
	unsigned int Filter_Max;		// filtering mode if texture pixels > screen pixels


	// constructor (sets default texture mode)
	Texture2D();

	// generate texture from image data
	void Generate(unsigned int width, unsigned int height, unsigned char* data);

	// binds the texture as the current active GL_TEXTUTE_2D texture object
	void Bind() const;

	void unBind() const;
};