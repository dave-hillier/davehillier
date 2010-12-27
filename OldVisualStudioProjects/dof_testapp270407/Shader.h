#pragma once

#include <fstream>
#include <string>

class Shader
{
public:
	Shader(){}
	virtual ~Shader(){}

	virtual void init(const char* fragmentFileName, const char* vertexFileName)
	{
		fragment_ = glCreateShader(GL_FRAGMENT_SHADER);
		vertex_ = glCreateShader(GL_VERTEX_SHADER);

		std::ifstream file;
		file.open(vertexFileName, std::ios::in);
		if ( file.is_open() )
		{
			std::string source(std::istreambuf_iterator<char>(file.rdbuf()),
				std::istreambuf_iterator<char>());
			file.close();

			const char *vv = source.c_str();
			glShaderSource(vertex_, 1, &vv, NULL);
			checkError ("1");
			glCompileShader(vertex_);
			checkShaderErrors(vertex_);

			file.open(fragmentFileName, std::ios::in);
			if ( file.is_open() )
			{
				std::string source(std::istreambuf_iterator<char>(file.rdbuf()),
					std::istreambuf_iterator<char>());
				file.close();

				const char * ff = source.c_str();
				glShaderSource(fragment_, 1, &ff, NULL);
				glCompileShader(fragment_);

				checkShaderErrors(fragment_);

				program_ = glCreateProgram();

				glAttachShader(program_,vertex_);
				glAttachShader(program_,fragment_);

				glLinkProgram(program_);

				checkProgramErrors(program_);
				checkError ("init shader");
			}	
			else
			{
				std::cerr << "Failed to open fragment shader: " << vertexFileName << " " << fragmentFileName << std::endl;
			}
		}
		else
		{
			std::cerr << "Failed to open vertex shader: " << vertexFileName << " " << fragmentFileName << std::endl;
		}
	}

	void destroy()
	{
		glDetachShader(program_, fragment_);
		glDetachShader(program_, vertex_);
		glDeleteShader(fragment_);
		glDeleteShader(vertex_);
		glDeleteProgram(program_);
	}

	virtual void useProgram()
	{
		glUseProgram(program_);
	}

	virtual void useFixedFunction()
	{
		glUseProgram(0);
	}


	virtual void setVariable(const char* name, int value)
	{
		glUseProgram(program_);
		GLint uniform_location = glGetUniformLocation(program_, name);
		glUniform1iARB(uniform_location, value);
		glUseProgram(0);
	}

	virtual void setVariable(const char* name, float value)
	{
		glUseProgram(program_);
		GLint uniform_location = glGetUniformLocation(program_, name);
		glUniform1fARB(uniform_location, value);
		glUseProgram(0);
	}
private:

	void checkShaderErrors(GLuint obj)
	{
		int infologLength = 0;
		int charsWritten  = 0;
		char *infoLog;

		glGetShaderiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

		if (infologLength > 1)
		{
			infoLog = new char[infologLength];
			glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
			std::cout << "Shader error: " << infoLog << std::endl;
			delete [] infoLog;
		}
	}

	void checkProgramErrors(GLuint obj)
	{
		int infologLength = 0;
		int charsWritten  = 0;
		char *infoLog;

		glGetProgramiv(obj, GL_INFO_LOG_LENGTH,&infologLength);

		if (infologLength > 1)
		{
			infoLog = new char[infologLength];
			glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
			std::cout << "Shader error: " << infoLog << std::endl;
			delete [] infoLog;
		}
	}

	void checkError(const char* where)
	{
		GLenum error;

		while ((error = glGetError()) != GL_NO_ERROR) {
			debugLog << "Shader error: (" << where << ") " << gluErrorString(error) << std::endl;
		}

		assert(glGetError() == GL_NO_ERROR);
	}



	unsigned vertex_;
	unsigned fragment_;

	unsigned program_;
};
