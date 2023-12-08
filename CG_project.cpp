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
void tree_collisions();

GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수

float floor_xz[] = {
	 -15.0, 0.0, -15.0,    0.0, 1.0, 0.0,  0, 0,
	 -15.0, 0.0, 15.0,     0.0, 1.0, 0.0,  0, 1,
	 15.0, 0.0, 15.0,      0.0, 1.0, 0.0,  1, 1,
	 -15.0, 0.0, -15.0,    0.0, 1.0, 0.0,  0, 0,
	 15.0, 0.0, 15.0,      0.0, 1.0, 0.0,  1, 1,
	 15.0, 0.0, -15.0,     0.0, 1.0, 0.0,  1, 0
};

float rides_data[5][12] = {
	{},
	{9.0, 0.0, 3.0, -9.0, 0.0, 3.0, -9.0, 0.0, -3.0, 9.0, 0.0, -3.0},
	{5.0, 0.0, 2.0, -5.0, 0.0, 2.0, -5.0, 0.0, -2.0, 5.0, 0.0, -2.0},
	{4.0, 0.0, 4.0, -4.0, 0.0, 4.0, -4.0, 0.0, -4.0, 4.0, 0.0, -4.0},
	{4.0, 0.0, 4.0, -4.0, 0.0, 4.0, -4.0, 0.0, -4.0, 4.0, 0.0, -4.0}
};

// 롤러코스터
GLfloat point[9][3] = { {-8, 0.0, 2.5}, {-2.0, 0.0, 2.5}, {2.0, 0.0, 2.5}, {8, 0.0, 2.5},
						{8, 0.0, -2.5}, {2.0, 0.0, -2.5}, {-2.0, 0.0, -2.5}, {-8, 0.0, -2.5}, {} };
GLfloat matrix_2[3][3] = { {2.0, -4.0, 2.0}, {-3.0, 4.0, -1.0}, {1.0, 0.0, 0.0} };
GLfloat matrix_3[4][4] = { {-1.0, 3.0, -3.0, 1.0},
						  {2.0, -5.0, 4.0, -1.0},
						  {-1.0, 0.0, 1.0, 0.0},
						  {0.0, 2.0, 0.0, 0.0} 
};

GLuint vao[1000], vbo[1000];
std::vector<GLfloat> data[1000];
glm::vec3 cameraPos;      //--- 카메라 위치
glm::vec3 cameraDirection; //--- 카메라 바라보는 방향
glm::vec3 cameraUp;        //--- 카메라 위쪽 방향
float cameraPos_x, cameraPos_y, cameraPos_z;
float cameraDirection_x, cameraDirection_y, cameraDirection_z;
float cameraSpeed;
float pitch, yaw;                                // 마우스 화면 이동
float fov;                                       // 줌
BOOL left_button;
int window_w, window_h;
BOOL key_w, key_a, key_s, key_d;

int view_check;                                  // 시점 확인   0. top_view / 1. front_view
float rides_x[5], rides_z[5];    // 기구 중점
int rides_sel_cnt;                 // 0. 없음 / 1. 롤러코스터 / 2. 관람차 / 3. 자이로드롭 / 4. 회전목마
BOOL rides_sel_check[5];             // 기구가 선택되었는지 0. 선택 안됨
BOOL rides_install_check[5];  // 기구가 설치되었는지
BOOL rides_collision[5];  // 설치 기구가 충돌하는지    충돌하면 true / 안하면 false
float len_x[5], len_z[5];  // 기구 가로 세로 길이
float rides_radian[5];     // 기구 회전
float tree_x[30], tree_z[30];   // 나무 좌표
int tree_cnt;     // 설치된 나무의 개수 
BOOL point_sel[8];  // 롤러코스터 정점 선택
int point_sel_index;   // 선택된 정점의 인덱스
float point_save[9][3];  // 초기 롤러코스터 위치;
BOOL key_c, key_t;  // 롤러코스터 편집

void menu() {
	printf("------------ top view 명령어-------------\n");
	printf("1 : 롤러코스터 생성\n");
	printf("2 : 관람차 생성\n");
	printf("3 : 자이로드롭 생성\n");
	printf("4 : 회전목마 생성\n");
	printf("b : 기구 설치\n");
	printf("d : 기구 삭제\n");
	printf("----------- front view 명령어------------\n");
	printf("1 : 롤러코스터 시점\n");
	printf("2 : 관람차 시점\n");
	printf("3 : 자이로드롭 시점\n");
	printf("4 : 회전목마 시점\n");
	printf("w, a, s, d : 카메라 이동\n");
	printf("------------- 공용 명령어 ---------------\n");
	printf("c : 롤러코스터 정점 위치 변경\n");
	printf("t : c 상태에서 시점 변경\n");
	printf("v : 시점 변경\n");
	printf("` : 리셋\n");
	printf("q : 프로그램 종료\n");
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
	tree_cnt = 30;
	key_c = key_t = false;
	// 기구의 길이
	len_x[1] = 9, len_x[2] = 5, len_x[3] = len_x[4] = 4;
	len_z[1] = 3, len_z[2] = 2, len_z[3] = len_z[4] = 4;
	// 기구 설치
	for (int i = 0; i < 5; ++i) {
		rides_install_check[i] = false;
		rides_sel_check[i] = false;
		rides_x[i] = rides_z[i] = 0;
		rides_collision[i] = false;
		rides_radian[i] = 0;
	}
	// 롤러코스터 정점 랜덤값
	for (int i = 0; i < 8; ++i) {
		point[i][0] = point_save[i][0];
		point[i][1] = (rand() % 501 + 500) * 0.01;    // 5 <= y <= 10
		point[i][2] = point_save[i][2];
		point_sel[i] = false;
	}
	point[8][0] = point[0][0];
	point[8][1] = point[0][1];
	point[8][2] = point[0][2];
	point_sel_index = 0;
	// 나무 위치
	for (int i = 0; i < tree_cnt; ++i) {
		tree_x[i] = (rand() % 2801 - 1400) * 0.01;
		tree_z[i] = (rand() % 2801 - 1400) * 0.01;
	}

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	const char* cube_obj = "obj\\cube.obj";
	const char* cylinder_obj = "obj\\cylinder.obj";
	const char* sphere_obj = "obj\\sphere.obj";
	const char* cone_obj = "obj\\cone.obj";
	FILE* file_cube;
	FILE* file_cylinder;
	FILE* file_sphere;
	FILE* file_cone;
	file_cube = fopen(cube_obj, "r");
	ReadObj(file_cube, 0);
	fclose(file_cube);
	file_cylinder = fopen(cylinder_obj, "r");
	ReadObj(file_cylinder, 1);
	fclose(file_cylinder);
	file_sphere = fopen(sphere_obj, "r");
	ReadObj(file_sphere, 2);
	fclose(file_sphere);
	file_cone = fopen(cone_obj, "r");
	ReadObj(file_cone, 3);
	fclose(file_cone);
}

void main(int argc, char** argv)
{
	srand(time(NULL));
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(300, 30);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Amusement Park");
	glewExperimental = GL_TRUE;
	menu();
	for (int i = 0; i < 9; ++i) {
		point_save[i][0] = point[i][0];
		point_save[i][2] = point[i][2];
	}
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
	glClearColor(0.2, 0.5, 0.8, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	int modelLoc = glGetUniformLocation(shaderProgramID, "model"); //--- 버텍스 세이더에서 모델링 변환 행렬 변수값을 받아온다.
	int viewLoc = glGetUniformLocation(shaderProgramID, "view"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
	int projLoc = glGetUniformLocation(shaderProgramID, "projection");

	unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, 0.0, 5.0, 0.0);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos_x, cameraPos_y, cameraPos_z);

	if (!key_c) {
		// top view 
		if (view_check == 0) {
			glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 0.0f);
			glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, 1.0f);

			glm::mat4 vTransform = glm::mat4(1.0f);
			vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

			glm::mat4 pTransform = glm::mat4(1.0f);
			pTransform = glm::ortho(16.0f, -16.0f, 16.0f, -16.0f, -16.0f, 16.0f);
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);
		}
		// front view
		else if (view_check == 1) {
			cameraDirection_x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
			cameraDirection_y = sin(glm::radians(pitch));
			cameraDirection_z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

			cameraPos = glm::vec3(cameraPos_x, cameraPos_y, cameraPos_z);
			cameraDirection = glm::vec3(cameraDirection_x, cameraDirection_y, cameraDirection_z);
			cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

			glm::mat4 vTransform = glm::mat4(1.0f);
			vTransform = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
			glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

			glm::mat4 pTransform = glm::mat4(1.0f);
			pTransform = glm::perspective(glm::radians(fov), (float)window_w / (float)window_h, 0.1f, 200.0f);
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);
		}
		// 롤러코스터
		else if (view_check == 2) {}
		// 관람차
		else if (view_check == 3) {}
		// 자이로드롭
		else if (view_check == 4) {}
		// 회전 목마
		else if (view_check == 5) {}

		// 놀이 기구
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
				if (i == 1) { // 롤러코스터인 경우
					// 정점 찍기
					glPointSize(10.0f);
					glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(border));
					glUniform3f(objColorLocation, 1.0, 0.0, 0.0);
					glBegin(GL_POINTS);
					for (int i = 0; i < 9; ++i)
						glVertex3f(point[i][0], point[i][1], point[i][2]);
					glEnd();

					// 곡선 그리기
					glLineWidth(20.0f);
					GLfloat result[4][3];
					GLfloat t = 0.0f;
					GLfloat point_x, point_y, point_z;

					memset(result, 0, sizeof(result));
					for (int i = 0; i < 3; ++i) {
						for (int j = 0; j < 3; ++j) {
							result[i][0] += matrix_2[i][j] * point[j][0];
							result[i][1] += matrix_2[i][j] * point[j][1];
							result[i][2] += matrix_2[i][j] * point[j][2];
						}
					}
					glUniform3f(objColorLocation, 0.0, 1.0, 0.0);
					glBegin(GL_LINE_STRIP);
					while (t < 0.5f) {
						point_x = result[2][0] + t * (result[1][0] + result[0][0] * t);
						point_y = result[2][1] + t * (result[1][1] + result[0][1] * t);
						point_z = result[2][2] + t * (result[1][2] + result[0][2] * t);
						glVertex3f(point_x, point_y, point_z);
						t += 0.01f;
					}
					glEnd();

					for (int cubic_case = 0; cubic_case < 9 - 2; ++cubic_case) {
						memset(result, 0, sizeof(result));
						for (int i = 0; i < 4; ++i) {
							for (int j = 0; j < 4; ++j) {
								result[i][0] += matrix_3[i][j] * point[j + cubic_case][0];
								result[i][1] += matrix_3[i][j] * point[j + cubic_case][1];
								result[i][2] += matrix_3[i][j] * point[j + cubic_case][2];
							}
						}

						t = 0.0f;
						glBegin(GL_LINE_STRIP);
						while (t < 1.0f) {
							point_x = (result[3][0] + t * (result[2][0] + t * (result[1][0] + result[0][0] * t))) * 0.5f;
							point_y = (result[3][1] + t * (result[2][1] + t * (result[1][1] + result[0][1] * t))) * 0.5f;
							point_z = (result[3][2] + t * (result[2][2] + t * (result[1][2] + result[0][2] * t))) * 0.5f;
							glVertex3f(point_x, point_y, point_z);
							t += 0.01f;
						}
						glEnd();
					}

					// 원기둥
					for (int i = 0; i < 8; ++i) {
						glUniform3f(objColorLocation, 0.3, 0.3, 0.3);
						glm::mat4 pillar = glm::mat4(1.0f);
						glm::mat4 pillar_T = glm::mat4(1.0f);
						glm::mat4 pillar_S = glm::mat4(1.0f);
						pillar_T = glm::translate(pillar_T, glm::vec3(point[i][0], point[i][1] / 2, point[i][2]));
						pillar_S = glm::scale(pillar_S, glm::vec3(0.3, point[i][1] / 2, 0.3));
						pillar = border_T * pillar_T * pillar_S;
						glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(pillar));
						glBindVertexArray(vao[1]);
						glDrawArrays(GL_TRIANGLES, 0, 84);
					}
				}
				else if (i == 2) {   // 관람차인 경우

				}
				else if (i == 3) {   // 자이로드롭인 경우

				}
				else if (i == 4) {   // 회전목마인 경우

				}
			}
		}

		// 바닥
		glm::mat4 floor = glm::mat4(1.0f);
		glUniform3f(objColorLocation, 0.5, 0.5, 0.5);
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(floor));
		glBindVertexArray(vao[10]);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// 나무
		for (int i = 0; i < tree_cnt; ++i) {
			glm::mat4 tree = glm::mat4(1.0f);
			glm::mat4 tree_T = glm::mat4(1.0f);
			glm::mat4 tree_S = glm::mat4(1.0f);
			tree_T = glm::translate(tree_T, glm::vec3(tree_x[i], 0.7, tree_z[i]));
			tree_S = glm::scale(tree_S, glm::vec3(0.3, 0.7, 0.3));
			tree = tree_T * tree_S;
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tree));
			glBindVertexArray(vao[0]);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			glm::mat4 tree_leaf = glm::mat4(1.0f);
			glm::mat4 tree_leaf_T = glm::mat4(1.0f);
			glm::mat4 tree_leaf_S = glm::mat4(1.0f);
			tree_leaf_T = glm::translate(tree_leaf_T, glm::vec3(tree_x[i], 2.5, tree_z[i]));
			tree_leaf_S = glm::scale(tree_leaf_S, glm::vec3(1.0, 1.5, 1.0));
			tree_leaf = tree_leaf_T * tree_leaf_S;
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(tree_leaf));
			glBindVertexArray(vao[2]);
			glDrawArrays(GL_TRIANGLES, 0, 242);
		}
	}
	else {
		// top view
		if (!key_t) {

		}
		// front view
		else {

		}
	}

	glutSwapBuffers();
}

void InitBuffer()
{
	glGenVertexArrays(1000, vao);
	glGenBuffers(1000, vbo);

	for (int i = 0; i < 4; ++i) {
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
	if (rides_install_check[1] == true) {

	}
	if (rides_install_check[2] == true) {

	}
	if (rides_install_check[3] == true) {

	}
	if (rides_install_check[4] == true) {

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
			if (rides_collision[rides_sel_cnt] == false) {
				tree_collisions();
				rides_install_check[rides_sel_cnt] = true;
			}
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
	case 'c':
		if(rides_install_check[1] == true)
			key_c = !key_c;
		break;
	case '`':   // 리셋
		reset();
		InitBuffer();
		break;
	case 'q':   // 프로그램 종료
		std::cout << "프로그램을 종료합니다." << std::endl;
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

	if (view_check == 0) {
	}
	else if (view_check == 1) {
		normalized_x = (2.0 * x / 600) - 1.0;
		normalized_y = 1.0 - (2.0 * y / 600);
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			left_button = true;
			prev_mouse_x = normalized_x;  // 저장된 이전 마우스 위치
			prev_mouse_y = normalized_y;
		}
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			left_button = false;
		}
	}
	if (key_c) {
		normalized_x = ((2.0 * x / 600) - 1.0) * 15;
		normalized_y = ((2.0 * y / 600) - 1.0) * 15;
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && rides_install_check[1] == true && rides_sel_cnt == 1) {
			for (int i = 0; i < 8; ++i) {
				if (normalized_x > point[i][0] - 1.0 && normalized_x < point[i][0] + 1.0 &&
					normalized_y > point[i][2] - 1.0 && normalized_y < point[i][2] + 1.0 ) {
					point_sel[i] = true;
					point_sel_index = i;
					break;
				}
			}
		}
	}
}

void Motion(int x, int y) {
	float normalized_x, normalized_y;

	if (view_check == 0) {
		normalized_x = ((2.0 * x / 600) - 1.0) * 15;
		normalized_y = ((2.0 * y / 600) - 1.0) * 15;
		if (rides_install_check[rides_sel_cnt] == false) {

			if (rides_sel_check[rides_sel_cnt] == true) {
				rides_x[rides_sel_cnt] = normalized_x;
				rides_z[rides_sel_cnt] = normalized_y;
				rides_collision_check();
			}
		}
		if (point_sel[point_sel_index] == true) {
			if (normalized_x >= rides_x[1] - 8 && normalized_x <= rides_x[1] + 8 &&
				normalized_y >= rides_z[1] - 2.5 && normalized_y <= rides_z[1] + 2.5) {
				if (point_sel_index == 0) {
					point[0][0] = point[8][0] = normalized_x;
					point[0][2] = point[8][2] = normalized_y;
				}
				else {
					point[point_sel_index][0] = normalized_x;
					point[point_sel_index][2] = normalized_y;
				}
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

// 기구의 충돌 검사
BOOL rides_collision_check() {
	if (rides_radian[rides_sel_cnt] == 0) {   // 회전하지 않은 기구가 바닥을 벗어난 경우
		if (rides_x[rides_sel_cnt] - len_x[rides_sel_cnt] < -15) return rides_collision[rides_sel_cnt] = true;
		if (rides_x[rides_sel_cnt] + len_x[rides_sel_cnt] > 15) return rides_collision[rides_sel_cnt] = true;
		if (rides_z[rides_sel_cnt] - len_z[rides_sel_cnt] < -15) return rides_collision[rides_sel_cnt] = true;
		if (rides_z[rides_sel_cnt] + len_z[rides_sel_cnt] > 15) return rides_collision[rides_sel_cnt] = true;
	}
	else if (rides_radian[rides_sel_cnt] == 90) {   // 회전한 기구가 바닥을 벗어난 경우
		if (rides_x[rides_sel_cnt] - len_z[rides_sel_cnt] < -15) return rides_collision[rides_sel_cnt] = true;
		if (rides_x[rides_sel_cnt] + len_z[rides_sel_cnt] > 15) return rides_collision[rides_sel_cnt] = true;
		if (rides_z[rides_sel_cnt] - len_x[rides_sel_cnt] < -15) return rides_collision[rides_sel_cnt] = true;
		if (rides_z[rides_sel_cnt] + len_x[rides_sel_cnt] > 15) return rides_collision[rides_sel_cnt] = true;
	}
	for (int i = 1; i < 5; ++i) {
		if (i != rides_sel_cnt && rides_install_check[i] == true) {
			if (rides_radian[rides_sel_cnt] == 0) {              // 기구가 회전하지 않은 경우
				if(rides_radian[i] == 0){          // 설치된 기구가 회전하지 않은 경우
					if (rides_x[rides_sel_cnt] - len_x[rides_sel_cnt] < rides_x[i] + len_x[i] &&
						rides_x[rides_sel_cnt] + len_x[rides_sel_cnt] > rides_x[i] - len_x[i] &&
						rides_z[rides_sel_cnt] - len_z[rides_sel_cnt] < rides_z[i] + len_z[i] &&
						rides_z[rides_sel_cnt] + len_z[rides_sel_cnt] > rides_z[i] - len_z[i]) 
						return rides_collision[rides_sel_cnt] = true;
				}         
				else if (rides_radian[i] == 90) {    // 설치된 기구가 회전한 경우
					if (rides_x[rides_sel_cnt] - len_x[rides_sel_cnt] < rides_x[i] + len_z[i] &&
						rides_x[rides_sel_cnt] + len_x[rides_sel_cnt] > rides_x[i] - len_z[i] &&
						rides_z[rides_sel_cnt] - len_z[rides_sel_cnt] < rides_z[i] + len_x[i] &&
						rides_z[rides_sel_cnt] + len_z[rides_sel_cnt] > rides_z[i] - len_x[i])
						return rides_collision[rides_sel_cnt] = true;
				}
			}
			else if (rides_radian[rides_sel_cnt] == 90) {   // 기구가 회전한 경우
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

// 기구 설치 범위 내에 나무가 있으면 제거
void tree_collisions() {
	if (rides_radian[rides_sel_cnt] == 0) {   // 회전하지 않은 기구
		for (int i = 0; i < tree_cnt; ++i) {
			if (rides_x[rides_sel_cnt] - len_x[rides_sel_cnt] < tree_x[i] + 0.8 && rides_x[rides_sel_cnt] + len_x[rides_sel_cnt] > tree_x[i] - 0.8 &&
				rides_z[rides_sel_cnt] - len_z[rides_sel_cnt] < tree_z[i] + 0.8 && rides_z[rides_sel_cnt] + len_z[rides_sel_cnt] > tree_z[i] - 0.8) {
				for (int j = i; j < tree_cnt - 1; ++j) {
					tree_x[j] = tree_x[j + 1];
					tree_z[j] = tree_z[j + 1];
				}
				tree_x[tree_cnt - 1] = tree_z[tree_cnt - 1] = 0;
				--tree_cnt;
				--i;
			}
		}
	}
	else if (rides_radian[rides_sel_cnt] == 90) {   // 회전한 기구
		for (int i = 0; i < tree_cnt; ++i) {
			if (rides_x[rides_sel_cnt] - len_z[rides_sel_cnt] < tree_x[i] + 0.8 && rides_x[rides_sel_cnt] + len_z[rides_sel_cnt] > tree_x[i] - 0.8 &&
				rides_z[rides_sel_cnt] - len_x[rides_sel_cnt] < tree_z[i] + 0.8 && rides_z[rides_sel_cnt] + len_x[rides_sel_cnt] > tree_z[i] - 0.8) {
				for (int j = i; j < tree_cnt - 1; ++j) {
					tree_x[j] = tree_x[j + 1];
					tree_z[j] = tree_z[j + 1];
				}
				tree_x[tree_cnt - 1] = tree_z[tree_cnt - 1] = 0;
				--tree_cnt;
				--i;
			}
		}
	}
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
	make_vertexShaders(); //--- 버텍스 세이더 만들기
	make_fragmentShaders(); //--- 프래그먼트 세이더 만들기
	//-- shader Program
	shaderProgramID = glCreateProgram();
	glAttachShader(shaderProgramID, vertexShader);
	glAttachShader(shaderProgramID, fragmentShader);
	glLinkProgram(shaderProgramID);
	//--- 세이더 삭제하기
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	//--- Shader Program 사용하기
	glUseProgram(shaderProgramID);
}

void make_vertexShaders()
{
	vertexSource = filetobuf("vertex.glsl");
	//--- 버텍스 세이더 객체 만들기
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(vertexShader, 1, (const GLchar**)&vertexSource, 0);
	//--- 버텍스 세이더 컴파일하기
	glCompileShader(vertexShader);
	//--- 컴파일이 제대로 되지 않은 경우: 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "ERROR: vertex shader 컴파일 실패\n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	fragmentSource = filetobuf("fragment.glsl");
	//--- 프래그먼트 세이더 객체 만들기
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	//--- 세이더 코드를 세이더 객체에 넣기
	glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentSource, 0);
	//--- 프래그먼트 세이더 컴파일
	glCompileShader(fragmentShader);
	//--- 컴파일이 제대로 되지 않은 경우: 컴파일 에러 체크
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: fragment shader 컴파일 실패\n" << errorLog << std::endl;
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