#include "GlShader.h"



int CGlShader::CompileShader(int shaderType, const std::string& source)
{
	int shader;
	GLint compiled;
	// Create the shader object
	shader = glCreateShader(shaderType);
	/*if (!shader)
	{
		return -1;
	}*/
	const char* strsource = source.c_str();
	// Load the shader source
	glShaderSource(shader, 1, &strsource, NULL);
	//Compile the shader
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled) {
		glDeleteShader(shader);
		return -1;
	}
	return shader;
}

CGlShader::CGlShader(const std::string& vertexSource, const std::string& fragmentSource)
{
	GLint linked;
	m_nState = 0;
	m_nVertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
	m_nFragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
	m_nProgram = glCreateProgram();
	if (m_nProgram == -1)
	{
		m_nState = -1;
	}
	else
	{
		glAttachShader(m_nProgram, m_nVertexShader);
		glAttachShader(m_nProgram, m_nFragmentShader);

		// Link the program
		glLinkProgram(m_nProgram);

		// Check the link status
		glGetProgramiv(m_nProgram, GL_LINK_STATUS, &linked);

		if (!linked) {
			glDeleteProgram(m_nProgram);
			m_nState = -1;
		}
	}
}

int CGlShader::GetAttribLocation(const std::string& label)
{
	if (m_nState != 0)
	{
		//The program has been created with error
		return -1;
	}
	if (m_nProgram == -1)
	{
		//The program has been released
		return -1;
	}
	return glGetAttribLocation(m_nProgram, label.c_str());
}

void CGlShader::SetVertexAttribArray(const std::string& label, int dimension, float* buffer)
{
	if (m_nState != 0 || buffer == NULL)
	{
		//The program has been created with error
		return ;
	}
	if (m_nProgram == -1)
	{
		//The program has been released
		return ;
	}
	GLuint location = GetAttribLocation(label);
	glEnableVertexAttribArray(location);
	glVertexAttribPointer(location, dimension, GL_FLOAT, false, 0, buffer);
}

int CGlShader::GetUniformLocation(const std::string& label)
{
	if (m_nState != 0)
	{
		//The program has been created with error
		return -1;
	}
	if (m_nProgram == -1)
	{
		//The program has been released
		return -1;
	}
	GLint location = glGetUniformLocation(m_nProgram, label.c_str());
	if (location < 0) {
		
	}
	return location;
}

bool CGlShader::UseProgram()
{
	if (m_nState != 0)
	{
		//The program has been created with error
		return false;
	}
	if (m_nProgram == -1)
	{
		//The program has been released
		return false;
	}
	glUseProgram(m_nProgram);
	return true;
}

void CGlShader::Release()
{
	if (m_nVertexShader != -1) {
		glDeleteShader(m_nVertexShader);
		m_nVertexShader = -1;
	}
	if (m_nFragmentShader != -1) {
		glDeleteShader(m_nFragmentShader);
		m_nFragmentShader = -1;
	}
	// Delete program, automatically detaching any shaders from it.
	if (m_nProgram != -1) {
		glDeleteProgram(m_nProgram);
		m_nProgram = -1;
	}
}
