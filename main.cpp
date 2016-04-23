//Jackson Rushing
//DATE
#include <GL/glew.h>//openGL wrangler stuff
#include <SDL.h>
#include <SDL_opengl.h>
#include <stdio.h>
#include <chrono>
//#define GLEW_STATIC
int main(int argc, char *argv[])
{
	//initializing the video module
	SDL_Init(SDL_INIT_VIDEO);
	//telling SDL that we want a forward compatible OpenGL 3.2 context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	//creating window
	SDL_Window* window = SDL_CreateWindow("My Window", 100, 100, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_FULLSCREEN*/);

	//creating OpenGL context
	SDL_GLContext context = SDL_GL_CreateContext(window);

	//Wrangling OpenGL
	glewExperimental = GL_TRUE;
	glewInit();
	//GLuint = GL unsigned int 
	//vbo = Vertex Buffer Object
	GLuint vbo;
	glGenBuffers(1, &vbo);
	//testing if init went OK
	printf("%u\n", vbo);

	//a VAO is a Vertex Array Object, which stores all the links between the attributes and your VBOs with "raw vertex data".
	//a VAO is created the same way as a VBO
	GLuint vao;
	glGenVertexArrays(1, &vao);
	//testing if init went OK
	printf("%u\n", vao);

	//to start using it, you have to bind it
	glBindVertexArray(vao);

	//creating event cue
	SDL_Event windowEvent;

	//making array of vertices
	float vertices[] = {
		0.0f,  0.5f, 1.0f, 0.0f, 0.0f, // Vertex 1 (X, Y, R, G, B): red
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // Vertex 2 (X, Y, R, G, B): blue
	   -0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // Vertex 3 (X, Y, R, G, B): green

	//	0.0f,  0.5f, // Vertex 1 (X, Y)
	//	0.5f, -0.5f, // Vertex 2 (X, Y)
	//	-0.5f, -0.5f  // Vertex 3 (X, Y)
	};

	//making VBO the active object
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	//Copy the vertex data to the buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//vertex shader source
	GLchar* vertexSource =
	{
	R"(	#version 150

		in vec2 position;
		in vec3 color;

		out vec3 Color;

		void main()
		{
			Color = color;
			gl_Position = vec4(position, 0.0, 1.0);
		}
	)"
	};
	//creating the vertex shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);

	//compiling vertex shader
	glCompileShader(vertexShader);

	//if status = GL_TRUE then vertex shader compiled correctly
	GLint vertexstatus;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &vertexstatus);

	if (vertexstatus == GL_TRUE)
	{
		printf("\nvertex shader compiled successfully!\n");
	}
	else
	{
		printf("\nvertex shader compilation failed!\n");
		char buffer[512];
		glGetShaderInfoLog(vertexShader, 512, NULL, buffer);
		printf(buffer);
	}
	//fragment shader source
	GLchar* fragmentSource =
	{
		R"(
		#version 150
		in vec3 Color;		
		out vec4 outColor;

		void main()
		{
			outColor = vec4(Color, 1.0);
		}
	)"
	};
	//creating the fragment shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

	//compiling fragment shader
	glCompileShader(fragmentShader);

	//if status = GL_TRUE then fragment shader compiled correctly
	GLint fragmentstatus;
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &fragmentstatus);

	if (fragmentstatus == GL_TRUE)
	{
		printf("\nfragment shader compiled successfully!\n");
	}
	else
	{
		printf("\nfragment shader compilation failed!\n");
		char buffer[512];
		glGetShaderInfoLog(fragmentShader, 512, NULL, buffer);
		printf(buffer);
	}

	//combining the 2 shaders into a "program"
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	//linking the program
	glLinkProgram(shaderProgram);

	//this call lets us actually use the shaders
	glUseProgram(shaderProgram);

	//making a link between vertex data and attributes
	//getting the location of the position attribute
	GLint posAttrib = glGetAttribLocation(shaderProgram, "position");

	//"specify how the data for that input is retrieved from the array"
	//First parameter references the input.
	//Second parameter specifies the number of values for the input, in this case the number of components of the 'vec'.
	//Third parameter specifies the type of each component.
	//Fourth parammeter specifies whether the input values should be normalized between -1.0 and 1.0 if they aren't floating point numbers.
	//Last two parameters specify how the attribute is laid out in the vertx array.	  First number specifies the "stride", or how many bytes are in between each position attribute in the array.
	//The number 0 indicates that there is no data inbetween. This is because the position of eaxh vertex is immediately followed by the position of the next vertex. 
	//The last parameter specifies the "offset", or how many bytes from the start of the array the attribute occurs. Since there aren't any other attributes, this is 0 as well.
	glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

	//enable the vertex attribute array
	glEnableVertexAttribArray(posAttrib);

	//getting the location of "color", from shaderProgram
	GLint colAttrib = glGetAttribLocation(shaderProgram, "color");

	//enabling colAttrib
	glEnableVertexAttribArray(colAttrib);
	
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));


	auto t_start = std::chrono::high_resolution_clock::now();

	while (true)
	{
		
		
		if (glGetError() != 0)
		{
			printf("error!");
		}
		if (SDL_PollEvent(&windowEvent))
		{
			if (windowEvent.type == SDL_QUIT)
			{
				break;
			}
			else if (windowEvent.type == SDL_KEYUP && windowEvent.key.keysym.sym == SDLK_ESCAPE)
			{
				break;
			}
		}

		


		//drawing the triangle
		glDrawArrays(GL_TRIANGLES, 0, 3);
		SDL_GL_SwapWindow(window);
	}


	SDL_GL_DeleteContext(context);
	SDL_Quit();
	return 0;
}