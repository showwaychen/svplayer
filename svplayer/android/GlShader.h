#ifndef GLSHADER_H_
#define GLSHADER_H_
#include "GLES2/gl2.h"
#include<string>
class CGlShader
{
protected:
	int m_nVertexShader;
	int m_nFragmentShader;
	int m_nProgram;
	int m_nState;
	int CompileShader(int shaderType, const std::string& source);
public:
	CGlShader(const std::string& vertexSource, const std::string& fragmentSource);

	int GetAttribLocation(const std::string& label);
	void SetVertexAttribArray(const std::string& label, int dimension, float* buffer);
	int GetUniformLocation(const std::string& label);
	bool UseProgram();
	void Release();
};
#endif 
