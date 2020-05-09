/*---------------------------------------------------------*/
/* ------------- Proyecto Final: Residencial --------------*/
/*-----------------    2020-2   ---------------------------*/
/*-------- Alumno: Su�rez Espinoza Mario Alberto ----------*/
//#define STB_IMAGE_IMPLEMENTATION
#include <glew.h>
#include <glfw3.h>
#include <stb_image.h>

#include "camera.h"
#include "Model.h"
#include "Texture.h"

// Other Libs
#include "SOIL2/SOIL2.h"

void resize(GLFWwindow* window, int width, int height);
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// settings
// Window size
int SCR_WIDTH = 3800;
int SCR_HEIGHT = 7600;

GLFWmonitor *monitors;
GLuint VBO, VAO, EBO;
GLuint skyboxVBO, skyboxVAO;

//Camera
Camera camera(glm::vec3(20.0f, 2.0f, -10.0f));
double	lastX = 0.0f,
		lastY = 0.0f;
bool firstMouse = true;

//Timing
double	deltaTime = 0.0f,
		lastFrame = 0.0f;

//Lighting
glm::vec3 lightPosition(0.0f, 4.0f, 3.0f);
glm::vec3 lightDirection(-0.2f, -1.0f, -1.0f);

void myData(void);
void myData2(void);
void display(Shader, Shader, Model, Model);
void getResolution(void);
void animate(void);
void LoadTextures(void);
unsigned int generateTextures(char*, bool);

//For Keyboard
float	movX = 0.0f,
		movY = 0.0f,
		movZ = -5.0f,
		rotY = 0.0f;

//Texture
unsigned int t_piso_m, t_piso_b, t_grass_m,
		t_c1_frontal,
		t_c1_inferior,
		t_c1_lateral,
		t_c2_lateral,
		t_c4_frontal,
		t_c4_lateral_izq,
		t_c5_lateral,
		t_c5_frontal,
		t_c6_superior,
		t_c6_lateral,
		t_c6_frontal,
		t_t1_frontal,
		t_t1_lateral,
		t_t1_trasera,
		t_t3_frontal,
		t_t3_trasera;

//Keyframes
float	posX = 0.0f, //variables de manipulaci�n del dibujo 
		posY = 0.0f, 
		posZ = 0.0f, 
		rotRodIzq = 0.0f, //Pie
		giroMonito = 0.0f,
		movBrazo = 0.0f,
		rotCabeza = 0.0f;

float	incX = 0.0f, //Variables para el c�lculo de incrementos
		incY = 0.0f,
		incZ = 0.0f,
		rotInc = 0.0f,
		giroMonitoInc = 0.0f,
		movBrazoInc = 0.0f,
		rotCabezaInc = 0.0f;

#define MAX_FRAMES 9
int i_max_steps = 190; //Cantidad m�xima de frames intermedios
int i_curr_steps = 0;
typedef struct _frame
{
	//Variables para GUARDAR Key Frames
	float posX;		//Variable para PosicionX
	float posY;		//Variable para PosicionY
	float posZ;		//Variable para PosicionZ
	float rotRodIzq; 
	float giroMonito;
	float movBrazo;
	float rotCabeza;

}FRAME;

FRAME KeyFrame[MAX_FRAMES];
int FrameIndex = 0;			//introducir datos
bool play = false;
int playIndex = 0;

void saveFrame(void)
{

	printf("frameindex %d\n", FrameIndex);

	KeyFrame[FrameIndex].posX = posX;
	KeyFrame[FrameIndex].posY = posY;
	KeyFrame[FrameIndex].posZ = posZ;

	KeyFrame[FrameIndex].rotRodIzq = rotRodIzq;
	KeyFrame[FrameIndex].giroMonito = giroMonito;
	KeyFrame[FrameIndex].movBrazo = movBrazo;
	KeyFrame[FrameIndex].rotCabeza = rotCabeza;

	FrameIndex++;
}


//Sustituye lo que tiene actualmente el personaje por
//lo guardado en el cuadro clave cero
void resetElements(void)
{
	posX = KeyFrame[0].posX;
	posY = KeyFrame[0].posY;
	posZ = KeyFrame[0].posZ;

	rotRodIzq = KeyFrame[0].rotRodIzq;
	giroMonito = KeyFrame[0].giroMonito;
	movBrazo = KeyFrame[0].movBrazo;
	rotCabeza = KeyFrame[0].rotCabeza;

}

void interpolation(void)
{

	incX = (KeyFrame[playIndex + 1].posX - KeyFrame[playIndex].posX) / i_max_steps;
	incY = (KeyFrame[playIndex + 1].posY - KeyFrame[playIndex].posY) / i_max_steps;
	incZ = (KeyFrame[playIndex + 1].posZ - KeyFrame[playIndex].posZ) / i_max_steps;

	rotInc = (KeyFrame[playIndex + 1].rotRodIzq - KeyFrame[playIndex].rotRodIzq) / i_max_steps;
	giroMonitoInc = (KeyFrame[playIndex + 1].giroMonito - KeyFrame[playIndex].giroMonito) / i_max_steps;
	movBrazoInc = (KeyFrame[playIndex + 1].movBrazo - KeyFrame[playIndex].movBrazo) / i_max_steps;
	rotCabezaInc = (KeyFrame[playIndex + 1].rotCabeza - KeyFrame[playIndex].rotCabeza) / i_max_steps;
}


unsigned int generateTextures(const char* filename, bool alfa)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	int width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	
	unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (data)
	{
		if (alfa)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		else
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		return textureID;
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
		return 100;
	}

	stbi_image_free(data);
}

void getResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	SCR_WIDTH = mode->width;
	SCR_HEIGHT = (mode->height) - 80;

	lastX = SCR_WIDTH / 2.0f;
	lastY = SCR_HEIGHT / 2.0f;

}

void LoadTextures()
{
	t_piso_m = generateTextures("Texturas/piso_m.jpg", 0);
	t_piso_b = generateTextures("Texturas/concreto_m.jpg", 0);
	t_grass_m = generateTextures("Texturas/grass_m.jpg", 0);
	//Casa Rick
	t_c1_frontal = generateTextures("Texturas/CasaRick/c1_frontal.jpg", 0);
	t_c1_inferior = generateTextures("Texturas/CasaRick/c1_inferior.jpg", 0);
	t_c1_lateral = generateTextures("Texturas/CasaRick/c1_lateral.jpg", 0);
	t_c6_superior = generateTextures("Texturas/CasaRick/c6_superior.jpg", 0);
	t_t1_frontal = generateTextures("Texturas/CasaRick/t1_frontal.jpg", 0);
	t_t1_lateral = generateTextures("Texturas/CasaRick/t1_lateral.jpg", 0);
	t_t1_trasera = generateTextures("Texturas/CasaRick/t1_trasera.jpg", 0);
	t_c2_lateral = generateTextures("Texturas/CasaRick/c2_lateral.jpg", 0);
	t_c4_frontal = generateTextures("Texturas/CasaRick/c4_frontal.jpg", 0);
	t_c4_lateral_izq = generateTextures("Texturas/CasaRick/c4_lateral_izq.jpg", 0);
	t_c5_lateral = generateTextures("Texturas/CasaRick/c5_lateral.jpg", 0);
	t_c5_frontal = generateTextures("Texturas/CasaRick/c5_frontal.jpg", 0);
	t_c6_lateral = generateTextures("Texturas/CasaRick/c6_lateral.jpg", 0);
	t_c6_frontal = generateTextures("Texturas/CasaRick/c6_frontal.jpg", 0);
	t_t3_frontal = generateTextures("Texturas/CasaRick/t3_frontal.jpg", 0);
	t_t3_trasera = generateTextures("Texturas/CasaRick/t3_trasera.jpg", 0);
}

void myData()
{	
	float vertices[] = {
		// positions          // normals           // texture coords

		//Cubo
		-0.5f, -0.5f, 0.5f, 	0.0f,0.0f,1.0f, 	0.0f, 0.0f,//V0 - Frontal
		0.5f, -0.5f, 0.5f,		0.0f,0.0f,1.0f, 	1.0f, 0.0f,//V1
		0.5f, 0.5f, 0.5f,			0.0f,0.0f,1.0f,		1.0f, -1.0f,//V5

		-0.5f, -0.5f, 0.5f, 	0.0f,0.0f,1.0f,		0.0f, 0.0f,//V0
		0.5f, 0.5f, 0.5f,			0.0f,0.0f,1.0f,		1.0f, -1.0f,//V5
		-0.5f, 0.5f, 0.5f,		0.0f,0.0f,1.0f,		0.0f, -1.0f,//V4

		-0.5f, -0.5f, -0.5f, 	0.0f,0.0f,-1.0f,	1.0f, 0.0f,//V3 - Trasera
		0.5f, -0.5f, -0.5f,	 	0.0f,0.0f,-1.0f,	0.0f, 0.0f,//V2
		0.5f, 0.5f, -0.5f,	 	0.0f,0.0f,-1.0f,	0.0f, -1.0f,//V6

		-0.5f, -0.5f, -0.5f, 	0.0f,0.0f,-1.0f,	1.0f, 0.0f,//V3
		0.5f, 0.5f, -0.5f,	 	0.0f,0.0f,-1.0f,	0.0f, -1.0f,//V6
		-0.5f, 0.5f, -0.5f,	 	0.0f,0.0f,-1.0f,	1.0f, -1.0f,//V7

		-0.5f, -0.5f, 0.5f,	 	-1.0f,0.0f,0.0f,		1.0f, 0.0f,//V0 - Izq
		-0.5f, -0.5f, -0.5f, 	-1.0f,0.0f,0.0f,		0.0f, 0.0f,//V3
		-0.5f, 0.5f, -0.5f,	 	-1.0f,0.0f,0.0f,		0.0f, -1.0f,//V7

		-0.5f, -0.5f, 0.5f, 	-1.0f,0.0f,0.0f,		1.0f, 0.0f,//V0
		-0.5f, 0.5f, -0.5f, 	-1.0f,0.0f,0.0f,		0.0f, -1.0f,//V7
		-0.5f, 0.5f, 0.5f,		-1.0f,0.0f,0.0f,		1.0f, -1.0f,//V4

		0.5f, -0.5f, 0.5f,		1.0f,0.0f,0.0f,	0.0f, 0.0f,//V1 - Der
		0.5f, -0.5f, -0.5f, 	1.0f,0.0f,0.0f,	1.0f, 0.0f,//V2
		0.5f, 0.5f, -0.5f,		1.0f,0.0f,0.0f,	1.0f, -1.0f,//V6

		0.5f, -0.5f, 0.5f, 		1.0f,0.0f,0.0f,	0.0f, 0.0f,//V1
		0.5f, 0.5f, -0.5f, 		1.0f,0.0f,0.0f,	1.0f, -1.0f,//V6
		0.5f, 0.5f, 0.5f,	 		1.0f,0.0f,0.0f,	0.0f, -1.0f,//V5

		-0.5f, 0.5f, 0.5f, 		0.0f,1.0f,0.0f,		0.0f, 0.0f,//V4 - Sup
		0.5f, 0.5f, 0.5f,	 		0.0f,1.0f,0.0f, 	1.0f, 0.0f,//V5
		0.5f, 0.5f, -0.5f, 		0.0f,1.0f,0.0f,		1.0f, -1.0f,//V6

		-0.5f, 0.5f, 0.5f,		0.0f,1.0f,0.0f,		0.0f, 0.0f,//V4
		0.5f, 0.5f, -0.5f,		0.0f,1.0f,0.0f,		1.0f, -1.0f,//V6
		-0.5f, 0.5f, -0.5f, 	0.0f,1.0f,0.0f,		0.0f, -1.0f,//V7

		-0.5f, -0.5f, 0.5f, 	0.0f,-1.0f,0.0f,	0.0f, -1.0f,//V0 - Inf
		0.5f, -0.5f, 0.5f,		0.0f,-1.0f,0.0f,	1.0f, -1.0f,//V1
		0.5f, -0.5f, -0.5f, 	0.0f,-1.0f,0.0f,	1.0f, 0.0f,//V2

		-0.5f, -0.5f, 0.5f,	 	0.0f,-1.0f,0.0f,	0.0f, -1.0f,//V0
		0.5f, -0.5f, -0.5f,	 	0.0f,-1.0f,0.0f,	1.0f, 0.0f,//V2
		-0.5f, -0.5f, -0.5f, 	0.0f,-1.0f,0.0f,	0.0f, 0.0f,//V3


		//techo, indice=36
		-0.5f, -0.33333, 0.5f, 	0.0f,0.0f,1.0f,				0.0f, 0.0f,//V0 - Frontal
		0.5f, -0.33333, 0.5f,	 	0.0f,0.0f,1.0f,				1.0f, 0.0f,//V1
		0.0f, 0.66667, 0.5f,	 	0.0f,0.0f,1.0f,				0.5f, -1.0f,//V2

		-0.5f, -0.33333, -0.5f,	0.0f,0.0f,-1.0f, 			0.0f, 0.0f,//V3 - Trasera
		0.5f, -0.33333, -0.5f, 	0.0f,0.0f,-1.0f, 			1.0f, 0.0f,//V4
		0.0f, 0.66667, -0.5f,	 	0.0f,0.0f,-1.0f, 			0.5f, -1.0f,//V5

		0.5f, -0.33333, 0.5f,	 	-0.866025f,0.5f,0.0f, 0.0f, 0.0f,//V1 - Lateral izquierda
		0.5f, -0.33333, -0.5f, 	-0.866025f,0.5f,0.0f, 1.0f, 0.0f,//V4
		0.0f, 0.66667, -0.5f,	 	-0.866025f,0.5f,0.0f, 1.0f, -1.0f,//V5

		0.5f, -0.33333, 0.5f, 	-0.866025f,0.5f,0.0f, 0.0f, 0.0f,//V1
		0.0f, 0.66667, -0.5f, 	-0.866025f,0.5f,0.0f,	1.0f, -1.0f,//V5
		0.0f, 0.66667, 0.5f,		-0.866025f,0.5f,0.0f,	0.0f, -1.0f,//V2

		-0.5f, -0.33333, 0.5f,	 0.866025f,0.5f,0.0f,	1.0f, 0.0f,//V0 - Lateral derecha
		-0.5f, -0.33333, -0.5f,  0.866025f,0.5f,0.0f,	0.0f, 0.0f,//V3
		0.0f, 0.66667, -0.5f,		 0.866025f,0.5f,0.0f,	0.0f, -1.0f,//V5

		-0.5f, -0.33333, 0.5f, 	0.866025f,0.5f,0.0f,	1.0f, 0.0f,//V0
		0.0f, 0.66667, -0.5f,	 	0.866025f,0.5f,0.0f,	0.0f, -1.0f,//V5
		0.0f, 0.66667, 0.5f,	 	0.866025f,0.5f,0.0f,	1.0f, -1.0f,//V2

		0.5f, -0.33333, 0.5f,		0.0f,-1.0f,0.0f,	0.0f, -1.0f,//V1 - Plano inferior
		0.5f, -0.33333, -0.5f,	0.0f,-1.0f,0.0f,	1.0f, -1.0f,//V4
		-0.5f, -0.33333, -0.5f, 0.0f,-1.0f,0.0f,	1.0f, 0.0f,//V3

		0.5f, -0.33333, 0.5f,		0.0f,-1.0f,0.0f,	0.0f, -1.0f,//V1
		-0.5f, -0.33333, -0.5f, 0.0f,-1.0f,0.0f,	1.0f, 0.0f,//V3
		-0.5f, -0.33333, 0.5f,	0.0f,-1.0f,0.0f,	0.0f, 0.0f,//V0


		//Piso, índice= 60
		-0.5f,  0.0f, -0.5f,  	0.0f,  1.0f,  0.0f, 	0.0f,  0.0f,
		 0.5f,  0.0f, -0.5f,  	0.0f,  1.0f,  0.0f,  	10.0f,  0.0f,
		 0.5f,  0.0f,  0.5f,  	0.0f,  1.0f,  0.0f,  	10.0f,  10.0f,
		-0.5f,  0.0f,  0.5f,  	0.0f,  1.0f,  0.0f,  	0.0f,  10.0f,



	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

}

void myData2()
{
	GLfloat skyboxVertices[] = {
		// Positions
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);

	glBindVertexArray(skyboxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

	// SkyBox
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid *)0);

}


void animate(void)
{
	if (play)
	{
		if (i_curr_steps >= i_max_steps) //end of animation between frames?
		{
			playIndex++;
			if (playIndex > FrameIndex - 2)	//end of total animation?
			{
				printf("termina anim\n");
				playIndex = 0;
				play = false;
			}
			else //Next frame interpolations
			{
				i_curr_steps = 0; //Reset counter
								  //Interpolation
				interpolation();
			}
		}
		else
		{
			//Draw animation
			posX += incX;
			posY += incY;
			posZ += incZ;

			rotRodIzq += rotInc;
			giroMonito += giroMonitoInc;

			movBrazo += movBrazoInc;

			rotCabeza += rotCabezaInc;

			i_curr_steps++;
		}

	}
	
}

void display(	Shader shader, Shader skyboxShader, GLuint skybox, Model edificio5,
				Model edificio6, Model edificio7, Model tree1, Model tree2,
				Model tree3)
{
	shader.use();

	//Setup Advanced Lights
	shader.setVec3("viewPos", camera.Position);
	shader.setVec3("dirLight.direction", lightDirection);
	shader.setVec3("dirLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	shader.setVec3("pointLight[0].position", lightPosition);
	shader.setVec3("pointLight[0].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("pointLight[0].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("pointLight[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));
	shader.setFloat("pointLight[0].constant", 1.0f);
	shader.setFloat("pointLight[0].linear", 0.009f);
	shader.setFloat("pointLight[0].quadratic", 0.0032f);

	shader.setVec3("pointLight[1].position", glm::vec3(0.0, 0.0f, 0.0f));
	shader.setVec3("pointLight[1].ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("pointLight[1].diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setVec3("pointLight[1].specular", glm::vec3(0.0f, 0.0f, 0.0f));
	shader.setFloat("pointLight[1].constant", 1.0f);
	shader.setFloat("pointLight[1].linear", 0.009f);
	shader.setFloat("pointLight[1].quadratic", 0.032f);

	shader.setFloat("material_shininess", 32.0f);

	// create transformations and Projection
	glm::mat4 tmp = glm::mat4(1.0f);
	glm::mat4 origin,temp = glm::mat4(1.0f); //Pivotes casa de rick
	glm::mat4 model = glm::mat4(1.0f);		// initialize Matrix, Use this matrix for individual models
	glm::mat4 view = glm::mat4(1.0f);		//Use this matrix for ALL models
	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection

	//Use "projection" to include Camera
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 50.0f);
	view = camera.GetViewMatrix();

	//Scenario Rotation
	view = glm::rotate(view,
		glm::radians(rotY),
		glm::vec3(0.0f, 1.0f, 0.0f));

	// pass them to the shaders
	shader.setMat4("model", model);
	shader.setMat4("view", view);
	// note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
	shader.setMat4("projection", projection);


	//Piso
	model = glm::scale(glm::mat4(1.0f), glm::vec3(80.0f, 1.0f, 60.0f));
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t_piso_m);
	glDrawArrays(GL_QUADS, 60, 4);
	glBindVertexArray(0);

	//Sector B
	model = glm::translate(glm::mat4(1.0f), glm::vec3(19.0f, 0.01f, -23.0f));
	tmp = model;
	origin = model;
	model = glm::scale(model, glm::vec3(42.0f, 1.0f, 14.0f));
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t_piso_b);
	glDrawArrays(GL_QUADS, 60, 4);
	glBindVertexArray(0);

	//Edificio 5
	model = glm::translate(tmp, glm::vec3(-9.5f, 0.0f, -1.0f));
	model = glm::rotate(model,glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.01217f, 0.01217f, 0.01217f));
	shader.setMat4("model", model);
	edificio5.Draw(shader);

	//Casa de rick
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	origin = glm::translate(tmp, glm::vec3(8.0f, 0.8f, 0.0f));
	origin = glm::scale(origin, glm::vec3(0.4f, 0.4f, 0.4f));
	model = glm::scale(origin, glm::vec3(6.0f, 4.0f, 10.0f));
	shader.setMat4("model", model);

	//Cubo 1
	glBindTexture(GL_TEXTURE_2D, t_c1_frontal);
	glDrawArrays(GL_TRIANGLES, 0, 6); //Cara frontal
	glBindTexture(GL_TEXTURE_2D, t_c1_lateral);
	glDrawArrays(GL_TRIANGLES, 6, 6); //Cara Trasera
	glDrawArrays(GL_TRIANGLES, 12, 6); //Lateral Izquierda
	glDrawArrays(GL_TRIANGLES, 18, 6); //Lateral Derecha
	glBindTexture(GL_TEXTURE_2D, t_c1_inferior);
	glDrawArrays(GL_TRIANGLES, 24, 6); //Superior 
	glDrawArrays(GL_TRIANGLES, 30, 6); //Inferior
	
	model = glm::translate(origin, glm::vec3(0.0f, 3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(7.0f, 3.0f, 10.0f));
	shader.setMat4("model", model);

	//Techo 1
	glBindTexture(GL_TEXTURE_2D, t_t1_frontal);
	glDrawArrays(GL_TRIANGLES, 36, 3); //Frontal
	glBindTexture(GL_TEXTURE_2D, t_t1_trasera);
	glDrawArrays(GL_TRIANGLES, 39, 3); //Trasera
	glBindTexture(GL_TEXTURE_2D, t_t1_lateral);
	glDrawArrays(GL_TRIANGLES, 42, 6); //Lateral Izquierda
	glDrawArrays(GL_TRIANGLES, 48, 6); //Lateral Derecha
	glBindTexture(GL_TEXTURE_2D, t_c6_superior);
	glDrawArrays(GL_TRIANGLES, 54, 6); //Inferior

	model = glm::translate(origin, glm::vec3(5.5f, 0.0f, -2.5f));
	model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	temp = model;
	model = glm::translate(model, glm::vec3(0.0f, 0.0f, 1.5f));
	model = glm::scale(model, glm::vec3(5.0f, 4.0f, 8.0f));
	shader.setMat4("model", model);

	// Cubo 2
	//glBindTexture(GL_TEXTURE_2D, t_c1_frontal);
	//glDrawArrays(GL_TRIANGLES, 0, 6); //Cara frontal
	//glBindTexture(GL_TEXTURE_2D, t_c1_lateral);
	//glDrawArrays(GL_TRIANGLES, 6, 6);	 //Cara Trasera
	glBindTexture(GL_TEXTURE_2D, t_c2_lateral);
	glDrawArrays(GL_TRIANGLES, 12, 6); //Lateral Izquierda
	glBindTexture(GL_TEXTURE_2D, t_c1_lateral);
	glDrawArrays(GL_TRIANGLES, 18, 6); //Lateral Derecha
	glBindTexture(GL_TEXTURE_2D, t_c1_inferior);
	glDrawArrays(GL_TRIANGLES, 24, 6); //Superior
	glDrawArrays(GL_TRIANGLES, 30, 6); //Inferior

	model = glm::translate(temp, glm::vec3(0.0f, 2.8333f, 0.0f));
	model = glm::scale(model, glm::vec3(5.0f, 2.5f, 11.0f));
	shader.setMat4("model", model);
	shader.setVec3("aColor", 0.5f, 0.5f, 0.5f);

	//Techo 2
	//glBindTexture(GL_TEXTURE_2D, t_t1_frontal);
	//glDrawArrays(GL_TRIANGLES, 36, 3); //Frontal
	//glBindTexture(GL_TEXTURE_2D, t_t1_trasera);
	//glDrawArrays(GL_TRIANGLES, 39, 3); //Trasera
	glBindTexture(GL_TEXTURE_2D, t_t1_lateral);
	glDrawArrays(GL_TRIANGLES, 42, 6); //Lateral Izquierda
	glDrawArrays(GL_TRIANGLES, 48, 6); //Lateral Derecha
	glBindTexture(GL_TEXTURE_2D, t_c6_superior);
	glDrawArrays(GL_TRIANGLES, 54, 6); //Inferior

	model = glm::translate(temp, glm::vec3(0.0f, 0.0f, 8.5f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	temp = model;
	model = glm::scale(model, glm::vec3(6.0f, 4.0f, 5.0f));
	shader.setMat4("model", model);
	
	//Cubo 3
	//glBindTexture(GL_TEXTURE_2D, t_c1_frontal);
	//glDrawArrays(GL_TRIANGLES, 0, 6); //Cara frontal
	glBindTexture(GL_TEXTURE_2D, t_c1_lateral);
	glDrawArrays(GL_TRIANGLES, 6, 6);	 //Cara Trasera
	//glDrawArrays(GL_TRIANGLES, 12, 6); //Lateral Izquierda
	glDrawArrays(GL_TRIANGLES, 18, 6); //Lateral Derecha
	glBindTexture(GL_TEXTURE_2D, t_c1_inferior);
	//glDrawArrays(GL_TRIANGLES, 24, 6); //Superior
	glDrawArrays(GL_TRIANGLES, 30, 6); //Inferior

	model = glm::translate(temp, glm::vec3(0.0f, 0.0f, 5.5f));
	model = glm::scale(model, glm::vec3(6.0f, 4.0f, 6.0f));
	shader.setMat4("model", model);
	
	//Cubo 4
	glBindTexture(GL_TEXTURE_2D, t_c4_frontal);
	glDrawArrays(GL_TRIANGLES, 0, 6); //Cara frontal
	glBindTexture(GL_TEXTURE_2D, t_c4_lateral_izq);
	//glDrawArrays(GL_TRIANGLES, 6, 6);	 //Cara Trasera
	glDrawArrays(GL_TRIANGLES, 12, 6); //Lateral Izquierda
	glBindTexture(GL_TEXTURE_2D, t_c1_lateral);
	glDrawArrays(GL_TRIANGLES, 18, 6); //Lateral Derecha
	glBindTexture(GL_TEXTURE_2D, t_c1_inferior);
	//glDrawArrays(GL_TRIANGLES, 24, 6); //Superior
	glDrawArrays(GL_TRIANGLES, 30, 6); //Inferior

	model = glm::translate(temp, glm::vec3(0.0f, 2.5f, 5.5f));
	model = glm::scale(model, glm::vec3(6.5f, 1.0f, 6.0f));
	shader.setMat4("model", model);
	
	//Cubo 6
	glBindTexture(GL_TEXTURE_2D, t_c6_frontal);
	glDrawArrays(GL_TRIANGLES, 0, 6); //Cara frontal
	glBindTexture(GL_TEXTURE_2D, t_c6_lateral);
	//glDrawArrays(GL_TRIANGLES, 6, 6);	 //Cara Trasera
	glDrawArrays(GL_TRIANGLES, 12, 6); //Lateral Izquierda
	glDrawArrays(GL_TRIANGLES, 18, 6); //Lateral Derecha
	glBindTexture(GL_TEXTURE_2D, t_c6_superior);
	glDrawArrays(GL_TRIANGLES, 24, 6); //Superior
	glDrawArrays(GL_TRIANGLES, 30, 6); //Inferior

	model = glm::translate(temp, glm::vec3(0.0f, 3.5f, 0.0f));
	temp = model;
	model = glm::scale(model, glm::vec3(6.0f, 3.0f, 5.0f));
	shader.setMat4("model", model);
	
	//Cubo 5
	glBindTexture(GL_TEXTURE_2D, t_c5_frontal);
	glDrawArrays(GL_TRIANGLES, 0, 6); //Cara frontal
	glBindTexture(GL_TEXTURE_2D, t_c5_lateral);
	glDrawArrays(GL_TRIANGLES, 6, 6);	 //Cara Trasera
	glDrawArrays(GL_TRIANGLES, 12, 6); //Lateral Izquierda
	glDrawArrays(GL_TRIANGLES, 18, 6); //Lateral Derecha
	//glBindTexture(GL_TEXTURE_2D, t_c1_inferior);
	//glDrawArrays(GL_TRIANGLES, 24, 6); //Superior
	//glDrawArrays(GL_TRIANGLES, 30, 6); //Inferior

	model = glm::translate(temp, glm::vec3(0.0f, 2.1666f, 0.0f));
	model = glm::scale(model, glm::vec3(7.0f, 2.0f, 5.0f));
	shader.setMat4("model", model);
	
	//Techo 3
	glBindTexture(GL_TEXTURE_2D, t_t3_frontal);
	glDrawArrays(GL_TRIANGLES, 36, 3); //Frontal
	glBindTexture(GL_TEXTURE_2D, t_t3_trasera);
	glDrawArrays(GL_TRIANGLES, 39, 3); //Trasera
	glBindTexture(GL_TEXTURE_2D, t_t1_lateral);
	glDrawArrays(GL_TRIANGLES, 42, 6); //Lateral Izquierda
	glDrawArrays(GL_TRIANGLES, 48, 6); //Lateral Derecha
	glBindTexture(GL_TEXTURE_2D, t_c6_superior);
	glDrawArrays(GL_TRIANGLES, 54, 6); //Inferior
	
	glBindVertexArray(0);
	
	//Sector D
	model = glm::translate(glm::mat4(1.0f), glm::vec3(30.0f, 0.01f, 7.0f));
	tmp = model;
	model = glm::scale(model, glm::vec3(20.0f, 1.0f, 30.0f));
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t_piso_b);
	glDrawArrays(GL_QUADS, 60, 4);
	glBindVertexArray(0);

	//Edificio 6
	model = glm::translate(tmp, glm::vec3(-4.5f, 0.01f, -9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.001112f, 0.001112f, 0.001112f));
	shader.setMat4("model", model);
	edificio6.Draw(shader);

	//Edificio 7
	model = glm::translate(tmp, glm::vec3(-4.5f, 0.01f, 9.0f));
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
	shader.setMat4("model", model);
	edificio7.Draw(shader);

	//Pasto sector D
	model = glm::translate(tmp, glm::vec3(7.0f, 0.01f, 0.0f));
	tmp = model;
	model = glm::scale(model, glm::vec3(4.0f, 1.0f, 25.0f));
	shader.setMat4("model", model);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t_grass_m);
	glDrawArrays(GL_QUADS, 60, 4);
	glBindVertexArray(0);

	//�rbol 1
	model = glm::scale(tmp, glm::vec3(0.0131f, 0.0131f, 0.0131f));
	shader.setMat4("model", model);
	tree1.Draw(shader);

	//�rbol 2
	for (int i = 1; i <= 2; i++) {
		model = glm::translate(tmp, glm::vec3(0.0f, 0.01f,
			5.0f * (float) pow(-1.0f,(float)i)));
		model = glm::scale(model, glm::vec3(0.0131f, 0.0131f, 0.0131f));
		shader.setMat4("model", model);
		tree2.Draw(shader);
	}

	//�rbol 3
	for (int i = 1; i <= 2; i++) {
		model = glm::translate(tmp, glm::vec3(0.0f, 0.01f,
			10.0f * (float)pow(-1.0f, (float)i)));
		model = glm::scale(model, glm::vec3(0.0131f, 0.0131f, 0.0131f));
		shader.setMat4("model", model);
		tree3.Draw(shader);
	}
	

	// Draw skybox as last
	glDepthFunc(GL_LEQUAL);  // Change depth function so depth test passes when values are equal to depth buffer's content
	skyboxShader.use();
	view = glm::mat4(glm::mat3(view));	// Remove any translation component of the view matrix

	skyboxShader.setMat4("view", view);
	skyboxShader.setMat4("projection", projection);

	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS); // Set depth function back to default
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    /*glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw window creation
    // --------------------
	monitors = glfwGetPrimaryMonitor();
	getResolution();

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Practica 10 2020-2", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
	glfwSetWindowPos(window, 0, 30);
    glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, my_input);
    glfwSetFramebufferSizeCallback(window, resize);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	//To Enable capture of our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	glewInit();


	//Mis funciones
	//Datos a utilizar
	LoadTextures();
	myData();
	myData2();
	glEnable(GL_DEPTH_TEST);
	
	//Shaders
	Shader modelShader("Shaders/shader_Lights.vs", "Shaders/shader_Lights.fs");
	Shader SkyBoxshader("Shaders/SkyBox.vs", "Shaders/SkyBox.frag");

	// Load models
	/*
	Model botaDer = ((char *)"Models/Personaje/bota.obj");
	Model piernaDer = ((char *)"Models/Personaje/piernader.obj");
	Model piernaIzq = ((char *)"Models/Personaje/piernader.obj");
	Model torso = ((char *)"Models/Personaje/torso.obj");
	Model brazoDer = ((char *)"Models/Personaje/brazoder.obj");
	Model brazoIzq = ((char *)"Models/Personaje/brazoizq.obj");
	Model cabeza = ((char *)"Models/Personaje/cabeza.obj");
	Model pisoModel = ((char *)"Models/piso/piso.obj");*/
	Model edificio5 = ((char *)"Models/Edificio5/edificio5.obj");
	Model edificio6 = ((char *)"Models/Edificio6/edificio6.obj");
	Model edificio7 = ((char *)"Models/Edificio7/edificio7.obj");
	Model tree1 = ((char *)"Models/Trees/Tree1/tree1.obj");
	Model tree2 = ((char *)"Models/Trees/Tree2/tree2.obj");
	Model tree3 = ((char *)"Models/Trees/Tree3/tree3.obj");

	//Inicializaci�n de KeyFrames
	for (int i = 0; i < MAX_FRAMES; i++)
	{
		KeyFrame[i].posX = 0;
		KeyFrame[i].posY = 0;
		KeyFrame[i].posZ = 0;
		KeyFrame[i].rotRodIzq = 0;
		KeyFrame[i].giroMonito = 0;
		KeyFrame[i].movBrazo = 0;
		KeyFrame[i].rotCabeza = 0;
	}


	// Load textures
	vector<const GLchar*> faces;
	faces.push_back("SkyBox/west.jpg");
	faces.push_back("SkyBox/east.jpg");
	faces.push_back("SkyBox/up.jpg");
	faces.push_back("SkyBox/down.jpg");
	faces.push_back("SkyBox/south.jpg");
	faces.push_back("SkyBox/north.jpg");

	GLuint cubemapTexture = TextureLoading::LoadCubemap(faces);

	glm::mat4 projection = glm::mat4(1.0f);	//This matrix is for Projection
	projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	// render loop
    // While the windows is not closed
    while (!glfwWindowShouldClose(window))
    {
		// per-frame time logic
		// --------------------
		double currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
        // input
        // -----
        //my_input(window);
		animate();

        // render
        // Background color
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//display(modelShader, ourModel, llantasModel);
		display(modelShader, SkyBoxshader, cubemapTexture, 
				edificio5, edificio6, edificio7,
				tree1, tree2, tree3);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void my_input(GLFWwindow *window, int key, int scancode, int action, int mode)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_UP) == GLFW_PRESS)
		camera.ProcessKeyboard(UPWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_PAGE_DOWN) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWNWARD, (float)deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		rotY-=15;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		rotY += 15;
	//To Configure Model
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		posZ++;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		posZ--;
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		posX--;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		posX++;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		rotRodIzq--;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
		rotRodIzq++;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
		giroMonito--;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		giroMonito++;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		movBrazo--;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		movBrazo++;
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		posY++;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		posY--;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		rotCabeza+=20;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		rotCabeza-=20;
	
	
	//To play KeyFrame animation 
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (play == false && (FrameIndex > 1))
		{
			resetElements();
			//First Interpolation				
			interpolation();

			play = true;
			playIndex = 0;
			i_curr_steps = 0;
		}
		else
		{
			play = false;
		}
	}

	//To Save a KeyFrame
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
	{
		if (FrameIndex < MAX_FRAMES)
		{
			saveFrame();
		}
	}
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void resize(GLFWwindow* window, int width, int height)
{
    // Set the Viewport to the size of the created window
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}