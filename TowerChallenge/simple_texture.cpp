#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <time.h>     // for snapshot
#include "framework.h"  // for win_get_window ()
#include "simple_texture.h"
#include "gl_basics.h" //For GL_ERROR
#include "log.h"
#define STB_IMAGE_IMPLEMENTATION   
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"
#include "fileio.h"


int TEX::GetTexID()
{
	return texid;
}
//////////////////////////////////////////////////////////////////////////////////////////////////
TEX::~TEX()
{
	if (texid)
	{
		glDeleteTextures(1, &texid);
		wrlog("Texture Delete Called!!!!!---->");
	}
}

//----------------------------------TEXTURE Handling----------------------------------
// This is just a minimum build.
TEX::TEX(std::string const &filename, std::string const &archname, int numcomponents, int filter)
{
	//In this basic function, the filter passed to it is ignored. I'll add something for this later.
	unsigned char *image_data;
	loaded = false;

	// Automatically flip
	stbi_set_flip_vertically_on_load(true);

	// If no archive name is supplied, it's assumed the file is being loaded from the file system.
	if (archname.empty())
	{
		image_data = stbi_load(filename.c_str(), &width, &height, &comp, STBI_rgb_alpha);
	}
	else
	{
		// Otherwise load with our generic zip functionality
		image_data = stbi_load_from_memory(load_generic_zip(archname.c_str(), filename.c_str()), (int)get_last_zip_file_size(), &width, &height, &comp, STBI_rgb_alpha);
	}


	if (!image_data) {
		wrlog("ERROR: could not load %s\n", filename.c_str());
		return;
	}
	wrlog("Texture x is %d, y is %d, components %d", width, height, comp);
	// NPOT check, we checked the card capabilities beforehand, flag if it's not npot

	if ((width & (width - 1)) != 0 || (height & (height - 1)) != 0) {
		wrlog("WARNING: texture %s is not power-of-2 dimensions\n", filename.c_str());
	}

	/*
	//Flip Texture , left in for reference purposes

	int width_in_bytes = x * STBI_rgb_alpha;
	unsigned char *top = NULL;
	unsigned char *bottom = NULL;
	unsigned char temp = 0;
	int half_height = y / 2;

	for (int row = 0; row < half_height; row++) {
	top = image_data + row * width_in_bytes;
	bottom = image_data + (y - row - 1) * width_in_bytes;
	for (int col = 0; col < width_in_bytes; col++) {
	temp = *top;
	*top = *bottom;
	*bottom = temp;
	top++;
	bottom++;
	}
	}
	*/
	//Create Texture
/*
	try {
		temptex = new (TEX);
	}
	catch (std::bad_alloc&) {
		// Handle the failure
	}
	*/


	glGenTextures(1, &texid);
	glBindTexture(GL_TEXTURE_2D, texid);
	/* OLD OpenGL 2.0 style binding
	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		x,
		y,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		image_data
	);
	*/
	glTexStorage2D(GL_TEXTURE_2D, floor(log2(max(width, height))) + 1, GL_RGBA8, width, height);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	// Check for errors
	// check_gl_error();

	//The below gluBuild2DMipmaps is ONLY for maximum compatibility using OpenGL 1.4 - 2.0.
	//In a real program, you should be using GLEW or some other library to allow glGenerateMipmap to work. 
	// it's much better,and you should be using an OpenGL 3+ context anyways. This is just for my example program.
	//gluBuild2DMipmaps(GL_TEXTURE_2D, 4, x, y, GL_RGBA, GL_UNSIGNED_BYTE, image_data); //Never use this in anything but an opengl 2 demo.
	//This is openGL3 now so:
	glGenerateMipmap(GL_TEXTURE_2D); //Only if using opengl 3+, next example...
	GLfloat max_aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);
	// Set the maximum!
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);
	// Now that we're done making a texture, free the image data
	stbi_image_free(image_data);
	// Set image data properties

	name = filename;

	wrlog("New Texture created:");
	wrlog("Texture ID: %d", texid);
	wrlog("Components: %d", comp);
	wrlog("Width: %d", width);
	wrlog("Height: %d", height);
	wrlog("Name: %s", name.c_str());
	loaded = true;
}


void TEX::UseTexture(GLboolean linear, GLboolean mipmapping)
{
	glEnable(GL_TEXTURE_2D);
	GLenum filter = linear ? GL_LINEAR : GL_NEAREST;
	glBindTexture(GL_TEXTURE_2D, texid);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (mipmapping)
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)filter);
}


void TEX::SetBlendMode(int mode)
{
	if (mode) {
		glEnable(GL_ALPHA_TEST);
		glDisable(GL_BLEND);
		glAlphaFunc(GL_GREATER, 0.5f);
	}
	else {
		glDisable(GL_ALPHA_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}

