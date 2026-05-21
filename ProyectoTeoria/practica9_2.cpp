/*
Proyecto teoría - CGIHM (Pasillo Principal)
-Carolina Mota García - 316255314
*/


//para cargar imagenes.
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

// ─── miniaudio: audio ────────────────────────────────────────────────
// miniaudio.h se incluye aquí solo como declaraciones.

#include "miniaudio.h"

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

// ─── Escenario: expo booths ───────────────────────────────────────
Model ExpoBooks_M;
Model ExpoBooths_M;

// ─── ChicaUnam: modelos ───────────────────────────────────────────
Model ChicaUnam_Cuerpo_M;
Model ChicaUnam_MusloDer_M;
Model ChicaUnam_MusloIzq_M;
Model ChicaUnam_PiernaDer_M;
Model ChicaUnam_PiernaIzq_M;

// ─── Profesor vendedor: modelos ──────────────────────────────────────
Model Prof_Torso_M, Prof_Cabeza_M;
Model Prof_BrazoDer_M, Prof_BrazoIzq_M;
Model Prof_ManoDer_M,  Prof_ManoIzq_M;
Model Prof_PiernaDer_M, Prof_PiernaIzq_M;
Model Prof_PieDer_M,   Prof_PieIzq_M;

// ─── Estudiante1: modelos ─────────────────────────────────────────────
Model Est1_Torso_M,   Est1_Cabeza_M;
Model Est1_BrazoDer_M, Est1_BrazoIzq_M;
Model Est1_ManoDer_M,  Est1_ManoIzq_M;
Model Est1_PiernaDer_M, Est1_PiernaIzq_M;
Model Est1_ZapatoDer_M, Est1_ZapatoIzq_M;

// ─── PerroRobot: modelos ─────────────────────────────────────────────
Model PR_Cuerpo_M;
Model PR_BrazoDer_M,  PR_AntebrazoDer_M;
Model PR_BrazoIzq_M,  PR_AntebrazoIzq_M;
Model PR_MusloDer_M,  PR_PieDer_M;
Model PR_MusloIzq_M,  PR_PieIzq_M;

// ─── Dron: modelos ──────────────────────────────────────────────────
Model Dron_Cuerpo_M;
Model Dron_Helice_M;

// ─── Offroad: modelos ────────────────────────────────────────────────
Model Offroad_Auto_M;
Model Offroad_LlantaSupDer_M;
Model Offroad_LlantaSupIzq_M;
Model Offroad_LlantaInfDer_M;
Model Offroad_LlantaInfIzq_M;

// ─── Offroad: animación (adelante/atrás sobre la mesa) ───────────────
float offroad_posZ    = 0.0f;   // desplazamiento local Z relativo al centro de la mesa
float offroad_dir     = 1.0f;   // +1 avanza, -1 retrocede
float offroad_speed   = 0.3f;   // unidades/seg (más lento)
float offroad_llanta  = 0.0f;   // ángulo acumulado de rotación de llantas (grados)
const float OFFROAD_RANGE = 0.4f; // rango de movimiento a cada lado del centro

// ─── Paloma: modelos ─────────────────────────────────────────────────
Model Paloma_Cuerpo_M;
Model Paloma_AlaDer_M;
Model Paloma_AlaIzq_M;

// ─── Paloma: animación (vuelo keyframes + aleteo) ────────────────────
float paloma_x   = -3.0f;
float paloma_y   =  5.0f;
float paloma_z   = 10.0f;
float paloma_yaw =  0.0f;
float paloma_aleteo = 0.0f;  // acumulador de tiempo para sinf del ala

// ─── Orrery: modelos ─────────────────────────────────────────────────
Model Orrery_Nucleo_M;
Model Orrery_Planeta_M[8];


// ─── Personaje procedural: estado de animación ───────────────────────────────
// ─── ChicaUnam: animación procedural de caminar ──────────────────
// Técnica: sinf(walkCycle) igual que ProyectoLab — suave y sin keyframes
// Tecla C: toggle caminar on/off
bool  chicaWalking    = true;  // arranca caminando por defecto
bool  cKeyWasPressed  = false;
float chica_walkCycle = 0.0f;  // acumula tiempo para la animacion
// Posicion global de la chica en el pasillo
float chica_posZ = 16.0f;
float chica_posX = 0.0f; // centrada en el pasillo
float chica_dir  = 1.0f; // +1 = avanza hacia -Z, -1 = avanza hacia +Z

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture pisoTexture;
Texture piso2Texture;
Texture AgaveTexture;
Texture FlechaTexture;
Texture FuegoTexture;
Texture NumerosTexture;
Texture Numero1Texture;
Texture Numero2Texture;

// Flyers: 2 por booth × 5 booths × 2 lados = 20 flyers únicos (flyer1.png…flyer20.png)
// Índice: bi*4 + side*2 + slot  (bi=0-4, side=0pared/1barda, slot=0trasero/1lateral)
Texture FlyerTextures[20];
Texture plainTexture;



Model Kitt_M;
Model Llanta_M;
Model Pista_M;
Model Nave_M;
Model Ala_M;
Model Aeolipile_base_M;
Model Aeolipile_M;

Skybox skybox;      // skybox de día
Skybox skyboxNight; // skybox de noche

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// Ciclo día/noche
static const float CYCLE_DURATION = 60.0f; // duración total en segundos (1 minuto)
float cycleElapsed = 0.0f; // arranca en mediodía (cosf(0) = 1 = dayFactor máximo)
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

	// Solo cuerpo lateral (sin tapas → no se ven los círculos en pilares)
	int sideBase = 0;
	for (int i = 0; i < sectors; i++) {
		float a0 = 2.0f * PI * i / sectors;
		float a1 = 2.0f * PI * (i + 1) / sectors;
		float x0 = cosf(a0), z0 = sinf(a0);
		float x1 = cosf(a1), z1 = sinf(a1);
		float u0 = (float)i / sectors, u1 = (float)(i + 1) / sectors;
		// 4 verts del quad: BL, BR, TR, TL — normales outward (hacia afuera)
		// Winding CCW visto desde afuera: BL→TR→BR y BL→TL→TR
		verts.insert(verts.end(), { x0, 0.0f, z0,  u0, 0.0f,  x0, 0.0f, z0 });
		verts.insert(verts.end(), { x1, 0.0f, z1,  u1, 0.0f,  x1, 0.0f, z1 });
		verts.insert(verts.end(), { x1, 1.0f, z1,  u1, 1.0f,  x1, 0.0f, z1 });
		verts.insert(verts.end(), { x0, 1.0f, z0,  u0, 1.0f,  x0, 0.0f, z0 });
		unsigned b = sideBase + i * 4;
		// Invertir winding para que las normales apunten hacia afuera
		inds.insert(inds.end(), { b, b+2, b+1,  b, b+3, b+2 });
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



// Esfera suavizada (para la cabeza)

// Torso: caja suavizada con hombros redondeados (cápsula achatada en X)

// Índices globales del personaje procedural (asignados en CreateObjects)
int MESH_PERS_PIE      = -1; // cápsula achatada

// ─── PerroRobot: animación por keyframes (backflip en loop) ─────────
// Variables interpoladas que el render lee cada frame
float pr_walkCycle = 0.0f;   // ciclo de trote para las patas (sigue activo)
bool  pr_walking   = true;

// Variables del backflip interpoladas
float pr_bodyRotX  = 0.0f;   // rotación X del cuerpo (0→360 en el flip)
float pr_bodyY     = 0.0f;   // offset Y extra del cuerpo (salto)
float pr_legFlex   = 0.0f;   // flexión extra de las 4 patas (impulso/aterrizaje)

// ── Keyframes del backflip ────────────────────────────────────────────
// Secuencia: reposo (pausa) → carga profunda → despegue → vuelo
//            → aterrizaje con flex → endereza → reposo (loop)
//
// TRUCO LOOP LIMPIO: el flip completa 360° pero el render usa fmod(rotX,360).
// Nunca interpolamos de 360 a 0 — reseteamos la variable antes del loop.
//
// bodyRotX: rotación X del cuerpo (grados, acumulado durante el flip)
// bodyY:    offset vertical extra
// legFlex:  flexión extra de las 4 patas
#define PR_MAX_FRAMES 9
struct PRFrame {
    float bodyRotX, bodyY, legFlex;
    float bodyRotXInc, bodyYInc, legFlexInc;
};
PRFrame pr_kf[PR_MAX_FRAMES] = {
//   rotX      Y      flex
    {  0.0f,  0.00f,  0.0f, 0,0,0 }, // KF0  reposo
    {  0.0f,  0.00f,  0.0f, 0,0,0 }, // KF1  pausa en reposo (hold)
    {-28.0f, -0.05f, 42.0f, 0,0,0 }, // KF2  carga: agache pronunciado
    { 55.0f,  0.20f,  0.0f, 0,0,0 }, // KF3  despegue explosivo
    {180.0f,  0.40f,  0.0f, 0,0,0 }, // KF4  mitad del flip
    {320.0f,  0.18f,  0.0f, 0,0,0 }, // KF5  casi completo, bajando
    {360.0f,  0.00f, 40.0f, 0,0,0 }, // KF6  aterrizaje, absorción
    {360.0f,  0.00f, 40.0f, 0,0,0 }, // KF7  hold aterrizaje (pausa dramática)
    {360.0f,  0.00f,  0.0f, 0,0,0 }, // KF8  endereza patas → loop (rotX se resetea al llegar aquí)
};
int pr_stepsPerSeg[PR_MAX_FRAMES - 1] = {
    15,  // KF0→1  hold reposo (corto, es la misma pose)
    55,  // KF1→2  carga lenta y dramática
    18,  // KF2→3  despegue rápido
    50,  // KF3→4  vuelo primera mitad
    45,  // KF4→5  vuelo segunda mitad
    20,  // KF5→6  aterrizaje
    25,  // KF6→7  hold aterrizaje
    35,  // KF7→8  endereza patas
};
int  pr_playIndex = 0;
int  pr_currSteps = 0;
bool pr_play      = true;

void pr_interpolate() {
    int ki    = pr_playIndex;
    int steps = pr_stepsPerSeg[ki];
    pr_kf[ki].bodyRotXInc = (pr_kf[ki+1].bodyRotX - pr_kf[ki].bodyRotX) / steps;
    pr_kf[ki].bodyYInc    = (pr_kf[ki+1].bodyY    - pr_kf[ki].bodyY)    / steps;
    pr_kf[ki].legFlexInc  = (pr_kf[ki+1].legFlex  - pr_kf[ki].legFlex)  / steps;
}

void pr_animate(float /*dt*/) {
    if (!pr_play) return;
    if (pr_currSteps >= pr_stepsPerSeg[pr_playIndex]) {
        pr_playIndex++;
        if (pr_playIndex >= PR_MAX_FRAMES - 1) {
            // Loop limpio: forzar variables a KF0 exacto antes de reiniciar.
            // bodyRotX se resetea a 0 aquí — nunca se interpola de 360 a 0.
            pr_playIndex = 0;
            pr_bodyRotX  = 0.0f;
            pr_bodyY     = 0.0f;
            pr_legFlex   = 0.0f;
        }
        pr_currSteps = 0;
        pr_interpolate();
    } else {
        pr_bodyRotX += pr_kf[pr_playIndex].bodyRotXInc;
        pr_bodyY    += pr_kf[pr_playIndex].bodyYInc;
        pr_legFlex  += pr_kf[pr_playIndex].legFlexInc;
        pr_currSteps++;
    }
}

// ─── Profesor vendedor: keyframes ────────────────────────────────────
// Variables interpoladas leídas en el render:
//   prof_yaw       : giro del torso en Y (hacia el pasillo)
//   prof_brazoDerX : rotación X brazo derecho (sube/baja para "pasele")
//   prof_brazoDerZ : rotación Z brazo derecho (abre hacia afuera)
//   prof_brazoIzqZ : brazo izquierdo va a la cadera (Z)
//   prof_cabezaY   : giro cabeza hacia el pasillo
float prof_yaw       = 0.0f;
float prof_brazoDerX = 0.0f;
float prof_brazoDerZ = 0.0f;
float prof_brazoIzqZ = 0.0f;
float prof_cabezaY   = 0.0f;

#define PROF_MAX_KF 10
struct ProfFrame {
    float yaw, brazoDerX, brazoDerZ, brazoIzqZ, cabezaY;
    float yawInc, brazoDerXInc, brazoDerZInc, brazoIzqZInc, cabezaYInc;
};
// Secuencia "pasele pasele":
//  pose neutral → gira hacia pasillo con brazo der arriba invitando
//  → baja brazo → otra vez sube con más énfasis → baja
//  → mano izq a cadera y brazo der extendido lateral → reposo (loop)
ProfFrame prof_kf[PROF_MAX_KF] = {
//  yaw   brazDerX brazDerZ brazoIzqZ cabezaY
    {  0,     0,      0,       0,        0,    0,0,0,0,0 }, // KF0 neutral
    {  0,     0,      0,       0,        0,    0,0,0,0,0 }, // KF1 hold neutral
    { 25,   -60,    -20,       0,       20,    0,0,0,0,0 }, // KF2 gira + brazo der sube invitando
    { 25,   -60,    -20,       0,       20,    0,0,0,0,0 }, // KF3 hold "pasele"
    { 25,   -20,    -10,       0,       20,    0,0,0,0,0 }, // KF4 brazo baja un poco
    { 25,   -65,    -25,       0,       20,    0,0,0,0,0 }, // KF5 sube de nuevo con énfasis
    { 25,   -65,    -25,       0,       20,    0,0,0,0,0 }, // KF6 hold
    { 15,   -30,    -35,       0,       15,    0,0,0,0,0 }, // KF7 der lateral
    { 15,   -30,    -35,       0,       15,    0,0,0,0,0 }, // KF8 hold lateral
    {  0,     0,      0,       0,        0,    0,0,0,0,0 }, // KF9 vuelve neutral = KF0 (loop)
};
int prof_stepsPerSeg[PROF_MAX_KF - 1] = {
    20,   // KF0→1 hold
    50,   // KF1→2 gira y sube brazo
    35,   // KF2→3 hold pasele
    30,   // KF3→4 baja brazo
    25,   // KF4→5 sube con énfasis
    30,   // KF5→6 hold
    40,   // KF6→7 transición lateral
    35,   // KF7→8 hold lateral
    55,   // KF8→9 vuelve neutral
};
int  prof_playIndex = 0;
int  prof_currSteps = 0;

void prof_interpolate() {
    int ki = prof_playIndex;
    int st = prof_stepsPerSeg[ki];
    prof_kf[ki].yawInc       = (prof_kf[ki+1].yaw       - prof_kf[ki].yaw)       / st;
    prof_kf[ki].brazoDerXInc = (prof_kf[ki+1].brazoDerX - prof_kf[ki].brazoDerX) / st;
    prof_kf[ki].brazoDerZInc = (prof_kf[ki+1].brazoDerZ - prof_kf[ki].brazoDerZ) / st;
    prof_kf[ki].brazoIzqZInc = (prof_kf[ki+1].brazoIzqZ - prof_kf[ki].brazoIzqZ) / st;
    prof_kf[ki].cabezaYInc   = (prof_kf[ki+1].cabezaY   - prof_kf[ki].cabezaY)   / st;
}

void prof_animate() {
    if (prof_currSteps >= prof_stepsPerSeg[prof_playIndex]) {
        prof_playIndex++;
        if (prof_playIndex >= PROF_MAX_KF - 1) {
            prof_playIndex = 0;
            prof_yaw = prof_brazoDerX = prof_brazoDerZ = prof_brazoIzqZ = prof_cabezaY = 0.0f;
        }
        prof_currSteps = 0;
        prof_interpolate();
    } else {
        prof_yaw       += prof_kf[prof_playIndex].yawInc;
        prof_brazoDerX += prof_kf[prof_playIndex].brazoDerXInc;
        prof_brazoDerZ += prof_kf[prof_playIndex].brazoDerZInc;
        prof_brazoIzqZ += prof_kf[prof_playIndex].brazoIzqZInc;
        prof_cabezaY   += prof_kf[prof_playIndex].cabezaYInc;
        prof_currSteps++;
    }
}

// ─── Estudiante1: keyframes (animación: escribiendo en laptop dentro de booth) ─
// Variables interpoladas: brazo der/izq en X (bajan/suben para escribir),
//   cabeza inclina en X (baja a ver pantalla), cuerpo leve balanceo Z
float est1_brazoDerX = 0.0f;
float est1_brazoIzqX = 0.0f;
float est1_cabezaX   = 0.0f; // inclinación de la cabeza (baja a ver)
float est1_cuerpoZ   = 0.0f; // leve giro torso

#define EST1_MAX_KF 8
struct Est1Frame {
    float brazoDerX, brazoIzqX, cabezaX, cuerpoZ;
    float brazoDerXInc, brazoIzqXInc, cabezaXInc, cuerpoZInc;
};
// Animación "tecleando en laptop":
//  brazos bajan hacia frente (X positivo = hacia abajo/frente en local),
//  cabeza inclina levemente hacia abajo, torso leve inclinación
Est1Frame est1_kf[EST1_MAX_KF] = {
//  brazDerX  brazIzqX  cabezaX  cuerpoZ
    {  20.0f,  20.0f,  15.0f,  0.0f,  0,0,0,0 }, // KF0 postura base (brazos semiflexionados, mira pantalla)
    {  30.0f,  15.0f,  15.0f,  3.0f,  0,0,0,0 }, // KF1 brazo der teclca
    {  20.0f,  25.0f,  15.0f, -3.0f,  0,0,0,0 }, // KF2 brazo izq teclea
    {  32.0f,  18.0f,  18.0f,  2.0f,  0,0,0,0 }, // KF3 der teclea rápido
    {  18.0f,  28.0f,  14.0f, -2.0f,  0,0,0,0 }, // KF4 izq teclea
    {  25.0f,  22.0f,  20.0f,  0.0f,  0,0,0,0 }, // KF5 pausa, inclina cabeza a leer
    {  22.0f,  22.0f,  16.0f,  4.0f,  0,0,0,0 }, // KF6 leve giro como si leyera
    {  20.0f,  20.0f,  15.0f,  0.0f,  0,0,0,0 }, // KF7 = KF0 loop
};
int est1_stepsPerSeg[EST1_MAX_KF - 1] = {
    20,  // KF0→1
    25,  // KF1→2
    20,  // KF2→3
    22,  // KF3→4
    25,  // KF4→5
    40,  // KF5→6 pausa lectura
    35,  // KF6→7 vuelve
};
int est1_playIndex = 0;
int est1_currSteps = 0;

void est1_interpolate() {
    int ki = est1_playIndex;
    int st = est1_stepsPerSeg[ki];
    est1_kf[ki].brazoDerXInc = (est1_kf[ki+1].brazoDerX - est1_kf[ki].brazoDerX) / st;
    est1_kf[ki].brazoIzqXInc = (est1_kf[ki+1].brazoIzqX - est1_kf[ki].brazoIzqX) / st;
    est1_kf[ki].cabezaXInc   = (est1_kf[ki+1].cabezaX   - est1_kf[ki].cabezaX)   / st;
    est1_kf[ki].cuerpoZInc   = (est1_kf[ki+1].cuerpoZ   - est1_kf[ki].cuerpoZ)   / st;
}

void est1_animate() {
    if (est1_currSteps >= est1_stepsPerSeg[est1_playIndex]) {
        est1_playIndex++;
        if (est1_playIndex >= EST1_MAX_KF - 1) {
            est1_playIndex = 0;
            est1_brazoDerX = 20.0f;
            est1_brazoIzqX = 20.0f;
            est1_cabezaX   = 15.0f;
            est1_cuerpoZ   = 0.0f;
        }
        est1_currSteps = 0;
        est1_interpolate();
    } else {
        est1_brazoDerX += est1_kf[est1_playIndex].brazoDerXInc;
        est1_brazoIzqX += est1_kf[est1_playIndex].brazoIzqXInc;
        est1_cabezaX   += est1_kf[est1_playIndex].cabezaXInc;
        est1_cuerpoZ   += est1_kf[est1_playIndex].cuerpoZInc;
        est1_currSteps++;
    }
}

// ─── Estudiante ANIM2: saludando (levanta brazo der arriba y lo baja) ──────────
float est2_brazoDerX = 0.0f;
float est2_brazoIzqX = 0.0f;
float est2_cabezaX   = 0.0f;

#define EST2_MAX_KF 6
struct Est2Frame {
    float brazoDerX, brazoIzqX, cabezaX;
    float brazoDerXInc, brazoIzqXInc, cabezaXInc;
};
Est2Frame est2_kf[EST2_MAX_KF] = {
//  brazDerX  brazIzqX  cabezaX
    {   0.0f,   10.0f,   0.0f,  0,0,0 }, // KF0 reposo
    { -90.0f,   10.0f, -10.0f,  0,0,0 }, // KF1 brazo der sube (saludo)
    {-120.0f,   10.0f, -15.0f,  0,0,0 }, // KF2 bien arriba
    { -90.0f,   10.0f, -10.0f,  0,0,0 }, // KF3 baja a medio
    {-120.0f,   10.0f, -15.0f,  0,0,0 }, // KF4 sube otra vez (doble saludo)
    {   0.0f,   10.0f,   0.0f,  0,0,0 }, // KF5 = KF0 loop
};
int est2_stepsPerSeg[EST2_MAX_KF - 1] = { 35, 20, 25, 20, 50 };
int est2_playIndex = 0, est2_currSteps = 0;

void est2_interpolate() {
    int ki = est2_playIndex, st = est2_stepsPerSeg[ki];
    est2_kf[ki].brazoDerXInc = (est2_kf[ki+1].brazoDerX - est2_kf[ki].brazoDerX) / st;
    est2_kf[ki].brazoIzqXInc = (est2_kf[ki+1].brazoIzqX - est2_kf[ki].brazoIzqX) / st;
    est2_kf[ki].cabezaXInc   = (est2_kf[ki+1].cabezaX   - est2_kf[ki].cabezaX)   / st;
}
void est2_animate() {
    if (est2_currSteps >= est2_stepsPerSeg[est2_playIndex]) {
        est2_playIndex++;
        if (est2_playIndex >= EST2_MAX_KF - 1) {
            est2_playIndex = 0;
            est2_brazoDerX = 0.0f; est2_brazoIzqX = 10.0f; est2_cabezaX = 0.0f;
        }
        est2_currSteps = 0; est2_interpolate();
    } else {
        est2_brazoDerX += est2_kf[est2_playIndex].brazoDerXInc;
        est2_brazoIzqX += est2_kf[est2_playIndex].brazoIzqXInc;
        est2_cabezaX   += est2_kf[est2_playIndex].cabezaXInc;
        est2_currSteps++;
    }
}

// ─── Estudiante ANIM3: aplaudiendo (ambos brazos suben y se juntan) ────────────
float est3_brazoDerX = 0.0f;
float est3_brazoIzqX = 0.0f;
float est3_brazoDerZ = 0.0f; // rotación Z: acercar brazo al centro (aplaudir)
float est3_brazoIzqZ = 0.0f;
float est3_cabezaX   = 0.0f;

// Aplaudir: brazos rotan en Z hacia el centro + leve X hacia frente
// brazDerZ > 0 = brazo der se acerca hacia centro izq
// brazIzqZ < 0 = brazo izq se acerca hacia centro der
#define EST3_MAX_KF 5
struct Est3Frame {
    float brazoDerX, brazoIzqX, brazoDerZ, brazoIzqZ, cabezaX;
    float brazoDerXInc, brazoIzqXInc, brazoDerZInc, brazoIzqZInc, cabezaXInc;
};
Est3Frame est3_kf[EST3_MAX_KF] = {
//  brazDerX  brazIzqX  brazDerZ  brazIzqZ  cabezaX
//  X negativo = brazo va hacia ADELANTE del modelo
    { -50.0f, -50.0f,    0.0f,    0.0f,   -5.0f,  0,0,0,0,0 }, // KF0 brazos al frente separados
    { -55.0f, -55.0f,   50.0f,  -50.0f,  -8.0f,  0,0,0,0,0 }, // KF1 manos se juntan (aplauso)
    { -50.0f, -50.0f,    0.0f,    0.0f,   -5.0f,  0,0,0,0,0 }, // KF2 manos se abren
    { -55.0f, -55.0f,   50.0f,  -50.0f,  -8.0f,  0,0,0,0,0 }, // KF3 juntan de nuevo
    { -50.0f, -50.0f,    0.0f,    0.0f,   -5.0f,  0,0,0,0,0 }, // KF4 = KF0 loop
};
int est3_stepsPerSeg[EST3_MAX_KF - 1] = { 14, 10, 14, 10 };
int est3_playIndex = 0, est3_currSteps = 0;

void est3_interpolate() {
    int ki = est3_playIndex, st = est3_stepsPerSeg[ki];
    est3_kf[ki].brazoDerXInc  = (est3_kf[ki+1].brazoDerX  - est3_kf[ki].brazoDerX)  / st;
    est3_kf[ki].brazoIzqXInc  = (est3_kf[ki+1].brazoIzqX  - est3_kf[ki].brazoIzqX)  / st;
    est3_kf[ki].brazoDerZInc  = (est3_kf[ki+1].brazoDerZ  - est3_kf[ki].brazoDerZ)  / st;
    est3_kf[ki].brazoIzqZInc  = (est3_kf[ki+1].brazoIzqZ  - est3_kf[ki].brazoIzqZ)  / st;
    est3_kf[ki].cabezaXInc    = (est3_kf[ki+1].cabezaX    - est3_kf[ki].cabezaX)    / st;
}
void est3_animate() {
    if (est3_currSteps >= est3_stepsPerSeg[est3_playIndex]) {
        est3_playIndex++;
        if (est3_playIndex >= EST3_MAX_KF - 1) {
            est3_playIndex = 0;
            est3_brazoDerX = -50.0f; est3_brazoIzqX = -50.0f;
            est3_brazoDerZ =   0.0f; est3_brazoIzqZ =   0.0f;
            est3_cabezaX   =  -5.0f;
        }
        est3_currSteps = 0; est3_interpolate();
    } else {
        est3_brazoDerX += est3_kf[est3_playIndex].brazoDerXInc;
        est3_brazoIzqX += est3_kf[est3_playIndex].brazoIzqXInc;
        est3_brazoDerZ += est3_kf[est3_playIndex].brazoDerZInc;
        est3_brazoIzqZ += est3_kf[est3_playIndex].brazoIzqZInc;
        est3_cabezaX   += est3_kf[est3_playIndex].cabezaXInc;
        est3_currSteps++;
    }
}

// ─── Dron: animación keyframes (órbita elíptica alrededor del corredor) ────
// El corredor va de Z=-20 a Z=+18, X=-5 a +5.  El dron orbita por fuera y arriba.
// Variables interpoladas leídas en el render:
float dron_x   =  0.0f;
float dron_y   =  8.0f;
float dron_z   = 24.0f;
float dron_yaw =  0.0f;   // giro en Y (acumula 0→360, se resetea al loopear)
float dron_helixAngle = 0.0f; // ángulo de giro de las hélices (acumulado)

#define DRON_MAX_KF 9
struct DronKF {
    float x, y, z, yaw;
    float xInc, yInc, zInc, yawInc;
};
// 8 segmentos + KF8=KF0 para cerrar el loop limpiamente.
// Órbita CCW vista desde arriba: frente → derecha → atrás → izquierda → frente
DronKF dron_kf[DRON_MAX_KF] = {
//    x      y      z     yaw    (incs se calculan en dron_interpolate)
    {  0.0f,  8.0f, 24.0f,   0.0f, 0,0,0,0 }, // KF0  frente del corredor
    { 14.0f, 11.0f, 14.0f,  45.0f, 0,0,0,0 }, // KF1  esquina derecha-frente
    { 16.0f,  9.0f, -2.0f,  90.0f, 0,0,0,0 }, // KF2  lado derecho, centro
    { 12.0f, 13.0f,-22.0f, 135.0f, 0,0,0,0 }, // KF3  esquina derecha-atrás  (pico alto)
    {  0.0f, 10.0f,-28.0f, 180.0f, 0,0,0,0 }, // KF4  atrás del corredor
    {-14.0f, 12.0f,-20.0f, 225.0f, 0,0,0,0 }, // KF5  esquina izquierda-atrás
    {-16.0f,  9.0f,  2.0f, 270.0f, 0,0,0,0 }, // KF6  lado izquierdo, centro
    {-10.0f, 11.0f, 20.0f, 315.0f, 0,0,0,0 }, // KF7  esquina izquierda-frente
    {  0.0f,  8.0f, 24.0f, 360.0f, 0,0,0,0 }, // KF8  = KF0, cierra el loop
};
// ~200 pasos por segmento ≈ 3.3 s/segmento a 60fps → vuelta completa ~26 s
int dron_stepsPerSeg[DRON_MAX_KF - 1] = { 200, 200, 200, 200, 200, 200, 200, 200 };
int dron_playIndex = 0;
int dron_currSteps = 0;

void dron_interpolate() {
    int ki = dron_playIndex, st = dron_stepsPerSeg[ki];
    dron_kf[ki].xInc   = (dron_kf[ki+1].x   - dron_kf[ki].x)   / st;
    dron_kf[ki].yInc   = (dron_kf[ki+1].y   - dron_kf[ki].y)   / st;
    dron_kf[ki].zInc   = (dron_kf[ki+1].z   - dron_kf[ki].z)   / st;
    dron_kf[ki].yawInc = (dron_kf[ki+1].yaw - dron_kf[ki].yaw) / st;
}
void dron_animate() {
    if (dron_currSteps >= dron_stepsPerSeg[dron_playIndex]) {
        dron_playIndex++;
        if (dron_playIndex >= DRON_MAX_KF - 1) {
            dron_playIndex = 0;
            // Resetear a KF0 exacto (evita error acumulado y wrap de yaw)
            dron_x   =  0.0f; dron_y   =  8.0f;
            dron_z   = 24.0f; dron_yaw =  0.0f;
        }
        dron_currSteps = 0;
        dron_interpolate();
    } else {
        dron_x   += dron_kf[dron_playIndex].xInc;
        dron_y   += dron_kf[dron_playIndex].yInc;
        dron_z   += dron_kf[dron_playIndex].zInc;
        dron_yaw += dron_kf[dron_playIndex].yawInc;
        dron_currSteps++;
    }
}

// ─── Orrery: animación procedural (ángulos acumulados por deltaTime) ────────
// Posición: encima de la mesa del booth Z=3, lado barda
// xs_barda = COR_XBARDA(5) - 1.30 - mesaAncho/2(0.35) = 3.35
// mesaTop  = mesaH(0.75) + mesaAlto(0.07) = 0.82
const float ORRERY_X = 3.35f;
const float ORRERY_Y = 0.84f;   // 0.82 mesa top + pequeño offset
const float ORRERY_Z = 3.0f;

float orrery_angles[8] = { 0.0f, 45.0f, 90.0f, 135.0f,
                            180.0f, 225.0f, 270.0f, 315.0f }; // desfase inicial
// Velocidades deg/seg — interior más rápido (Kepler-ish)
const float ORRERY_SPEEDS[8] = { 90.0f, 72.0f, 55.0f, 42.0f,
                                  32.0f, 24.0f, 17.0f, 12.0f };
// Radios de órbita en unidades mundo
const float ORRERY_RADII[8]  = { 0.18f, 0.27f, 0.37f, 0.48f,
                                  0.60f, 0.74f, 0.89f, 1.05f };
// Inclinación: amplitud en Y para que no queden todas en un plano exacto
const float ORRERY_INCL[8]   = { 0.00f, 0.04f, 0.00f, 0.06f,
                                  0.03f, 0.07f, 0.00f, 0.05f };

// Índices globales de meshes de la FI (se asignan en CreateObjects)
int MESH_CILINDRO   = -1;
int MESH_CAJA       = -1;
int MESH_PISO       = -1; // quad del piso del corredor con UVs correctas
int MESH_PISO2      = -1; // quad del piso del nivel superior (barda)
int MESH_FLYER      = -1; // quad vertical para flyers/estandartes

// ─── Paloma: keyframes de vuelo (órbita por el pasillo, similar al dron) ────
#define PALOMA_MAX_KF 9
struct PalomaKF {
    float x, y, z, yaw;
    float xInc, yInc, zInc, yawInc;
};
// Órbita más amplia y alta que el dron — vuela sobre el corredor
PalomaKF paloma_kf[PALOMA_MAX_KF] = {
//    x      y      z     yaw   (incs calculados en paloma_interpolate)
    { -3.0f, 5.0f,  10.0f,   0.0f,  0,0,0,0 }, // KF0
    {  0.0f, 6.5f,   0.0f,  45.0f,  0,0,0,0 }, // KF1
    {  3.5f, 5.5f, -10.0f,  90.0f,  0,0,0,0 }, // KF2
    {  2.0f, 7.0f, -20.0f, 135.0f,  0,0,0,0 }, // KF3
    { -2.0f, 6.0f, -25.0f, 180.0f,  0,0,0,0 }, // KF4
    { -4.0f, 7.5f, -15.0f, 225.0f,  0,0,0,0 }, // KF5
    { -5.0f, 5.0f,  -5.0f, 270.0f,  0,0,0,0 }, // KF6
    { -4.0f, 6.0f,   5.0f, 315.0f,  0,0,0,0 }, // KF7
    { -3.0f, 5.0f,  10.0f, 360.0f,  0,0,0,0 }, // KF8 = KF0, cierra loop
};
int paloma_stepsPerSeg[PALOMA_MAX_KF - 1] = { 180, 180, 180, 180, 180, 180, 180, 180 };
int paloma_playIndex = 0;
int paloma_currSteps = 0;

void paloma_interpolate() {
    int ki = paloma_playIndex, st = paloma_stepsPerSeg[ki];
    paloma_kf[ki].xInc   = (paloma_kf[ki+1].x   - paloma_kf[ki].x)   / st;
    paloma_kf[ki].yInc   = (paloma_kf[ki+1].y   - paloma_kf[ki].y)   / st;
    paloma_kf[ki].zInc   = (paloma_kf[ki+1].z   - paloma_kf[ki].z)   / st;
    paloma_kf[ki].yawInc = (paloma_kf[ki+1].yaw - paloma_kf[ki].yaw) / st;
}
void paloma_animate() {
    if (paloma_currSteps >= paloma_stepsPerSeg[paloma_playIndex]) {
        paloma_playIndex++;
        if (paloma_playIndex >= PALOMA_MAX_KF - 1) {
            paloma_playIndex = 0;
            paloma_x = -3.0f; paloma_y = 5.0f;
            paloma_z = 10.0f; paloma_yaw = 0.0f;
        }
        paloma_currSteps = 0;
        paloma_interpolate();
    } else {
        paloma_x   += paloma_kf[paloma_playIndex].xInc;
        paloma_y   += paloma_kf[paloma_playIndex].yInc;
        paloma_z   += paloma_kf[paloma_playIndex].zInc;
        paloma_yaw += paloma_kf[paloma_playIndex].yawInc;
        paloma_currSteps++;
    }
}

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

	// Piso del corredor: quad en XZ, UVs exactas pedidas
	// Ancho (X): de COR_XPARED=-5 a COR_XBARDA=5  → U de 0.5 a 3.5 (media+1+1+media = 3 rep)
	// Largo (Z): de COR_Z0=-20 a COR_Z0+COR_LEN=18 → V de 0 a 16
	{
		unsigned int pisoIdx[] = { 0, 2, 1,  1, 2, 3 };
		GLfloat pisoVerts[] = {
			// x       y     z        u     v      nx     ny     nz
			-5.0f,  0.0f, -20.0f,   0.5f,  0.0f,  0.0f,  1.0f,  0.0f,
			 5.0f,  0.0f, -20.0f,   3.5f,  0.0f,  0.0f,  1.0f,  0.0f,
			-5.0f,  0.0f,  18.0f,   0.5f, 16.0f,  0.0f,  1.0f,  0.0f,
			 5.0f,  0.0f,  18.0f,   3.5f, 16.0f,  0.0f,  1.0f,  0.0f,
		};
		Mesh* mp = new Mesh();
		mp->CreateMesh(pisoVerts, pisoIdx, 32, 6);
		meshList.push_back(mp);
		MESH_PISO = (int)meshList.size() - 1;
	}

	// Piso del nivel superior (encima del techo del corredor, Y = COR_YTECHO + 0.27)
	// Misma extensión en X y Z que el corredor
	// UVs: repite la textura igual que el suelo principal
	{
		unsigned int piso2Idx[] = { 0, 2, 1,  1, 2, 3 };
		GLfloat piso2Verts[] = {
			// x       y      z        u     v      nx     ny     nz
			-5.0f,  4.12f, -20.0f,   0.5f,  0.0f,  0.0f,  1.0f,  0.0f,
			 5.0f,  4.12f, -20.0f,   3.5f,  0.0f,  0.0f,  1.0f,  0.0f,
			-5.0f,  4.12f,  18.0f,   0.5f, 16.0f,  0.0f,  1.0f,  0.0f,
			 5.0f,  4.12f,  18.0f,   3.5f, 16.0f,  0.0f,  1.0f,  0.0f,
		};
		Mesh* mp2 = new Mesh();
		mp2->CreateMesh(piso2Verts, piso2Idx, 32, 6);
		meshList.push_back(mp2);
		MESH_PISO2 = (int)meshList.size() - 1;
	}

	// Quad vertical para flyers: unidad [0,1] en X e Y, face hacia +Z
	{
		unsigned int flyerIdx[] = { 0, 1, 2,  0, 2, 3 };
		GLfloat flyerVerts[] = {
			//  x      y      z     u     v     nx    ny    nz
			0.0f,  0.0f,  0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			1.0f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
			1.0f,  1.0f,  0.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
			0.0f,  1.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
		};
		Mesh* mf = new Mesh();
		mf->CreateMesh(flyerVerts, flyerIdx, 32, 6);
		meshList.push_back(mf);
		MESH_FLYER = (int)meshList.size() - 1;
	}

}


void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}




ma_engine audioEngine; // motor de audio miniaudio

int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();
	pr_interpolate();
	prof_interpolate();
	est1_interpolate();
	est2_interpolate();
	est3_interpolate();
	dron_interpolate();
	paloma_interpolate();

	// ─── Audio: inicializar miniaudio y reproducir música en loop ────────
	ma_sound bgm;
	bool bgmLoaded = false;
	if (ma_engine_init(NULL, &audioEngine) == MA_SUCCESS)
	{
		if (ma_sound_init_from_file(&audioEngine, "Audio/background_sound.mp3",
			MA_SOUND_FLAG_STREAM, NULL, NULL, &bgm) == MA_SUCCESS)
		{
			ma_sound_set_looping(&bgm, MA_TRUE);
			ma_sound_set_volume(&bgm, 0.3f); // volumen al 30%
			ma_sound_start(&bgm);
			bgmLoaded = true;
		}
	}

	CreateObjects();
	CreateShaders();

	// Cámara al inicio del corredor FI, mirando hacia Z negativo (adentro del pasillo)
	// Centro X = 0 (entre COR_XPARED=-5 y COR_XBARDA=+5), altura de persona
	camera = Camera(glm::vec3(0.0f, 1.7f, 16.5f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 3.5f, 0.25f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	pisoTexture = Texture("Textures/suelo.png");
	pisoTexture.LoadTextureA();
	piso2Texture = Texture("Textures/suelo2.png");
	piso2Texture.LoadTextureA();
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

	// Cargar flyers (Textures/Flyers/flyer1.png … flyer20.png)
	for (int fi = 0; fi < 20; fi++) {
		std::string flyerPath = "Textures/Flyers/flyer" + std::to_string(fi + 1) + ".png";
		FlyerTextures[fi] = Texture(flyerPath.c_str());
		FlyerTextures[fi].LoadTextureA();
	}
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();


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
	ExpoBooks_M.LoadModel("Models/Escenario/expo_books.glb");
	ExpoBooths_M.LoadModel("Models/Escenario/expo_booth.glb");
	ChicaUnam_Cuerpo_M.LoadModel("Models/ChicaUNAM/chicaunam_cuerpo.glb");
	ChicaUnam_MusloDer_M.LoadModel("Models/ChicaUNAM/chicaunam_musloder.glb");
	ChicaUnam_MusloIzq_M.LoadModel("Models/ChicaUNAM/chicaunam_musloizq.glb");
	ChicaUnam_PiernaDer_M.LoadModel("Models/ChicaUNAM/chicaunam_piernader.glb");
	ChicaUnam_PiernaIzq_M.LoadModel("Models/ChicaUNAM/chicaunam_piernaizq.glb");
	Prof_Torso_M    .LoadModel("Models/Profesor/profesor_torso.glb");
	Prof_Cabeza_M   .LoadModel("Models/Profesor/profesor_cabeza.glb");
	Prof_BrazoDer_M .LoadModel("Models/Profesor/profesor_brazoder.glb");
	Prof_BrazoIzq_M .LoadModel("Models/Profesor/profesor_brazoizq.glb");
	Prof_ManoDer_M  .LoadModel("Models/Profesor/profesor_manoder.glb");
	Prof_ManoIzq_M  .LoadModel("Models/Profesor/profesor_manoizq.glb");
	Prof_PiernaDer_M.LoadModel("Models/Profesor/profesor__piernaderglb.glb");
	Prof_PiernaIzq_M.LoadModel("Models/Profesor/profesor__piernaizq.glb");
	Prof_PieDer_M   .LoadModel("Models/Profesor/profesor_pieder.glb");
	Prof_PieIzq_M   .LoadModel("Models/Profesor/profesor_pieizq.glb");

	Est1_Torso_M    .LoadModel("Models/Estudiante1/estudiante1_torso.glb");
	Est1_Cabeza_M   .LoadModel("Models/Estudiante1/estudiante1_cabeza.glb");
	Est1_BrazoDer_M .LoadModel("Models/Estudiante1/estudiante1_brazoder.glb");
	Est1_BrazoIzq_M .LoadModel("Models/Estudiante1/estudiante1_brazoizq.glb");
	Est1_ManoDer_M  .LoadModel("Models/Estudiante1/estudiante1_manoder.glb");
	Est1_ManoIzq_M  .LoadModel("Models/Estudiante1/estudiante1_manoizq.glb");
	Est1_PiernaDer_M.LoadModel("Models/Estudiante1/estudiante1_piernader.glb");
	Est1_PiernaIzq_M.LoadModel("Models/Estudiante1/estudiante1_piernaizq.glb");
	Est1_ZapatoDer_M.LoadModel("Models/Estudiante1/estudiante1_zapatoder.glb");
	Est1_ZapatoIzq_M.LoadModel("Models/Estudiante1/estudiante1_zapatoizq.glb");

	PR_Cuerpo_M      .LoadModel("Models/PerroRobot/perrorobot_cuerpo.glb");
	PR_BrazoDer_M    .LoadModel("Models/PerroRobot/perrorobot_brazoder.glb");
	PR_AntebrazoDer_M.LoadModel("Models/PerroRobot/perrorobot_antebrazoder.glb");
	PR_BrazoIzq_M    .LoadModel("Models/PerroRobot/perrorobot_brazoizq.glb");
	PR_AntebrazoIzq_M.LoadModel("Models/PerroRobot/perrorobot_antebrazoizq.glb");
	PR_MusloDer_M    .LoadModel("Models/PerroRobot/perrorobot_musloder.glb");
	PR_PieDer_M      .LoadModel("Models/PerroRobot/perrorobot_piederglb.glb");
	PR_MusloIzq_M    .LoadModel("Models/PerroRobot/perrorobot_musloizq.glb");
	PR_PieIzq_M      .LoadModel("Models/PerroRobot/perrorobot_pieizq.glb");

	Dron_Cuerpo_M.LoadModel("Models/Dron/dron_cuerpo.glb");
	Dron_Helice_M.LoadModel("Models/Dron/dron_helice.glb");

	Offroad_Auto_M      .LoadModel("Models/Offroad/Offroad-auto.glb");
	Offroad_LlantaSupDer_M.LoadModel("Models/Offroad/Offroad-llantasupder.glb");
	Offroad_LlantaSupIzq_M.LoadModel("Models/Offroad/Offroad-llantasupizq.glb");
	Offroad_LlantaInfDer_M.LoadModel("Models/Offroad/Offroad-llantainfder.glb");
	Offroad_LlantaInfIzq_M.LoadModel("Models/Offroad/Offroad-llantainfizq.glb");

	Paloma_Cuerpo_M.LoadModel("Models/Paloma/paloma_cuerpo.glb");
	Paloma_AlaDer_M .LoadModel("Models/Paloma/paloma_alader.glb");
	Paloma_AlaIzq_M .LoadModel("Models/Paloma/paloma_alaizq.glb");

	Orrery_Nucleo_M.LoadModel("Models/Orrery/orrery_nucleo.glb");
	for (int oi = 0; oi < 8; oi++) {
		std::string oPath = "Models/Orrery/orrery_planeta" + std::to_string(oi + 1) + ".glb";
		Orrery_Planeta_M[oi].LoadModel(oPath.c_str());
	}

	// Skybox de día
	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/new_Skybox/miramar_rt.tga");
	skyboxFaces.push_back("Textures/new_Skybox/miramar_lf.tga");
	skyboxFaces.push_back("Textures/new_Skybox/miramar_dn.tga");
	skyboxFaces.push_back("Textures/new_Skybox/miramar_up.tga");
	skyboxFaces.push_back("Textures/new_Skybox/miramar_bk.tga");
	skyboxFaces.push_back("Textures/new_Skybox/miramar_ft.tga");
	skybox = Skybox(skyboxFaces);

	// Skybox de noche
	std::vector<std::string> skyboxNightFaces;
	skyboxNightFaces.push_back("Textures/new_Skybox/miramar_rt_night.png");
	skyboxNightFaces.push_back("Textures/new_Skybox/miramar_lf_night.png");
	skyboxNightFaces.push_back("Textures/new_Skybox/miramar_dn_night.png");
	skyboxNightFaces.push_back("Textures/new_Skybox/miramar_up_night.png");
	skyboxNightFaces.push_back("Textures/new_Skybox/miramar_bk_night.png");
	skyboxNightFaces.push_back("Textures/new_Skybox/miramar_ft_night.png");
	skyboxNight = Skybox(skyboxNightFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	// Las point lights de las lámparas se configuran cada frame en el render loop
	unsigned int pointLightCount = 0;

	unsigned int spotLightCount = 0;
	// Linterna de la cámara (flash)
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;



	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0, uniformTextureOffset=0;
	GLuint uniformColor = 0, uniformAlpha = 0, uniformNoLighting = 0;
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

		// --- Tecla C: toggle caminar ChicaUnam ---
		{
			bool* keys = mainWindow.getsKeys();
			if (keys[GLFW_KEY_C] && !cKeyWasPressed)
				chicaWalking = !chicaWalking;
			cKeyWasPressed = keys[GLFW_KEY_C];
		}
		// PerroRobot: ciclo de trote para patas + keyframes del backflip
		pr_walkCycle += deltaTime * 2.5f;
		pr_animate(deltaTime);
		prof_animate();
		est1_animate();
		est2_animate();
		est3_animate();
		dron_animate();
		dron_helixAngle += deltaTime * 720.0f; // 2 vueltas/seg
		for (int oi = 0; oi < 8; oi++)
			orrery_angles[oi] += ORRERY_SPEEDS[oi] * deltaTime;

		// ─── Offroad: movimiento adelante/atrás sobre la mesa ────────────
		offroad_posZ += offroad_dir * offroad_speed * deltaTime;
		if (offroad_posZ >  OFFROAD_RANGE) { offroad_posZ =  OFFROAD_RANGE; offroad_dir = -1.0f; }
		if (offroad_posZ < -OFFROAD_RANGE) { offroad_posZ = -OFFROAD_RANGE; offroad_dir =  1.0f; }
		// Llantas giran proporcional al desplazamiento (radio ~0.3 en escala local)
		offroad_llanta += offroad_dir * offroad_speed * deltaTime * (360.0f / (2.0f * 3.14159f * 0.3f));

		// ─── Paloma: vuelo + aleteo ──────────────────────────────────────
		paloma_animate();
		paloma_aleteo += deltaTime * 5.0f; // ciclo de aleteo rápido

		if (chicaWalking) {
			chica_walkCycle += deltaTime * 4.0f;
			chica_posZ -= chica_dir * deltaTime * 1.5f;  // avanza según dirección
			if (chica_posZ < -18.0f) { chica_posZ = -18.0f; chica_dir = -1.0f; } // vuelta al fondo
			if (chica_posZ >  16.0f) { chica_posZ =  16.0f; chica_dir =  1.0f; } // vuelta al inicio
		}


		// --- Personaje procedural: avanza igual que la chica ---
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

		// --- Tecla 7: ciclo día/noche (igual que ProyectoLab) ---
		{
			bool* keys = mainWindow.getsKeys();
			static bool key7Prev = false;
			if (keys[GLFW_KEY_7] && !key7Prev) { cycleElapsed = 0.0f; cycleRunning = true; }
			key7Prev = keys[GLFW_KEY_7];
		}

		// --- Teclas 1-4: posiciones predefinidas de cámara ---
		// Constructor: Camera(pos, up, yaw, pitch, moveSpeed, turnSpeed)
		// yaw: -90 = mira hacia -Z (adentro del corredor), 90 = hacia +Z, 0 = hacia +X, 180 = hacia -X
		{
			bool* keys = mainWindow.getsKeys();
			static bool key1Prev = false, key2Prev = false,
			            key3Prev = false, key4Prev = false;

			// Tecla 1: Puerta principal — inicio del corredor, mirando hacia adentro
			if (keys[GLFW_KEY_1] && !key1Prev)
				camera = Camera(glm::vec3(0.0f, 1.7f, 16.5f),
				                glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 3.5f, 0.25f);
			key1Prev = keys[GLFW_KEY_1];

			// Tecla 2: Cerca del Perro Robot (booth Z=-12, lado barda X=2.0)
			if (keys[GLFW_KEY_2] && !key2Prev)
				camera = Camera(glm::vec3(0.5f, 1.2f, -12.0f),
				                glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, -5.0f, 3.5f, 0.25f);
			key2Prev = keys[GLFW_KEY_2];

			// Tecla 3: Siguiendo al dron — cámara se posiciona detrás y arriba del dron en tiempo real
			if (keys[GLFW_KEY_3] && !key3Prev)
				camera = Camera(glm::vec3(dron_x, dron_y + 1.5f, dron_z + 4.0f),
				                glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -15.0f, 3.5f, 0.25f);
			key3Prev = keys[GLFW_KEY_3];

			// Tecla 4: Cerca del Orrery (elemento de gran detalle, mesa Z=3, lado barda X=3.35)
			if (keys[GLFW_KEY_4] && !key4Prev)
				camera = Camera(glm::vec3(2.0f, 1.3f, 3.0f),
				                glm::vec3(0.0f, 1.0f, 0.0f), 135.0f, -15.0f, 3.5f, 0.25f);
			key4Prev = keys[GLFW_KEY_4];
		}
		if (cycleRunning) {
			cycleElapsed += deltaTime;
			if (cycleElapsed >= CYCLE_DURATION) { cycleElapsed -= CYCLE_DURATION; } // loop continuo
		}

		float cycleTime = cycleElapsed / CYCLE_DURATION;
		float sunAngle  = cycleTime * 2.0f * 3.14159265f;
		float dayFactor = glm::max(0.0f, cosf(sunAngle)); // 1=mediodía, 0=noche
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
			0.05f + dayFactor * 0.18f,
			dayFactor * 0.45f,
			sunDirX, sunDirY, sunDirZ
		);

		// Lámparas del corredor: siempre encendidas, más brillantes de noche.
		// Atenuación agresiva (exponent alto) = área de luz pequeña y notoria.
		{
			float nightFactor = 1.0f - dayFactor;
			float lampAmbient = 0.1f + 0.5f * nightFactor;
			float lampDiffuse = 1.0f + 3.0f * nightFactor;
			const float LAMP_Y = 1.85f; // altura de la luz (punto luminoso)
			float lampZPos[3] = { -20.0f + 38.0f*0.2f, -20.0f + 38.0f*0.5f, -20.0f + 38.0f*0.8f };
			// constant bajo + exponent alto = caída brusca → círculo de luz bien definido
			// constant=1.0, linear=0.8, exponent=0.6 → caída rápida, no traspasa techo
			pointLights[0] = PointLight(1.0f, 1.0f, 0.92f, lampAmbient, lampDiffuse, 0.0f, LAMP_Y, lampZPos[0], 1.0f, 0.8f, 0.6f);
			pointLights[1] = PointLight(1.0f, 1.0f, 0.92f, lampAmbient, lampDiffuse, 0.0f, LAMP_Y, lampZPos[1], 1.0f, 0.8f, 0.6f);
			pointLights[2] = PointLight(1.0f, 1.0f, 0.92f, lampAmbient, lampDiffuse, 0.0f, LAMP_Y, lampZPos[2], 1.0f, 0.8f, 0.6f);
		}

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Skybox unico: tint lo oscurece de noche igual que ProyectoLab
		glm::vec3 skyTintColor = glm::max(sunColor, glm::vec3(0.15f, 0.15f, 0.35f));
		skybox.SetTint(skyTintColor.r, skyTintColor.g, skyTintColor.b);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);

		shaderList[0].UseShader();
		uniformModel      = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView       = shaderList[0].GetViewLocation();
		uniformEyePosition= shaderList[0].GetEyePositionLocation();
		uniformColor      = shaderList[0].getColorLocation();
		uniformTextureOffset = shaderList[0].getOffsetLocation();
		uniformAlpha      = shaderList[0].getAlphaLocation();
		uniformNoLighting = shaderList[0].getNoLightingLocation();

		glUniform1f(uniformAlpha, 1.0f);
		glUniform1i(uniformNoLighting, 0);

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
		shaderList[0].SetPointLights(pointLights, 3); // 3 lámparas del techo
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
			color = glm::vec3(1.0f, 1.0f, 1.0f); // blanco para no teñir la textura
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f); // UVs ya están en el mesh, no hace falta escalar
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			pisoTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_PISO]->RenderMesh();
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
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			for (int ci = 0; ci < NUM_COLS; ci++)
			{
				float zc = COR_Z0 + 0.5f + ci * COL_SEP;

				// Lado explanada — +0.01 del suelo, -0.02 del techo para evitar z-fighting
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(COR_XBARDA - 0.5f, COR_YSUELO + 0.01f, zc));
				model = glm::scale(model, glm::vec3(COL_R * 2.0f, COR_YTECHO - 0.02f, COL_R * 2.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				meshList[MESH_CILINDRO]->RenderMesh();

				// Lado pared
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(COR_XPARED + 0.5f, COR_YSUELO + 0.01f, zc));
				model = glm::scale(model, glm::vec3(COL_R * 2.0f, COR_YTECHO - 0.02f, COL_R * 2.0f));
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

			// (tubo amarillo y tubitos removidos)
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

			// (tubo amarillo y tubitos removidos)
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
			model = glm::translate(model, glm::vec3(COR_XBARDA - BORDE_W, COR_YTECHO + 0.272f, COR_Z0));
			model = glm::scale(model, glm::vec3(BORDE_W, BORDE_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();
			// Lado servicios
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(COR_XPARED, COR_YTECHO + 0.272f, COR_Z0));
			model = glm::scale(model, glm::vec3(BORDE_W, BORDE_H, COR_LEN));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			meshList[MESH_CAJA]->RenderMesh();

			// Borde SUPERIOR gris (encima del techo, pegado al borde exterior, sube desde la capa de ladrillo)
			{
				const float BORDE_SUP_W = 0.40f;  // grosor en X
				const float BORDE_SUP_H = 0.35f;  // altura en Y (sube sobre el ladrillo)
				// La capa de ladrillo termina en COR_YTECHO + 0.15 + 0.12 = COR_YTECHO + 0.27
				const float BORDE_SUP_Y = COR_YTECHO + 0.271f; // +0.001 evita z-fight con borde ladrillo
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

		// ----- PISO DEL NIVEL SUPERIOR (encima del techo) -----
		{
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			piso2Texture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			meshList[MESH_PISO2]->RenderMesh();
		}

		// ----- BARDA DEL NIVEL SUPERIOR -----
		// Estructura: rectángulos verticales delgados y altos, muy juntos,
		// unidos arriba por un rectángulo acostado que va a todo lo largo.
		// La barda arranca en Y = 3.77 (superficie del piso2) y va en ambos lados (X = ±5).
		{
			const float BARDA_Y0    = 4.12f;   // encima del desnivel
			const float BARDA_H     = 1.10f;
			const float BARDA_W     = 0.04f;   // barras delgadas
			const float BARDA_GAP   = 0.10f;
			const float CORONA_H    = 0.08f;   // corona delgada
			const float CORONA_EXTRA = 0.02f;

			// color amarillo UNAM
			color = glm::vec3(0.95f, 0.82f, 0.10f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			plainTexture.UseTexture();
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			int numBarras = (int)(COR_LEN / BARDA_GAP);

			for (int side = 0; side < 2; side++) {
				float sideX = (side == 0) ? COR_XBARDA : COR_XPARED - BARDA_W;

				// Rectángulos verticales
				for (int bi = 0; bi <= numBarras; bi++) {
					float bz = COR_Z0 + bi * BARDA_GAP;
					if (bz > COR_Z0 + COR_LEN) break;
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(sideX, BARDA_Y0 + 0.005f, bz));  // +0.005 evita z-fight con piso2
					model = glm::scale(model, glm::vec3(BARDA_W, BARDA_H - 0.005f, BARDA_W));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					meshList[MESH_CAJA]->RenderMesh();
				}

				// Corona acostada encima
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(sideX - CORONA_EXTRA, BARDA_Y0 + BARDA_H, COR_Z0));
				model = glm::scale(model, glm::vec3(BARDA_W + CORONA_EXTRA * 2.0f, CORONA_H, COR_LEN));
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

			// Las point lights de las lámparas ya se configuraron antes del render loop
			// (encendidas de noche, apagadas de día) junto al SetPointLights general.
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

		// Mural geométrico removido

			// ----- MESAS DE EXPO (1 por booth × 2 lados = 10 mesas) -----
		// Centradas en el mismo Z que cada booth, una por lado del pasillo.
		{
			const float mesaLargo = 1.2f;
			const float mesaAncho = 0.7f;
			const float mesaAlto  = 0.07f;
			const float mesaH     = 0.75f;
			const float boothMesaZ[5] = { -12.0f, -7.0f, -2.0f, 3.0f, 8.0f };

			for (int mi = 0; mi < 5; mi++) {
				float zs = boothMesaZ[mi];

				// Lado pared: mesa más hacia el centro del pasillo
				float xs_pared = COR_XPARED + 1.30f + mesaAncho * 0.5f;
				// Lado barda: mesa más hacia el centro del pasillo
				float xs_barda = COR_XBARDA - 1.30f - mesaAncho * 0.5f;

				for (int side = 0; side < 2; side++) {
					float xs_centro = (side == 0) ? xs_pared : xs_barda;

					// Tablero
					color = glm::vec3(0.78f, 0.72f, 0.58f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(xs_centro - mesaAncho * 0.5f, mesaH, zs - mesaLargo * 0.5f));
					model = glm::scale(model, glm::vec3(mesaAncho, mesaAlto, mesaLargo));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					plainTexture.UseTexture();
					Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
					meshList[MESH_CAJA]->RenderMesh();

					// Patas (4 cilindros)
					color = glm::vec3(0.48f, 0.48f, 0.50f);
					glUniform3fv(uniformColor, 1, glm::value_ptr(color));
					plainTexture.UseTexture();
					Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
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
				}
			}
		}

		// ─── FLYERS EN BOOTHS ──────────────────────────────────────────────
		// 20 flyers únicos: 5 booths × 2 lados × 2 por booth = 20
		// Índice en FlyerTextures[]: bi*4 + side*2 + slot
		//   bi   = booth 0-4
		//   side = 0 (pared) | 1 (barda)
		//   slot = 0 (trasero) | 1 (lateral)
		// → flyer1…flyer20.png mapeados linealmente
		{
			const float boothFlyZ[5] = { -12.0f, -7.0f, -2.0f, 3.0f, 8.0f };
			const float BOOTH_DEPTH  = 1.6f;   // profundidad del booth desde xWall
			const float BOOTH_HALF_W = 1.2f;   // semi-ancho en Z

			// Tamaños variados por booth [bi][slot]
			const float FW[5][2] = { {0.6f,0.9f}, {0.5f,0.8f}, {0.7f,1.0f}, {0.55f,0.85f}, {0.65f,0.95f} };
			const float FH[5][2] = { {0.9f,0.6f}, {0.8f,0.5f}, {1.0f,0.7f}, {0.85f,0.55f}, {0.95f,0.65f} };

			glDisable(GL_CULL_FACE);
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			for (int bi = 0; bi < 5; bi++) {
				float zc = boothFlyZ[bi];

				for (int side = 0; side < 2; side++) {
					// side=0: lado pared (X negativo), side=1: lado barda (X positivo)
					float signX  = (side == 0) ? -1.0f :  1.0f;
					float xBack  = signX * (3.5f - BOOTH_DEPTH); // cara interna del booth
					float xWall  = signX * 3.5f;                 // pared exterior
					int   fBase  = bi * 4 + side * 2;            // índice base en FlyerTextures[]

					// Ambos flyers son laterales: uno en cada extremo en Z del booth.
					// El quad MESH_FLYER está en plano XY mirando hacia +Z.
					// flyBX: pegado a xWall (pared exterior), 4cm adentro.
					float flyBX = xWall - signX * 0.04f;

					// ── Flyer A: pared lateral en Z = zc + BOOTH_HALF_W (extremo +Z) ──
					{
						float fw = FW[bi][0], fh = FH[bi][0];
						float fy = 1.0f;
						FlyerTextures[fBase + 0].UseTexture();
						color = glm::vec3(1.0f, 1.0f, 1.0f);
						glUniform3fv(uniformColor, 1, glm::value_ptr(color));
						model = glm::mat4(1.0f);
						model = glm::translate(model, glm::vec3(flyBX, fy, zc + BOOTH_HALF_W - 0.04f));
						model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0,1,0));
						model = glm::scale(model, glm::vec3(fw, fh, 1.0f));
						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
						meshList[MESH_FLYER]->RenderMesh();
					}

					// ── Flyer B: pared lateral en Z = zc - BOOTH_HALF_W (extremo -Z) ──
					{
						float fw = FW[bi][1], fh = FH[bi][1];
						float fy = 0.9f;
						FlyerTextures[fBase + 1].UseTexture();
						color = glm::vec3(1.0f, 1.0f, 1.0f);
						glUniform3fv(uniformColor, 1, glm::value_ptr(color));
						model = glm::mat4(1.0f);
						// extremo -Z del booth: el quad mira hacia -Z → no necesita rotación extra
						model = glm::translate(model, glm::vec3(flyBX, fy, zc - BOOTH_HALF_W + 0.04f));
						model = glm::scale(model, glm::vec3(fw, fh, 1.0f));
						glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
						meshList[MESH_FLYER]->RenderMesh();
					}
				}
			}
			glEnable(GL_CULL_FACE);
		}

		// Restaurar color blanco
		color = glm::vec3(1.0f, 1.0f, 1.0f);
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		toffset = glm::vec2(0.0f, 0.0f);
		glUniform2fv(uniformTextureOffset, 1, glm::value_ptr(toffset));


		// ─── PERRO ROBOT (junto al booth en Z=-2, lado pared) ──────────────
		// Posición: X=2.5, Y=0.55, Z=-2.0 (tercer booth, lado barda)
		// Orientación: el GLB tiene -90°X + +90°X internos (se cancelan), no se necesita corrección
		// Jerarquía: cuerpo→(brazo→antebrazo)×2, cuerpo→(muslo→pie)×2
		{
			const glm::vec3 PR_BASE(2.0f, 0.55f, -12.0f); // un poco más al frente del booth del fondo
			const float     PR_SCALE = 0.0022f;         // modelo Blender muy grande
			const float     PR_YAW   = -30.0f;          // girado levemente hacia el pasillo

			// Amplitudes de animación
			float wc       = pr_walkCycle;
			float legSwing = sinf(wc) * 10.0f;
			float kneeFlx  = fabsf(sinf(wc)) * 8.0f;
			float armSwing = -sinf(wc) * 9.0f; // contrafase
			float elbowFlx = fabsf(sinf(wc)) * 6.0f;

			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);


			// mRoot: posición base + altura de salto (pr_bodyY) + yaw
			glm::mat4 mRoot = glm::mat4(1.0f);
			mRoot = glm::translate(mRoot, PR_BASE + glm::vec3(0, pr_bodyY - pr_legFlex * 0.004f, 0));
			mRoot = glm::rotate(mRoot, glm::radians(PR_YAW), glm::vec3(0,1,0));

			// Helper render: offset en espacio-mundo + rotación + escala
			auto prPartW = [&](glm::mat4 parentWorld, glm::vec3 offsetWorld,
							   float rotDeg, glm::vec3 rotAxis, float sc, Model& mdl) -> glm::mat4
			{
				glm::mat4 mWorld = glm::translate(parentWorld, offsetWorld);
				if (rotDeg != 0.0f)
					mWorld = glm::rotate(mWorld, glm::radians(rotDeg), rotAxis);
				glm::mat4 mRender = glm::scale(mWorld, glm::vec3(sc));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mRender));
				glDisable(GL_CULL_FACE); mdl.RenderModel(); glEnable(GL_CULL_FACE);
				return mWorld;
			};

			// Eje Z local del robot (adelante/atrás del cuerpo) para swing de patas
			glm::vec3 axisSwing = glm::vec3(mRoot[2]);
			// Eje Z local del robot para el backflip (rota hacia atrás, no de lado)
			glm::vec3 axisFlip  = glm::vec3(mRoot[2]);

			// ── Cuerpo: aplica rotación del backflip sobre eje X local ──
			glm::mat4 mCuerpo = glm::translate(glm::mat4(1.0f), PR_BASE + glm::vec3(0, pr_bodyY - pr_legFlex * 0.004f, 0));
			mCuerpo = glm::rotate(mCuerpo, glm::radians(PR_YAW), glm::vec3(0,1,0));
			mCuerpo = glm::rotate(mCuerpo, glm::radians(pr_bodyRotX), axisFlip);
			{
				glm::mat4 mRender = glm::scale(mCuerpo, glm::vec3(PR_SCALE));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mRender));
				glDisable(GL_CULL_FACE); PR_Cuerpo_M.RenderModel(); glEnable(GL_CULL_FACE);
			}

			// Las patas heredan mCuerpo como padre (giran con el cuerpo en el flip)
			// Flexión extra en el impulso/aterrizaje: pr_legFlex se suma a kneeFlx
			float totalFlex = kneeFlx + pr_legFlex;

			// ── Pata delantera derecha (X+, Z+) ──
			glm::mat4 mBrazoDer = prPartW(mCuerpo,
				glm::vec3( 0.20f, -0.05f,  0.14f),  legSwing, axisSwing, PR_SCALE, PR_BrazoDer_M);
			prPartW(mBrazoDer,
				glm::vec3(-0.20f, -0.22f,  0.0f),  totalFlex, axisSwing, PR_SCALE, PR_AntebrazoDer_M);

			// ── Pata delantera izquierda (X+, Z-) ──
			glm::mat4 mBrazoIzq = prPartW(mCuerpo,
				glm::vec3( 0.20f, -0.05f, -0.14f), -legSwing, axisSwing, PR_SCALE, PR_BrazoIzq_M);
			prPartW(mBrazoIzq,
				glm::vec3(-0.20f, -0.22f,  0.0f),  totalFlex, axisSwing, PR_SCALE, PR_AntebrazoIzq_M);

			// ── Pata trasera derecha (X-, Z+) ──
			glm::mat4 mMusloDer = prPartW(mCuerpo,
				glm::vec3(-0.20f, -0.05f,  0.14f), -legSwing, axisSwing, PR_SCALE, PR_MusloDer_M);
			prPartW(mMusloDer,
				glm::vec3(-0.20f, -0.22f,  0.0f),  totalFlex, axisSwing, PR_SCALE, PR_PieDer_M);

			// ── Pata trasera izquierda (X-, Z-) ──
			glm::mat4 mMusloIzq = prPartW(mCuerpo,
				glm::vec3(-0.20f, -0.05f, -0.14f),  legSwing, axisSwing, PR_SCALE, PR_MusloIzq_M);
			prPartW(mMusloIzq,
				glm::vec3(-0.20f, -0.22f,  0.0f),  totalFlex, axisSwing, PR_SCALE, PR_PieIzq_M);
		}

		// ─── PROFESOR VENDEDOR ───────────────────────────────────────────
		// Partes en espacio LOCAL (bbox en local, no world-origin).
		// Torso bbox center=(−0.113, 0.276, −0.007) → el torso mide ~0.585 en Y,
		//   su base inferior está en Y ≈ 0.276 − 0.293 = −0.017 ≈ 0.
		//   Su tope está en Y ≈ 0.276 + 0.293 = 0.569.
		// Cabeza center=(0,0.112,0.026) → se coloca en Y ≈ 0.60 sobre el torso.
		// Brazos center=(±0.13, −0.209) → pivot en el hombro (Y≈0.50, X≈±0.30)
		// Piernas center=(±0.04, −0.411) → cadera en Y≈−0.01, pivot en Y≈0
		// Pies center=(0.13/0.02, −0.058) → fondo de pierna Y≈−0.41−0.51=−0.92
		// El modelo apunta hacia +Z → PROF_FACE=180° para que mire hacia −Z (pasillo)
		{
			const glm::vec3 PROF_BASE(4.2f, 1.05f, -15.5f);
			const float     PROF_FACE = 180.0f; // modelo apunta +Z → giramos 180° para −Z (pasillo)

			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			// Raíz: traslado + orientación + yaw de animación
			glm::mat4 mRoot = glm::mat4(1.0f);
			mRoot = glm::translate(mRoot, PROF_BASE);
			mRoot = glm::rotate(mRoot, glm::radians(PROF_FACE), glm::vec3(0,1,0));
			mRoot = glm::rotate(mRoot, glm::radians(prof_yaw),   glm::vec3(0,1,0));

			// helper: render a part at offset from parent, with optional rotation
			auto pRender = [&](glm::mat4 parent, glm::vec3 off, Model& mdl) -> glm::mat4 {
				glm::mat4 m = glm::translate(parent, off);
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
				glDisable(GL_CULL_FACE); mdl.RenderModel(); glEnable(GL_CULL_FACE);
				return m;
			};

			// Torso: centrado en el origen del modelo (base Y≈0 ya)
			glm::mat4 mTorso = pRender(mRoot, glm::vec3(0.0f, 0.0f, 0.0f), Prof_Torso_M);

			// Cabeza: más pegada al torso — offset 0.50 (antes 0.58)
			// Torso tope Y≈0.569, cabeza bbox size Y≈0.31 → juntar a 0.50
			glm::mat4 mCabPivot = glm::translate(mTorso, glm::vec3(0.0f, 0.50f, 0.0f));
			mCabPivot = glm::rotate(mCabPivot, glm::radians(prof_cabezaY), glm::vec3(0,1,0));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mCabPivot));
			glDisable(GL_CULL_FACE); Prof_Cabeza_M.RenderModel(); glEnable(GL_CULL_FACE);

			// Brazo derecho: X -0.15→-0.19
			glm::vec3 pivBDer(-0.19f, 0.46f, 0.0f);
			glm::mat4 mBrazoDer = glm::translate(mTorso, pivBDer);
			mBrazoDer = glm::rotate(mBrazoDer, glm::radians(prof_brazoDerX), glm::vec3(1,0,0));
			mBrazoDer = glm::rotate(mBrazoDer, glm::radians(-prof_brazoDerZ), glm::vec3(0,0,1));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mBrazoDer));
			glDisable(GL_CULL_FACE); Prof_BrazoDer_M.RenderModel(); glEnable(GL_CULL_FACE);
			pRender(mBrazoDer, glm::vec3(-0.20f, -0.35f, 0.02f), Prof_ManoDer_M);

			// Brazo izquierdo: X +0.15→+0.19
			glm::vec3 pivBIzq(0.19f, 0.46f, 0.0f);
			glm::mat4 mBrazoIzq = glm::translate(mTorso, pivBIzq);
			mBrazoIzq = glm::rotate(mBrazoIzq, glm::radians(-prof_brazoIzqZ), glm::vec3(0,0,1));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mBrazoIzq));
			glDisable(GL_CULL_FACE); Prof_BrazoIzq_M.RenderModel(); glEnable(GL_CULL_FACE);
			pRender(mBrazoIzq, glm::vec3(0.20f, -0.35f, 0.02f), Prof_ManoIzq_M);

			// Piernas ±0.06, pies más separados ±0.08
			glm::mat4 mPiernaDer = pRender(mTorso, glm::vec3(-0.06f, -0.02f, 0.0f), Prof_PiernaDer_M);
			pRender(mPiernaDer, glm::vec3(-0.08f, -0.90f, 0.0f), Prof_PieDer_M);

			glm::mat4 mPiernaIzq = pRender(mTorso, glm::vec3( 0.06f, -0.02f, 0.0f), Prof_PiernaIzq_M);
			pRender(mPiernaIzq, glm::vec3( 0.08f, -0.90f, 0.0f), Prof_PieIzq_M);
		}

		// ─── ESTUDIANTES EN BOOTHS (10 instancias, 3 animaciones rotadas) ──────────
		// 5 booths × 2 lados. Booths en Z={-12,-7,-2,3,8}.
		// Lado pared (X≈-5): estudiante en X=-4.4, mira hacia +X (face=90°)
		// Lado barda (X≈+5): estudiante en X=+4.4, mira hacia -X (face=-90°)
		// Animaciones: 0=teclear, 1=saludar, 2=aplaudir  (rotadas por booth)
		{
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			const float boothZ[5]  = { -12.0f, -7.0f, -2.0f, 3.0f, 8.0f };
			// X del estudiante: pegado a la pared/barda pero visible detrás de la mesa
			const float xPared =  -4.4f; // lado pared
			const float xBarda =   4.4f; // lado barda
			// Cara: pared mira hacia +X (90°), barda mira hacia -X (-90°)
			const float facePared =  90.0f;
			const float faceBarda = -90.0f;

			// helper render: recibe base, brazoDerX, brazoIzqX, cabezaX
			auto renderEst = [&](glm::vec3 base, float faceY,
			                     float brazDX, float brazIX, float cabX,
			                     float brazDZ = 0.0f, float brazIZ = 0.0f)
			{
				glm::mat4 eRoot = glm::mat4(1.0f);
				eRoot = glm::translate(eRoot, base);
				eRoot = glm::rotate(eRoot, glm::radians(faceY), glm::vec3(0,1,0));

				auto eR = [&](glm::mat4 m, Model& mdl) {
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
					glDisable(GL_CULL_FACE); mdl.RenderModel(); glEnable(GL_CULL_FACE);
				};

				eR(eRoot, Est1_Torso_M);

				// Piernas ±0.05
				eR(glm::translate(eRoot, glm::vec3(-0.05f, 1.05f, 0.0f)), Est1_PiernaDer_M);
				eR(glm::translate(eRoot, glm::vec3( 0.05f, 1.05f, 0.0f)), Est1_PiernaIzq_M);
				// Zapatos ±0.09
				eR(glm::translate(eRoot, glm::vec3(-0.09f, 0.22f, 0.0f)), Est1_ZapatoDer_M);
				eR(glm::translate(eRoot, glm::vec3( 0.09f, 0.22f, 0.0f)), Est1_ZapatoIzq_M);

				// Cabeza con inclinación
				{
					glm::vec3 pC(0.0f, 1.52f, 0.0f);
					glm::mat4 mC = glm::translate(eRoot, pC);
					mC = glm::rotate(mC, glm::radians(cabX), glm::vec3(1,0,0));
					mC = glm::translate(mC, -pC);
					eR(mC, Est1_Cabeza_M);
				}
				// Brazo der: X = adelante/atrás, Z = acercarse al centro (aplaudir)
				{
					glm::vec3 pBD(-0.186f, 1.480f, -0.033f);
					glm::mat4 mBD = glm::translate(eRoot, pBD);
					mBD = glm::rotate(mBD, glm::radians(brazDX), glm::vec3(1,0,0));
					mBD = glm::rotate(mBD, glm::radians(brazDZ), glm::vec3(0,0,1));
					eR(mBD, Est1_BrazoDer_M);
					eR(glm::translate(mBD, glm::vec3(-0.08f, -0.15f, 0.0f)), Est1_ManoDer_M);
				}
				// Brazo izq: X = adelante/atrás, Z = acercarse al centro (aplaudir)
				{
					glm::vec3 pBI(0.180f, 1.474f, -0.036f);
					glm::mat4 mBI = glm::translate(eRoot, pBI);
					mBI = glm::rotate(mBI, glm::radians(brazIX), glm::vec3(1,0,0));
					mBI = glm::rotate(mBI, glm::radians(brazIZ), glm::vec3(0,0,1));
					eR(mBI, Est1_BrazoIzq_M);
					eR(glm::translate(mBI, glm::vec3( 0.08f, -0.15f, 0.0f)), Est1_ManoIzq_M);
				}
			};

			// 10 instancias: bi=0..4, side=0(pared)/1(barda)
			// Animación asignada: (bi*2 + side) % 3
			for (int bi = 0; bi < 5; bi++) {
				float zc = boothZ[bi];
				int animPared = (bi * 2 + 0) % 3;
				int animBarda = (bi * 2 + 1) % 3;

				// --- lado pared ---
				float bDXp, bIXp, cabXp, bDZp = 0.0f, bIZp = 0.0f;
				if      (animPared == 0) { bDXp = est1_brazoDerX; bIXp = est1_brazoIzqX; cabXp = est1_cabezaX; }
				else if (animPared == 1) { bDXp = est2_brazoDerX; bIXp = est2_brazoIzqX; cabXp = est2_cabezaX; }
				else                     { bDXp = est3_brazoDerX; bIXp = est3_brazoIzqX; cabXp = est3_cabezaX;
				                           bDZp = est3_brazoDerZ;  bIZp = est3_brazoIzqZ; }
				renderEst(glm::vec3(xPared, 0.0f, zc), facePared, bDXp, bIXp, cabXp, bDZp, bIZp);

				// --- lado barda ---
				float bDXb, bIXb, cabXb, bDZb = 0.0f, bIZb = 0.0f;
				if      (animBarda == 0) { bDXb = est1_brazoDerX; bIXb = est1_brazoIzqX; cabXb = est1_cabezaX; }
				else if (animBarda == 1) { bDXb = est2_brazoDerX; bIXb = est2_brazoIzqX; cabXb = est2_cabezaX; }
				else                     { bDXb = est3_brazoDerX; bIXb = est3_brazoIzqX; cabXb = est3_cabezaX;
				                           bDZb = est3_brazoDerZ;  bIZb = est3_brazoIzqZ; }
				renderEst(glm::vec3(xBarda, 0.0f, zc), faceBarda, bDXb, bIXb, cabXb, bDZb, bIZb);
			}
		}

		// ─── EXPO BOOKS (expo_books.glb) ────────────────────────────────
		// 2 estantes JUNTOS por cada posición (total: 4 por lado).
		// El GLB tiene baked -90°X (Blender→glTF): compensamos con +90°X primero,
		// luego rotamos ±90°Y para que la cara mire al pasillo.
		// Escala 0.001 → ~1.46m ancho, ~1.26m alto, ~1.26m profundo.
		// 2 posiciones de grupo: inicio (~-15.5) y final (~+12.5)
		{
			const float BOOK_SCALE   = 0.00085f;
			const float BOOK_W_WORLD = 1463.9f * BOOK_SCALE; // ancho del estante en Z tras rot
			const float BOOK_D_WORLD = 1258.5f * BOOK_SCALE; // profundidad (hacia pared) tras rot
			const float BOOK_GAP     = 0.02f;   // separación entre los 2 estantes juntos
			const float GAP_PARED    = 0.10f;   // separación entre pared y frente del estante

			// 2 grupos de 2 estantes a lo largo del pasillo
			const float zBookGroups[2] = { -15.5f, 12.5f };

			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			for (int gi = 0; gi < 2; gi++) {
				float zGroup = zBookGroups[gi];
				// Los 2 estantes: centrados en zGroup, uno en -offset, otro en +offset
				const float offsets[2] = {
					-(BOOK_W_WORLD * 0.5f + BOOK_GAP * 0.5f),
					  BOOK_W_WORLD * 0.5f + BOOK_GAP * 0.5f
				};

				for (int si = 0; si < 2; si++) {
					float zOff = offsets[si];

					// Lado pared (X=-5): +90°X para pararlo, luego -90°Y para mirar al pasillo
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(-5.0f + GAP_PARED + BOOK_D_WORLD, 0.0f, zGroup + zOff));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::scale(model, glm::vec3(BOOK_SCALE));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					glDisable(GL_CULL_FACE); ExpoBooks_M.RenderModel(); glEnable(GL_CULL_FACE);

					// Lado barda (X=+5): -90°X para pararlo, luego +90°Y para mirar al pasillo
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(5.0f - GAP_PARED - BOOK_D_WORLD, 0.0f, zGroup - zOff));
					model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
					model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
					model = glm::scale(model, glm::vec3(BOOK_SCALE));
					glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
					glDisable(GL_CULL_FACE); ExpoBooks_M.RenderModel(); glEnable(GL_CULL_FACE);
				}
			}
		}

		// ─── EXPO BOOTHS (expo_booth.glb) ─────────────────────────────────────
		// 1 booth por lado entre cada par de pilares (excepto donde van los expo_books)
		// Escala 0.8 (unidades metro). Rotacion: +90°X para pararlo + ±90°Y para orientar.
		// Booths en Z: -12, -7, -2, +3, +8  (5 gaps entre pilares)
		{
			const float BOOTH_SCALE = 0.8f;
			const float boothZ[5] = { -12.0f, -7.0f, -2.0f, 3.0f, 8.0f };

			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			for (int bi = 0; bi < 5; bi++) {
				float zc = boothZ[bi];

				// Lado pared (X = -5): +90°X (pararlo) + 180°Y (voltearlo al frente) + 90°Y (orientar)
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(-3.5f, 0.9f, zc));
				model = glm::rotate(model, glm::radians(90.0f),  glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(BOOTH_SCALE));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glDisable(GL_CULL_FACE); ExpoBooths_M.RenderModel(); glEnable(GL_CULL_FACE);

				// Lado barda (X = +5): simétrico
				model = glm::mat4(1.0f);
				model = glm::translate(model, glm::vec3(3.5f, 0.9f, zc));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(BOOTH_SCALE));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glDisable(GL_CULL_FACE); ExpoBooths_M.RenderModel(); glEnable(GL_CULL_FACE);
			}
		}

		// ChicaUnam
		{
			float wc       = chica_walkCycle;
			float walkAmt  = chicaWalking ? 1.0f : 0.0f;
			float legSwing = sinf(wc) * 25.0f * walkAmt;
			float kneeFlx  = fabsf(sinf(wc)) * 18.0f * walkAmt;
			float bodyBob  = fabsf(sinf(wc)) * 0.04f * walkAmt;
			glm::vec3 chBase(chica_posX, bodyBob, chica_posZ);
			float faceYaw = (chica_dir > 0) ? 0.0f : 180.0f;
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
			auto chPart = [&](glm::vec3 offset, float rotX, Model& mdl) {
				glm::mat4 m = glm::mat4(1.0f);
				m = glm::translate(m, chBase);
				m = glm::rotate(m, glm::radians(faceYaw), glm::vec3(0.0f,1.0f,0.0f));
				m = glm::translate(m, offset);
				if (rotX != 0.0f)
					m = glm::rotate(m, glm::radians(rotX), glm::vec3(1.0f,0.0f,0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
				glDisable(GL_CULL_FACE); mdl.RenderModel(); glEnable(GL_CULL_FACE);
			};
			auto chPartInherited = [&](glm::mat4 parentM, glm::vec3 offset, float rotX, Model& mdl) -> glm::mat4 {
				glm::mat4 m = parentM;
				m = glm::translate(m, offset);
				if (rotX != 0.0f)
					m = glm::rotate(m, glm::radians(rotX), glm::vec3(1.0f,0.0f,0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(m));
				glDisable(GL_CULL_FACE); mdl.RenderModel(); glEnable(GL_CULL_FACE);
				return m;
			};

			chPart(glm::vec3( 0.0f, 0.90f, 0.0f), 0.0f, ChicaUnam_Cuerpo_M);

			// Muslo der
			{
				const float MUSLO_LEN = 0.38f;
				glm::mat4 mMuslo = glm::mat4(1.0f);
				mMuslo = glm::translate(mMuslo, chBase);
				mMuslo = glm::rotate(mMuslo, glm::radians(faceYaw), glm::vec3(0,1,0));
				mMuslo = glm::translate(mMuslo, glm::vec3( 0.10f, 0.88f, 0.0f));
				mMuslo = glm::rotate(mMuslo, glm::radians(legSwing), glm::vec3(1,0,0));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mMuslo));
				glDisable(GL_CULL_FACE); ChicaUnam_MusloDer_M.RenderModel(); glEnable(GL_CULL_FACE);
				chPartInherited(mMuslo, glm::vec3(0.0f, -MUSLO_LEN, 0.0f), kneeFlx, ChicaUnam_PiernaDer_M);
			}
			// Muslo izq
			{
				const float MUSLO_LEN = 0.38f;
				glm::mat4 mMuslo = glm::mat4(1.0f);
				mMuslo = glm::translate(mMuslo, chBase);
				mMuslo = glm::rotate(mMuslo, glm::radians(faceYaw), glm::vec3(0,1,0));
				mMuslo = glm::translate(mMuslo, glm::vec3(-0.10f, 0.88f, 0.0f));
				mMuslo = glm::rotate(mMuslo, glm::radians(-legSwing), glm::vec3(1,0,0));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mMuslo));
				glDisable(GL_CULL_FACE); ChicaUnam_MusloIzq_M.RenderModel(); glEnable(GL_CULL_FACE);
				chPartInherited(mMuslo, glm::vec3(0.0f, -MUSLO_LEN, 0.0f), kneeFlx, ChicaUnam_PiernaIzq_M);
			}
		}

		// ─── ORRERY ──────────────────────────────────────────────────────
		{
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

			// Núcleo: posición fija sobre la mesa
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(ORRERY_X, ORRERY_Y, ORRERY_Z));
			model = glm::scale(model, glm::vec3(0.015f));
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glDisable(GL_CULL_FACE); Orrery_Nucleo_M.RenderModel(); glEnable(GL_CULL_FACE);

			// Planetas: cada uno en su órbita
			// Color distinto por planeta para distinguirlos
			const glm::vec3 planetColors[8] = {
				glm::vec3(0.80f, 0.80f, 0.80f), // P1 gris (Mercurio-ish)
				glm::vec3(0.95f, 0.85f, 0.60f), // P2 ocre (Venus-ish)
				glm::vec3(0.30f, 0.55f, 0.95f), // P3 azul (Tierra-ish)
				glm::vec3(0.85f, 0.40f, 0.25f), // P4 rojo (Marte-ish)
				glm::vec3(0.85f, 0.75f, 0.55f), // P5 naranja pálido (Júpiter-ish)
				glm::vec3(0.90f, 0.85f, 0.65f), // P6 dorado (Saturno-ish)
				glm::vec3(0.60f, 0.85f, 0.95f), // P7 cian (Urano-ish)
				glm::vec3(0.35f, 0.45f, 0.90f), // P8 azul oscuro (Neptuno-ish)
			};

			// Planetas: mismo origen que el núcleo, giran en su propio eje Y
			for (int oi = 0; oi < 8; oi++) {
				glUniform3fv(uniformColor, 1, glm::value_ptr(planetColors[oi]));
				model = glm::mat4(1.0f);
				// Mismo origen que el núcleo + pequeño offset Y solo para los planetas
				model = glm::translate(model, glm::vec3(ORRERY_X, ORRERY_Y + 0.18f, ORRERY_Z));
				// Autorotación sobre su propio eje Y (cada planeta a su velocidad)
				model = glm::rotate(model, glm::radians(orrery_angles[oi]), glm::vec3(0.0f, 1.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.008f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
				glDisable(GL_CULL_FACE); Orrery_Planeta_M[oi].RenderModel(); glEnable(GL_CULL_FACE);
			}
		}

		// ─── DRON ────────────────────────────────────────────────────────
		{
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);

			// Matriz base del dron (posición orbital + yaw)
			glm::mat4 dronBase = glm::mat4(1.0f);
			dronBase = glm::translate(dronBase, glm::vec3(dron_x, dron_y, dron_z));
			dronBase = glm::rotate(dronBase, glm::radians(dron_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
			dronBase = glm::scale(dronBase, glm::vec3(0.05f));

			// Cuerpo
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(dronBase));
			glDisable(GL_CULL_FACE); Dron_Cuerpo_M.RenderModel(); glEnable(GL_CULL_FACE);

			// 4 hélices: offsets en espacio local del dron (frente-izq, frente-der, atrás-izq, atrás-der)
			// Las hélices alternas giran en sentido contrario (realismo)
			const glm::vec3 heliceOffsets[4] = {
				glm::vec3(-11.0f, 1.0f,  11.0f), // FL
				glm::vec3( 11.0f, 1.0f,  11.0f), // FR
				glm::vec3(-11.0f, 1.0f, -11.0f), // BL
				glm::vec3( 11.0f, 1.0f, -11.0f), // BR
			};
			const float heliceSigns[4] = { 1.0f, -1.0f, -1.0f, 1.0f }; // CW/CCW alternado

			for (int hi = 0; hi < 4; hi++) {
				glm::mat4 mH = dronBase;
				mH = glm::translate(mH, heliceOffsets[hi]);
				mH = glm::rotate(mH, glm::radians(dron_helixAngle * heliceSigns[hi]), glm::vec3(0.0f, 1.0f, 0.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mH));
				glDisable(GL_CULL_FACE); Dron_Helice_M.RenderModel(); glEnable(GL_CULL_FACE);
			}
		}

		// ─── OFFROAD ─────────────────────────────────────────────────────
		// Mesa booth Z=-2, lado pared: xs_pared = -5 + 1.30 + 0.35 = -3.35
		// Superficie de la mesa: mesaH(0.75) + mesaAlto(0.07) = 0.82
		{
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			const float MESA_X   = -3.35f;
			const float MESA_Y   =  0.90f;  // justo encima de la mesa
			const float MESA_Z   = -2.0f;
			const float OR_SCALE =  0.003f;

			// Matriz base: translate → rotar -90°X para pararlo → escalar
			glm::mat4 autoBase = glm::mat4(1.0f);
			autoBase = glm::translate(autoBase, glm::vec3(MESA_X, MESA_Y, MESA_Z + offroad_posZ));
			autoBase = glm::rotate(autoBase, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
			autoBase = glm::scale(autoBase, glm::vec3(OR_SCALE));

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(autoBase));
			glDisable(GL_CULL_FACE); Offroad_Auto_M.RenderModel(uniformColor); glEnable(GL_CULL_FACE);

			// Llantas: heredan autoBase (jerarquía completa)
			// Tras rotate -90°X en autoBase: eje Y local del modelo = eje de rodadura real de la llanta
			struct { Model* m; glm::vec3 off; float sign; } llantas[4] = {
				{ &Offroad_LlantaSupDer_M, glm::vec3( 55.0f, -40.0f, 0.0f),  1.0f },
				{ &Offroad_LlantaSupIzq_M, glm::vec3(-55.0f, -40.0f, 0.0f), -1.0f },
				{ &Offroad_LlantaInfDer_M, glm::vec3( 55.0f,  40.0f, 0.0f),  1.0f },
				{ &Offroad_LlantaInfIzq_M, glm::vec3(-55.0f,  40.0f, 0.0f), -1.0f },
			};
			for (int li = 0; li < 4; li++) {
				glm::mat4 mL = autoBase;
				mL = glm::translate(mL, llantas[li].off);
				// Parar la llanta y orientarla paralela al vehículo
				mL = glm::rotate(mL, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				mL = glm::rotate(mL, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
				// Rodadura: eje Z local (tras 90°X y -90°Y el eje de rodadura queda en Z)
				mL = glm::rotate(mL, glm::radians(offroad_llanta * llantas[li].sign), glm::vec3(0.0f, 0.0f, 1.0f));
				glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(mL));
				glDisable(GL_CULL_FACE); llantas[li].m->RenderModel(uniformColor); glEnable(GL_CULL_FACE);
			}
		}

		// ─── PALOMA ──────────────────────────────────────────────────────
		{
			color = glm::vec3(1.0f, 1.0f, 1.0f);
			glUniform3fv(uniformColor, 1, glm::value_ptr(color));
			Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

			const float PAL_SCALE = 0.06f;

			// Yaw suavizado: atan2 de la dirección de movimiento del keyframe actual
			int ki = paloma_playIndex;
			float dx = paloma_kf[ki].xInc;
			float dz = paloma_kf[ki].zInc;
			float yawReal = 0.0f;
			if (dx != 0.0f || dz != 0.0f)
				yawReal = glm::degrees(atan2f(dx, dz));

			// Suavizado del yaw: interpolar gradualmente hacia yawReal usando paloma_aleteo
			// como referencia de tiempo, pero con una variable dedicada
			paloma_yaw += (yawReal - paloma_yaw) * 0.02f; // lerp lento = giro suave

			// Matriz base: posición interpolada por keyframes + yaw suavizado + escala
			glm::mat4 palomaBase = glm::mat4(1.0f);
			palomaBase = glm::translate(palomaBase, glm::vec3(paloma_x, paloma_y, paloma_z));
			palomaBase = glm::rotate(palomaBase, glm::radians(paloma_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
			palomaBase = glm::scale(palomaBase, glm::vec3(PAL_SCALE));

			// Solo cuerpo — las alas están desactivadas
			// Se pasa uniformColor para que use los colores del material del GLB
			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(palomaBase));
			glDisable(GL_CULL_FACE); Paloma_Cuerpo_M.RenderModel(uniformColor); glEnable(GL_CULL_FACE);
		}

		// Presentar frame
		glFlush();
		mainWindow.swapBuffers();
	}

	ma_sound_uninit(&bgm);
	ma_engine_uninit(&audioEngine);

	return 0;
}
