#include "gl/glew.h"

#include "sdl2/sdl.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "imgui/imgui.h"

#include "Cube.h"
#include "HalfCube.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"
#include "Shader.h"
#include "Object.h"
#include "Camera.h"
#include "SimulationData.h"

#include <iostream>
#include <fstream>

// TODO: this should be loaded from the user interface
const std::string DATA_FILE_NAME = "demo.json";

unsigned SPHERE_DETAIL = 40;
unsigned CONE_DETAIL = 18;
unsigned CONE_CIRCLES = 4;

unsigned CYLINDER_DETAIL = 18;
unsigned CYLINDER_CIRCLES = 2;

GLfloat s_camMovementSpeed = 0.25f;
static glm::vec4 FORWARD(0.0f, 0.0f, -1.0f, 1.0f);

glm::vec4 FILL_COLOR_ANIMAL(1,1,1,1);

glm::vec4 FILL_COLOR_SENSOR[] =
{
	glm::vec4(1,1,0,1),
	glm::vec4(1,0,0,1),
	glm::vec4(0,1,0,1),
	glm::vec4(0,0,1,1)
};

glm::vec4 FILL_COLOR_SENSOR_SPHERE[] =
{
	glm::vec4(1,1,0,1),
	glm::vec4(1,0,0,1),
	glm::vec4(0,1,0,1),
	glm::vec4(0,0,1,1)
};

glm::vec4 FILL_COLOR_FOOD[] =
{
	glm::vec4(1,1,0,1),
	glm::vec4(1,0,0,1),
	glm::vec4(0,1,0,1),
	glm::vec4(0,0,1,1),
	glm::vec4(0,1,1,1),
};
glm::vec4 FILL_COLOR_FOOD_SPHERE[] =
{
	glm::vec4(1,1,0,0.3),
	glm::vec4(1,0,0,0.3),
	glm::vec4(0,1,0,0.3),
	glm::vec4(0,0,1,0.3),
	glm::vec4(0,1,1,0.3),
};
glm::vec4 HALO_COLOR_ANIMAL(1, 1, 1, 0);
glm::vec4 HALO_COLOR_SENSOR(0, 0, 0, 1);
glm::vec4 HALO_COLOR_FOOD[] =
{
	glm::vec4(0,0,0,1),
	glm::vec4(0,0,0,1),
	glm::vec4(0,0,0,1),
	glm::vec4(0,0,0,1),
	glm::vec4(0,0,0,1),
};
glm::vec4 HALO_COLOR_FOOD_SPHERE[] =
{
	glm::vec4(0,0,0,0.2),
	glm::vec4(0,0,0,0.2),
	glm::vec4(0,0,0,0.2),
	glm::vec4(0,0,0,0.2),
	glm::vec4(0,0,0,0.2),
};
glm::vec3 LIGHT_COLOR(1.0);
glm::vec3 LIGHT_DIRECTION=glm::normalize(glm::vec3(-0.5, -0.3, -1));
glm::vec3 LIGHT_POSITION(0.0, 0.0, 0.0);
glm::vec3 LIGHT_AMBIENT_COLOR(0.0);
glm::vec3 LIGHT_ATTENUATION(1.0, 0.09, 0.032);

SDL_Window* window;
SDL_GLContext context;
float deltaTime = 0, totalTime = 0, alpha=glm::radians(90.0f);

Cube* cube = NULL;
Sphere* sphere = NULL;
Cone* cone = NULL;
Cylinder* cylinder = NULL;
Shader* phong_directional = NULL;
Shader* phong_point = NULL;
Shader* unlit = NULL;
Shader* gui = NULL;
Object* animal = NULL;
Object* background = NULL;
std::vector<std::vector<Object*>> lines;
std::vector<Object*> sensors;
std::vector<Object*> foods;
std::vector<Object*> foodsSphere;
Camera* mainCamera = NULL;
SimulationData* simData = NULL, *simData_ = NULL;
glm::vec3 cubeMinCoord(999,999,999), cubeMaxCoord(-999,-999,-999);

size_t numFrames = 0;
size_t maxFrames = 1000;

// Control variables
static int cameraMode = 0, frame = 0;
static float t = 0.1, z = 0.02, a = 0.1, h = 1.5, l = 0.003;
static bool bckVisible = true, linesVisible = true;

glm::vec3 cameraRotation, cameraPosition(glm::vec3(0, 0, 10));
unsigned currentFrame = 0;
int animSpeed = 60;
bool animating = false;
float animDelta = 0;

std::vector<Food> refFoodPositions;

std::ostream& operator << (std::ostream& lhs, const glm::vec3& rhs)
{
	lhs << "(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
	return lhs;
}

void updateVisibility()
{
	GLfloat cutoff = simData->getData().m_structure.m_angle;

	int areVisible[5] = { 0,0,0,0,0 };
	int whichCanSeeBest[5] = { 0,0,0,0,0 };
	int biggestIntensity[5] = { 0,0,0,0,0 };

	for (int i = 0;i < sensors.size();i++)
	{
		glm::vec3 OE = glm::normalize(sensors[i]->getPosition() - animal->getPosition());
				
		for (int j = 0; j < foods.size(); j++)
		{
			glm::vec3 EK = glm::normalize(foods[j]->getPosition() - sensors[i]->getPosition());
			GLfloat angle = glm::acos(glm::dot(OE, EK));

			if (angle < cutoff / 2.0f)
			{
				glm::vec3 sensorPos = sensors[i]->getPosition();
				glm::vec3 foodPos = foods[j]->getPosition();

				float distance = glm::distance(sensorPos, foodPos);
				glm::vec3 center = (sensorPos + foodPos) / 2.0f;
				glm::vec3 dir = foodPos - sensorPos;

				lines[j][i]->setPosition(center);
				lines[j][i]->setScale(glm::vec3(l, distance, l));
				lines[j][i]->setFillColor(glm::vec4(1,1,1,1));
				lines[j][i]->lookAt(foodPos);
				if (linesVisible)
				{
					lines[j][i]->setScale(glm::vec3(l, distance, l));
				}
				else
					lines[j][i]->setScale(glm::vec3(0.0f));

				areVisible[j] = 1;
			}
			else
			{
				lines[j][i]->setFillColor(glm::vec4(0, 0, 0, 0));
			}
		}

		for (int i = 0; i < 5;i++)
		{
			if (areVisible[i] == 1)
			{
				foodsSphere[i]->setScale(glm::vec3(t*h));
			}
			else 
				foodsSphere[i]->setScale(glm::vec3(0.0, 0.0, 0.0));
		}
	}
}

void initRenderSystem()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	glCullFace(GL_BACK);
}

void initFood(const unsigned& frameId)
{
	for (unsigned i = 0; i < simData->getData().m_frames[frameId].m_foods.size(); ++i)
	{
		const auto& foodData = simData->getData().m_frames[frameId].m_foods[i];

		Object* food = new Object();
		food->setMesh(sphere);
		food->setFillColor(FILL_COLOR_FOOD[glm::min<unsigned>(i, sizeof(FILL_COLOR_FOOD) / sizeof(glm::vec4) - 1)]);
		food->setHaloColor(HALO_COLOR_FOOD[glm::min<unsigned>(i, sizeof(HALO_COLOR_FOOD) / sizeof(glm::vec4) - 1)]);
		food->setScale(glm::vec3(0.1));
		food->setPosition(foodData.m_position);
		food->setShader(phong_directional);
		foods.push_back(food);

		Object* foodSphere = new Object();
		foodSphere->setMesh(sphere);
		foodSphere->setFillColor(FILL_COLOR_FOOD_SPHERE[glm::min<unsigned>(i, sizeof(FILL_COLOR_FOOD) / sizeof(glm::vec4) - 1)]);
		foodSphere->setHaloColor(HALO_COLOR_FOOD_SPHERE[glm::min<unsigned>(i, sizeof(HALO_COLOR_FOOD) / sizeof(glm::vec4) - 1)]);
		foodSphere->setScale(glm::vec3(0.1, 0.1, 0.1));
		foodSphere->setPosition(foodData.m_position);
		foodSphere->setShader(phong_directional);
		foodsSphere.push_back(foodSphere);

		std::vector<Object*> sensorLines(simData->getData().m_structure.m_sensors.size());
		for (unsigned i = 0;i < sensorLines.size();++i)
		{
			Object* line = new Object();
			line->setMesh(cylinder);
			line->setHaloColor(glm::vec4(0, 0, 0, 0));
			line->setFillColor(glm::vec4(0, 0, 0, 1));
			line->setScale(glm::vec3(0.1, 0.1, 0.1));
			line->setShader(phong_directional);
			sensorLines[i] = line;
		}
		lines.push_back(sensorLines);
	}
}

glm::vec4 getBarycentricCoordinates(glm::vec3 forThis, glm::vec3 R1, glm::vec3 R2, glm::vec3 R3, glm::vec3 R4) {

	glm::mat4 Ainv;
	glm::mat4 A;

	A[0][0] = R1.x - R4.x;
	A[1][0] = R2.x - R4.x;
	A[2][0] = R3.x - R4.x;
	A[0][1] = R1.y - R4.y;
	A[1][1] = R2.y - R4.y;
	A[2][1] = R3.y - R4.y;
	A[0][2] = R1.z - R4.z;
	A[1][2] = R2.z - R4.z;
	A[2][2] = R3.z - R4.z;

	Ainv = glm::inverse(A);

	glm::vec4 lambda_hom = Ainv*glm::vec4(forThis - R4, 1);
	glm::vec3 lambda = glm::vec3(lambda_hom.x / lambda_hom.w, lambda_hom.y / lambda_hom.w, lambda_hom.z / lambda_hom.w);

	return glm::vec4(lambda.x, lambda.y, lambda.z, 1 - lambda.x - lambda.y - lambda.z);
}

glm::vec3 getPointInBarycentricSystem(const glm::vec4& lambda2, const glm::vec3& R1, const glm::vec3& R2, const glm::vec3& R3, const glm::vec3& R4) {
	return glm::vec3(
		lambda2.x * R1.x + lambda2.y * R2.x + lambda2.z * R3.x + lambda2.w * R4.x,
		lambda2.x * R1.y + lambda2.y * R2.y + lambda2.z * R3.y + lambda2.w * R4.y,
		lambda2.x * R1.z + lambda2.y * R2.z + lambda2.z * R3.z + lambda2.w * R4.z);
}

void calcCubeSize(float margin = 1.0f)
{
	for (const auto& temp_food : simData->getData().m_frames[0].m_foods)
	{
		cubeMinCoord = glm::min(cubeMinCoord, temp_food.m_position + margin);
		cubeMaxCoord = glm::max(cubeMaxCoord, temp_food.m_position + margin);
		cubeMinCoord = glm::min(cubeMinCoord, temp_food.m_position - margin);
		cubeMaxCoord = glm::max(cubeMaxCoord, temp_food.m_position - margin);
	}

	for (unsigned i = 0; i < numFrames; ++i)
	{
		std::vector<Food> foods2 = simData->getData().m_frames[i].m_foods;
		GLint resetIndex = -1;

		for (int j = 0; j < 5; j++)
		{
			if (foods2[j].m_reset == 1)
				resetIndex = j;
		}

		if (resetIndex != -1)
		{
			glm::vec4 lambda = getBarycentricCoordinates(foods2[resetIndex].m_position,
				foods2[(resetIndex + 1) % 5].m_position,
				foods2[(resetIndex + 2) % 5].m_position,
				foods2[(resetIndex + 3) % 5].m_position,
				foods2[(resetIndex + 4) % 5].m_position);

			glm::vec3 updatedFood = getPointInBarycentricSystem(lambda,
				refFoodPositions[(resetIndex + 1) % 5].m_position,
				refFoodPositions[(resetIndex + 2) % 5].m_position,
				refFoodPositions[(resetIndex + 3) % 5].m_position,
				refFoodPositions[(resetIndex + 4) % 5].m_position);

			refFoodPositions[resetIndex].m_position.x = updatedFood.x;
			refFoodPositions[resetIndex].m_position.y = updatedFood.y;
			refFoodPositions[resetIndex].m_position.z = updatedFood.z;

			for (int j = 0; j < 5; j++)
			{
				cubeMinCoord = glm::min(cubeMinCoord, refFoodPositions[j].m_position + margin);
				cubeMaxCoord = glm::max(cubeMaxCoord, refFoodPositions[j].m_position + margin);
				cubeMinCoord = glm::min(cubeMinCoord, refFoodPositions[j].m_position - margin);
				cubeMaxCoord = glm::max(cubeMaxCoord, refFoodPositions[j].m_position - margin);
			}
		}

		glm::vec4 lambda = getBarycentricCoordinates(glm::vec3(0, 0, 0),
			foods2[0].m_position, foods2[1].m_position, foods2[2].m_position, foods2[3].m_position);

		glm::vec3 newPos = getPointInBarycentricSystem(lambda,
			refFoodPositions[0].m_position,
			refFoodPositions[1].m_position,
			refFoodPositions[2].m_position,
			refFoodPositions[3].m_position);

		cubeMinCoord = glm::min(cubeMinCoord, newPos - margin);
		cubeMaxCoord = glm::max(cubeMaxCoord, newPos - margin);
		cubeMinCoord = glm::min(cubeMinCoord, newPos + margin);
		cubeMaxCoord = glm::max(cubeMaxCoord, newPos + margin);
	}
}

void initResources()
{
	cube = new Cube();
	sphere = new Sphere(SPHERE_DETAIL);
	cone = new Cone(CONE_DETAIL, CONE_CIRCLES);
	cylinder = new Cylinder(CYLINDER_DETAIL, CYLINDER_CIRCLES);
	phong_directional = new Shader("phong_directional");
	phong_point = new Shader("phong_point");
	unlit = new Shader("unlit");
	gui = new Shader("imgui");

	simData = new SimulationData(DATA_FILE_NAME);
	numFrames = glm::min(simData->getData().m_frames.size(), maxFrames);
	
	refFoodPositions = simData->getData().m_frames[0].m_foods;
	calcCubeSize();
	cameraPosition.x = (cubeMinCoord.x + cubeMaxCoord.x) * 0.5f;
	cameraPosition.y = (cubeMinCoord.y + cubeMaxCoord.y) * 0.5f;
	cameraPosition.z = glm::max(cubeMaxCoord.x-cubeMinCoord.x, glm::max(cubeMaxCoord.y - cubeMinCoord.y, cubeMaxCoord.z - cubeMinCoord.z));

	refFoodPositions = simData->getData().m_frames[0].m_foods;

	background = new Object();
	background->setMesh(cube);
	background->setHaloColor(glm::vec4(0, 0, 0, 1));
	background->setFillColor(glm::vec4(0.2, 0.3, 0.3, 0));
	background->setPosition((cubeMinCoord+cubeMaxCoord)*0.5f);
	background->setScale(cubeMaxCoord-cubeMinCoord);
	background->setShader(phong_point);

	animal = new Object();
	animal->setMesh(sphere);
	animal->setFillColor(FILL_COLOR_ANIMAL);
	animal->setHaloColor(HALO_COLOR_ANIMAL);
	animal->setScale(glm::vec3(0.15, 0.15, 0.15));
	animal->setShader(phong_directional);

	std::vector<Food> foods2 = simData->getData().m_frames[0].m_foods;

	int i = 0;
	for (const auto& sensorData : simData->getData().m_structure.m_sensors)
	{
		glm::vec3 eyeInit = animal->getScale() * 
			glm::vec3(
				sin(sensorData.m_theta)*cos(sensorData.m_phi), 
				sin(sensorData.m_theta)*sin(sensorData.m_phi), 
				cos(sensorData.m_theta));
		glm::vec4 lambdaForEye = getBarycentricCoordinates(eyeInit,
			foods2[0].m_position, foods2[1].m_position, foods2[2].m_position, foods2[3].m_position);

		glm::vec3 newPosForEye = getPointInBarycentricSystem(lambdaForEye,
			refFoodPositions[0].m_position,
			refFoodPositions[1].m_position,
			refFoodPositions[2].m_position,
			refFoodPositions[3].m_position);

		Object* sensor;
		sensor = new Object();
		sensor->setMesh(sphere);
		sensor->setFillColor(FILL_COLOR_SENSOR[i]);
		sensor->setHaloColor(HALO_COLOR_SENSOR);
		sensor->setScale(glm::vec3(0.05, 0.05, 0.05));
		sensor->setPosition(newPosForEye);
		sensor->setRotation(glm::vec3(0, -sensorData.m_phi, -sensorData.m_theta));
		sensor->setShader(phong_directional);
		sensors.push_back(sensor);
		++i;
	}

	initFood(0);
	updateVisibility();

	mainCamera = new Camera();
	mainCamera->setPosition(cameraPosition);
	mainCamera->setRotation(cameraRotation);
	mainCamera->setNearZ(0.01f);
	mainCamera->setFarZ(1000.0f);

	int width, height;
	SDL_GetWindowSize(window, &width, &height);
	mainCamera->setAspectRatio((float)width / (float)height);
}

void initImgui()
{
	// Initialize imgui.
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	int width, height;
	SDL_GetWindowSize(window, &width, &height);

	io.DisplaySize = ImVec2(width, height);
	io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
	io.RenderDrawListsFn = nullptr;

	// Extract the default imgui font
	int fontWidth, fontHeight;
	unsigned char* pixels;
	ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &fontWidth, &fontHeight);

	GLuint font;

	// Upload the texture data.
	glGenTextures(1, &font);
	glBindTexture(GL_TEXTURE_2D, font);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fontWidth, fontHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 16.0f);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	io.Fonts->TexID = (void*)font;

	io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDLK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDLK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDLK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDLK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDLK_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDLK_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDLK_HOME;
	io.KeyMap[ImGuiKey_End] = SDLK_END;
	io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SDLK_a;
	io.KeyMap[ImGuiKey_C] = SDLK_c;
	io.KeyMap[ImGuiKey_V] = SDLK_v;
	io.KeyMap[ImGuiKey_X] = SDLK_x;
	io.KeyMap[ImGuiKey_Y] = SDLK_y;
	io.KeyMap[ImGuiKey_Z] = SDLK_z;
}

bool init()
{
	SDL_LogSetAllPriority(SDL_LOG_PRIORITY_VERBOSE);

	if (SDL_Init(SDL_INIT_EVERYTHING)<0)
	{
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to init SDL");
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(0, &displayMode);
	window = SDL_CreateWindow("AnimalSimulation Framework", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE);
	context = SDL_GL_CreateContext(window);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK)
	{
		SDL_LogError(SDL_LOG_CATEGORY_SYSTEM, "Failed to init GLEW, cause: %s", glewGetErrorString(err));
		return false;
	}

	initRenderSystem();
	initResources();
	initImgui();

	return true;
}

void releaseFood()
{
	for (Object* object : foods)
		delete object;
	foods.clear();
}

void releaseResources()
{
	delete simData;
	delete mainCamera;
	releaseFood();
	
	for (Object* object : sensors)
		delete object;

	delete animal;
	delete phong_directional;
	delete phong_point;
	delete unlit;
	
	delete cone;
	delete sphere;
}

void cleanUp()
{
	releaseResources();

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);

	SDL_Quit();
}

void renderGui()
{
	// Scene options window
	ImGui::SetNextWindowPos(ImVec2(100, 600), ImGuiSetCond_FirstUseEver);
	if (ImGui::Begin("Options", nullptr, ImVec2(250, 350), -1.0f, ImGuiWindowFlags_MenuBar))
	{
		// Menu bar
		if (ImGui::BeginMenuBar())
		{
			// Menu
			if (ImGui::BeginMenu("Menu"))
			{
				if (ImGui::MenuItem("Quit"))
				{
					SDL_Event ev;
					ev.type = SDL_QUIT;
					SDL_PushEvent(&ev);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		// Camera mode
		ImGui::RadioButton("Default", &cameraMode, 0); ImGui::SameLine();
		ImGui::RadioButton("Yellow", &cameraMode, 1); ImGui::SameLine();
		ImGui::RadioButton("Red", &cameraMode, 2); ImGui::SameLine();
		ImGui::RadioButton("Blue", &cameraMode, 3); ImGui::SameLine();
		ImGui::RadioButton("Green", &cameraMode, 4);

		ImGui::Separator();

		// Sizes
		ImGui::SliderFloat("Size of foods", &t, 0.02, 0.3f);
		ImGui::SliderFloat("Size of food halo", &h, 1.0, 2.0f);
		ImGui::SliderFloat("Size of sensors", &z, 0.02, 0.1f);
		ImGui::SliderFloat("Size of animal", &a, 0.02, 0.3f);
		ImGui::SliderFloat("Width of visibility lines", &l, 0.002, 0.03f);

		// Change the food sizes
		for (const auto& food : foods)
			food->setScale(glm::vec3(t));

		// Change the sensor sizes
		for (const auto& sensor : sensors)
			sensor->setScale(glm::vec3(z));

		animal->setScale(glm::vec3(a));

		ImGui::Checkbox("Show background", &bckVisible);
		ImGui::Checkbox("Show lines", &linesVisible);

		if (ImGui::Button("-"))
		{
			if (frame > 0)
				--frame;
		}

		ImGui::SameLine();

		if (ImGui::Button("+"))
		{
			if (frame < numFrames - 2)
				++frame;
		}
		ImGui::SameLine();

		
		// Frames slider
		ImGui::SliderInt("Frame", &frame, 0, numFrames - 1);
		ImGui::SliderInt("Animation Speed", &animSpeed, 0, 120);

		if (ImGui::Button("Play"))
		{
			animating = true;
			animDelta = 0;
		}
		ImGui::SameLine();

		if (ImGui::Button("Pause"))
		{
			animating = false;
		}
		ImGui::SameLine();

		if (ImGui::Button("Stop"))
		{
			animating = false;
			frame = 0;
		}

		// Switch the background rendering
		if (bckVisible == true)
			background->setFillColor(glm::vec4(0.2, 0.3, 0.3, 1.0));
		else
			background->setFillColor(glm::vec4(0.2, 0.3, 0.3, 0.0));

		//Frames
		if (currentFrame > frame)
		{
			for (int i = 0; i < foods.size(); i++)
			{
				foods[i]->setPosition(simData->getData().m_frames[0].m_foods[i].m_position);
				foodsSphere[i]->setPosition(simData->getData().m_frames[0].m_foods[i].m_position);
			}

			refFoodPositions = simData->getData().m_frames[0].m_foods;
			currentFrame = 0;
		}

		for (int tempFrame = currentFrame; tempFrame <= frame; ++tempFrame)
		{
			GLint resetIndex = -1;

			std::vector<Food> const& foods2 = simData->getData().m_frames[tempFrame].m_foods;

			for (int i = 0; i < 5; i++)
			{
				if (foods2[i].m_reset == 1)
				{
					resetIndex = i;
				}
			}

			if (resetIndex != -1)
			{
				glm::vec4 lambda = getBarycentricCoordinates(foods2[resetIndex].m_position,
					foods2[(resetIndex + 1) % 5].m_position,
					foods2[(resetIndex + 2) % 5].m_position,
					foods2[(resetIndex + 3) % 5].m_position,
					foods2[(resetIndex + 4) % 5].m_position);

				glm::vec3 updatedFood = getPointInBarycentricSystem(lambda,
					refFoodPositions[(resetIndex + 1) % 5].m_position,
					refFoodPositions[(resetIndex + 2) % 5].m_position,
					refFoodPositions[(resetIndex + 3) % 5].m_position,
					refFoodPositions[(resetIndex + 4) % 5].m_position);

				foods[resetIndex]->setPosition(updatedFood);
				foodsSphere[resetIndex]->setPosition(updatedFood);

				refFoodPositions[resetIndex].m_position.x = updatedFood.x;
				refFoodPositions[resetIndex].m_position.y = updatedFood.y;
				refFoodPositions[resetIndex].m_position.z = updatedFood.z;
			}
		}
		currentFrame = frame;

		// Finish generating the window
		ImGui::End();
	}

	std::vector<Food> const& foods2 = simData->getData().m_frames[currentFrame].m_foods;

	glm::vec4 lambda = getBarycentricCoordinates(glm::vec3(0, 0, 0),
		foods2[0].m_position, foods2[1].m_position, foods2[2].m_position, foods2[3].m_position);

	glm::vec3 newPos = getPointInBarycentricSystem(lambda,
		refFoodPositions[0].m_position,
		refFoodPositions[1].m_position,
		refFoodPositions[2].m_position,
		refFoodPositions[3].m_position);

	for (int i = 0;i < simData->getData().m_structure.m_sensors.size();i++)
	{
		glm::vec3 eyeInit = animal->getScale() * glm::vec3(sin(simData->getData().m_structure.m_sensors[i].m_theta) *
			cos(simData->getData().m_structure.m_sensors[i].m_phi),
			sin(simData->getData().m_structure.m_sensors[i].m_theta) *
			sin(simData->getData().m_structure.m_sensors[i].m_phi),
			cos(simData->getData().m_structure.m_sensors[i].m_theta));
		glm::vec4 lambdaForEye = getBarycentricCoordinates(eyeInit,
			foods2[0].m_position, foods2[1].m_position, foods2[2].m_position, foods2[3].m_position);

		glm::vec3 newPosForEye = getPointInBarycentricSystem(lambdaForEye,
			refFoodPositions[0].m_position,
			refFoodPositions[1].m_position,
			refFoodPositions[2].m_position,
			refFoodPositions[3].m_position);

		sensors[i]->setPosition(newPosForEye);
	}

	animal->setPosition(newPos);

	// Camera mode: default
	if (cameraMode == 0)
	{
		mainCamera->setPosition(cameraPosition);
		mainCamera->setRotation(cameraRotation);
		mainCamera->setParent(nullptr);
		mainCamera->setFov(glm::radians(60.0f));
	}

	// Camera mode: sensor
	else
	{
		glm::vec3 delta = glm::normalize(sensors[cameraMode - 1]->getWorldPosition() - animal->getWorldPosition());

		mainCamera->setPosition(glm::vec3(0, 0, 0));
		mainCamera->setRotation(glm::vec3(glm::half_pi<float>(), 0, 0));
		mainCamera->setParent(sensors[cameraMode - 1]);
		mainCamera->setFov(simData->getData().m_structure.m_angle);
	}

	updateVisibility();
}

glm::vec3 baseX = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 baseY = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 baseZ = glm::vec3(0.0f, 0.0f, 1.0f);

bool input()
{
	SDL_Event ev;
	bool running = true;

	ImGuiIO& io = ImGui::GetIO();

	while (SDL_PollEvent(&ev))
	{
		std::vector<Food> foods2 = simData->getData().m_frames[currentFrame].m_foods;
		GLint resetIndex = -1;

		switch (ev.type)
		{
		case SDL_QUIT:
			running = false;
			break;

		case SDL_WINDOWEVENT_RESIZED:
			mainCamera->setAspectRatio((float)ev.window.data1 / (float)ev.window.data2);
			break;

		case SDL_TEXTINPUT:
			io.AddInputCharactersUTF8(ev.text.text);
			break;

		case SDL_KEYUP:
			io.KeysDown[ev.key.keysym.sym & ~SDLK_SCANCODE_MASK] = false;
			break;

		case SDL_KEYDOWN:
			io.KeysDown[ev.key.keysym.sym & ~SDLK_SCANCODE_MASK] = true;

			switch (ev.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				running = false;
				break;

			case SDLK_LEFT:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL)
				{
					cameraRotation.y += 0.03;//glm::radians(1.0f);
					mainCamera->setRotation(cameraRotation);
				}
				break;

			case SDLK_RIGHT:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL)
				{
					cameraRotation.y -= 0.03;//glm::radians(1.0f);
					mainCamera->setRotation(cameraRotation);
				}
				break;

			case SDLK_UP:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL)
				{
					cameraRotation.x += 0.03;//glm::radians(1.0f);
					mainCamera->setRotation(cameraRotation);
				}
				break;

			case SDLK_DOWN:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL)
				{
					cameraRotation.x -= 0.03;//glm::radians(1.0f);
					mainCamera->setRotation(cameraRotation);
				}
				break;

			case SDLK_o:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL)
				{
					cameraRotation.z += 0.03;//glm::radians(1.0f);
					mainCamera->setRotation(cameraRotation);
				}
				break;

			case SDLK_p:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL)
				{
					cameraRotation.z -= glm::radians(1.0f);
					mainCamera->setRotation(cameraRotation);
				}

			case SDLK_t:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL)
				{
					alpha += glm::radians(1.0f);
					cameraPosition.x = 2 * glm::cos(alpha);
					cameraPosition.z = 2 * glm::sin(alpha);
					mainCamera->setPosition(cameraPosition);
				}
				break;

			case SDLK_z:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL)
				{
					alpha -= glm::radians(1.0f);
					cameraPosition.x = 2 * glm::cos(alpha);
					cameraPosition.z = 2 * glm::sin(alpha);
					mainCamera->setPosition(cameraPosition);
				}
				break;

			case SDLK_w:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL) {
					cameraPosition += mainCamera->getForward() * s_camMovementSpeed;
					mainCamera->setPosition(cameraPosition);
				}
				break;

			case SDLK_s:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL) {
					cameraPosition -= mainCamera->getForward() * s_camMovementSpeed;
					mainCamera->setPosition(cameraPosition);
				}
				break;

			case SDLK_a:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL) {
					cameraPosition -= mainCamera->getRight() * s_camMovementSpeed;
					mainCamera->setPosition(cameraPosition);
				}
				break;

			case SDLK_d:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL) {
					cameraPosition += mainCamera->getRight() * s_camMovementSpeed;
					mainCamera->setPosition(cameraPosition);
				}
				break;

			case SDLK_e:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL) {
					cameraPosition += mainCamera->getUp() * s_camMovementSpeed;
					mainCamera->setPosition(cameraPosition);
				}
				break;

			case SDLK_q:
				if (mainCamera->getParent() == animal || mainCamera->getParent() == NULL) {
					cameraPosition -= mainCamera->getUp() * s_camMovementSpeed;
					mainCamera->setPosition(cameraPosition);
				}
				break;
			}
			break;
		}
	}
	
	io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
	io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
	io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
	io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);

	int mouseX, mouseY;
	auto mouseState = SDL_GetMouseState(&mouseX, &mouseY);

	io.MouseDown[0] = mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) ? 1 : 0;
	io.MouseDown[1] = mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT) ? 1 : 0;
	io.MouseDown[2] = mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE) ? 1 : 0;
	io.MousePos = ImVec2((float)mouseX, (float)mouseY);
	
	return running;
}

void renderObject(Object* object)
{
	glm::mat4 transform = mainCamera->getViewProjection() * object->getModel();
	glm::mat4 view = mainCamera->getViewProjection();
	glm::mat4 model = object->getModel();
	glm::mat4 normal = glm::inverseTranspose(object->getModel());


	if (object->getFillColor().a > 0.0f)
	{	
		Shader* shader = object->getShader();

		shader->use();

		glm::vec3 lightPosition = animal->getWorldPosition() + LIGHT_POSITION;

		glUniformMatrix4fv(shader->getUniformLocation("mTransform"), 1, GL_FALSE, glm::value_ptr(transform));
		glUniformMatrix4fv(shader->getUniformLocation("mView"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(shader->getUniformLocation("mModel"), 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(shader->getUniformLocation("mNormal"), 1, GL_FALSE, glm::value_ptr(normal));

		glUniform4fv(shader->getUniformLocation("vColor"), 1, glm::value_ptr(object->getFillColor()));
		glUniform3fv(shader->getUniformLocation("vLightColor"), 1, glm::value_ptr(LIGHT_COLOR));
		glUniform3fv(shader->getUniformLocation("vLightDirection"), 1, glm::value_ptr(LIGHT_DIRECTION));
		glUniform3fv(shader->getUniformLocation("vLightPosition"), 1, glm::value_ptr(lightPosition));
		glUniform3fv(shader->getUniformLocation("vAmbientColor"), 1, glm::value_ptr(LIGHT_AMBIENT_COLOR));
		glUniform3fv(shader->getUniformLocation("vLightAttenuation"), 1, glm::value_ptr(LIGHT_ATTENUATION));
		

		object->getMesh()->draw();
	}

	if (object->getHaloColor().a > 0.0f)
	{
		unlit->use();

		glUniformMatrix4fv(unlit->getUniformLocation("mTransform"), 1, GL_FALSE, glm::value_ptr(transform));

		glUniform4fv(unlit->getUniformLocation("vColor"), 1, glm::value_ptr(object->getHaloColor()));

		glEnable(GL_POLYGON_OFFSET_LINE);

		glPolygonOffset(-0.1, 5);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_POLYGON_OFFSET_LINE);
	}
}

void renderImgui()
{
	ImGuiIO& io{ ImGui::GetIO() };

	// Begin an imgui frame
	ImGui::NewFrame();

	renderGui();

	// Render the GUI
	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();

	int fb_width{ (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x) };
	int fb_height{ (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y) };

	drawData->ScaleClipRects(io.DisplayFramebufferScale);

	// Backup GL state
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
	GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
	GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Bind the imgui shader
	gui->use();

	// Setup viewport, orthographic projection matrix
	glViewport(0, 0, fb_width, fb_height);
	auto proj = glm::ortho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
	glUniformMatrix4fv(gui->getUniformLocation("mProj"), 1, GL_FALSE, glm::value_ptr(proj));

	// Generate the vao and buffers
	GLuint vao, vbo, ibo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ibo);

	// Configure the vao
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, pos));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, uv));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), (GLvoid*)offsetof(ImDrawVert, col));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

	// Render command lists
	for (int n = 0; n < drawData->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = drawData->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert), (GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx), (GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glUniform1i(gui->getUniformLocation("sTexture"), 0);
				glBindTexture(GL_TEXTURE_2D, (GLuint)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	// Unbind the vao and buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// Delete the buffers and vao
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1, &ibo);

	// Restore modified GL state
	glUseProgram(last_program);
	glActiveTexture(last_active_texture);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindVertexArray(last_vertex_array);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFunc(last_blend_src, last_blend_dst);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

void render()
{
	glClearColor(0.78, 0.67, 0.6, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDisable(GL_CULL_FACE);
	renderObject(background);
	glEnable(GL_CULL_FACE);
	renderObject(animal);
	for (Object* object : sensors)
		renderObject(object);
	for (Object* object : foods)
		renderObject(object);
	for (Object* object : foodsSphere)
		renderObject(object);
	for (const auto& lineVec : lines)
		for (Object* object : lineVec)
			renderObject(object);

	renderImgui();

	SDL_GL_SwapWindow(window);
}

void update(float deltaTime, float totalTime)
{
	if (animating)
	{
		float frameUpdateDelta = 1.0f / float(animSpeed);

		animDelta += deltaTime;

		while (animDelta > frameUpdateDelta)
		{
			animDelta -= frameUpdateDelta;
			
			if (frame < numFrames)
				++frame;
		}
	}
}

Uint32 prevTick = 0;
Uint32 unprocessedTicks = 0;
Uint32 frameCount = 0, fps = 0;
Uint32 frameFraction = 0;

bool mainLoop()
{
	if (!input())
		return false;

	static const Uint32 FPS = 60;
	static const Uint32 SKIP_TICKS = 1000 / 60;

	Uint32 currentTick = SDL_GetTicks();
	Uint32 delta = (currentTick - prevTick);
	prevTick = currentTick;
	unprocessedTicks += delta;
	frameFraction += delta;

	deltaTime = delta *0.001;
	totalTime += deltaTime;

	if (frameFraction >= 1000)
	{
		fps = frameCount;
		frameCount = 0;
		frameFraction -= 1000;

		std::string windowTitle = "[FPS]: " + std::to_string(fps);
		SDL_SetWindowTitle(window, windowTitle.c_str());
	}

	if (unprocessedTicks >= SKIP_TICKS)
	{
		frameCount += 1;

		update(deltaTime, totalTime);
		render();
		unprocessedTicks %= SKIP_TICKS;
	}

	return true;
}

int main(int argc, char** argv)
{
	if (!init())
		return -1;
	refFoodPositions = simData->getData().m_frames[0].m_foods;
	while (mainLoop())
		;

	cleanUp();

	return 0;
}