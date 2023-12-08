#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <vector>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <gl/freeglut_ext.h>
#include <gl/glm/glm/glm.hpp>
#include <gl/glm/glm/ext.hpp>
#include <gl/glm/glm/gtc/matrix_transform.hpp>

void make_vertexShaders();
void make_fragmentShaders();
void make_shaderProgram();
void InitBuffer();
void menu();
void reset();
void timer(int value);
GLvoid drawScene();
GLvoid Reshape(int w, int h);
GLvoid Keyboard(unsigned char key, int x, int y);
GLvoid KeyboardUp(unsigned char key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
GLvoid MouseWheel(int wheel, int direction, int x, int y);
char* filetobuf(const char* file);
void ReadObj(FILE* path, int index);

BOOL rides_collision_check();
void RollerCoaster_rand_num();

GLuint shaderProgramID; //--- ���̴� ���α׷� �̸�
GLuint vertexShader; //--- ���ؽ� ���̴� ��ü
GLuint fragmentShader; //--- �����׸�Ʈ ���̴� ��ü
GLchar* vertexSource, * fragmentSource; //--- �ҽ��ڵ� ���� ����

float floor_xz[] = {
	 -10.0, 0.0, -10.0,    0.0, 1.0, 0.0,  0, 0,
	 -10.0, 0.0, 10.0,     0.0, 1.0, 0.0,  0, 1,
	 10.0, 0.0, 10.0,      0.0, 1.0, 0.0,  1, 1,
	 -10.0, 0.0, -10.0,    0.0, 1.0, 0.0,  0, 0,
	 10.0, 0.0, 10.0,      0.0, 1.0, 0.0,  1, 1,
	 10.0, 0.0, -10.0,     0.0, 1.0, 0.0,  1, 0
};

float rides_data[5][12] = {
	{},
	{9.0, 0.0, 3.0, -9.0, 0.0, 3.0, -9.0, 0.0, -3.0, 9.0, 0.0, -3.0},
	{5.0, 0.0, 2.0, -5.0, 0.0, 2.0, -5.0, 0.0, -2.0, 5.0, 0.0, -2.0},
	{4.0, 0.0, 4.0, -4.0, 0.0, 4.0, -4.0, 0.0, -4.0, 4.0, 0.0, -4.0},
	{4.0, 0.0, 4.0, -4.0, 0.0, 4.0, -4.0, 0.0, -4.0, 4.0, 0.0, -4.0}
};

// �ѷ��ڽ���
GLfloat point[8][3];
GLfloat matrix_2[3][3] = { {2.0f, -4.0f, 2.0f}, {-3.0f, 4.0f, -1.0f}, {1.0f, 0.0f, 0.0f} };
GLfloat matrix_3[4][4] = { {-1.0f, 3.0f, -3.0f, 1.0f},
						  {2.0f, -5.0f, 4.0f, -1.0f},
						  {-1.0f, 0.0f, 1.0f, 0.0f},
						  {0.0f, 2.0f, 0.0f, 0.0f} };

GLuint vao[1000], vbo[1000];
std::vector<GLfloat> data[1000];
glm::vec3 cameraPos;      //--- ī�޶� ��ġ
glm::vec3 cameraDirection; //--- ī�޶� �ٶ󺸴� ����
glm::vec3 cameraUp;        //--- ī�޶� ���� ����
float cameraPos_x, cameraPos_y, cameraPos_z;
float cameraDirection_x, cameraDirection_y, cameraDirection_z;
float cameraSpeed;
float pitch, yaw;                                // ���콺 ȭ�� �̵�
float fov;                                       // ��
BOOL left_button;
int window_w, window_h;
BOOL key_w, key_a, key_s, key_d;

int view_check;                                  // ���� Ȯ��   0. top_view / 1. front_view
float rides_x[5], rides_z[5];    // �ⱸ ����
int rides_sel_cnt;                 // 0. ���� / 1. �ѷ��ڽ��� / 2. ������ / 3. ���̷ε�� / 4. ȸ����
BOOL rides_sel_check[5];             // �ⱸ�� ���õǾ����� 0. ���� �ȵ�
BOOL rides_install_check[5];  // �ⱸ�� ��ġ�Ǿ�����
BOOL rides_collision[5];  // ��ġ �ⱸ�� �浹�ϴ���    �浹�ϸ� true / ���ϸ� false
float len_x[5], len_z[5];  // �ⱸ ���� ���� ����
float rides_radian[5];     // �ⱸ ȸ��

void menu() {
	printf("------------ top view ��ɾ�-------------\n");
	printf("1 : �ѷ��ڽ��� ����\n");
	printf("2 : ������ ����\n");
	printf("3 : ���̷ε�� ����\n");
	printf("4 : ȸ���� ����\n");
	printf("b : �ⱸ ��ġ\n");
	printf("d : �ⱸ ����\n");
	printf("----------- front view ��ɾ�------------\n");
	printf("1 : �ѷ��ڽ��� ����\n");
	printf("2 : ������ ����\n");
	printf("3 : ���̷ε�� ����\n");
	printf("4 : ȸ���� ����\n");
	printf("w, a, s, d : ī�޶� �̵�\n");
	printf("------------- ���� ��ɾ� ---------------\n");
	printf("v : ���� ����\n");
	printf("` : ����\n");
	printf("q : ���α׷� ����\n");
	printf("-----------------------------------------\n");
}

void reset() {
	key_w = key_a = key_s = key_d = false;
	cameraPos_x = 0.0f;
	cameraPos_y = 2.0f;
	cameraPos_z = 10.0f;
	pitch = 0;
	yaw = -90;
	fov = 50;
	view_check = 1;
	rides_sel_cnt = 0;
	len_x[1] = 9, len_x[2] = 5, len_x[3] = len_x[4] = 4;
	len_z[1] = 3, len_z[2] = 2, len_z[3] = len_z[4] = 4;
	for (int i = 0; i < 5; ++i) {
		rides_install_check[i] = false;
		rides_sel_check[i] = false;
		rides_x[i] = rides_z[i] = 0;
		rides_collision[i] = false;
		rides_radian[i] = 0;
	}
	RollerCoaster_rand_num();   // �ѷ��ڽ��� ���� ������

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	const char* cube_obj = "cube.obj";
	FILE* file_cube[7];
	for (int i = 0; i < 7; ++i) {
		file_cube[i] = fopen(cube_obj, "r");
		ReadObj(file_cube[i], i);
		fclose(file_cube[i]);
	}
}

void RollerCoaster_rand_num() {
	srand(time(NULL));
	for (int i = 0; i < 7; i++)
	{
		point[i][0] = (rand() % 1801 - 900) * 0.01;    // -9 <= x <= 9
		point[i][1] = (rand() % 501 + 500) * 0.01;    // 5 <= y <= 10
		point[i][2] = (rand() % 601 - 300) * 0.01;    // -3 <= z <= 3
	}
	point[7][0] = point[0][0];
	point[7][1] = point[0][1];
	point[7][2] = point[0][2];
}

void main(int argc, char** argv)
{
	srand(time(NULL));
	//--- ������ �����ϱ�
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 30);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Amusement Park");
	glewExperimental = GL_TRUE;
	menu();
	reset();
	glewInit();
	make_shaderProgram();
	InitBuffer();
	glutDisplayFunc(drawScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutKeyboardUpFunc(KeyboardUp);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutMouseWheelFunc(MouseWheel);
	glutTimerFunc(100, timer, 1);
	glutMainLoop();
}

GLvoid drawScene() {
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	int modelLoc = glGetUniformLocation(shaderProgramID, "model"); //--- ���ؽ� ���̴����� �𵨸� ��ȯ ��� �������� �޾ƿ´�.
	int viewLoc = glGetUniformLocation(shaderProgramID, "view"); //--- ���ؽ� ���̴����� ���� ��ȯ ��� �������� �޾ƿ´�.
	int projLoc = glGetUniformLocation(shaderProgramID, "projection");

	unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, 0.0, 5.0, 0.0);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos_x, cameraPos_y, cameraPos_z);

	if (view_check == 0) {
		glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 0.0f);      //--- ī�޶� ��ġ
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- ī�޶� �ٶ󺸴� ����
		glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);        //--- ī�޶� ���� ����

		glm::mat4 vTransform = glm::mat4(1.0f);
		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

		glm::mat4 pTransform = glm::mat4(1.0f);
		pTransform = glm::ortho(11.0f, -11.0f, 11.0f, -11.0f, -11.0f, 11.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);
	}
	else if (view_check == 1) {
		cameraDirection_x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		cameraDirection_y = sin(glm::radians(pitch));
		cameraDirection_z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

		cameraPos = glm::vec3(cameraPos_x, cameraPos_y, cameraPos_z);      //--- ī�޶� ��ġ
		cameraDirection = glm::vec3(cameraDirection_x, cameraDirection_y, cameraDirection_z); //--- ī�޶� �ٶ󺸴� ����
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);        //--- ī�޶� ���� ����

		glm::mat4 vTransform = glm::mat4(1.0f);
		vTransform = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

		glm::mat4 pTransform = glm::mat4(1.0f);
		pTransform = glm::perspective(glm::radians(fov), (float)window_w / (float)window_h, 0.1f, 200.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);
	}

	// ���� �ⱸ
	for (int i = 1; i < 5; ++i) {
		if (rides_sel_check[i] == true || rides_install_check[i] == true) {
			glm::mat4 border = glm::mat4(1.0f);
			glm::mat4 border_T = glm::mat4(1.0f);
			glm::mat4 border_R = glm::mat4(1.0f);
			border_T = glm::translate(border_T, glm::vec3(rides_x[i], 0.0, rides_z[i]));
			border_R = glm::rotate(border_R, glm::radians(rides_radian[i]), glm::vec3(0.0, 1.0, 0.0));
			border = border_T * border_R;
			if (view_check == 0) {
				glLineWidth(2);
				if (!rides_collision[i])
					glUniform3f(objColorLocation, 0.0, 1.0, 0.0);
				else
					glUniform3f(objColorLocation, 1.0, 0.0, 0.0);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(border));
				glBindVertexArray(vao[i + 10]);
				glDrawArrays(GL_LINE_LOOP, 0, 4);
			}
			if (i == 1) { // �ѷ��ڽ����� ���
				// ���� ���
				glPointSize(7.0f);
				glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(border));
				glUniform3f(objColorLocation, 1.0, 0.0, 0.0);
				glBegin(GL_POINTS);
				for (int i = 0; i < 8; ++i)
					glVertex3f(point[i][0], point[i][1], point[i][2]);
				glEnd();
			}
			else if (i == 2) {   // �������� ���
				 
			}
			else if (i == 3) {   // ���̷ε���� ���

			}
			else if (i == 4) {   // ȸ������ ���

			}
		}
	}

	// �ٴ�
	glm::mat4 floor = glm::mat4(1.0f);
	glUniform3f(objColorLocation, 0.5, 0.5, 0.5);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(floor));
	glBindVertexArray(vao[10]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers();
}

void InitBuffer()
{
	glGenVertexArrays(1000, vao);
	glGenBuffers(1000, vbo);

	for (int i = 0; i < 7; ++i) {
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, data[i].size() * sizeof(GLfloat), data[i].data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	glBindVertexArray(vao[10]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_xz), floor_xz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	for (int i = 11; i < 15; ++i) {
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), rides_data[i - 10], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(0);
	}
}

void timer(int value) {
	if (key_w) {
		cameraPos_x += cameraDirection_x;
		cameraPos_y += cameraDirection_y;
		cameraPos_z += cameraDirection_z;
	}
	if (key_a) {
		cameraPos_x -= glm::normalize(glm::cross(cameraDirection, cameraUp)).x;
		cameraPos_y -= glm::normalize(glm::cross(cameraDirection, cameraUp)).y;
		cameraPos_z -= glm::normalize(glm::cross(cameraDirection, cameraUp)).z;
	}
	if (key_s) {
		cameraPos_x -= cameraDirection_x;
		cameraPos_y -= cameraDirection_y;
		cameraPos_z -= cameraDirection_z;
	}
	if (key_d) {
		cameraPos_x += glm::normalize(glm::cross(cameraDirection, cameraUp)).x;
		cameraPos_y += glm::normalize(glm::cross(cameraDirection, cameraUp)).y;
		cameraPos_z += glm::normalize(glm::cross(cameraDirection, cameraUp)).z;
	}
	drawScene();
	glutTimerFunc(100, timer, 1);
}

GLvoid Keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		if (view_check == 1)
			key_w = true;
		break;
	case 'a':
		if (view_check == 1)
			key_a = true;
		break;
	case 's':
		if (view_check == 1)
			key_s = true;
		break;
	case 'd':
		if (view_check == 1)
			key_d = true;
		else if (view_check == 0) {
			rides_install_check[rides_sel_cnt] = false;
		}
		break;
	case 'v':
		if (view_check == 1) {
			key_w = key_a = key_s = key_d = false;
			view_check = 0;
		}
		else if (view_check == 0) {
			for (int i = 0; i < 5; ++i) {
				rides_sel_check[i] = false;
			}
			rides_sel_cnt = 0;
			view_check = 1;
		}
		break;
	case '1':
		if (view_check == 1){}
		else if (view_check == 0) {
			rides_sel_check[1] = !rides_sel_check[1];
			rides_sel_check[2] = rides_sel_check[3] = rides_sel_check[4] = false;
 			rides_sel_cnt = 1;
			rides_collision_check();
		}
		break;
	case '2':
		if (view_check == 1) {}
		else if (view_check == 0) {
			rides_sel_check[2] = !rides_sel_check[2];
			rides_sel_check[1] = rides_sel_check[3] = rides_sel_check[4] = false;
			rides_sel_cnt = 2;
			rides_collision_check();
		}
		break;
	case '3':
		if (view_check == 1) {}
		else if (view_check == 0) {
			rides_sel_check[3] = !rides_sel_check[3];
			rides_sel_check[2] = rides_sel_check[1] = rides_sel_check[4] = false;
			rides_sel_cnt = 3;
			rides_collision_check();
		}
		break;
	case '4':
		if (view_check == 1) {}
		else if (view_check == 0) {
			rides_sel_check[4] = !rides_sel_check[4];
			rides_sel_check[2] = rides_sel_check[3] = rides_sel_check[1] = false;
			rides_sel_cnt = 4;
			rides_collision_check();
		}
		break;
	case 'b':
		if (view_check == 0) {
			if (rides_collision[rides_sel_cnt] == false)
				rides_install_check[rides_sel_cnt] = true;
		}
		break;
	case 'r':
		if (view_check == 0) {
			if (rides_install_check[rides_sel_cnt] == false) {
				rides_radian[rides_sel_cnt] = 90 - rides_radian[rides_sel_cnt];
				rides_collision_check();
			}
		}
		break;
	case '`':   // ����
		reset();
		InitBuffer();
		break;
	case 'q':   // ���α׷� ����
		std::cout << "���α׷��� �����մϴ�." << std::endl;
		glutLeaveMainLoop();
		break;
	}
	drawScene();
}

GLvoid KeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		if (view_check == 1)
			key_w = false;
		break;
	case 'a':
		if (view_check == 1)
			key_a = false;
		break;
	case 's':
		if (view_check == 1)
			key_s = false;
		break;
	case 'd':
		if (view_check == 1)
			key_d = false;
		break;
	}
	drawScene();
}

float prev_mouse_x, prev_mouse_y;
void Mouse(int button, int state, int x, int y) {
	float normalized_x, normalized_y;

	normalized_x = (2.0 * x / 600) - 1.0;
	normalized_y = 1.0 - (2.0 * y / 600);
	if (view_check == 0) {}
	else if (view_check == 1) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			left_button = true;
			prev_mouse_x = normalized_x;  // ����� ���� ���콺 ��ġ
			prev_mouse_y = normalized_y;
		}
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			left_button = false;
		}
	}
}

void Motion(int x, int y) {
	float normalized_x, normalized_y;


	if (view_check == 0) {
		if (rides_install_check[rides_sel_cnt] == false) {
			normalized_x = ((2.0 * x / 600) - 1.0) * 10;
			normalized_y = ((2.0 * y / 600) - 1.0) * 10;
			if (rides_sel_check[rides_sel_cnt] == true) {
				rides_x[rides_sel_cnt] = normalized_x;
				rides_z[rides_sel_cnt] = normalized_y;
				rides_collision_check();
			}
		}
	}
	else if (view_check == 1) {
		normalized_x = (2.0 * x / 600) - 1.0;
		normalized_y = 1.0 - (2.0 * y / 600);
		if (left_button == true) {
			float delta_x = normalized_x - prev_mouse_x;
			float delta_y = normalized_y - prev_mouse_y;

			prev_mouse_x = normalized_x;
			prev_mouse_y = normalized_y;

			yaw += delta_x * 50;
			pitch += delta_y * 50;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;
		}
	}
	drawScene();
}

// �ⱸ�� �浹 �˻�
BOOL rides_collision_check() {
	if (rides_radian[rides_sel_cnt] == 0) {   // ȸ������ ���� �ⱸ�� �ٴ��� ��� ���
		if (rides_x[rides_sel_cnt] - len_x[rides_sel_cnt] < -10) return rides_collision[rides_sel_cnt] = true;
		if (rides_x[rides_sel_cnt] + len_x[rides_sel_cnt] > 10) return rides_collision[rides_sel_cnt] = true;
		if (rides_z[rides_sel_cnt] - len_z[rides_sel_cnt] < -10) return rides_collision[rides_sel_cnt] = true;
		if (rides_z[rides_sel_cnt] + len_z[rides_sel_cnt] > 10) return rides_collision[rides_sel_cnt] = true;
	}
	else if (rides_radian[rides_sel_cnt] == 90) {   // ȸ���� �ⱸ�� �ٴ��� ��� ���
		if (rides_x[rides_sel_cnt] - len_z[rides_sel_cnt] < -10) return rides_collision[rides_sel_cnt] = true;
		if (rides_x[rides_sel_cnt] + len_z[rides_sel_cnt] > 10) return rides_collision[rides_sel_cnt] = true;
		if (rides_z[rides_sel_cnt] - len_x[rides_sel_cnt] < -10) return rides_collision[rides_sel_cnt] = true;
		if (rides_z[rides_sel_cnt] + len_x[rides_sel_cnt] > 10) return rides_collision[rides_sel_cnt] = true;
	}
	for (int i = 1; i < 5; ++i) {
		if (i != rides_sel_cnt && rides_install_check[i] == true) {
			if (rides_radian[rides_sel_cnt] == 0) {              // �ⱸ�� ȸ������ ���� ���
				if(rides_radian[i] == 0){          // ��ġ�� �ⱸ�� ȸ������ ���� ���
					if (rides_x[rides_sel_cnt] - len_x[rides_sel_cnt] < rides_x[i] + len_x[i] &&
						rides_x[rides_sel_cnt] + len_x[rides_sel_cnt] > rides_x[i] - len_x[i] &&
						rides_z[rides_sel_cnt] - len_z[rides_sel_cnt] < rides_z[i] + len_z[i] &&
						rides_z[rides_sel_cnt] + len_z[rides_sel_cnt] > rides_z[i] - len_z[i]) 
						return rides_collision[rides_sel_cnt] = true;
				}         
				else if (rides_radian[i] == 90) {    // ��ġ�� �ⱸ�� ȸ���� ���
					if (rides_x[rides_sel_cnt] - len_x[rides_sel_cnt] < rides_x[i] + len_z[i] &&
						rides_x[rides_sel_cnt] + len_x[rides_sel_cnt] > rides_x[i] - len_z[i] &&
						rides_z[rides_sel_cnt] - len_z[rides_sel_cnt] < rides_z[i] + len_x[i] &&
						rides_z[rides_sel_cnt] + len_z[rides_sel_cnt] > rides_z[i] - len_x[i])
						return rides_collision[rides_sel_cnt] = true;
				}
			}
			else if (rides_radian[rides_sel_cnt] == 90) {   // �ⱸ�� ȸ���� ���
				if (rides_radian[i] == 0) {
					if (rides_x[rides_sel_cnt] - len_z[rides_sel_cnt] < rides_x[i] + len_x[i] &&
						rides_x[rides_sel_cnt] + len_z[rides_sel_cnt] > rides_x[i] - len_x[i] &&
						rides_z[rides_sel_cnt] - len_x[rides_sel_cnt] < rides_z[i] + len_z[i] &&
						rides_z[rides_sel_cnt] + len_x[rides_sel_cnt] > rides_z[i] - len_z[i])
						return rides_collision[rides_sel_cnt] = true;
				}
				else if (rides_radian[i] == 90) {
					if (rides_x[rides_sel_cnt] - len_z[rides_sel_cnt] < rides_x[i] + len_z[i] &&
						rides_x[rides_sel_cnt] + len_z[rides_sel_cnt] > rides_x[i] - len_z[i] &&
						rides_z[rides_sel_cnt] - len_x[rides_sel_cnt] < rides_z[i] + len_x[i] &&
						rides_z[rides_sel_cnt] + len_x[rides_sel_cnt] > rides_z[i] - len_x[i])
						return rides_collision[rides_sel_cnt] = true;
				}
			}
		}
	}
	return rides_collision[rides_sel_cnt] = false;
}

GLvoid MouseWheel(int wheel, int direction, int x, int y) {
	if (view_check == 1) {
		if (direction == 1) {
			if (fov >= 1.0f && fov <= 70.0f)
				fov -= 1;
			if (fov <= 1.0f)
				fov = 1.0f;
		}
		else if (direction == -1) {
			if (fov >= 1.0f && fov <= 70.0f)
				fov += 1;
			if (fov >= 70.0f)
				fov = 70.0f;
		}
	}
}

GLvoid Reshape(int w, int h) {
	window_w = w;
	window_h = h;
	glViewport(0, 0, w, h);
}

void make_shaderProgram()
{
	make_vertexShaders(); //--- ���ؽ� ���̴� �����
	make_fragmentShaders(); //--- �����׸�Ʈ ���̴� �����
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- ���̴� �����ϱ�
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program ����ϱ�
	glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	//--- ���ؽ� ���̴� ��ü �����
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- ���ؽ� ���̴� �������ϱ�
	glCompileShader(vertexShader);
	//--- �������� ����� ���� ���� ���: ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- �����׸�Ʈ ���̴� ��ü �����
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- ���̴� �ڵ带 ���̴� ��ü�� �ֱ�
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- �����׸�Ʈ ���̴� ������
	glCompileShader(fragmentShader);
	//--- �������� ����� ���� ���� ���: ������ ���� üũ
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader ������ ����\n" << errorLog << std::endl;
		return;
	}
}

char* filetobuf(const char* file)
{
	FILE* fptr;
	long length;
	char* buf;
	fptr = fopen(file, "rb");
	if (!fptr)
		return NULL;
	fseek(fptr, 0, SEEK_END);
	length = ftell(fptr);
	buf = (char*)malloc(length + 1);
	fseek(fptr, 0, SEEK_SET);
	fread(buf, length, 1, fptr);
	fclose(fptr);
	buf[length] = 0;
	return buf;
}

void ReadObj(FILE* path, int index) {
	data[index].clear();
	char bind[128];
	memset(bind, 0, sizeof(bind));
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> faces;
	std::vector<glm::vec3> faces_normals;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> uvData;
	std::vector<glm::vec3> normals;

	while (!feof(path)) {
		fscanf(path, "%s", bind);
		if (bind[0] == 'v' && bind[1] == '\0') {
			glm::vec3 vertex;
			fscanf(path, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertices.push_back(vertex);
		}
		else if (bind[0] == 'f' && bind[1] == '\0') {
			unsigned int temp_face[3], temp_uv[3], temp_normal[3];
			fscanf(path, "%d/%d/%d %d/%d/%d %d/%d/%d\n",
				&temp_face[0], &temp_uv[0], &temp_normal[0],
				&temp_face[1], &temp_uv[1], &temp_normal[1],
				&temp_face[2], &temp_uv[2], &temp_normal[2]);

			faces.emplace_back(temp_face[0] - 1, temp_face[1] - 1, temp_face[2] - 1);
			uvData.emplace_back(temp_uv[0] - 1, temp_uv[1] - 1, temp_uv[2] - 1);
			faces_normals.emplace_back(temp_normal[0] - 1, temp_normal[1] - 1, temp_normal[2] - 1);
		}
		else if (bind[0] == 'v' && bind[1] == 't' && bind[2] == '\0') {
			glm::vec2 uv;
			fscanf(path, "%f %f\n", &uv.x, &uv.y);
			uvs.push_back(uv);
		}
		else if (bind[0] == 'v' && bind[1] == 'n' && bind[2] == '\0') {
			glm::vec3 normal;
			fscanf(path, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
	}
	for (int i = 0; i < faces.size(); ++i) {
		data[index].push_back(vertices[faces[i].x].x);
		data[index].push_back(vertices[faces[i].x].y);
		data[index].push_back(vertices[faces[i].x].z);
		data[index].push_back(normals[faces_normals[i].x].x);
		data[index].push_back(normals[faces_normals[i].x].y);
		data[index].push_back(normals[faces_normals[i].x].z);
		data[index].push_back(uvs[uvData[i].x].x);
		data[index].push_back(uvs[uvData[i].x].y);

		data[index].push_back(vertices[faces[i].y].x);
		data[index].push_back(vertices[faces[i].y].y);
		data[index].push_back(vertices[faces[i].y].z);
		data[index].push_back(normals[faces_normals[i].y].x);
		data[index].push_back(normals[faces_normals[i].y].y);
		data[index].push_back(normals[faces_normals[i].y].z);
		data[index].push_back(uvs[uvData[i].y].x);
		data[index].push_back(uvs[uvData[i].y].y);

		data[index].push_back(vertices[faces[i].z].x);
		data[index].push_back(vertices[faces[i].z].y);
		data[index].push_back(vertices[faces[i].z].z);
		data[index].push_back(normals[faces_normals[i].z].x);
		data[index].push_back(normals[faces_normals[i].z].y);
		data[index].push_back(normals[faces_normals[i].z].z);
		data[index].push_back(uvs[uvData[i].z].x);
		data[index].push_back(uvs[uvData[i].z].y);
	}
}