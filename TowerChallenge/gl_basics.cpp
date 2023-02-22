/*
BMFont example implementation with Kerning, for C++ and OpenGL 2.0

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

#include <windows.h>
#include <cstdio>
#include <cstdlib>
#include <time.h>
#include "glew.h"
#include "wglew.h"
#include "log.h"
#include "framework.h"
#include "gl_basics.h"
#include <iostream> // for cerr
#include "stb_image.h"
#include "stb_image_write.h"

//For CheckOpenGLError StringStream
#include <sstream>

//For simple shader code
#include <vector>
#include <string>
#include <fstream>

//OpenGL Globals for context
HDC hDC;
HGLRC hRC;

#pragma warning (disable : 4996)


// Code below from https://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
void _check_gl_error(const char *file, int line) {
	GLenum err(glGetError());

	while (err != GL_NO_ERROR) {
		std::string error;

		switch (err) {
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		wrlog("OpenGL Error: %s in %s file at line %d ", error.c_str(), file, line);
		err = glGetError();
		}
	}


//Enable OpenGL 3.3 Context
void CreateGLContext()
{
	PIXELFORMATDESCRIPTOR pfd;
	int iFormat;
	// Get the device context (DC)
	hDC = GetDC(win_get_window());
	// Set the pixel format for the DC
	ZeroMemory(&pfd, sizeof(pfd));
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32; //24
	pfd.cDepthBits = 24; //16
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;
	iFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, iFormat, &pfd);

	// Create and enable the render context (RC)
	//hRC = wglCreateContext(hDC);
	HGLRC tempContext = wglCreateContext(hDC);
	wglMakeCurrent(hDC, tempContext);

	// Setup GLEW which loads OGL function pointers
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		wrlog("Glew Init Error %s ", glewGetErrorString(err));
		//return false;
	}
	LOG_DEBUG("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	GLint attribs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		// Uncomment this for forward compatibility mode
		//WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		// Uncomment this for Compatibility profile
		//WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,//Remove
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		// Uncomment for Core profile 
		//WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	if (wglewIsSupported("WGL_ARB_create_context") == 1)
	{
		hRC = wglCreateContextAttribsARB(hDC, 0, attribs);
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(tempContext);
		wglMakeCurrent(hDC, hRC);
	}
	else
	{	//It's not possible to make a GL 3.x context. Use the old style context (GL 2.1 and before)
		hRC = tempContext;
		wrlog("OpenGl 3+ Initilization Error, falling back to GL 2.0+ , I am assuming your program is going to crash.");
	}
}


// Disable OpenGL Context
void DeleteGLContext()
{
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(win_get_window(), hDC);
}

void SetvSync(bool enabled)
{
	wglSwapIntervalEXT(enabled);
}


void glSwap()
{
	SwapBuffers(hDC);
}


void CheckGLVersionSupport()
{
	const char *version;
	int major, minor;

	version = (char *)glGetString(GL_VERSION);
	sscanf(version, "%d.%d", &major, &minor);
	LOG_DEBUG("OpenGl Version supported %d.%d", major, minor);

	if (major < 2)
	{
		MessageBox(NULL, "This program may not work, your supported opengl version is less then 2.0", "OpenGL version warning", MB_ICONERROR | MB_OK);
	}
}



GLvoid ReSizeGLScene(GLsizei width, GLsizei height)             // Resize And Initialize The GL Window
{
	if (height == 0)                              // Prevent A Divide By Zero By
	{
		height = 1;                           // Making Height Equal One
	}
	glViewport(0, 0, width, height);                    // Reset The Current Viewport
}


void ViewOrtho(int width, int height)
{
	glViewport(0, 0, width, height);             // Set Up An Ortho View	 
	glMatrixMode(GL_PROJECTION);			  // Select Projection
	glLoadIdentity();						  // Reset The Matrix
	glOrtho(0, width, 0,height,  -1, 1);	  // Select Ortho 2D Mode (640x480)
	glMatrixMode(GL_MODELVIEW);				  // Select Modelview Matrix
	glLoadIdentity();						  // Reset The Matrix
}


void GLPoint(float x, float y)
{
	glDisable(GL_TEXTURE_2D);
	glColor3f(.5f, 1.0f, .5f);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	glVertex2f(x, y);
	glEnd();
}

void GLLine(float sx, float sy, float ex, float ey)
{
	glBegin(GL_LINES);
	glVertex2f(sx, sy);
	glVertex2f(ex, ey);
	glEnd();
}

void GLRect(int xmin, int xmax, int ymin, int ymax)
{
	glBegin(GL_QUADS);
	glTexCoord2i(0, 1); glVertex2i(xmin, ymin);
	glTexCoord2i(0, 0); glVertex2i(xmin, ymax);
	glTexCoord2i(1, 0); glVertex2i(xmax, ymax);
	glTexCoord2i(1, 1); glVertex2i(xmax, ymin);
	glEnd();
}


// Really simple shader loader copied from the OpenGL examples on the web
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path)
{
	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open()) {
		std::string Line = "";
		while (getline(VertexShaderStream, Line))
			VertexShaderCode += "\n" + Line;
		VertexShaderStream.close();
	}
	else {
		wrlog("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open()) {
		std::string Line = "";
		while (getline(FragmentShaderStream, Line))
			FragmentShaderCode += "\n" + Line;
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	wrlog("Compiling shader : %s\n", vertex_file_path);
	char const * VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		wrlog("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	wrlog("Compiling shader : %s\n", fragment_file_path);
	char const * FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		wrlog("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	wrlog("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		wrlog("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		wrlog("GL Error in shader processing  %d", err);
		//Process/log the error.
	}

	return ProgramID;
}

bool is_valid(GLuint programme) {
	glValidateProgram(programme);
	int params = -1;
	glGetProgramiv(programme, GL_VALIDATE_STATUS, &params);
	wrlog("program %i GL_VALIDATE_STATUS = %i\n", programme, params);
	if (GL_TRUE != params) {
		//_print_programme_info_log(programme);
		return false;
	}
	return true;
}


void SnapShot()
{
	unsigned char *bmpBuffer;
	time_t tim;
	struct tm tm;
	char buf[15];
	char temppath[80];

	int Width = 0;
	int Height = 0;
	int i = 0;

	RECT rect;

	GetClientRect(win_get_window(), &rect);
	Width = rect.right - rect.left;
	Height = rect.bottom - rect.top;

	memset(&temppath[0], 0, sizeof(temppath));

	//Get time/date stamp
	tim = time(0);
	tm = *localtime(&tim);
	strftime(buf, sizeof buf, "%Y%m%d%H%M%S", &tm);
	puts(buf);
	//////
	bmpBuffer = (unsigned char*)malloc(Width*Height * 4);

	if (!bmpBuffer) wrlog("Error creating buffer for snapshot"); return;

	glReadPixels((GLint)0, (GLint)0, (GLint)Width, (GLint)Height, GL_RGBA, GL_UNSIGNED_BYTE, bmpBuffer);

	//Flip Texture
	int width_in_bytes = Width * STBI_rgb_alpha;
	unsigned char *top = nullptr;
	unsigned char *bottom = nullptr;
	unsigned char temp = 0;
	int half_height = Height / 2;

	for (int row = 0; row < half_height; row++)
	{
		top = bmpBuffer + row * width_in_bytes;
		bottom = bmpBuffer + (Height - row - 1) * width_in_bytes;
		for (int col = 0; col < width_in_bytes; col++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;
			top++;
			bottom++;
		}
	}

	strcat(temppath, "SS");
	strcat(temppath, buf);
	strcat(temppath, ".png");
	strcat(temppath, "\0");
	LOG_DEBUG("Saving Snapshot: %s", temppath);
	stbi_write_png(temppath, Width, Height, 4, bmpBuffer, Width * 4);
	free(bmpBuffer);
}