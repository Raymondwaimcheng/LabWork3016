#include "scenebasic_uniform.h"

#include <cstdio>
#include <cstdlib>

#include <string>
using std::string;

#include <sstream>
#include "scenebasic_uniform.h"
#include <iostream>
using std::cerr;
using std::endl;

#include "helper/glutils.h"
#include "helper/texture.h"
#include "helper/particleutils.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//End at Lab04 Part6

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() :
    time(0),
    tPrev(0),
    angle(0.0f),
    rotSpeed(glm::pi<float>()/ 8.0f),
    plane(20, 20, 1, 1),
    lightPos(5.0f, 5.0f, 5.0f, 1.0f),
    sky(100.0f),
    teapot(14, glm::mat4(1.0f)),
    
    particleLifeTime(5.5f),
    nParticles(8000.0f),
    emitterPos(1, 1, 0),
    emitterDir(1, 2, 0)
    /*torus(1.75f * 0.75f, 1.75f * 0.75f, 50, 50)*/
{
    cuby = ObjMesh::load("media/rock.obj", false, true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(0.0f, 0.25f, 3.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 5.0f, 0.0f));
    //projection = mat4(1.0f);
    //angle = 0.0f;

    angle = half_pi<float>();
    initBuffers();

    /*glActiveTexture(GL_TEXTURE4);
    Texture::loadTexture("media/texture/fire.png");

    progFire.use();
    progFire.setUniform("ParticleTexture", 0);
    progFire.setUniform("ParticleLifeTime", particleLifeTime);
    progFire.setUniform("ParticleSize", 0.05f);
    progFire.setUniform("ParticleGravity", vec3(0.0f, -0.2f, 0.0f));
    progFire.setUniform("EmitterPos", emitterPos);*/

    //PBR
    projection = perspective(radians(50.f), (float)width / height, 0.5f, 100.0f);
    lightAngle = 0.0f;
    lightRotationSpeed = 1.5f;

    prog.setUniform("PBRLight[0].L", vec3(25.f));
    prog.setUniform("PBRLight[0].Position", view * lightPos);
    prog.setUniform("PBRLight[1].L", vec3(0.3f));
    prog.setUniform("PBRLight[1].Position", vec4(0.f, 0.15f, -1.f, 0.f));
    prog.setUniform("PBRLight[2].L", vec3(25.f));
    prog.setUniform("PBRLight[2].Position", view * vec4(-7.f, 3.f, 7.f, 1.f));

    prog.use();
    prog.setUniform("Light.L", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));
    //prog.setUniform("Color", vec4(0.4f, 0.4f, 0.4f, 1.0f));

    GLuint cubeTex = Texture::loadHdrCubeMap("media/texture/cube/pisa-hdr/pisa");
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTex);

    GLuint diffTex = Texture::loadTexture("media/texture/stone_base.jpg");
    GLuint normalTex = Texture::loadTexture("media/texture/stone_normal.jpg");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffTex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalTex);


    //Fog
    prog.setUniform("Fog.MaxDist", 7.0f);
    prog.setUniform("Fog.MinDist", 5.0f);
    prog.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));

    glActiveTexture(GL_TEXTURE4);
    Texture::loadTexture("media/texture/fire.png");
    progFire.use();
    progFire.setUniform("ParticleTexture", 0);
    progFire.setUniform("ParticleLifeTime", particleLifeTime);
    progFire.setUniform("ParticleSize", 2.5f);
    progFire.setUniform("ParticleGravity", vec3(0.0f, -0.2f, 0.0f));
    progFire.setUniform("EmitterPos", emitterPos);
    progFire.setUniform("Color", vec4(0.4f, 0.4f, 0.4f, 1.0f));
}

void SceneBasic_Uniform::initBuffers() {
    glGenBuffers(1, &initVel);
    glGenBuffers(1, &startTime);

    int size = nParticles * sizeof(float);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferData(GL_ARRAY_BUFFER, size * 3, 0, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferData(GL_ARRAY_BUFFER, size, 0, GL_STATIC_DRAW);

    glm::mat3 emitterBasis = ParticleUtils::makeArbitraryBasis(emitterDir);
    vec3 v(0.0f);
    float velocity, theta, phi;
    std::vector<GLfloat> data(nParticles * 3);

    for (uint32_t i = 0; i < nParticles; i++) {
        theta = glm::mix(0.0f, glm::pi<float>() / 20.0f, randFloat());
        phi = glm::mix(0.0f, glm::two_pi<float>(), randFloat());

        v.x = sinf(theta) * cosf(phi);
        v.y = cosf(theta);
        v.z = sinf(theta) * sinf(phi);

        velocity = glm::mix(1.25f, 1.5f, randFloat());
        v = glm::normalize(emitterBasis * v) * velocity;

        data[3 * i] = v.x;
        data[3 * i + 1] = v.y;
        data[3 * i + 2] = v.z;
    }

    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size * 3, data.data());

    float rate = particleLifeTime / nParticles;

    for (int i = 0; i < nParticles; i++) {
        data[i] = rate * i;
    }

    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data.data());

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenVertexArrays(1, &particles);
    glBindVertexArray(particles);
    glBindBuffer(GL_ARRAY_BUFFER, initVel);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, startTime);
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);

    glBindVertexArray(0);
}

float SceneBasic_Uniform::randFloat() 
{
    return random.nextFloat();

}

void SceneBasic_Uniform::compile()
{
	try {
        progFire.compileShader("shader/basic_fire.vert");
        progFire.compileShader("shader/basic_fire.frag");
        progFire.link();
        progFire.use();

		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::drawScene()
{
    drawFloor();

    int numObj = 9;
    vec3 objBaseColor(0.15f, 0.15f, 0.15f);
    for (int i = 0; i < numObj; i++) {
        float objX = i * (10.f / (numObj - 1)) - 5.0f;
        float rough = (i + 1) * (1.f / numObj);
        drawSpot(vec3(objX, 0.f, 0.f), 0.03f, 0, objBaseColor);
    }

}

void SceneBasic_Uniform::drawFloor()
{
    prog.setUniform("PBRMaterial.Rough", 0.9f);
    prog.setUniform("PBRMaterial.Metal", 0);
    prog.setUniform("PBRMaterial.Color", vec3(0.2f));

    model = mat4(1.0f);
    model = translate(model, vec3(0.0f, -1.75f, 0.0f));
    setMatrices(prog);
    plane.render();
}

void SceneBasic_Uniform::drawSpot(const vec3& pos, float rough, int metal, const vec3& color)
{
    prog.setUniform("PBRMaterial.Rough", rough);
    prog.setUniform("PBRMaterial.Metal", metal);
    prog.setUniform("PBRMaterial.Color", color);

    model = mat4(1.0f);
    model = translate(model, pos);
    model = rotate(model, radians(180.f), vec3(0.f, 1.f, 0.f));
    setMatrices(prog);
    cuby->render();
}

void SceneBasic_Uniform::update( float t )
{
    //PBR
    float deltaT = t - tPrev;
    if (tPrev == 0.f) {
        deltaT - 0.f;
    }
    tPrev = t;



    if (animating()) {
        lightAngle = mod(lightAngle + deltaT * lightRotationSpeed, two_pi<float>());
        lightPos.x = cos(lightAngle) * 7.f;
        lightPos.y = 3.f;
        lightPos.z = sin(lightAngle) * 7.f;
    }

    /*float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += 0.1f * deltaT;
    
    if (this->m_animate) {
        angle += rotSpeed * deltaT;
        if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    }*/

    //Skybox
    /*if (angle > glm::two_pi<float>()) {
        angle -= glm::two_pi<float>();
    }*/

    //fireTime = t;
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



    //PBR
    prog.setUniform("PBRLight[0].Position", view * lightPos);
    drawScene();

    progFire.use();
    setMatrices(progFire);
    grid.render();


    glDepthMask(GL_FALSE);
    progFire.use();
    setMatrices(progFire);
    progFire.setUniform("ParticleTime", time);
    glBindVertexArray(particles);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticles);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE);
    glFinish();
    
    //Light
    /*vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
    prog.setUniform("Light.Position", vec4(view * lightPos));*/

    /*prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    prog.setUniform("Material.Shininess", 180.0f);*/

    //SkyBox Placing
    prog.use();
    model = mat4(1.0f);
    sky.render();

    //plane.render();
    //obj placing
    float dist = 0.0f;
    for (int i = 0; i < 5; i++) {
        model = mat4(1.0f);
        model = glm::translate(model, vec3(dist * 0.6f - 1.0f, 0.0f, -dist));
        //model = glm::rotate(model, glm::radians(-90.0f), vec3(1.0f, 0.0f, 0.0f));
        setMatrices(prog);
        cuby->render();
        dist += 2.0f;
    }


    //Fire
    //progFire.use();
    //progFire.setUniform("Time", fireTime);
    //progFire.setUniform("Material.Kd", 0.2f, 0.5f, 0.9f);
    //progFire.setUniform("Material.Ka", 0.82f, 0.5f, 0.9f);
    //progFire.setUniform("Material.Ks", 0.8f, 0.8f, 0.8f);
    //progFire.setUniform("Material.Shininess", 100.0f);
    ///*vec3 cameraPos(10.f * cos(angle), 4.0f, 10.0f * sin(angle));
    //view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    //model = mat4(1.0f);
    //model = glm::rotate(model, glm::radians(-10.0f), vec3(0.0f, 0.0f, 1.0f));
    //model = glm::rotate(model, glm::radians(50.0f), vec3(1.0f, 0.0f, 0.0f));*/

    //setMatrices(progFire);
    //plane.render();
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
    //projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices(GLSLProgram& prog) {
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
    prog.setUniform("projection", projection);
}

void SceneBasic_Uniform::camUp() {
    y += 0.1f;
    view = glm::lookAt(vec3(0.0f + x, 0.25f + y, 3.0f + z), vec3(x, y, z), vec3(x, y, z));
}

void SceneBasic_Uniform::camDown() {
    y -= 0.1f;
    view = glm::lookAt(vec3(0.0f + x, 0.25f + y, 3.0f + z), vec3(x, y, z), vec3(x, y, z));
}

void SceneBasic_Uniform::camRight() {
    z += 0.1f;
    view = glm::lookAt(vec3(0.0f + x, 0.25f + y, 3.0f + z), vec3(x, y, z), vec3(x, y, z));
}

void SceneBasic_Uniform::camLeft() {
    z -= 0.1f;
    view = glm::lookAt(vec3(0.0f + x, 0.25f + y, 3.0f + z), vec3(x, y, z), vec3(x, y, z));
}
