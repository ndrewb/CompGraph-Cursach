#define _CRT_SECURE_NO_WARNINGS
#include "Render.h"

#include <sstream>
#include <iostream>
#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include <conio.h>
#include "MyOGL.h"
#include "Camera.h"
#include "Light.h"
#include "Primitives.h"
#include "GUItextRectangle.h"
#include <fstream>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <Mmsystem.h>
#include <mciapi.h>
#include <time.h>

#pragma comment(lib, "Winmm.lib")

using namespace std;


double lowest_point = 100;

bool textureMode = true;
bool lightMode = true;
int doMusicPlay = 1;
bool doLoadObject = true;
bool doLoadTerrain = true;
bool doLoadEnemy = true;
bool DEBUG = false;
bool isEnemyReached = false;
int kills = 0;

long int head_edges_number;
long int body_edges_number;
long int nail_edges_number;
long int cloak_edges_number;

//Размер загружаемой модели
double size_coefficient = 0.2; 

//Переменные для удара мечом
int swing = 0;
double swing_angle_old = 0;
double swing_angle = 0;
double nu = 0;

//Переменные для прыжка
int jump = 0;
double jump_angle = 0;
double jump_nu = 0;

//Переменные для движения
double walk_angle = 0;
double walk_x=5;
double walk_y = 5;
double walk_z=5;

//Координаты моба
double enemy_x = 5;
double enemy_y = -5;

//Точка
struct Point {
	double x;
	double y;
	double z;
};

//Полигон
struct Edge {
	long int A;
	long int B;
	long int C;
};

Edge* head_edges = (Edge*)malloc(sizeof(Edge));
Point* head_points = (Point*)malloc(sizeof(Point));

Edge* body_edges = (Edge*)malloc(sizeof(Edge));
Point* body_points = (Point*)malloc(sizeof(Point));

Edge* nail_edges = (Edge*)malloc(sizeof(Edge));
Point* nail_points = (Point*)malloc(sizeof(Point));

Edge* cloak_edges = (Edge*)malloc(sizeof(Edge));
Point* cloak_points = (Point*)malloc(sizeof(Point));

void strout(string text)
{
	const char* outp = text.c_str();
	_cputs(outp);
}

void LoadObj(const char* file_path, string part)
{
	long int edges_number;

	Edge* part_edges = (Edge*)malloc(sizeof(Edge));
	Point* part_points = (Point*)malloc(sizeof(Point));

	string j_str;
	const char* outp;
	
	char load_string[50] = "none";
	FILE* model;
	model = fopen(file_path, "r");
	if (model != 0)
	{
		long int j = 0;
		long int k = 0;
		while (fgets(load_string, 50, model) != NULL )
		{
			int i = 0;

			while (i < 50)	
			{
				if (load_string[0] == 'v' && load_string[1] == 32)
				{
					j++;


					string x_str;
					string y_str;
					string z_str;
					double x;
					double y;
					double z;
					i=i+2;
					while (load_string[i] != 32)
					{
						x_str = x_str + load_string[i];
						i++;
					}

					x = std::stod(x_str);
					i = i + 1;

					while (load_string[i] != 32)
					{
						y_str = y_str + load_string[i];
						i++;
					}

					y = std::stod(y_str);
					i = i + 1;

					while (load_string[i] != 32 && i<50)
					{
						z_str = z_str + load_string[i];
						i++;
					}
					z = std::stod(z_str);

					size_t size = _msize(part_points);
					part_points = (Point*)realloc(part_points, size + sizeof(Point));
					part_points[j].x = x * size_coefficient;
					part_points[j].y = y * size_coefficient;
					part_points[j].z = z * size_coefficient;
				}

				if (load_string[0] == 'f')
				{
					k++;

					string A_str;
					string B_str;
					string C_str;
					double A;
					double B;
					double C;
					i = i + 2;
					while (load_string[i] != 32)
					{
						if (load_string[i] == 47)
						{
							while (load_string[i] != 32)
							{
								i++;
							}
						}
						else
						{
							A_str = A_str + load_string[i];
							i++;
						}
					}

					A = std::stod(A_str);

					if (DEBUG == true)
					{
						outp = A_str.c_str();
						_cputs(outp);
						_cputs(" ");
					}

					i = i + 1;
					while (load_string[i] != 32)
					{
						if (load_string[i] == 47)
						{
							while (load_string[i] != 32)
							{
								i++;
							}
						}
						else
						{
							B_str = B_str + load_string[i];
							i++;
						}


					}
					B = std::stod(B_str);

					if (DEBUG == true)
					{
						outp = B_str.c_str();
						_cputs(outp);
						_cputs(" ");
					}
					i = i + 1;

					while (load_string[i] != 32  && i < 50)
					{
						if (load_string[i] == 47)
						{
							while (load_string[i] != 32)
							{
								i++;
							}
						}
						else
						{
							C_str = C_str + load_string[i];
							i++;
						}

					}
					C = std::stod(C_str);

					if (DEBUG == true)
					{
						outp = C_str.c_str();
						_cputs(outp);
						_cputs("\n");
					}

					size_t size = _msize(part_edges);
					part_edges = (Edge*)realloc(part_edges, size + sizeof(Edge));
					part_edges[k].A = A;
					part_edges[k].B = B;
					part_edges[k].C = C;
				}

				i = 60;
			}
		}

		edges_number = k;
	}

	if (part == "head")
	{
		head_edges_number = edges_number;
		head_edges = part_edges;
		head_points = part_points;
	}

	if (part == "nail")
	{
		nail_edges_number = edges_number;
		nail_edges = part_edges;
		nail_points = part_points;
	}

	if (part == "cloak")
	{
		cloak_edges_number = edges_number;
		cloak_edges = part_edges;
		cloak_points = part_points;
	}

	if (part == "body")
	{
		body_edges_number = edges_number;
		body_edges = part_edges;
		body_points = part_points;
	}
	strout(part + " loaded!\n");
}


int GetRandomNumber(int min, int max)
{
	// Установить генератор случайных чисел
	srand(time(NULL));

	// Получить случайное число - формула
	int num = min + rand() % (max - min + 1);

	return num;
}

//класс для настройки камеры
class CustomCamera : public Camera
{
public:
	//дистанция камеры
	double camDist;
	//углы поворота камеры
	double fi1, fi2;

	
	//значния масеры по умолчанию
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//считает позицию камеры, исходя из углов поворота, вызывается движком
	void SetUpCamera()
	{
		//отвечает за поворот камеры мышкой
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//функция настройки камеры
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //создаем объект камеры


//Класс для настройки света
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//начальная позиция света
		pos = Vector3(1, 1, 3);
	}

	
	//рисует сферу и линии под источником света, вызывается движком
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//линия от источника света до окружности
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//рисуем окруность
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { -21, -32, 45, 1. };

		// параметры источника света
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// характеристики излучаемого света
		// фоновое освещение (рассеянный свет)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// диффузная составляющая света
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// зеркально отражаемая составляющая света
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //создаем источник света




//старые координаты мыши
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//меняем углы камеры при нажатой левой кнопке мыши
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//двигаем свет по плоскости, в точку где мышь
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{

		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
	
	if (key == 1 && swing!=1 &&swing_angle<=0)
	{
		swing = 1;
		if (isEnemyReached == 1)
		{
			kills++;
			enemy_x = GetRandomNumber(-45, 45);
			enemy_y = GetRandomNumber(-45, 45);
		}
	}

	if (key == 'W')
	{
		//walk_y = walk_y - 1;
		walk_y = walk_y - cos(walk_angle * PI / 180);
		walk_x = walk_x + sin(walk_angle * PI / 180);
	}

	if (key == 'S')
	{
		walk_y = walk_y + cos(walk_angle * PI / 180);
		walk_x = walk_x - sin(walk_angle * PI / 180);
	}

	if (key == 'A')
	{
		walk_angle = walk_angle + 15;
		if (abs(walk_angle) >= 360)
			walk_angle = 0;
		//walk_x = walk_x + 1;
	}

	if (key == 'D')
	{
		walk_angle = walk_angle - 15;
		if (abs(walk_angle) >= 360 )
			walk_angle = 0;
		//walk_x = walk_x - 1;
	}

	if (key == 'M')
	{
		doMusicPlay = !doMusicPlay;
		if (doMusicPlay == 1)
		{
			mciSendString("resume mp3", NULL, 0, NULL);
		}
		else
			mciSendString("pause mp3", NULL, 0, NULL);
	}

	if (key == 32 && jump != 1 && jump_angle <= 0)
	{
		jump=1;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}

GLUquadricObj *quadObj = gluNewQuadric();;
GLuint tex;

//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
	enemy_x = GetRandomNumber(-45, 45);
	enemy_x = GetRandomNumber(-45, 45);

	if (!AllocConsole())
	{
		MessageBox(NULL,
			"Console allocation failed",
			"Ошибка Win32 API",
			MB_OK | MB_ICONINFORMATION
		);
	}

if (doLoadObject == true)
{
	LoadObj("Objects\\head.obj", "head");
	LoadObj("Objects\\body.obj", "body");
	LoadObj("Objects\\nail.obj", "nail");
	LoadObj("Objects\\cloak.obj", "cloak");
}

	//настройка текстур

	//4 байта на хранение пикселя
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//настройка режима наложения текстур
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//включаем текстуры
	glEnable(GL_TEXTURE_2D);
	

	//массив трехбайтных элементов  (R G B)
	RGBTRIPLE *texarray;

	//массив символов, (высота*ширина*4      4, потомучто   выше, мы указали использовать по 4 байта на пиксель текстуры - R G B A)
	char *texCharArray;
	int texW, texH;


	OpenGL::LoadBMP("texture_lowx4.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	//генерируем ИД для текстуры
	glGenTextures(1, &tex);

	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, tex);

	//загружаем текстуру в видеопямять, в оперативке нам больше  она не нужна
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//отчистка памяти
	free(texCharArray);
	free(texarray);


	//наводим шмон
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//камеру и свет привязываем к "движку"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// нормализация нормалей : их длины будет равна 1
	glEnable(GL_NORMALIZE);

	// устранение ступенчатости для линий
	glEnable(GL_LINE_SMOOTH); 


	//   задать параметры освещения
	//  параметр GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  лицевые и изнаночные рисуются одинаково(по умолчанию), 
	//                1 - лицевые и изнаночные обрабатываются разными режимами       
	//                соответственно лицевым и изнаночным свойствам материалов.    
	//  параметр GL_LIGHT_MODEL_AMBIENT - задать фоновое освещение, 
	//                не зависящее от сточников
	// по умолчанию (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;

	mciSendString("open \"greenpath.mp3\" type mpegvideo alias mp3", NULL, 0, NULL);
	mciSendString("play mp3", NULL, 0, NULL);



}



void Render(OpenGL *ogl)
{

	if (jump == 1)
	{
		jump_nu = jump_nu + 3.141 / 20;
		jump_angle = 4*sin(jump_nu);
		if (jump_angle >= 3.99)
		{
			jump = 0;
		}
	}
	else
		if (jump_angle > 0)
		{
			jump_nu = jump_nu - 3.141 / 20;
			jump_angle = 4*sin(jump_nu);
		}
		else
		{
			jump = 0;
		}

	if (swing==1)
	{
		nu = nu + 3.141/20;
		swing_angle_old = swing_angle;
		swing_angle= 360*sin(nu);
		if (swing_angle >= 359)
		{
			swing = 0;
		}
	}
	else
		if (swing_angle > 0)
		{
			swing_angle_old = swing_angle;
			nu = nu - 3.141 / 20;
			swing_angle = 360 * sin(nu);
		}
		else
		{
			swing = 0;
		}


	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//альфаналожение
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//настройка материала
	//GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	//GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	//GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	//GLfloat sh = 0.1f * 256;
	
	GLfloat amb[] = { 1, 1, 1, 1. };
	GLfloat dif[] = { 1, 1, 1, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); 
		//размер блика
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  

	glTranslatef(walk_x, walk_y, jump_angle + 9.0288);
	glRotatef(walk_angle, 0, 0, 1);

	for (int i = 1; i < head_edges_number; i++)
	{
		double P1[] = { head_points[head_edges[i].A].x,head_points[head_edges[i].A].y,head_points[head_edges[i].A].z };
		double P2[] = { head_points[head_edges[i].B].x,head_points[head_edges[i].B].y,head_points[head_edges[i].B].z };
		double P3[] = { head_points[head_edges[i].C].x,head_points[head_edges[i].C].y,head_points[head_edges[i].C].z };

		Point a;
		Point b;
		a.x = P1[0] - P2[0];
		a.y = P1[1] - P2[1];
		a.z = P1[2] - P2[2];

		b.x = P1[0] - P3[0];
		b.y = P1[1] - P3[1];
		b.z = P1[2] - P3[2];
		double n[] = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
		glBindTexture(GL_TEXTURE_2D, tex);
		glColor3d(0.95, 0.95, 0.95);
		glBegin(GL_TRIANGLES);
		glNormal3d(n[0], n[1], n[2]);
		glTexCoord2f(0, 0.5);
		glVertex3f(P1[0], P1[1], P1[2]);
		glTexCoord2d(0.5, 0.5);
		glVertex3f(P2[0], P2[1], P2[2]);
		glTexCoord2d(0.5, 1);
		glVertex3f(P3[0], P3[1], P3[2]);
		glEnd();
	}

	for (int i = 1; i < body_edges_number; i++)
	{
		double P1[] = { body_points[body_edges[i].A].x,body_points[body_edges[i].A].y,body_points[body_edges[i].A].z };
		double P2[] = { body_points[body_edges[i].B].x,body_points[body_edges[i].B].y,body_points[body_edges[i].B].z };
		double P3[] = { body_points[body_edges[i].C].x,body_points[body_edges[i].C].y,body_points[body_edges[i].C].z };

		Point a;
		Point b;
		a.x = P1[0] - P2[0];
		a.y = P1[1] - P2[1];
		a.z = P1[2] - P2[2];

		b.x = P1[0] - P3[0];
		b.y = P1[1] - P3[1];
		b.z = P1[2] - P3[2];
		double n[] = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
		glBindTexture(GL_TEXTURE_2D, tex);
		glColor3d(0.95, 0.95, 0.95);
		glBegin(GL_TRIANGLES);
		glNormal3d(n[0], n[1], n[2]);
		glTexCoord2f(0, 0);
		glVertex3f(P1[0], P1[1], P1[2]);
		glTexCoord2d(0.5, 0);
		glVertex3f(P2[0], P2[1], P2[2]);
		glTexCoord2d(0.5, 0.5);
		glVertex3f(P3[0], P3[1], P3[2]);
		glEnd();
	}

	for (int i = 1; i < cloak_edges_number; i++)
	{
		double P1[] = { cloak_points[cloak_edges[i].A].x,cloak_points[cloak_edges[i].A].y,cloak_points[cloak_edges[i].A].z };
		double P2[] = { cloak_points[cloak_edges[i].B].x,cloak_points[cloak_edges[i].B].y,cloak_points[cloak_edges[i].B].z };
		double P3[] = { cloak_points[cloak_edges[i].C].x,cloak_points[cloak_edges[i].C].y,cloak_points[cloak_edges[i].C].z };

		Point a;
		Point b;
		a.x = P1[0] - P2[0];
		a.y = P1[1] - P2[1];
		a.z = P1[2] - P2[2];

		b.x = P1[0] - P3[0];
		b.y = P1[1] - P3[1];
		b.z = P1[2] - P3[2];
		double n[] = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
		glBindTexture(GL_TEXTURE_2D, tex);
		glColor3d(0.95, 0.95, 0.95);
		glBegin(GL_TRIANGLES);
		glNormal3d(n[0], n[1], n[2]);
		glTexCoord2f(0.5, 0.5);
		glVertex3f(P1[0], P1[1], P1[2]);
		glTexCoord2d(1, 0.5);
		glVertex3f(P2[0], P2[1], P2[2]);
		glTexCoord2d(1, 1);
		glVertex3f(P3[0], P3[1], P3[2]);
		glEnd();
	}

	for (int i = 1; i < nail_edges_number; i++)
	{
		double P1[] = { nail_points[nail_edges[i].A].x,nail_points[nail_edges[i].A].y,nail_points[nail_edges[i].A].z };
		double P2[] = { nail_points[nail_edges[i].B].x,nail_points[nail_edges[i].B].y,nail_points[nail_edges[i].B].z };
		double P3[] = { nail_points[nail_edges[i].C].x,nail_points[nail_edges[i].C].y,nail_points[nail_edges[i].C].z };

		Point a;
		Point b;
		a.x = P1[0] - P2[0];
		a.y = P1[1] - P2[1];
		a.z = P1[2] - P2[2];

		b.x = P1[0] - P3[0];
		b.y = P1[1] - P3[1];
		b.z = P1[2] - P3[2];
		double n[] = { a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x };
		glRotatef(swing_angle *3/4, 0, 0, 1);
		glRotatef(-swing_angle *1/6, 0, 1, 0);
		glRotatef(swing_angle *1/9, 1, 0, 0);
		
		glTranslatef(0, -5* swing_angle/360, -2 * swing_angle / 360);
		glBindTexture(GL_TEXTURE_2D, tex);
		glBegin(GL_TRIANGLES);
		glNormal3d(n[0], n[1], n[2]);
		glTexCoord2f(0.5, 0);
		glVertex3f(P1[0], P1[1], P1[2]);
		glTexCoord2d(1, 0);
		glVertex3f(P2[0], P2[1], P2[2]);
		glTexCoord2d(1, 0.5);
		glVertex3f(P3[0], P3[1], P3[2]);
		glEnd();
		glTranslatef(0, 5 * swing_angle / 360, 2 * swing_angle / 360);
		glRotatef(-swing_angle *1/9, 1, 0, 0);
		glRotatef(swing_angle *1/6, 0, 1, 0);
		glRotatef(-swing_angle*3/4, 0, 0, 1);
	}
	glRotatef(-walk_angle, 0, 0, 1);
	glTranslatef(-walk_x, -walk_y, -jump_angle - 10.0288);

	if (doLoadTerrain == true)
	{
		double Pl1[] = { -50,-50 };
		double Pl2[] = { -50,50 };
		double Pl3[] = { 50,50, };
		double Pl4[] = { 50,-50 };

		glBindTexture(GL_TEXTURE_2D, tex);
		glColor3d(0.6, 0.6, 0.6);
		glBegin(GL_QUADS);
		glNormal3d(0, 0, 1);
		glTexCoord2d(0, 0);
		glVertex2dv(Pl1);
		glTexCoord2d(1, 0);
		glVertex2dv(Pl2);
		glTexCoord2d(1, 1);
		glVertex2dv(Pl3);
		glTexCoord2d(0, 1);
		glVertex2dv(Pl4);
		glEnd();
	}
	
	//Находится ли точка в противнике
	if (doLoadEnemy == true)
	{
		Point a = { enemy_x - 5, enemy_y - 5,0};
		Point b = { enemy_x - 5, enemy_y + 5,0};
		Point c = { enemy_x + 5, enemy_y + 5,0};
		Point d = { enemy_x + 5, enemy_y - 5,0};

		double cursor_y = walk_y - 8*cos(walk_angle * PI / 180);
		double cursor_x = walk_x + 8*sin(walk_angle * PI / 180);

		Point p = { cursor_x, cursor_y,0};

		Point VectorAB = { b.x - a.x, b.y - a.y, b.z - a.z };
		Point VectorDB = { b.x - d.x, b.y - d.y, b.z - d.z };
		Point VectorBC = { c.x - b.x, c.y - b.y, c.z  -b.z };
		Point VectorCA = { a.x - c.x, a.y - c.y, a.z - c.z };
		Point VectorCD = { d.x - c.x, d.y - c.y, d.z - c.z };
		Point VectorAP = { p.x - a.x, p.y - a.y, p.z - a.z };
		Point VectorDP = { p.x - d.x, p.y - d.y, p.z - d.z };
		Point VectorBP = { p.x - b.x, p.y - b.y, p.z - b.z };
		Point VectorCP = { p.x - c.x, p.y - c.y, p.z - c.z };

		Point VectorBC_BP = { VectorBC.y * VectorBP.z - VectorBC.z * VectorBP.y, VectorBC.z * VectorBP.x - VectorBC.x * VectorBP.z, VectorBC.x * VectorBP.y - VectorBC.y * VectorBP.x };
		
		Point VectorAB_AP = { VectorAB.y * VectorAP.z - VectorAB.z * VectorAP.y, VectorAB.z * VectorAP.x - VectorAB.x * VectorAP.z, VectorAB.x * VectorAP.y - VectorAB.y * VectorAP.x };
		Point VectorCA_CP = { VectorCA.y * VectorCP.z - VectorCA.z * VectorCP.y, VectorCA.z * VectorCP.x - VectorCA.x * VectorCP.z, VectorCA.x * VectorCP.y - VectorCA.y * VectorCP.x };

		Point VectorDB_DP = { VectorDB.y * VectorDP.z - VectorDB.z * VectorDP.y, VectorDB.z * VectorDP.x - VectorDB.x * VectorDP.z, VectorDB.x * VectorDP.y - VectorAB.y * VectorDP.x };
		Point VectorCD_CP = { VectorCD.y * VectorCP.z - VectorCD.z * VectorCP.y, VectorCD.z * VectorCP.x - VectorCD.x * VectorCP.z, VectorCD.x * VectorCP.y - VectorCD.y * VectorCP.x };

		if ((VectorAB_AP.z > 0 && VectorBC_BP.z > 0 && VectorCA_CP.z > 0)||
			(VectorAB_AP.z < 0 && VectorBC_BP.z < 0 && VectorCA_CP.z < 0)||
			(VectorDB_DP.z > 0 && VectorBC_BP.z > 0 && VectorCD_CP.z > 0)||
			(VectorDB_DP.z < 0 && VectorBC_BP.z < 0 && VectorCD_CP.z < 0))
		{
			isEnemyReached = true;
		}
		else
			isEnemyReached = false;

		glTranslatef(cursor_x, cursor_y, 5);
		gluSphere(quadObj, 0.25, 10, 10);
		glTranslatef(-cursor_x, -cursor_y, -5);
		
	}

	if (doLoadEnemy == true)
	{
		double EnA[] = { enemy_x - 5, enemy_y - 5, 4 };
		double EnB[] = { enemy_x - 5, enemy_y + 5, 4 };
		double EnC[] = { enemy_x + 5, enemy_y + 5, 4 };
		double EnD[] = { enemy_x + 5, enemy_y - 5, 4 };
		double EnA1[] = { enemy_x - 5, enemy_y - 5, 15 };
		double EnB1[] = { enemy_x - 5, enemy_y + 5, 15 };
		double EnC1[] = { enemy_x + 5, enemy_y + 5, 15 };
		double EnD1[] = { enemy_x + 5, enemy_y - 5, 15 };
		glColor3d(0.6, 0.6, 0.6);
		glBegin(GL_QUADS);
		glNormal3d(0, 0, 1);
		glTexCoord2d(0, 0);
		glVertex3dv(EnA);
		glTexCoord2d(1, 0);
		glVertex3dv(EnA1);
		glTexCoord2d(1, 1);
		glVertex3dv(EnB1);
		glTexCoord2d(0, 1);
		glVertex3dv(EnB);
		glEnd();

		glBegin(GL_QUADS);
		glNormal3d(0, 0, 1);
		glTexCoord2d(0, 0);
		glVertex3dv(EnB);
		glTexCoord2d(1, 0);
		glVertex3dv(EnB1);
		glTexCoord2d(1, 1);
		glVertex3dv(EnC1);
		glTexCoord2d(0, 1);
		glVertex3dv(EnC);
		glEnd();

		glBegin(GL_QUADS);
		glNormal3d(0, 0, 1);
		glTexCoord2d(0, 0);
		glVertex3dv(EnC);
		glTexCoord2d(1, 0);
		glVertex3dv(EnC1);
		glTexCoord2d(1, 1);
		glVertex3dv(EnD1);
		glTexCoord2d(0, 1);
		glVertex3dv(EnD);
		glEnd();

		glBegin(GL_QUADS);
		glNormal3d(0, 0, 1);
		glTexCoord2d(0, 0);
		glVertex3dv(EnD);
		glTexCoord2d(1, 0);
		glVertex3dv(EnD1);
		glTexCoord2d(1, 1);
		glVertex3dv(EnA1);
		glTexCoord2d(0, 1);
		glVertex3dv(EnA);
		glEnd();

		glBegin(GL_QUADS);
		glNormal3d(0, 0, 1);
		glTexCoord2d(0, 0);
		glVertex3dv(EnA);
		glTexCoord2d(1, 0);
		glVertex3dv(EnB);
		glTexCoord2d(1, 1);
		glVertex3dv(EnC);
		glTexCoord2d(0, 1);
		glVertex3dv(EnD);
		glEnd();

		glBegin(GL_QUADS);
		glNormal3d(0, 0, 1);
		glTexCoord2d(0, 0);
		glVertex3dv(EnA1);
		glTexCoord2d(1, 0);
		glVertex3dv(EnB1);
		glTexCoord2d(1, 1);
		glVertex3dv(EnC1);
		glTexCoord2d(0, 1);
		glVertex3dv(EnD1);
		glEnd();
	}


   //Сообщение вверху экрана

	
	glMatrixMode(GL_PROJECTION);	//Делаем активной матрицу проекций. 
	                                //(всек матричные операции, будут ее видоизменять.)
	glPushMatrix();   //сохраняем текущую матрицу проецирования (которая описывает перспективную проекцию) в стек 				    
	glLoadIdentity();	  //Загружаем единичную матрицу
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //врубаем режим ортогональной проекции

	glMatrixMode(GL_MODELVIEW);		//переключаемся на модел-вью матрицу
	glPushMatrix();			  //сохраняем текущую матрицу в стек (положение камеры, фактически)
	glLoadIdentity();		  //сбрасываем ее в дефолт

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //классик моего авторства для удобной работы с рендером текста.
	rec.setSize(400, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);
	

	std::stringstream ss;
	ss << "Убито: " <<kills<< std::endl;
	//ss << "T - вкл/выкл текстур" << std::endl;
	//ss << "L - вкл/выкл освещение" << std::endl;
	//ss << "F - Свет из камеры" << std::endl;
	//ss << "G - двигать свет по горизонтали" << std::endl;
	//ss << "G+ЛКМ двигать свет по вертекали" << std::endl;
	//ss << "Коорд. света: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	//ss << "Коорд. камеры: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	//ss << "Параметры камеры: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //восстанавливаем матрицы проекции и модел-вью обратьно из стека.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}