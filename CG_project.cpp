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

GLuint shaderProgramID; //--- 세이더 프로그램 이름
GLuint vertexShader; //--- 버텍스 세이더 객체
GLuint fragmentShader; //--- 프래그먼트 세이더 객체
GLchar* vertexSource, * fragmentSource; //--- 소스코드 저장 변수

float floor_xz[36] = {
	 -10.0, 0.0, -10.0, 0.0, 1.0, 0.0,  -10.0, 0.0, 10.0, 0.0, 1.0, 0.0,  10.0, 0.0, 10.0, 0.0, 1.0, 0.0,
	 -10.0, 0.0, -10.0, 0.0, 1.0, 0.0,  10.0, 0.0, 10.0, 0.0, 1.0, 0.0,  10.0, 0.0, -10.0, 0.0, 1.0, 0.0
};

GLuint vao[11], vbo[11];
std::vector<GLfloat> data[7];
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


void menu() {
	printf("------------ top view 명령어-------------\n");
	printf("1 : 롤러코스터 생성\n");
	printf("2 : 관람차 생성\n");
	printf("3 : 자이로드롭 생성\n");
	printf("4 : 회전목마 생성\n");
	printf("d : 기구 삭제\n");
	printf("v : 시점 변경\n");
	printf("----------- front view 명령어------------\n");
	printf("1 : 롤러코스터 시점\n");
	printf("2 : 관람차 시점\n");
	printf("3 : 자이로드롭 시점\n");
	printf("4 : 회전목마 시점\n");
	printf("w, a, s, d : 카메라 이동\n");
	printf("v : 시점 변경\n");
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

void main(int argc, char** argv)
{
	srand(time(NULL));
	//--- 윈도우 생성하기
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowPosition(200, 30);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Example");
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
	int modelLoc = glGetUniformLocation(shaderProgramID, "model"); //--- 버텍스 세이더에서 모델링 변환 행렬 변수값을 받아온다.
	int viewLoc = glGetUniformLocation(shaderProgramID, "view"); //--- 버텍스 세이더에서 뷰잉 변환 행렬 변수값을 받아온다.
	int projLoc = glGetUniformLocation(shaderProgramID, "projection");

	if (view_check == 0) {
		glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 0.0f);      //--- 카메라 위치
		glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f); //--- 카메라 바라보는 방향
		glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);        //--- 카메라 위쪽 방향

		glm::mat4 vTransform = glm::mat4(1.0f);
		vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

		glm::mat4 pTransform = glm::mat4(1.0f);
		pTransform = glm::ortho(-15.0f, 15.0f, -15.0f, 15.0f, -15.0f, 15.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);
	}
	else if (view_check == 1) {
		cameraDirection_x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		cameraDirection_y = sin(glm::radians(pitch));
		cameraDirection_z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));

		cameraPos = glm::vec3(cameraPos_x, cameraPos_y, cameraPos_z);      //--- 카메라 위치
		cameraDirection = glm::vec3(cameraDirection_x, cameraDirection_y, cameraDirection_z); //--- 카메라 바라보는 방향
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);        //--- 카메라 위쪽 방향

		glm::mat4 vTransform = glm::mat4(1.0f);
		vTransform = glm::lookAt(cameraPos, cameraPos + cameraDirection, cameraUp);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

		glm::mat4 pTransform = glm::mat4(1.0f);
		pTransform = glm::perspective(glm::radians(fov), (float)window_w / (float)window_h, 0.1f, 200.0f);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);
	}

	unsigned int objColorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
	unsigned int lightPosLocation = glGetUniformLocation(shaderProgramID, "lightPos");
	glUniform3f(lightPosLocation, 0.0, 5.0, 0.0);
	unsigned int lightColorLocation = glGetUniformLocation(shaderProgramID, "lightColor");
	glUniform3f(lightColorLocation, 1.0, 1.0, 1.0);
	unsigned int viewPosLocation = glGetUniformLocation(shaderProgramID, "viewPos");
	glUniform3f(viewPosLocation, cameraPos_x, cameraPos_y, cameraPos_z);

	glm::mat4 Line = glm::mat4(1.0f);
	glUniform3f(objColorLocation, 0.5, 0.5, 0.5);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(Line));
	glBindVertexArray(vao[10]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glutSwapBuffers();
}

void InitBuffer()
{
	glGenVertexArrays(11, vao);
	glGenBuffers(11, vbo);

	for (int i = 0; i < 7; ++i) {
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[i]);
		glBufferData(GL_ARRAY_BUFFER, data[i].size() * sizeof(GLfloat), data[i].data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

	glBindVertexArray(vao[10]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[10]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floor_xz), floor_xz, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), 0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
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
		break;
	case 'v':
		if (view_check == 1)
			view_check = 0;
		else if (view_check == 0)
			view_check = 1;
		break;
	case '1':
		break;
	case '2':
		break;
	case '3':
		break;
	case '4':
		break;
	case 'r':   // 리셋
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

	normalized_x = (2.0 * x / 800) - 1.0;
	normalized_y = 1.0 - (2.0 * y / 600);
	if (view_check == 0) {}
	else if (view_check == 1) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			left_button = true;
			prev_mouse_x = normalized_x;  // 저장된 이전 마우스 위치
			prev_mouse_y = normalized_y;
		}
		if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
			left_button = false;
		}
	}
}

void Motion(int x, int y) {
	float normalized_x, normalized_y;

	normalized_x = (2.0 * x / 800) - 1.0;
	normalized_y = 1.0 - (2.0 * y / 600);
	if (view_check == 0) {}
	else if (view_check == 1) {
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
	fptr = fopen(file, "rb"); // Open file for reading 
	if (!fptr) // Return NULL on failure 
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file 
	length = ftell(fptr); // Find out how many bytes into the file we are 
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator 
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file 
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer 
	fclose(fptr); // Close the file 
	buf[length] = 0; // Null terminator 
	return buf; // Return the buffer 
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

		data[index].push_back(vertices[faces[i].y].x);
		data[index].push_back(vertices[faces[i].y].y);
		data[index].push_back(vertices[faces[i].y].z);
		data[index].push_back(normals[faces_normals[i].y].x);
		data[index].push_back(normals[faces_normals[i].y].y);
		data[index].push_back(normals[faces_normals[i].y].z);

		data[index].push_back(vertices[faces[i].z].x);
		data[index].push_back(vertices[faces[i].z].y);
		data[index].push_back(vertices[faces[i].z].z);
		data[index].push_back(normals[faces_normals[i].z].x);
		data[index].push_back(normals[faces_normals[i].z].y);
		data[index].push_back(normals[faces_normals[i].z].z);
	}
}