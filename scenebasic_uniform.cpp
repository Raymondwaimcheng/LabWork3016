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
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>

//End at Lab04 Part6

using glm::vec3;
using glm::vec4;
using glm::mat3;
using glm::mat4;

SceneBasic_Uniform::SceneBasic_Uniform() :
    tPrev(0),
    angle(0.0f),
    rotSpeed(glm::pi<float>()/ 8.0f),
    plane(25.0f, 25.0f, 1, 1),
    sky(100.0f),
    teapot(14, glm::mat4(1.0f))
  
    //torus(1.75f * 0.75f, 1.75f * 0.75f, 50, 50) 
    {
    cuby = ObjMesh::load("media/rock.obj", false, true);
}

void SceneBasic_Uniform::initScene()
{
    compile();
    glEnable(GL_DEPTH_TEST);
    vec3 cameraPos = vec3(0.0f, 0.25f, 3.0f);
    view = glm::lookAt(cameraPos, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    //rotation
    //model = glm::rotate(model, glm::radians(-35.0f), vec3(1.0f, 0.0f, 0.0f));
    //model = glm::rotate(model, glm::radians(15.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);
    angle = 0.0f;

    /*float x, z;
    for (int i = 0; i < 3; i++) {
        std::stringstream name;
        name << "lights[" << i << "].Position";
        x = 2.0f * cosf((glm::two_pi<float>() / 3) * i);
        z = 2.0f * sinf((glm::two_pi<float>() / 3) * i);
        prog.setUniform(name.str().c_str(), view * glm::vec4(x, 1.2f, z + 1.0f, 1.0f));

    }*/

    //prog.setUniform("Light.Position", view * glm::vec4(5.0f, 5.0f, 2.0f, 1.0f));
    //prog.setUniform("Light.La", vec3(0.4f, 0.4f, 0.4f));
    //prog.setUniform("Light.Ls", vec3(1.0f, 1.0f, 1.0f));

    prog.setUniform("Light.L", vec3(1.0f));
    prog.setUniform("Light.La", vec3(0.05f));

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
    prog.setUniform("Fog.MaxDist", 10.0f);
    prog.setUniform("Fog.MinDist", 1.0f);
    prog.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));
}

void SceneBasic_Uniform::compile()
{
	try {
		prog.compileShader("shader/basic_uniform.vert");
		prog.compileShader("shader/basic_uniform.frag");
		prog.link();
		prog.use();
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;
    angle += 0.1f * deltaT;
    
    if (this->m_animate) {
        angle += rotSpeed * deltaT;
        if (angle > glm::two_pi<float>()) angle -= glm::two_pi<float>();
    }

    //Skybox
    /*if (angle > glm::two_pi<float>()) {
        angle -= glm::two_pi<float>();
    }*/
}

void SceneBasic_Uniform::render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //Light
    vec4 lightPos = vec4(10.0f * cos(angle), 10.0f, 10.0f * sin(angle), 1.0f);
    prog.setUniform("Light.Position", vec4(view * lightPos));

    prog.setUniform("Material.Kd", vec3(0.2f, 0.55f, 0.9f));
    prog.setUniform("Material.Ks", vec3(0.95f, 0.95f, 0.95f));
    prog.setUniform("Material.Ka", vec3(0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f));
    prog.setUniform("Material.Shininess", 180.0f);

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
        setMatrices();
        cuby->render();
        dist += 2.0f;
    }
}

void SceneBasic_Uniform::resize(int w, int h)
{
    width = w;
    height = h;
    glViewport(0,0,w,h);
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}

void SceneBasic_Uniform::setMatrices() {
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2])));
    prog.setUniform("MVP", projection * mv);
}
