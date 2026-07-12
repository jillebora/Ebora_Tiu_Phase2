
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <list>
#include <chrono>
#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


#include "physicsWorld.h"
#include "renderParticle.h"
#include "shader.h"
#include "model.h"
#include "P6/cable.h"
#include "gravityForceGenerator.h"


using namespace std;
using namespace std::chrono_literals;

// GLOBAL VARIABLE
const int NUM = 5;

vector<P6::Particle*> cradleParticles;
vector<RenderParticle*> cradleRender;
vector<Cable*> cradleCables;
vector<glm::vec3> anchorPoints;

// Simulation Parameters
float cableLen = 300.f;
float particleGap = 100.f;
float particleRad = 30.f;
float gravityStrength = -50.f;

// Force entered by the user
glm::vec3 applyForce(500000.f, 0.f, 0.f);

const float MASS = 50.f;
const float RESTITUTION = 0.9f;

P6::Particle* drawCradle(P6::PhysicsWorld& pWorld, Model& sphere, GravityForceGenerator& gravityGen)
{
	
	const float startX = -particleGap * ((NUM - 1) / 2.f);
	const float anchorY = cableLen;

	for (int i = 0; i < NUM; i++)
	{
		float x = startX + particleGap * i;

		glm::vec3 anchor = glm::vec3(x, anchorY, 0.f);
		anchorPoints.push_back(anchor);

		// Particle
		P6::Particle* p = new P6::Particle();
		p->setName("Point" + std::to_string(i));
		p->Position = anchor - glm::vec3(0.f, cableLen, 0.f);
		p->mass = MASS;
		p->radius = particleRad;
		p->restitution = RESTITUTION;
		p->damping = 1.f;

		pWorld.AddParticle(p);
		//pWorld.forceRegistry.Add(p, &gravityGen);
		cradleParticles.push_back(p);

		// Render Sphere
		RenderParticle* rp = new RenderParticle(
			p, sphere.getRenderObject(), glm::vec3(1.f, 0.f, 0.f));
		rp->Scale = glm::vec3(particleRad);
		cradleRender.push_back(rp);

		// Cable
		Cable* c = new Cable();
		c->particles[0] = p;
		c->anchorPoint = anchor;
		c->maxLength = cableLen;
		c->restitution = 0.f;

		pWorld.Links.push_back(c);
		cradleCables.push_back(c);

	}

	return cradleParticles[0];
}

int main()
{
	cout << "Enter Cable Length: ";
	cin >> cableLen;

	cout << "Enter Particle Gap: ";
	cin >> particleGap;

	cout << "Enter Particle Radius: ";
	cin >> particleRad;

	cout << "Enter Gravity Strength: ";
	cin >> gravityStrength;

	cout << "\nEnter Force to Apply\n";

	cout << "Force X: ";
	cin >> applyForce.x;

	cout << "Force Y: ";
	cin >> applyForce.y;

	cout << "Force Z: ";
	cin >> applyForce.z;

	cout << "\nStarting simulation...\n";

	// Initialize GLFW
	if (!glfwInit())
	{
		return -1;
	}

	float windowWidth = 800;
	float windowHeight = 800;

	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Phase 2 - Jillana Ebora & Kaeden Tiu", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}
	
	P6::PhysicsWorld pWorld = P6::PhysicsWorld();

	glfwMakeContextCurrent(window);
	gladLoadGL();
	glEnable(GL_DEPTH_TEST);

	// SHADERS
	Shader shader("Shaders/sphere.vert", "Shaders/sphere.frag");

	// MODEL
	Model sphere;
	sphere.loadFromObj("3D/sphere.obj");
	sphere.setScale(glm::vec3(1.f));

	// CAMERA

	glm::mat4 proj = glm::ortho(-350.f, 350.f, -350.f, 350.f, -500.f, 1000.f);

	glm::mat4 view = glm::lookAt(
		glm::vec3(0.f, 0.f, 700.f),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f));

	// Wire shader + camera into the model's RenderObject
	sphere.setShader(&shader.ID);
	sphere.setCamera(&proj, &view);

	
	GravityForceGenerator gravity(glm::vec3(0.f, gravityStrength, 0.f));

	P6::Particle* leftmost = drawCradle(pWorld, sphere, gravity);

	// ---- line buffer (anchor -> particle), one per particle ----
	GLuint lineVAO, lineVBO;
	glGenVertexArrays(1, &lineVAO);
	glGenBuffers(1, &lineVBO);
	glBindVertexArray(lineVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
	glBufferData(GL_ARRAY_BUFFER, NUM * 6 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	// TIME

	using clock = std::chrono::high_resolution_clock;

	constexpr std::chrono::nanoseconds timestep(16ms);

	auto curr_time = clock::now();
	auto prev_time = curr_time;

	std::chrono::nanoseconds curr_ns(0);

	int currentRank = 1;
	bool printedResults = false;

	// input edge-detection state 
	bool spacePrev = false;   // so one press = one force
	int  camView = 1;       // 1 = front, 2 = side

	// LOOP

	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Apply the user-entered force once when SPACE is pressed
		bool spaceNow = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;

		if (spaceNow && !spacePrev)
		{
			leftmost->AddForce(applyForce);
		}

		spacePrev = spaceNow;

		curr_time = clock::now();

		auto dur = std::chrono::duration_cast<std::chrono::nanoseconds>(
			curr_time - prev_time
		);

		prev_time = curr_time;

		curr_ns += dur;

		if (curr_ns >= timestep)
		{
			constexpr float timestep_sec = timestep.count() / (float)1E09;

			curr_ns -= timestep;

			pWorld.Update(timestep_sec);

			//contact.Resolve(timestep_sec);

		}
		// ---- render ----
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// rebuild cable lines each frame (particles moved)
		std::vector<float> lineVerts;
		lineVerts.reserve(NUM * 6);
		for (int i = 0; i < NUM; i++)
		{
			glm::vec3 a = anchorPoints[i];
			glm::vec3 b = cradleParticles[i]->Position;
			lineVerts.insert(lineVerts.end(), { a.x, a.y, a.z, b.x, b.y, b.z });
		}

		shader.use();
		shader.setMat4("transform", glm::mat4(1.f));
		shader.setMat4("projection", proj);
		shader.setMat4("view", view);
		shader.setVec3("color", glm::vec3(1.f));   // white cables

		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, lineVerts.size() * sizeof(float),
			lineVerts.data(), GL_DYNAMIC_DRAW);
		glDrawArrays(GL_LINES, 0, (GLsizei)(lineVerts.size() / 3));
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);

		for (RenderParticle* rp : cradleRender)
			rp->Draw();

		glfwSwapBuffers(window);
	}

	// ---- cleanup ----
	glDeleteVertexArrays(1, &lineVAO);
	glDeleteBuffers(1, &lineVBO);
	for (Cable* c : cradleCables)           delete c;
	for (RenderParticle* rp : cradleRender) delete rp;
	for (P6::Particle* p : cradleParticles) delete p;

	sphere.destroy();
	shader.destroy();
	glfwTerminate();
	return 0;
}
