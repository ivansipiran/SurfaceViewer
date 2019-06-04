#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ArcBall.h"

using namespace std;

Shader* shader;
Model* model;
// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;

Camera camera(glm::vec3(5.0f, 5.0f, 5.0f));
glm::vec3 center;
bool firstMouse = true;

int index = 0;
glm::mat4 Transform;
glm::mat3 LastRot;
glm::mat3 ThisRot;
bool isClicked;
ArcBallT arc_ball;

float ext = 4.0;

void init_resources(){
	shader = new Shader("1.model_loading.vs", "1.model_loading.fs");
	model = new Model("texturedMesh.obj");
	cout << "Model loaded" << endl;
	model->ComputeData();
	center = model->centroid;
	std::cout << center.x <<" " << center.y << " " << center.z <<  std::endl;

	Transform = glm::mat4(1.0f);
	LastRot = glm::mat3(1.0f);
	ThisRot = glm::mat3(1.0f);

	isClicked = false;
	arc_ball.setBounds((GLfloat)SCR_WIDTH, (GLfloat)SCR_HEIGHT);
}

void onReshape(int w, int h){
	SCR_WIDTH = w;
	SCR_HEIGHT = h;

	arc_ball.setBounds((GLfloat)SCR_WIDTH, (GLfloat)SCR_HEIGHT);
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
}

void onDisplay(){
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// don't forget to enable shader before setting uniforms
	shader->use();

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
	glm::mat4 view = camera.GetViewMatrix();
	shader->setMat4("projection", projection);
	shader->setMat4("view", view);
	shader->setVec3("cam_position", camera.Position);

	// render the loaded model
	glm::mat4 mod = Transform *
		glm::scale(glm::mat4(1.0f), glm::vec3(model->scale, model->scale, model->scale)) * 
		glm::translate(glm::mat4(1.0f), glm::vec3(-center.x, -center.y, -center.z)); // translate it down so it's at the center of the scene
	// it's a bit too big for our scene, so scale it down
	glm::mat3 mat_inv_transp = glm::transpose(glm::inverse(glm::mat3(mod)));
	shader->setMat3("m_3x3", mat_inv_transp);
	shader->setVec3("mat_specular", glm::vec3(1.0, 1.0, 1.0));
	shader->setFloat("mat_s", 100);

	shader->setMat4("model", mod);
	model->Draw(*shader);

	glutSwapBuffers();
}

//Las funciones del mouse sirven para interacción con el usuario
void OnMouseClick(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		isClicked = true;
		LastRot = ThisRot;
		Point2fT auxPoint;
		auxPoint.s.X = (GLfloat)x;
		auxPoint.s.Y = (GLfloat)y;
		arc_ball.click(&auxPoint);
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		isClicked = false;
	}
}

void OnMouseMotion(int x, int y){
	if (isClicked){
		Quat4fT ThisQuat;
		Point2fT auxPoint;
		auxPoint.s.X = (GLfloat)x;
		auxPoint.s.Y = (GLfloat)y;
		arc_ball.drag(&auxPoint, &ThisQuat);

		Matrix3fT mThisRot;
		memcpy(mThisRot.M, glm::value_ptr(ThisRot), sizeof(float)* 9);
		Matrix3fT mLastRot;
		memcpy(mLastRot.M, glm::value_ptr(LastRot), sizeof(float)* 9);
		Matrix4fT mTransform;
		memcpy(mTransform.M, glm::value_ptr(Transform), sizeof(float)* 16);

		Matrix3fSetRotationFromQuat4f(&mThisRot, &ThisQuat);
		Matrix3fMulMatrix3f(&mThisRot, &mLastRot);
		Matrix4fSetRotationFromMatrix3f(&mTransform, &mThisRot);

		memcpy(glm::value_ptr(ThisRot), mThisRot.M, sizeof(float)* 9);
		memcpy(glm::value_ptr(LastRot), mLastRot.M, sizeof(float)* 9);
		memcpy(glm::value_ptr(Transform), mTransform.M, sizeof(float)* 16);

		glutPostRedisplay();
	}
}

void onMouseWheel(int button, int dir, int x, int y){
	if (dir > 0){
		model->scale *= 1.05;
		glutPostRedisplay();
	}
	else{
		model->scale *= 0.95;
		glutPostRedisplay();
	}
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);
	glutInitContextVersion(2, 0);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(SCR_WIDTH, SCR_HEIGHT);
	glutCreateWindow("Visualizador de superficies");

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK){
		cout << "Error inicializando GLEW!" << endl;
		exit(EXIT_FAILURE);
	}

	if (!GLEW_VERSION_2_0){
		cout << "Su tajeta gráfica no soporta OpenGL 2.0" << endl;
		exit(EXIT_FAILURE);
	}

	init_resources();

	glutDisplayFunc(onDisplay);
	glutReshapeFunc(onReshape);
	glutMouseFunc(OnMouseClick);
	glutMotionFunc(OnMouseMotion);
	glutMouseWheelFunc(onMouseWheel);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glutMainLoop();
	

}