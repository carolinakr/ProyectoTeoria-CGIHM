/*
Animaci�n:
Sesi�n 1:
Simple o b�sica:Por banderas y condicionales (m�s de 1 transformaci�n geom�trica se ve modificada
Sesi�n 2
Compleja: Por medio de funciones y algoritmos.
Textura Animada
*/
//para cargar imagens
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

//variables para animaci�n
float movCoche;
float movOffset;
float rotllanta;
float rotllantaOffset;
bool avanza;
float toffsetflechau = 0.0f;
float toffsetflechav = 0.0f;
float toffsetnumerou = 0.0f;
float toffsetnumerov = 0.0f;
float toffsetnumerocambiau = 0.0;
float toffsetnumerocambiav = 0.0;
int numeroActual = 0;
float angulovaria = 0.0f;

// Fuego y Aeolipile
bool fireActive = false;
bool fKeyWasPressed = false;
float fireStartWallTime = 0.0f;
float aeolipileAngle = 0.0f;
float fireScaleAnim = 1.0f;
float fireVibAngle = 0.0f;
float lastNumeroChangeTime = 0.0f;
bool fireStopping = false;
float fireStopWallTime = 0.0f;
const float SPIN_DECEL_SECS = 2.5f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture FuegoTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;



Model Kitt_M;
Model Llanta_M;
Model Pista_M;
Model Nave_M;
Model Ala_M;
Model Aeolipile_base_M;
Model Aeolipile_M;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Ciclo día/noche
static const float CYCLE_DURATION = 60.0f; // duración total del ciclo en segundos
float cycleElapsed = 0.0f;
bool  cycleRunning = false;

// Ciclo día/noche
static const float CYCLE_DURATION = 60.0f; // duración total en segundos
float cycleElapsed = 0.0f;
bool  cycleRunning = false;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";



//c�lculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


// ============================================================
//  GEOMETRÍA PROCEDURAL – Facultad de Ingeniería UNAM
//  Cada función empuja un Mesh* a meshList y devuelve su índice
// ============================================================

// --- Cilindro (tapa arriba + tapa abajo + cuerpo lateral) ---
// sectors: número de subdivisiones angulares (ej. 16)
// radio 1.0, altura 1.0 → escalar con glm::scale en el render
int MakeCylinder(int sectors)
{
	const float PI = 3.14159265f;
	std::vector<GLfloat> verts;
	std::vector<unsigned int> inds;

	// Centro tapa inferior (índice 0)
	verts.insert(verts.end(), { 0.0f, 0.0f, 0.0f,  0.5f, 0.5f,  0.0f, -1.0f, 0.0f });
	// Vértices tapa inferior (índices 1 .. sectors)
	for (int i = 0; i < sectors; i++) {
		float a = 2.0f * PI * i / sectors;
		float x = cosf(a), z = sinf(a);
		verts.insert(verts.end(), { x, 0.0f, z,  0.5f + x * 0.5f, 0.5f + z * 0.5f,  0.0f, -1.0f, 0.0f });
	}
	// Triángulos tapa inferior
	for (int i = 1; i <= sectors; i++)
		inds.insert(inds.end(), { 0u, (unsigned)(i % sectors + 1), (unsigned)i });

	int topCenter = sectors + 1;
	// Centro tapa superior
	verts.insert(verts.end(), { 0.0f, 1.0f, 0.0f,  0.5f, 0.5f,  0.0f, 1.0f, 0.0f });
	// Vértices tapa superior (índices topCenter+1 .. topCenter+sectors)
	for (int i = 0; i < sectors; i++) {
		float a = 2.0f * PI * i / sectors;
		float x = cosf(a), z = sinf(a);
		verts.insert(verts.end(), { x, 1.0f, z,  0.5f + x * 0.5f, 0.5f + z * 0.5f,  0.0f, 1.0f, 0.0f });
	}
	// Triángulos tapa superior
	for (int i = 1; i <= sectors; i++)
		inds.insert(inds.end(), { (unsigned)topCenter, (unsigned)(topCenter + i), (unsigned)(topCenter + i % sectors + 1) });

	// Cuerpo lateral: quad por cada sector
	// usa los vértices de tapa inferior/superior (solo posición, normal lateral)
	int sideBase = topCenter + sectors + 1;
	for (int i = 0; i < sectors; i++) {
		float a0 = 2.0f * PI * i / sectors;
		float a1 = 2.0f * PI * (i + 1) / sectors;
		float x0 = cosf(a0), z0 = sinf(a0);
		float x1 = cosf(a1), z1 = sinf(a1);
		float u0 = (float)i / sectors, u1 = (float)(i + 1) / sectors;
		// 4 verts del quad: BL, BR, TR, TL
		verts.insert(verts.end(), { x0, 0.0f, z0,  u0, 0.0f,  x0, 0.0f, z0 });
		verts.insert(verts.end(), { x1, 0.0f, z1,  u1, 0.0f,  x1, 0.0f, z1 });
		verts.insert(verts.end(), { x1, 1.0f, z1,  u1, 1.0f,  x1, 0.0f, z1 });
		verts.insert(verts.end(), { x0, 1.0f, z0,  u0, 1.0f,  x0, 0.0f, z0 });
		unsigned b = sideBase + i * 4;
		inds.insert(inds.end(), { b, b+1, b+2,  b, b+2, b+3 });
	}

	Mesh* m = new Mesh();
	m->CreateMesh(verts.data(), inds.data(), (unsigned)verts.size(), (unsigned)inds.size());
	meshList.push_back(m);
	return (int)meshList.size() - 1;
}

// --- Caja unitaria [0,1]^3, normales simples ---
int MakeBox()
{
	GLfloat v[] = {
		// cara frontal (z=1)  normal 0,0,1
		0,0,1,  0,0,  0,0,1,
		1,0,1,  1,0,  0,0,1,
		1,1,1,  1,1,  0,0,1,
		0,1,1,  0,1,  0,0,1,
		// cara trasera (z=0) normal 0,0,-1
		1,0,0,  0,0,  0,0,-1,
		0,0,0,  1,0,  0,0,-1,
		0,1,0,  1,1,  0,0,-1,
		1,1,0,  0,1,  0,0,-1,
		// izquierda (x=0) normal -1,0,0
		0,0,0,  0,0,  -1,0,0,
		0,0,1,  1,0,  -1,0,0,
		0,1,1,  1,1,  -1,0,0,
		0,1,0,  0,1,  -1,0,0,
		// derecha (x=1) normal 1,0,0
		1,0,1,  0,0,  1,0,0,
		1,0,0,  1,0,  1,0,0,
		1,1,0,  1,1,  1,0,0,
		1,1,1,  0,1,  1,0,0,
		// abajo (y=0) normal 0,-1,0
		0,0,0,  0,0,  0,-1,0,
		1,0,0,  1,0,  0,-1,0,
		1,0,1,  1,1,  0,-1,0,
		0,0,1,  0,1,  0,-1,0,
		// arriba (y=1) normal 0,1,0
		0,1,1,  0,0,  0,1,0,
		1,1,1,  1,0,  0,1,0,
		1,1,0,  1,1,  0,1,0,
		0,1,0,  0,1,  0,1,0,
	};
	unsigned int idx[] = {
		 0, 1, 2,  0, 2, 3,
		 4, 5, 6,  4, 6, 7,
		 8, 9,10,  8,10,11,
		12,13,14, 12,14,15,
		16,17,18, 16,18,19,
		20,21,22, 20,22,23,
	};
	Mesh* m = new Mesh();
	m->CreateMesh(v, idx, 24 * 8, 36);
	meshList.push_back(m);
	return (int)meshList.size() - 1;
}

// Índices globales de meshes de la FI (se asignan en CreateObjects)
int MESH_CILINDRO   = -1;
int MESH_CAJA       = -1;

void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};
	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};
	

	unsigned int flechaIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat flechaVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int scoreIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat scoreVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		1.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		1.0f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	unsigned int numeroIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	};

	GLfloat numeroVertices[] = {
		-0.5f, 0.0f, 0.5f,		0.0f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, 0.5f,		0.25f, 0.67f,		0.0f, -1.0f, 0.0f,
		0.5f, 0.0f, -0.5f,		0.25f, 1.0f,		0.0f, -1.0f, 0.0f,
		-0.5f, 0.0f, -0.5f,		0.0f, 1.0f,		0.0f, -1.0f, 0.0f,

	};

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);


	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	Mesh* obj5 = new Mesh();
	obj5->CreateMesh(flechaVertices, flechaIndices, 32, 6);
	meshList.push_back(obj5);

	Mesh* obj6 = new Mesh();
	obj6->CreateMesh(scoreVertices, scoreIndices, 32, 6);
	meshList.push_back(obj6);

	Mesh* obj7 = new Mesh();
	obj7->CreateMesh(numeroVertices, numeroIndices, 32, 6);
	meshList.push_back(obj7);

	// Fuego billboard vertical (meshList[7])
	unsigned int fuegoIndices[] = {
		0, 1, 2,
		0, 2, 3,
	};
	GLfloat fuegoVertices[] = {
		-0.5f, -0.5f, 0.0f,	0.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		 0.5f, -0.5f, 0.0f,	1.0f, 0.0f,		0.0f, 0.0f, -1.0f,
		 0.5f,  0.5f, 0.0f,	1.0f, 1.0f,		0.0f, 0.0f, -1.0f,
		-0.5f,  0.5f, 0.0f,	0.0f, 1.0f,		0.0f, 0.0f, -1.0f,
	};
	Mesh* obj8 = new Mesh();
	obj8->CreateMesh(fuegoVertices, fuegoIndices, 32, 6);
	meshList.push_back(obj8);

	// Geometría para la escena de la Facultad de Ingeniería
	MESH_CILINDRO = MakeCylinder(18);
	MESH_CAJA     = MakeBox();
}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CreateShaders();

	// Cámara al inicio del corredor FI, mirando hacia Z negativo (adentro del pasillo)
	// Centro X = 0 (entre COR_XPARED=-5 y COR_XBARDA=+5), altura de persona
	camera = Camera(glm::vec3(0.0f, 1.7f, 16.5f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 0.06f, 0.25f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();
	FlechaTexture = Texture("Textures/flechas.tga");
	FlechaTexture.LoadTextureA();
	FuegoTexture = Texture("Textures/P9-2/Fuego-humo.png");
	FuegoTexture.LoadTextureA();
	NumerosTexture = Texture("Textures/numerosbase.tga");
	NumerosTexture.LoadTextureA();
	Numero1Texture = Texture("Textures/numero1.tga");
	Numero1Texture.LoadTextureA();
	Numero2Texture = Texture("Textures/numero2.tga");
	Numero2Texture.LoadTextureA();


	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Pista_M = Model();
	Pista_M.LoadModel("Models/pista.obj");
	Nave_M = Model();
	Nave_M.LoadModel("Models/nave.obj");
	Ala_M = Model();
	Ala_M.LoadModel("Models/ala.obj");
	Aeolipile_base_M = Model();
	Aeolipile_base_M.LoadModel("Models/Aeolipile/Aeolipile_base.obj");
	Aeolipile_M = Model();
	Aeolipile_M.LoadModel("Models/Aeolipile/Aeolipile.obj");

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f, 2.5f, 1.5f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.0f, 0.0f, 1.0f,
		1.0f, 2.0f,
		5.0f, 10.0f, 0.0f,
		0.0f, -5.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		15.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);
	
	movCoche = 0.0f;
	movOffset = 0.01f;
	rotllanta = 0.0f;
	rotllantaOffset = 10.0f;

	glm::vec3 lowerLight(0.0f, 0.0f, 0.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	glm::vec2 toffset = glm::vec2(0.0f, 0.0f);

	
	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		angulovaria += 0.5f * deltaTime;

		// --- Numero cambiante a velocidad visible (cada 0.5s de tiempo real) ---
		{
			float nowReal = (float)glfwGetTime();
			if (nowReal - lastNumeroChangeTime > 0.5f) {
				numeroActual++;
				if (numeroActual > 8)
					numeroActual = 0;

				// Fila 1 (v=0.00): digitos 1-4
				if      (numeroActual == 0) { toffsetnumerocambiau = 0.0f;  toffsetnumerocambiav =  0.0f; }
				else if (numeroActual == 1) { toffsetnumerocambiau = 0.25f; toffsetnumerocambiav =  0.0f; }
				else if (numeroActual == 2) { toffsetnumerocambiau = 0.5f;  toffsetnumerocambiav =  0.0f; }
				else if (numeroActual == 3) { toffsetnumerocambiau = 0.75f; toffsetnumerocambiav =  0.0f; }
				// Fila 2 (v=-0.33): digitos 5-8
				else if (numeroActual == 4) { toffsetnumerocambiau = 0.0f;  toffsetnumerocambiav = -0.33f; }
				else if (numeroActual == 5) { toffsetnumerocambiau = 0.25f; toffsetnumerocambiav = -0.33f; }
				else if (numeroActual == 6) { toffsetnumerocambiau = 0.5f;  toffsetnumerocambiav = -0.33f; }
				else if (numeroActual == 7) { toffsetnumerocambiau = 0.75f; toffsetnumerocambiav = -0.33f; }
				// Fila 3 (v=-0.67): digito 9
				else if (numeroActual == 8) { toffsetnumerocambiau = 0.0f;  toffsetnumerocambiav = -0.67f; }

				lastNumeroChangeTime = nowReal;
			}
		}

		// --- Tecla F: toggle fuego ---
		{
			bool* keys = mainWindow.getsKeys();
			if (keys[GLFW_KEY_F] && !fKeyWasPressed) {
				fireActive = !fireActive;
				if (fireActive) {
					fireStartWallTime = (float)glfwGetTime();
					fireStopping = false;
				} else {
					// Apagar fuego: iniciar deceleration si ya estaba girando
					float elapsed = (float)glfwGetTime() - fireStartWallTime;
					if (elapsed > 5.0f) {
						fireStopping = true;
						fireStopWallTime = (float)glfwGetTime();
					}
				}
				fKeyWasPressed = true;
			}
			if (!keys[GLFW_KEY_F])
				fKeyWasPressed = false;
		}

		// --- Animacion del fuego y giro del Aeolipile (usando tiempo real) ---
		if (fireActive) {
			float elapsed = (float)glfwGetTime() - fireStartWallTime;
			fireScaleAnim = 1.0f + 0.2f * sinf(elapsed * 9.0f);
			fireVibAngle  = 8.0f * sinf(elapsed * 13.0f);
			if (elapsed > 5.0f) {
				aeolipileAngle += 120.0f * (deltaTime / 61.0f);
			}
		}

		// --- Inercia al apagar: desacelera el giro hasta detenerse ---
		if (fireStopping) {
			float stopElapsed = (float)glfwGetTime() - fireStopWallTime;
			float factor = 1.0f - (stopElapsed / SPIN_DECEL_SECS);
			if (factor > 0.0f) {
				aeolipileAngle += 120.0f * (deltaTime / 61.0f) * factor;
			} else {
				fireStopping = false;
			}
		}

		if (movCoche < 30.0f)
		{
			movCoche -= movOffset * deltaTime;
			//printf("avanza%f \n ",movCoche);
			rotllanta += rotllantaOffset * deltaTime;
		}



		//Recibir eventos del usuario
		glfwPollEvents();

		// --- Tecla 7: activar ciclo día/noche ---
		{
			bool* keys = mainWindow.getsKeys();
			static bool key7Prev = false;
			if (keys[GLFW_KEY_7] && !key7Prev) { cycleElapsed = 0.0f; cycleRunning = true; }
			key7Prev = keys[GLFW_KEY_7];
		}
		if (cycleRunning) {
			cycleElapsed += deltaTime;
			if (cycleElapsed >= CYCLE_DURATION) { cycleElapsed = CYCLE_DURATION; cycleRunning = false; }
		}

		// --- Calcular luz solar según el ciclo ---
		float cycleTime = cycleElapsed / CYCLE_DURATION;
		float sunAngle  = cycleTime * 2.0f * 3.14159265f;
		float dayFactor = glm::max(0.0f, cosf(sunAngle));
		float sunDirX   = 0.3f;
		float sunDirY   = -cosf(sunAngle);
		float sunDirZ   =  sinf(sunAngle);

		glm::vec3 dayColor(1.00f, 0.95f, 0.80f);
		glm::vec3 dawnColor(1.00f, 0.45f, 0.10f);
		glm::vec3 nightColor(0.02f, 0.02f, 0.10f);
		glm::vec3 sunColor;
		if      (dayFactor > 0.3f) sunColor = glm::mix(dawnColor, dayColor,  (dayFactor - 0.3f) / 0.7f);
		else if (dayFactor > 0.0f) sunColor = glm::mix(nightColor, dawnColor, dayFactor / 0.3f);
		else                       sunColor = nightColor;

		mainLight = DirectionalLight(
			sunColor.r, sunColor.g, sunColor.b,
			0.05f + dayFactor * 0.35f,
			dayFactor * 0.8f,
			sunDirX, sunDirY, sunDirZ
		);

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la c�mara de tipo flash
		lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);


		//reiniciar color y offset antes de la escena
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		// ============================================================
		//  ESCENA: CORREDOR (PUENTE) FACULTAD DE INGENIERÍA UNAM
		//
		//  Dimensiones:
		//    Largo: ~35m  (Z: -17.5 a +17.5)
		//    Ancho caminable: 5m  (X: -2.5 a +2.5)
		//    Lado pared (servicios): X = -2.5
		//    Lado explanada (barandal): X = +2.5
		//    Piso en Y = 0, techo en Y = 3.5
		//
		//  8 columnas por lado, separadas ~5m entre sí
		//
		//  Barandal (lado explanada, X = +2.5):
		//    - Muro de ~1m de alto
		//    - Ladrillo encima del muro (tira plana)
		//    - Tubo horizontal flotando +0.3m sobre el ladrillo
		//    - Dos tubitos verticales cada 1m conectando ladrillo con tubo
		// ============================================================

		glDisable(GL_BLEND);
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		// Parámetros del corredor
		const float COR_Z0     = -20.0f;  // inicio Z
		const float COR_LEN    =  38.0f;  // longitud: llega hasta Z=+18, justo donde está la cámara
		const float COR_XPARED = -5.0f;   // lado pared (servicios) — pasillo amplio
		const float COR_XBARDA =  5.0f;   // lado explanada (barandal)
		const int   NUM_COLS   = 8;
		const float COL_SEP    =  5.0f;   // separación entre columnas
		const float COL_R      =  0.18f;  // radio columna (un poco más ancho)
		const float COR_YSUELO =  0.0f;
		const float COR_YTECHO =  3.5f;

		// ----- PISO del corredor -----
		{
			color = glm::vec3(0.82f, 0.82f, 0.80f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED, COR_YSUELO - 0.05f, COR_Z0));
			model = glm::scale(model, glm::vec3(COR_XBARDA - COR_XPARED, 0.05f, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			pisoTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();
		}

		// ----- TECHO del corredor -----
		// Capa inferior: blanca (la que se ve desde adentro)
		{
			color = glm::vec3(0.96f, 0.96f, 0.96f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED, COR_YTECHO, COR_Z0));
			model = glm::scale(model, glm::vec3(COR_XBARDA - COR_XPARED, 0.15f, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();
		}
		// Capa superior: color ladrillo (la que va al cielo)
		{
			color = glm::vec3(0.62f, 0.38f, 0.28f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED, COR_YTECHO + 0.15f, COR_Z0));
			model = glm::scale(model, glm::vec3(COR_XBARDA - COR_XPARED, 0.12f, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();
		}

		// ----- BARDA lado servicios (igual que lado explanada) -----
		// Se dibuja en dos pasos usando el mismo bloque de barda abajo

		// ----- 8 COLUMNAS CILÍNDRICAS a cada lado -----
		// Las columnas empiezan en Z0 + 0.5 y se separan COL_SEP
		{
			color = glm::vec3(0.58f, 0.56f, 0.52f); // cantera gris
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

			for (int ci = 0; ci < NUM_COLS; ci++)
			{
				float zc = COR_Z0 + 0.5f + ci * COL_SEP;

				// Lado explanada
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(COR_XBARDA - 0.5f, COR_YSUELO, zc));
				model = glm::scale(model, glm::vec3(COL_R * 2.0f, COR_YTECHO, COL_R * 2.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CILINDRO]->RenderMesh();

				// Lado pared
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(COR_XPARED + 0.5f, COR_YSUELO, zc));
				model = glm::scale(model, glm::vec3(COL_R * 2.0f, COR_YTECHO, COL_R * 2.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CILINDRO]->RenderMesh();
			}
		}

		// -------------------------------------------------------
		//  BARANDAL (lado explanada, X = COR_XBARDA)
		//
		//  Estructura (de abajo hacia arriba):
		//    [0.0 - 1.0]  Muro de concreto/mampostería
		//    [1.0 - 1.12] Ladrillo encima del muro
		//    [1.12 - 1.42] espacio libre (30 cm)
		//    [1.42 - 1.50] Tubo horizontal "flotante"
		//
		//    Cada metro: dos tubitos verticales de dia ~4cm
		//    separados 10 cm entre sí, van de Y=1.12 a Y=1.42
		// -------------------------------------------------------
		{
			const float BARDA_X    = COR_XBARDA;  // cara interior del muro
			const float BARDA_W    = 0.15f;  // grosor del muro
			const float MURO_H     = 1.00f;  // altura del muro
			const float LADR_H     = 0.12f;  // altura del ladrillo
			const float TUBO_FLOT_Y = MURO_H + LADR_H + 0.30f; // Y base tubo flotante
			const float TUBO_D     = 0.05f;  // diámetro tubo (cilindro escala 0.05)
			const float TUBITO_H   = 0.30f;  // altura tubitos de soporte
			const float TUBITO_D   = 0.04f;
			const float TUBITO_SEP = 0.10f;  // separación interna entre los dos tubitos del par

			// Muro base (color concreto crema)
			color = glm::vec3(0.90f, 0.87f, 0.78f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(BARDA_X, COR_YSUELO, COR_Z0));
			model = glm::scale(model, glm::vec3(BARDA_W, MURO_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();

			// Ladrillo encima del muro (ligeramente más oscuro/rojizo)
			color = glm::vec3(0.70f, 0.50f, 0.38f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(BARDA_X - 0.02f, MURO_H, COR_Z0));
			model = glm::scale(model, glm::vec3(BARDA_W + 0.04f, LADR_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();

			// Tubo horizontal flotante (color amarillo metálico)
			color = glm::vec3(1.0f, 0.82f, 0.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

			// Tubo principal (cilindro acostado: escalar mucho en Y, poco en X y Z)
			// Lo hacemos como caja delgada para simplicidad visual
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(BARDA_X + BARDA_W * 0.3f, TUBO_FLOT_Y, COR_Z0));
			model = glm::scale(model, glm::vec3(TUBO_D, TUBO_D, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();

			// Dos tubitos verticales cada 2m a lo largo de TODO el corredor
			const float PASO = 2.0f;
			int numSoportes = (int)(COR_LEN / PASO);
			for (int ti = 0; ti <= numSoportes; ti++)
			{
				float zt = COR_Z0 + ti * PASO;
				float zOff1 = -TUBITO_SEP * 0.5f;
				float zOff2 =  TUBITO_SEP * 0.5f;

				// tubito 1
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(BARDA_X + BARDA_W * 0.3f, MURO_H + LADR_H, zt + zOff1));
				model = glm::scale(model, glm::vec3(TUBITO_D, TUBITO_H, TUBITO_D));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CAJA]->RenderMesh();

				// tubito 2
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(BARDA_X + BARDA_W * 0.3f, MURO_H + LADR_H, zt + zOff2));
				model = glm::scale(model, glm::vec3(TUBITO_D, TUBITO_H, TUBITO_D));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CAJA]->RenderMesh();
			}
		}

		// -------------------------------------------------------
		//  BARANDAL (lado pared/servicios, X = COR_XPARED)
		//
		//  Estructura (de abajo hacia arriba):
		//    [0.0 - 1.0]  Muro de concreto/mampostería
		//    [1.0 - 1.12] Ladrillo encima del muro
		//    [1.12 - 1.42] espacio libre (30 cm)
		//    [1.42 - 1.50] Tubo horizontal "flotante"
		//
		//    Cada metro: dos tubitos verticales de dia ~4cm
		//    separados 10 cm entre sí, van de Y=1.12 a Y=1.42
		// -------------------------------------------------------
		{
			const float BARDA_W    = 0.15f;  // grosor del muro
			const float BARDA_X    = COR_XPARED - BARDA_W;  // cara interior del muro
			const float MURO_H     = 1.00f;  // altura del muro
			const float LADR_H     = 0.12f;  // altura del ladrillo
			const float TUBO_FLOT_Y = MURO_H + LADR_H + 0.30f; // Y base tubo flotante
			const float TUBO_D     = 0.05f;  // diámetro tubo (cilindro escala 0.05)
			const float TUBITO_H   = 0.30f;  // altura tubitos de soporte
			const float TUBITO_D   = 0.04f;
			const float TUBITO_SEP = 0.10f;  // separación interna entre los dos tubitos del par

			// Muro base (color concreto crema)
			color = glm::vec3(0.90f, 0.87f, 0.78f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(BARDA_X, COR_YSUELO, COR_Z0));
			model = glm::scale(model, glm::vec3(BARDA_W, MURO_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();

			// Ladrillo encima del muro (ligeramente más oscuro/rojizo)
			color = glm::vec3(0.70f, 0.50f, 0.38f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(BARDA_X - 0.02f, MURO_H, COR_Z0));
			model = glm::scale(model, glm::vec3(BARDA_W + 0.04f, LADR_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();

			// Tubo horizontal flotante (color amarillo metálico)
			color = glm::vec3(1.0f, 0.82f, 0.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

			// Tubo principal (cilindro acostado: escalar mucho en Y, poco en X y Z)
			// Lo hacemos como caja delgada para simplicidad visual
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(BARDA_X + BARDA_W * 0.3f, TUBO_FLOT_Y, COR_Z0));
			model = glm::scale(model, glm::vec3(TUBO_D, TUBO_D, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();

			// Dos tubitos verticales cada 2m a lo largo de TODO el corredor
			const float PASO = 2.0f;
			int numSoportes = (int)(COR_LEN / PASO);
			for (int ti = 0; ti <= numSoportes; ti++)
			{
				float zt = COR_Z0 + ti * PASO;
				float zOff1 = -TUBITO_SEP * 0.5f;
				float zOff2 =  TUBITO_SEP * 0.5f;

				// tubito 1
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(BARDA_X + BARDA_W * 0.3f, MURO_H + LADR_H, zt + zOff1));
				model = glm::scale(model, glm::vec3(TUBITO_D, TUBITO_H, TUBITO_D));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CAJA]->RenderMesh();

				// tubito 2
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(BARDA_X + BARDA_W * 0.3f, MURO_H + LADR_H, zt + zOff2));
				model = glm::scale(model, glm::vec3(TUBITO_D, TUBITO_H, TUBITO_D));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CAJA]->RenderMesh();
			}
		}

		// ----- VENTANILLAS DE SERVICIOS ESCOLARES (pared X = COR_XPARED) -----
		// 5 ventanillas con marco amarillo y panel oscuro
		for (int vi = 0; vi < 5; vi++)
		{
			float zv = COR_Z0 + 3.0f + vi * 6.5f;

			// Marco amarillo
			color = glm::vec3(1.0f, 0.82f, 0.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED - 0.16f, 0.4f, zv - 0.65f));
			model = glm::scale(model, glm::vec3(0.16f, 2.0f, 1.3f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();

			// Panel interior (gris oscuro)
			color = glm::vec3(0.3f, 0.32f, 0.36f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED - 0.12f, 0.5f, zv - 0.55f));
			model = glm::scale(model, glm::vec3(0.05f, 1.6f, 1.1f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();
		}

		// ----- BORDES DEL TECHO (relieves a lo largo, en ambos lados) -----
		{
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			const float BORDE_W = 0.25f;
			const float BORDE_H = 0.18f;

			// Borde SUPERIOR ladrillo (encima de la capa de ladrillo del techo)
			color = glm::vec3(0.62f, 0.38f, 0.28f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			// Lado explanada
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XBARDA - BORDE_W, COR_YTECHO + 0.27f, COR_Z0));
			model = glm::scale(model, glm::vec3(BORDE_W, BORDE_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();
			// Lado servicios
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED, COR_YTECHO + 0.27f, COR_Z0));
			model = glm::scale(model, glm::vec3(BORDE_W, BORDE_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();

			// Borde SUPERIOR gris (encima del techo, pegado al borde exterior, sube desde la capa de ladrillo)
			{
				const float BORDE_SUP_W = 0.40f;  // grosor en X
				const float BORDE_SUP_H = 0.35f;  // altura en Y (sube sobre el ladrillo)
				// La capa de ladrillo termina en COR_YTECHO + 0.15 + 0.12 = COR_YTECHO + 0.27
				const float BORDE_SUP_Y = COR_YTECHO + 0.27f;
				color = glm::vec3(0.52f, 0.53f, 0.56f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				// Lado explanada
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(COR_XBARDA - BORDE_SUP_W, BORDE_SUP_Y, COR_Z0));
				model = glm::scale(model, glm::vec3(BORDE_SUP_W, BORDE_SUP_H, COR_LEN));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CAJA]->RenderMesh();
				// Lado servicios
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(COR_XPARED, BORDE_SUP_Y, COR_Z0));
				model = glm::scale(model, glm::vec3(BORDE_SUP_W, BORDE_SUP_H, COR_LEN));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CAJA]->RenderMesh();
			}
		}

		// ----- 3 LÁMPARAS COLGANTES en el centro del techo -----
		// Forma cuadrada (MESH_CAJA), muy pegadas al techo, cable corto centrado
		{
			float lampZPos[3] = {
				COR_Z0 + COR_LEN * 0.2f,
				COR_Z0 + COR_LEN * 0.5f,
				COR_Z0 + COR_LEN * 0.8f
			};
			float lampX = (COR_XPARED + COR_XBARDA) * 0.5f; // centro del pasillo en X

			// Dimensiones de la lámpara cuadrada
			const float LAMP_W    = 0.50f;  // lado del cuadrado de la pantalla (X y Z)
			const float LAMP_H    = 0.10f;  // grosor de la pantalla
			const float CABLE_LEN = 0.15f;  // cable muy corto
			const float CABLE_W   = 0.03f;  // grosor del cable
			// La pantalla cuelga del techo: su cara superior está a COR_YTECHO - CABLE_LEN - LAMP_H
			// Su cara inferior a COR_YTECHO - CABLE_LEN - LAMP_H - ... wait, simplificamos:
			// Base superior de la pantalla: COR_YTECHO - CABLE_LEN - LAMP_H
			const float LAMP_Y_TOP = COR_YTECHO - CABLE_LEN - LAMP_H; // Y base inferior del techo → top de pantalla

			for (int li = 0; li < 3; li++)
			{
				float lz = lampZPos[li];

				// Cable: del techo baja hasta la pantalla, centrado en (lampX, lz)
				color = glm::vec3(0.15f, 0.15f, 0.15f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				model = glm::mat4(1.0f);
				// Cable caja: centrado en X y Z → origen = lampX - CABLE_W/2
				model = glm::translate(model, glm::vec3(lampX - CABLE_W * 0.5f, COR_YTECHO - CABLE_LEN, lz - CABLE_W * 0.5f));
				model = glm::scale(model, glm::vec3(CABLE_W, CABLE_LEN, CABLE_W));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				plainTexture.UseTexture();
				Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshList[MESH_CAJA]->RenderMesh();

				// Pantalla cuadrada (caja achatada, blanca/amarillenta)
				color = glm::vec3(1.0f, 1.0f, 0.92f);
				glUniform3fv(uniformColor, 1, glm::value_ptr(color));
				model = glm::mat4(1.0f);
				// Centrada en (lampX, lz): origen = lampX - LAMP_W/2
				model = glm::translate(model, glm::vec3(lampX - LAMP_W * 0.5f, LAMP_Y_TOP, lz - LAMP_W * 0.5f));
				model = glm::scale(model, glm::vec3(LAMP_W, LAMP_H, LAMP_W));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				plainTexture.UseTexture();
				Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
				meshList[MESH_CAJA]->RenderMesh();
			}

			// Luces puntuales de las lámparas:
			// Se apagan durante el día (dayFactor alto) y se encienden de noche (dayFactor bajo).
			// nightFactor = 1 cuando es completamente de noche, 0 cuando es completamente de día.
			float nightFactor = 1.0f - dayFactor;
			float lampAmbient  = 0.6f * nightFactor;
			float lampDiffuse  = 1.8f * nightFactor;

			pointLights[0] = PointLight(1.0f, 1.0f, 0.92f,
				lampAmbient, lampDiffuse,
				lampX, LAMP_Y_TOP - 0.05f, lampZPos[0],
				0.3f, 0.1f, 0.02f);
			pointLights[1] = PointLight(1.0f, 1.0f, 0.92f,
				lampAmbient, lampDiffuse,
				lampX, LAMP_Y_TOP - 0.05f, lampZPos[1],
				0.3f, 0.1f, 0.02f);
			pointLights[2] = PointLight(1.0f, 1.0f, 0.92f,
				lampAmbient, lampDiffuse,
				lampX, LAMP_Y_TOP - 0.05f, lampZPos[2],
				0.3f, 0.1f, 0.02f);
			shaderList[0].SetPointLights(pointLights, 3);
		}

		// ----- SOPORTE METÁLICO INFERIOR (color gris, ~1m alto, a lo largo de cada lado) -----
		// Va debajo del piso del pasillo, como una viga que corre a todo lo largo
		{
			color = glm::vec3(0.45f, 0.46f, 0.48f); // gris metálico
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			const float SOP_H = 1.0f;  // altura del soporte
			const float SOP_W = 0.3f;  // grosor

			// Lado explanada
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XBARDA - SOP_W, COR_YSUELO - SOP_H, COR_Z0));
			model = glm::scale(model, glm::vec3(SOP_W, SOP_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();

			// Lado pared/servicios
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED, COR_YSUELO - SOP_H, COR_Z0));
			model = glm::scale(model, glm::vec3(SOP_W, SOP_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();
		}

		// ----- ARCO (media C) debajo del pasillo -----
		// Un solo arco por lado, abarca todo el largo del corredor.
		// Forma: la cima del arco (punto medio de la C) queda en Y=0 pegada al soporte gris,
		// y los dos extremos bajan hasta Y = -ARCO_PROF (hacia el suelo externo).
		// El arco es poco curvado (radio muy grande), grueso, y de ~3m de altura total.
		//
		// Parametrización con ángulo reducido para poca curvatura:
		//   Usamos solo un arco parcial (no semicírculo completo), de -HALF_ANG a +HALF_ANG,
		//   donde HALF_ANG < PI/2, para que sea abierto y poco curvado.
		//   Centro geométrico del círculo en Y = +R (encima del suelo),
		//   de modo que en theta=PI/2 el punto está en Y=0 (la cima), y
		//   en theta = PI/2 - HALF_ANG los extremos están en Y = -ARCO_PROF.
		{
			color = glm::vec3(0.40f, 0.41f, 0.43f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

			const float PI         = 3.14159265f;
			const int   ARCO_SEGS  = 20;          // muchos segmentos para curva suave
			const float SEG_R      = 0.18f;        // grosor del tubo del arco
			const float ARCO_PROF  = 3.0f;         // profundidad total del arco (altura de la C)
			// El corredor mide COR_LEN en Z; los extremos del arco están en Z0 y Z0+LEN
			// La mitad del ancho horizontal del arco = COR_LEN/2
			const float ARCO_HALF_Z = COR_LEN * 0.5f;

			// Radio del círculo del que es parte el arco:
			// En el punto cima (Z = centro, Y = 0): theta_cima = PI/2
			// En los extremos (Z = ±ARCO_HALF_Z, Y = -ARCO_PROF): theta_ext
			// R * cos(theta_ext) = ARCO_HALF_Z  → cos(theta_ext) = ARCO_HALF_Z / R
			// R - R*sin(theta_ext) = ARCO_PROF   → R*(1 - sin(theta_ext)) = ARCO_PROF
			// Tomamos R directamente grande para poca curvatura:
			// R = (ARCO_HALF_Z^2 + ARCO_PROF^2) / (2 * ARCO_PROF)  (fórmula de arco)
			const float ARCO_R = (ARCO_HALF_Z * ARCO_HALF_Z + ARCO_PROF * ARCO_PROF) / (2.0f * ARCO_PROF);

			// Ángulo en los extremos: sin(theta_ext) = 1 - ARCO_PROF/R
			float sinExt     = 1.0f - ARCO_PROF / ARCO_R;
			float thetaExt   = asinf(glm::clamp(sinExt, -1.0f, 1.0f));  // < PI/2
			float thetaCima  = PI * 0.5f;  // en la cima Y=0

			// Barremos de thetaExt a PI-thetaExt (de extremo izq a extremo der)
			float angStart = thetaExt;
			float angEnd   = PI - thetaExt;
			float angRange = angEnd - angStart;
			float ARCO_DANG = angRange / ARCO_SEGS;

			// Centro Z del arco: mitad del corredor
			const float ARCO_ZC = COR_Z0 + COR_LEN * 0.5f;
			// Centro Y del círculo: queda en Y = +ARCO_R (el círculo tiene su centro arriba)
			// Para que en theta=PI/2 (cima), Y = centro_Y - R*sin(PI/2) ... usamos otra parametrización:
			// Punto en el arco: z = ARCO_ZC - ARCO_R*cos(theta), y = COR_YSUELO - (ARCO_R - ARCO_R*sin(theta))
			//                                                       = COR_YSUELO - ARCO_R*(1 - sin(theta))
			// En theta=PI/2: y = COR_YSUELO - ARCO_R*(1-1) = COR_YSUELO = 0  ✓ (cima toca suelo)
			// En theta=thetaExt: y = COR_YSUELO - ARCO_R*(1 - sinExt) = -ARCO_PROF  ✓

			const float SOP_W_ARC = 0.3f;
			float arcXPos[2] = {
				COR_XBARDA - SOP_W_ARC * 0.5f,
				COR_XPARED + SOP_W_ARC * 0.5f
			};

			for (int side = 0; side < 2; side++)
			{
				float cx = arcXPos[side];

				for (int si2 = 0; si2 < ARCO_SEGS; si2++)
				{
					float angA = angStart + si2       * ARCO_DANG;
					float angB = angStart + (si2 + 1) * ARCO_DANG;

					glm::vec3 pA(cx,
						         COR_YSUELO - ARCO_R * (1.0f - sinf(angA)),
						         ARCO_ZC    - ARCO_R * cosf(angA));
					glm::vec3 pB(cx,
						         COR_YSUELO - ARCO_R * (1.0f - sinf(angB)),
						         ARCO_ZC    - ARCO_R * cosf(angB));

					glm::vec3 dir = pB - pA;
					float segLen  = glm::length(dir);
					if (segLen < 0.0001f) continue;
					glm::vec3 dirN = dir / segLen;

					glm::vec3 yAxis(0.0f, 1.0f, 0.0f);
					glm::vec3 rotAxis    = glm::cross(yAxis, dirN);
					float     rotAxisLen = glm::length(rotAxis);
					float     cosAngle   = glm::dot(yAxis, dirN);
					float     rotAngle   = acosf(glm::clamp(cosAngle, -1.0f, 1.0f));

					model = glm::mat4(1.0f);
					model = glm::translate(model, pA);
					if (rotAxisLen > 0.0001f)
						model = glm::rotate(model, rotAngle, rotAxis / rotAxisLen);
					model = glm::scale(model, glm::vec3(SEG_R, segLen, SEG_R));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					meshList[MESH_CILINDRO]->RenderMesh();
				}
			}
		}

		// ----- MURAL GEOMÉTRICO en pared de fondo (Z = COR_Z0) -----
		{
			// Panel azul
			color = glm::vec3(0.12f, 0.28f, 0.72f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED, 0.3f, COR_Z0 - 0.06f));
			model = glm::scale(model, glm::vec3(2.5f, 2.8f, 0.06f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();

			// Panel verde
			color = glm::vec3(0.08f, 0.58f, 0.22f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED + 1.8f, 0.5f, COR_Z0 - 0.06f));
			model = glm::scale(model, glm::vec3(2.0f, 2.4f, 0.06f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();

			// Panel naranja
			color = glm::vec3(0.92f, 0.42f, 0.04f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED + 3.2f, 0.8f, COR_Z0 - 0.06f));
			model = glm::scale(model, glm::vec3(1.8f, 2.0f, 0.06f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();
		}

		// ----- STANDS DE FERIA (3 puestos en el corredor) -----
		// Orientación: espalda a la barda de servicios (COR_XPARED).
		// Mesa con lado largo a lo largo del pasillo (eje Z).
		// Centro del stand en (xs_centro, zs).
		// MakeBox va de [0,1]: para centrar en xs_centro trasladamos xs_centro - mitad_ancho
		for (int si = 0; si < 3; si++)
		{
			float zs       = COR_Z0 + 8.0f + si * 12.0f; // posición Z central del stand
			float mesaLargo = 1.8f;   // largo de la mesa (a lo largo del pasillo, eje Z)
			float mesaAncho = 0.7f;   // ancho de la mesa (hacia el interior del pasillo, eje X)
			float mesaAlto  = 0.07f;  // grosor del tablero
			float mesaH     = 0.75f;  // altura del tablero
			// Centro X de la mesa: pegada a la barda, separada un poco
			float xs_centro = COR_XPARED + 0.15f + mesaAncho * 0.5f;

			// Tablero: centrado en (xs_centro, zs)
			color = glm::vec3(0.78f, 0.72f, 0.58f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(xs_centro - mesaAncho * 0.5f, mesaH, zs - mesaLargo * 0.5f));
			model = glm::scale(model, glm::vec3(mesaAncho, mesaAlto, mesaLargo));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_CAJA]->RenderMesh();

			// Patas (4 cilindros): simétricas respecto al centro de la mesa
			color = glm::vec3(0.48f, 0.48f, 0.50f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			float pxOff[4] = { -mesaAncho*0.5f + 0.05f,  mesaAncho*0.5f - 0.05f,
			                   -mesaAncho*0.5f + 0.05f,  mesaAncho*0.5f - 0.05f };
			float pzOff[4] = { -mesaLargo*0.5f + 0.05f, -mesaLargo*0.5f + 0.05f,
			                    mesaLargo*0.5f - 0.05f,  mesaLargo*0.5f - 0.05f };
			for (int p = 0; p < 4; p++) {
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(xs_centro + pxOff[p] - 0.035f, 0.0f, zs + pzOff[p] - 0.035f));
				model = glm::scale(model, glm::vec3(0.07f, mesaH, 0.07f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CILINDRO]->RenderMesh();
			}

			// Panel/lona vertical: pegado a la barda (en X), centrado en Z sobre la mesa
			color = glm::vec3(0.08f + si * 0.35f, 0.28f, 0.82f - si * 0.25f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED + 0.12f, 0.0f, zs - mesaLargo * 0.5f));
			model = glm::scale(model, glm::vec3(0.04f, 2.2f, mesaLargo));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();
		}

		// Restaurar color blanco
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));

		glUseProgram(0);
		mainWindow.swapBuffers();
	}

	return 0;
}
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               