#ifndef SCENEBASIC_UNIFORM_H
#define SCENEBASIC_UNIFORM_H

#include "helper/scene.h"

#include <glad/glad.h>
#include "helper/random.h"
#include "helper/grid.h"
#include "helper/glslprogram.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "helper/torus.h"
#include "helper/teapot.h"
#include "glm/glm.hpp"
#include "helper/plane.h"
#include "helper/objmesh.h"
#include "helper/cube.h"
#include "helper/skybox.h"

using namespace std;
using namespace glm;


class SceneBasic_Uniform : public Scene
{
private:
    GLSLProgram prog;
    //Torus torus;
    Plane plane;
    Teapot teapot;
    //Cube cube;
    SkyBox sky;
    float rotSpeed;
    //GLuint brick, moss;
    std::unique_ptr<ObjMesh> cuby;

    float tPrev;
    float lightAngle;
    float lightRotationSpeed;
    vec4 lightPos;

    float angle;
    float time;
    float x;
    float y;
    float z;

    glm::mat4 rotationMatrix;

    GLSLProgram progFire;
    Random random;
    GLuint initVel, startTime, particles, nParticles;
    Grid grid;
    vec3 emitterPos, emitterDir;
    float particleLifeTime;
    void initBuffers();
    float randFloat();

    void setMatrices(GLSLProgram& prog);
    void compile();

    void drawScene();
    void drawFloor();
    void drawSpot(const vec3& pos, float rough, int metal, const vec3& color);

public:
    SceneBasic_Uniform();

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);

    void camUp();
    void camDown();
    void camRight();
    void camLeft();
};

#endif // SCENEBASIC_UNIFORM_H
