#include <GLEW/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

// GLM Mathematics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <SOIL2/SOIL2.h>

using namespace std;

int width, height;
const double PI = 3.14159;
const float toRadians = PI / 180.0f;

// Declare Input Callback Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);

//ortho
bool isOrtho = false;

// Declare View Matrix
glm::mat4 viewMatrix;

// Camera Field of View
GLfloat fov = 45.0f;

void initiateCamera();
void TransformCamera();


// Array for triangle rotations
glm::float32 triRotations[] = { 0.0f, 60.0f, 120.0f, 180.0f, 240.0f, 300.0f };

// camera
glm::vec3 gCameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 target = glm::vec3(0.f, 0.f, 0.f);
static float cameraSpeed = 2.5f;

float gLastX = 320;
float gLastY = 240;
bool gFirstMouse = true;
float xChange, yChange;

//Radius, Pitch, Yaw
GLfloat radius = 3.f, rawYaw = 0.f, rawPitch = 0.f, degYaw, degPitch;

//chaging for camera orientation change
bool isOrbiting = false;


// timing
float gDeltaTime = 0.0f; // time between current frame and last frame
float gLastFrame = 0.0f;

// Boolean array for keys and mouse buttons
bool keys[1024], mouseButtons[3];



//light source position
glm::vec3 lightPosition(1.0f, 1.0f, 1.0f);


// Draw Primitive(s)
void draw()
{
	GLenum mode = GL_TRIANGLES;
	GLsizei indices = 6;
	glDrawElements(mode, indices, GL_UNSIGNED_BYTE, nullptr);
}



// Create and Compile Shaders
static GLuint CompileShader(const string& source, GLuint shaderType)
{
	// Create Shader object
	GLuint shaderID = glCreateShader(shaderType);
	const char* src = source.c_str();

	// Attach source code to Shader object
	glShaderSource(shaderID, 1, &src, nullptr);

	// Compile Shader
	glCompileShader(shaderID);

	// Return ID of Compiled shader
	return shaderID;

}

// Create Program Object
static GLuint CreateShaderProgram(const string& vertexShader, const string& fragmentShader)
{
	// Compile vertex shader
	GLuint vertexShaderComp = CompileShader(vertexShader, GL_VERTEX_SHADER);

	// Compile fragment shader
	GLuint fragmentShaderComp = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);

	// Create program object
	GLuint shaderProgram = glCreateProgram();

	// Attach vertex and fragment shaders to program object
	glAttachShader(shaderProgram, vertexShaderComp);
	glAttachShader(shaderProgram, fragmentShaderComp);

	// Link shaders to create executable
	glLinkProgram(shaderProgram);

	// Delete compiled vertex and fragment shaders
	glDeleteShader(vertexShaderComp);
	glDeleteShader(fragmentShaderComp);

	// Return Shader Program
	return shaderProgram;

}


int main(void)
{
	width = 640; height = 480;

	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Main Window", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	// Set input callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
		cout << "Error!" << endl;


	GLfloat vertices[] = {

		// Triangle 1
		-0.5, -0.5, 0.0, // index 0
		1.0, 0.0, 0.0, // red
		0.0, 0.0, //uv tl
		0.0f, 0.0f, 1.0f, //norm

		-0.5, 0.5, 0.0, // index 1
		0.0, 1.0, 0.0, // green
		0.0, 1.0,
		0.0f, 0.0f, 1.0f, //norm

		0.5, -0.5, 0.0,  // index 2	
		0.0, 0.0, 1.0, // blue
		1.0, 0.0,
		0.0f, 0.0f, 1.0f, //norm

		// Triangle 2	
		0.5, 0.5, 0.0,  // index 3	
		1.0, 0.0, 1.0, // purple
		1.0, 1.0,
		0.0f, 0.0f, 1.0f //norm
	};

	GLfloat triangleVertices[] = {
		// positon attributes (x,y,z)
		0.0f, .0f, 0.0f,  // vert 1
		1.0f, 0.0f, 0.0f, // red
		0.0, 1.0,

		0.5f, 0.866f, 0.0f, // vert 2
		0.0f, 1.0f, 0.0f, // green
		1.0, 2.0,

		1.0f, 0.0f, 0.0f, // vert 3
		0.0f, 0.0f, 1.0f, // blue,
		0.0, 1.0,

		0.5f, 0.866f, 0.0f, // vert 4
		0.0f, 1.0f, 0.0f, // green
		1.0, 1.0,

		0.5f, 0.866f, -2.0f, // vert 5
		0.0f, 1.0f, 0.0f, // green	
		1.0, 2.0,

		1.0f, 0.0f, 0.0f, // vert 6
		0.0f, 0.0f, 1.0f, // blue
		0.0, 1.0,

		1.0f, 0.0f, 0.0f, // vert 7
		0.0f, 0.0f, 1.0f, // blue
		1.0, 1.0,

		1.0f, 0.0f, -2.0f, // vert 8
		0.0f, 0.0f, 1.0f, // blue
		1.0, 1.0,

		0.5f, 0.866f, -2.0f, // vert 9
		0.0f, 1.0f, 0.0f, // green
		1.0, 1.0,
	};

	GLfloat lampVertices[] = {
		// Triangle 1
			-0.5, -0.5, 0.0, // index 0
			-0.5, 0.5, 0.0, // index 1
			0.5, -0.5, 0.0,  // index 2	
			0.5, 0.5, 0.0,  // index 3	
		};


	// Define element indices
	GLubyte indices[] = {
		0, 1, 2,
		1, 2, 3
	};


	unsigned int indicesPyr[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat verticesPyr[] = {
		-1.0f, -1.0f, 0.0f, 0.0, 0.0, //UV
		0.0f, -1.0f, 1.0f,  1.0, 0.0,
		1.0f, -1.0f, 0.0f,  2.0, 0.5,
		0.0f, 1.0f, 0.0f,   1.0, 2.0
	};

	// Plane Transforms
	glm::vec3 planePositions[] = {
		glm::vec3(1.2f,  0.0f,  1.7f),
		glm::vec3(1.7f,  0.0f,  1.2f),
		glm::vec3(1.2f,  0.0f,  0.7f),
		glm::vec3(0.7f, 0.0f,  1.2f),
		glm::vec3(1.2f, 0.5f,  1.2f),
		glm::vec3(1.2f, -0.5f,  1.2f)
	};

	glm::vec3 object2Positions[] = {
		glm::vec3(0.0f,  0.0f,  0.5f),
		glm::vec3(0.5f,  0.0f,  0.0f),
		glm::vec3(0.0f,  0.0f,  -0.5f),
		glm::vec3(-0.5f, 0.0f,  0.0f),
		glm::vec3(0.0f, 0.5f,  0.0f),
		glm::vec3(0.0f, -0.5f,  0.0f)
	};

	glm::float32 planeRotations[] = {
		0.0f, 90.0f, 180.0f, -90.0f, -90.f, 90.f
	};

	// Setup some OpenGL options
	glEnable(GL_DEPTH_TEST);

	// Wireframe mode
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLuint cubeVBO, cubeEBO, cubeVAO, floorVBO, floorEBO, floorVAO, triangleVAO, triangleVBO, triangleEBO, pyramidVAO, pryamidIBO, pyramidVBO, lampVBO, lampEBO, lampVAO;

	glGenBuffers(1, &cubeVBO); // Create VBO
	glGenBuffers(1, &cubeEBO); // Create EBO

	glGenBuffers(1, &floorVBO); // Create VBO
	glGenBuffers(1, &floorEBO); // Create EBO

	glGenVertexArrays(1, &cubeVAO); // Create VOA
	glGenVertexArrays(1, &floorVAO); // Create VOA

	glGenBuffers(1, &lampVBO);
	glGenBuffers(1, &lampEBO);
	glGenVertexArrays(1, &lampVAO);


	glGenVertexArrays(1, &triangleVAO); // Create VAO
	glGenBuffers(1, &triangleVBO); // Create VBO
	glGenBuffers(1, &triangleEBO); // Create VBO


	glGenVertexArrays(1, &pyramidVAO);
	glGenBuffers(1, &pryamidIBO);
	glGenBuffers(1, &pyramidVBO);

	glBindVertexArray(pyramidVAO);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pryamidIBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicesPyr), indicesPyr, GL_STATIC_DRAW);


	glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesPyr), verticesPyr, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0); // Unbind VAO (Optional but recommended)

	glBindVertexArray(triangleVAO); // Activate VAO for VBO association

	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO); // Enable VBO	
	glBindBuffer(GL_ARRAY_BUFFER, triangleEBO); // Enable VBO	
	glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW); // Copy Vertex data to VBO

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0); // Associate VBO with VA (Vertex Attribute)
	glEnableVertexAttribArray(0); // Enable VA

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat))); // Associate VBO with VA
	glEnableVertexAttribArray(1); // Enable VA

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat))); // Associate VBO with VA
	glEnableVertexAttribArray(2); // Enable VA

	glBindVertexArray(0); // Unbind VAO (Optional but recommended)

	glBindVertexArray(cubeVAO);

	// VBO and EBO Placed in User-Defined VAO
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO); // Select EBO


	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Load vertex attributes
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

																					 // Specify attribute location and layout to GPU
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0); // Unbind VOA or close off (Must call VOA explicitly in loop)


	glBindVertexArray(floorVAO);

	glBindBuffer(GL_ARRAY_BUFFER, floorVBO); // Select VBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floorEBO); // Select EBO

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // Load vertex attributes
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // Load indices 

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

	//define lampVAO
	glBindVertexArray(lampVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lampVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lampEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lampVertices), lampVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	//Load Textures
	int pyramidTexWidth, pyramidTexHeight, planeTexWidth, planeTexHeight, cylinderTexWidth, cylinderTexHeight, cubeTexHeight, cubeTexWidth, candleTexWidth, candleTexHeight;
	unsigned char* brickImage = SOIL_load_image("partyhat.jpg", &pyramidTexWidth, &pyramidTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* tableImage = SOIL_load_image("tabletop.jpg", &planeTexWidth, &planeTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* cupImage = SOIL_load_image("cup.jpg", &cylinderTexWidth, &cylinderTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* cakeImage = SOIL_load_image("sprinkles.jpg", &cubeTexWidth, &cubeTexHeight, 0, SOIL_LOAD_RGB);
	unsigned char* candleImage = SOIL_load_image("candle.jpg", &candleTexWidth, &candleTexHeight, 0, SOIL_LOAD_RGB);

	//Generate Textures - pyramid
	GLuint pyryamidTexture;
	glGenTextures(1, &pyryamidTexture);
	glBindTexture(GL_TEXTURE_2D, pyryamidTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pyramidTexWidth, pyramidTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, brickImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(brickImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Generate Textures- Plane
	GLuint planeTexture;
	glGenTextures(1, &planeTexture);
	glBindTexture(GL_TEXTURE_2D, planeTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, planeTexWidth, planeTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, tableImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(tableImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Generate Textures- Cylinder
	GLuint cylinderTexture;
	glGenTextures(1, &cylinderTexture);
	glBindTexture(GL_TEXTURE_2D, cylinderTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cylinderTexWidth, cylinderTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, cupImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(cupImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	//Generate Textures- Cube
	GLuint cubeTexture;
	glGenTextures(1, &cubeTexture);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, cubeTexWidth, cubeTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, cakeImage);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(cakeImage);
	glBindTexture(GL_TEXTURE_2D, 0);


	//Candle texture
	GLuint candleTexture;
	glGenTextures(1, &candleTexture);
	glBindTexture(GL_TEXTURE_2D, candleTexture);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, candleTexWidth, candleTexHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, candleImage);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); */
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(candleImage);
	glBindTexture(GL_TEXTURE_2D, 0);

	// Vertex shader source code
	string vertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"layout(location = 1) in vec3 aColor;"
		"layout(location = 2) in vec2 texCoord;"
		"layout(location = 3) in vec3 normal;"
		"out vec3 oColor;"
		"out vec2 oTexCoord;"
		"out vec3 oNormal;"
		"out vec3 FragPos;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"oColor = aColor;"
		"oTexCoord = texCoord;"
		"oNormal = mat3(transpose(inverse(model))) * normal;"
		"FragPos = vec3(model * vec4(vPosition, 1.0f));"
		"}\n";

	// Fragment shader source code
	string fragmentShaderSource =
		"#version 330 core\n"
		"in vec3 oColor;"
		"in vec2 oTexCoord;"
		"in vec3 oNormal;"
		"in vec3 FragPos;"
		"out vec4 fragColor;"
		"out vec4 fragColor2;"
		"uniform sampler2D myTexture;"
		"uniform vec3 objectColor;"
		"uniform vec3 lightColor;"
		"uniform vec3 lightPos;"
		"uniform vec3 viewPos;"
		"void main()\n"
		"{\n"
		"//Amvient\n"
		"float ambientStrength = 0.9f;"
		"vec3 ambient = ambientStrength * lightColor;"
		"//Diffuse\n"
		"vec3 norm = normalize(oNormal);"
		"vec3 lightDir = normalize(lightPos - FragPos);"
		"float diff = max(dot(norm, lightDir), 0.0);"
		"vec3 diffuse = diff * lightColor;"
		"//Specular\n"
		"float specularStrength = 1.5f;"
		"vec3 viewDir = normalize(viewPos - FragPos);"
		"vec3 reflectDir = reflect(-lightDir, norm);"
		"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);"
		"vec3 specular = specularStrength * spec * lightColor;"
		"vec3 result = (ambient + diffuse + specular) * objectColor;"
		"vec3 result2 = ambient * objectColor;"
		"fragColor = texture(myTexture, oTexCoord) * vec4(result, 1.0f);"
		"fragColor2 = texture(myTexture, oTexCoord) * vec4(result2, 1.0f);"
		"}\n";

	// Lamp shader source code
	string lampVertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"}\n";

	// Lamp Fragment shader source code
	string lampFragmentShaderSource =
		"#version 330 core\n"
		"out vec4 fragColor;"
		"void main()\n"
		"{\n"
		"fragColor =vec4(1.0f);"
		"}\n";

	// Lamp shader source code
	string lamp2VertexShaderSource =
		"#version 330 core\n"
		"layout(location = 0) in vec3 vPosition;"
		"uniform mat4 model;"
		"uniform mat4 view;"
		"uniform mat4 projection;"
		"void main()\n"
		"{\n"
		"gl_Position = projection * view * model * vec4(vPosition.x, vPosition.y, vPosition.z, 1.0);"
		"}\n";

	// Lamp Fragment shader source code
	string lamp2FragmentShaderSource =
		"#version 330 core\n"
		"out vec4 fragColor2;"
		"void main()\n"
		"{\n"
		"fragColor2 =vec4(0.99f, 0.95f, 0.78f, 1.0f);"
		"}\n";


	// Creating Shader Program
	GLuint shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);
	GLuint lampShaderProgram = CreateShaderProgram(lampVertexShaderSource, lampFragmentShaderSource);

	//second lamp
	// Creating Shader Program
	GLuint lampShader2Program = CreateShaderProgram(lamp2VertexShaderSource, lamp2FragmentShaderSource);


	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		// per-frame timing
	   // --------------------
		float currentFrame = glfwGetTime();
		gDeltaTime = currentFrame - gLastFrame;
		gLastFrame = currentFrame;

		// Resize window and graphics simultaneously
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glClearColor(0.47f, 0.72f, 0.72f, 0.5);

		// Use Shader Program exe and select VAO before drawing 
		glUseProgram(shaderProgram); // Call Shader per-frame when updating attributes


		// Declare transformations (can be initialized outside loop)		
		glm::mat4 projectionMatrix;

		// Define LookAt Matrix
		viewMatrix = glm::lookAt(gCameraPos, target, gCameraUp);

		// Define projection matrix
		projectionMatrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);

		if (isOrtho) {
			projectionMatrix = glm::ortho(-((GLfloat)width / 100.0f), (GLfloat)width / 100.0f, -((GLfloat)height / 1000.0f), (GLfloat)height / 100.0f, -1000.0f, 1000.0f);
			//projectionMatrix = glm::ortho(-((GLfloat)width / 100.0f), (GLfloat)width / 100.0f, (GLfloat)height / 100.0f, -((GLfloat)height / 1000.0f), -1000.0f, 1000.0f);
		}
		else {
			// Define projection matrix
			projectionMatrix = glm::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
		}

		// Get matrix's uniform location and set matrix
		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

		//Get light and object color location
		GLint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		GLint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

		//Assign Object and Light Colors
		glUniform3f(objectColorLoc, 0.97, 0.97, 1.0f); // 0.97, 0.97, 1.0f
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);

		//Set light position
		glUniform3f(lightPosLoc, lightPosition.x, lightPosition.y, lightPosition.z);

		// view position
		glUniform3f(viewPosLoc, gCameraPos.x, gCameraPos.y, gCameraPos.z);


		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));




		//Cake
		glBindVertexArray(cubeVAO); // User-defined VAO must be called before draw. 


		// Transform planes to form cube
		for (GLuint i = 0; i < 6; i++)
		{
			glm::mat4 modelMatrix;
			glBindTexture(GL_TEXTURE_2D, cubeTexture);
			modelMatrix = glm::translate(modelMatrix, planePositions[i]);
			modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			if (i >= 4)
				modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			// Draw primitive(s)
			draw();
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glBindVertexArray(cubeVAO); // User-defined VAO must be called before draw. 


		//Top layer Cake
		for (GLuint i = 0; i < 6; i++)
		{
			glm::mat4 modelMatrix;
			glBindTexture(GL_TEXTURE_2D, cubeTexture);
			modelMatrix = glm::translate(modelMatrix, planePositions[i] / glm::vec3(2., 2., 2.) + glm::vec3(0.6f, 0.65f, 0.5f));
			modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(.5f, .5f, .5f));
			if (i >= 4)
				modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			// Draw primitive(s)
			draw();
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after


		//Cylinder
		glBindVertexArray(triangleVAO);
		// Use loop to build Model matrix for triangle
		for (GLuint i = 0; i < 6; i++) {
			glBindTexture(GL_TEXTURE_2D, cylinderTexture);
			// Apply Transform to model // Build model matrix for tri
			glm::mat4 modelMatrix;
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.35f, 0.35f, 0.35));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.2f, -1.4f, -1.7f)); // Position strip at 0,0,0
			modelMatrix = glm::rotate(modelMatrix, glm::radians(90.f), glm::vec3(0.2f, 0.0f, 0.0f)); // Rotate strip 60 deg on x (z is now almost upright, and y rotates toward z)
			modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate strip on z by increments in array		
			//modelMatrix = viewMatrix * modelMatrix; // view and model matrix multiplied here (instead of in shader for better performance)
			//Copy perspective and MV matrices to uniforms
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


			glEnable(GL_DEPTH_TEST); // Z-buffer operation
			glDepthFunc(GL_LEQUAL); // Used with Depth test

			glDrawArrays(GL_TRIANGLES, 0, 9); // Render primitive or execute shader per draw


		}
		glBindVertexArray(0); //Incase different VAO wii be used after



		//candle

		//Cylinder
		glBindVertexArray(triangleVAO);
		// Use loop to build Model matrix for triangle
		for (GLuint i = 0; i < 6; i++) {
			glBindTexture(GL_TEXTURE_2D, candleTexture);
			// Apply Transform to model // Build model matrix for tri
			glm::mat4 modelMatrix;
			modelMatrix = glm::scale(modelMatrix, glm::vec3(0.07f, 0.07f, 0.07));
			modelMatrix = glm::translate(modelMatrix, glm::vec3(17.4f, 13.0f, 17.0f)); // Position strip at 0,0,0
			modelMatrix = glm::rotate(modelMatrix, glm::radians(90.f), glm::vec3(0.2f, 0.0f, 0.0f)); // Rotate strip 60 deg on x (z is now almost upright, and y rotates toward z)
			modelMatrix = glm::rotate(modelMatrix, glm::radians(triRotations[i]), glm::vec3(0.0f, 0.0f, 1.0f)); // Rotate strip on z by increments in array		
			//modelMatrix = viewMatrix * modelMatrix; // view and model matrix multiplied here (instead of in shader for better performance)
			//Copy perspective and MV matrices to uniforms
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			//glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));


			glEnable(GL_DEPTH_TEST); // Z-buffer operation
			glDepthFunc(GL_LEQUAL); // Used with Depth test

			glDrawArrays(GL_TRIANGLES, 0, 9); // Render primitive or execute shader per draw


		}
		glBindVertexArray(0); //Incase different VAO wii be used after


		//Party Hat
		glBindVertexArray(pyramidVAO);
		glBindTexture(GL_TEXTURE_2D, pyryamidTexture);
		glm::mat4 model(1.0f);
		//model = glm::translate(model, glm::vec3(triOffset, 0.0f, 0.0f));

		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.7));
		model = glm::translate(model, glm::vec3(-3.0f, -0.2f, 1.7f)); // Position strip at 0,0,0
		//model = glm::rotate(model, glm::radians(1.f), glm::vec3(0.2f, 0.0f, 0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pryamidIBO);
		glEnable(GL_DEPTH_TEST);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);// Optional unbinding but recommended

		// Select and transform floor
		glBindVertexArray(floorVAO);
		glBindTexture(GL_TEXTURE_2D, planeTexture);
		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.f, -.5f, 0.f));
		modelMatrix = glm::rotate(modelMatrix, 90.f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(5.f, 5.f, 5.f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
		draw();
		glBindVertexArray(0); //Incase different VAO will be used after



		glUseProgram(0); // Incase different shader will be used after


		glUseProgram(lampShaderProgram);

		GLint lampModelLoc = glGetUniformLocation(lampShaderProgram, "model");
		GLint lampViewLoc = glGetUniformLocation(lampShaderProgram, "view");
		GLint lampProjLoc = glGetUniformLocation(lampShaderProgram, "projection");

		glUniformMatrix4fv(lampViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(lampProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(lampVAO); // User-defined VAO must be called before draw. 


		// Transform planes to form cube
		for (GLuint i = 0; i < 6; i++)
		{
			glm::mat4 modelMatrix;

			modelMatrix = glm::translate(modelMatrix, planePositions[i] / glm::vec3(8., 8., 8.) + glm::vec3(0.0, 2.0, 0.0));
			modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(.125f, .125f, .125f));
			if (i >= 4)
				modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(lampModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			// Draw primitive(s)
			draw();
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0); // Incase different shader will be used after

		glUseProgram(lampShader2Program);

		GLint lamp2ModelLoc = glGetUniformLocation(lampShader2Program, "model");
		GLint lamp2ViewLoc = glGetUniformLocation(lampShader2Program, "view");
		GLint lamp2ProjLoc = glGetUniformLocation(lampShader2Program, "projection");

		glUniformMatrix4fv(lamp2ViewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(lamp2ProjLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

		glBindVertexArray(lampVAO); // User-defined VAO must be called before draw. 


		// Transform planes to form cube
		for (GLuint i = 0; i < 6; i++)
		{
			glm::mat4 modelMatrix;

			modelMatrix = glm::translate(modelMatrix, planePositions[i] / glm::vec3(8., 8., 8.) + glm::vec3(1.0, 2.0, 1.0));
			modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			modelMatrix = glm::scale(modelMatrix, glm::vec3(.125f, .125f, .125f));
			if (i >= 4)
				modelMatrix = glm::rotate(modelMatrix, planeRotations[i] * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
			glUniformMatrix4fv(lamp2ModelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
			// Draw primitive(s)
			draw();
		}

		// Unbind Shader exe and VOA after drawing per frame
		glBindVertexArray(0); //Incase different VAO wii be used after

		glUseProgram(0); // Incase different shader will be used after


		

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();

		// Poll Camera Transformations
		TransformCamera();

	}

	//Clear GPU resources
	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteBuffers(1, &cubeVBO);
	glDeleteBuffers(1, &cubeEBO);
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);
	glDeleteBuffers(1, &floorEBO);
	glDeleteBuffers(1, &triangleVAO);
	glDeleteBuffers(1, &triangleVBO);
	glDeleteBuffers(1, &triangleEBO);
	glDeleteBuffers(1, &pyramidVAO);
	glDeleteBuffers(1, &pyramidVBO);
	glDeleteBuffers(1, &pryamidIBO);


	glfwTerminate();
	return 0;
}

// Define input functions
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// Display ASCII Key code
	//std::cout <<"ASCII: "<< key << std::endl;	

	// Close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	// Assign true to Element ASCII if key pressed
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE) // Assign false to Element ASCII if key released
		keys[key] = false;

}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	if (yoffset > 0)
		cameraSpeed += 0.1f;
	cout << "Camera Speed Increased 0.1f" << endl;;
	if (yoffset < 0)
		cameraSpeed -= 0.1f;
	cout << "Camera Speed Decreased 0.1f" << endl;
	/*
	// Clamp FOV
	if (fov >= 1.0f && fov <= 55.0f)
		fov -= yoffset * 0.01;

	// Default FOV
	if (fov < 1.0f)
		fov = 1.0f;
	if (fov > 55.0f)
		fov = 55.0f;
		*/

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

	if (gFirstMouse)
	{
		gLastX = xpos;
		gLastY = ypos;
		gFirstMouse = false;
	}
	// Calculate mouse offset (Easing effect)
	xChange = xpos - gLastX;
	yChange = gLastY - ypos; // Inverted cam

							// Get current mouse (always starts at 0)
	gLastX = xpos;
	gLastY = ypos;


	if (isOrbiting)
	{
		// Update raw yaw and pitch with mouse movement
		rawYaw += xChange;
		rawPitch += yChange;

		// Conver yaw and pitch to degrees, and clamp pitch
		degYaw = glm::radians(rawYaw);
		degPitch = glm::clamp(glm::radians(rawPitch), -glm::pi<float>() / 2.f + .1f, glm::pi<float>() / 2.f - .1f);

		// Azimuth Altitude formula
		gCameraPos.x = target.x + radius * cosf(degPitch) * sinf(degYaw);
		gCameraPos.y = target.y + radius * sinf(degPitch);
		gCameraPos.z = target.z + radius * cosf(degPitch) * cosf(degYaw);
	}
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	// Assign boolean state to element Button code
	if (action == GLFW_PRESS)
		mouseButtons[button] = true;
	else if (action == GLFW_RELEASE)
		mouseButtons[button] = false;
}



// Define TransformCamera function
void TransformCamera()
{
	float cameraOffset = cameraSpeed * gDeltaTime;

	// Focus camera
	if (keys[GLFW_KEY_F])
		initiateCamera();

	if (keys[GLFW_KEY_W])
		gCameraPos += cameraOffset * gCameraFront;
	if (keys[ GLFW_KEY_S])
		gCameraPos -= cameraOffset * gCameraFront;
	if (keys[GLFW_KEY_A])
		gCameraPos -= glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraOffset;
	if (keys[ GLFW_KEY_D])
		gCameraPos += glm::normalize(glm::cross(gCameraFront, gCameraUp)) * cameraOffset;
	if (keys[GLFW_KEY_Q])
		gCameraPos += gCameraUp;
	if (keys[GLFW_KEY_Q])
		gCameraPos += gCameraUp * cameraOffset;
	if (keys[GLFW_KEY_E])
		gCameraPos -= gCameraUp * cameraOffset;
	if (glfwGetCursorPos)
		isOrbiting = true;
	if (keys[GLFW_KEY_O])
		isOrtho = true;

	// Orbit camera
	//if (glfwGetCursorPos)
		//isOrbiting = true;
	//else
		//isOrbiting = false;

	// Focus camera
	//if (keys[GLFW_KEY_F])
		//initiateCamera();
}

 // Define 
void initiateCamera()
{	// Define Camera Attributes
	glm::vec3 gCameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 gCameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 gCameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 target = glm::vec3(0.f, 0.f, 0.f);
	isOrtho = false;
	static float cameraSpeed = 2.5f; 
} 
