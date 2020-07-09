#include "Render.h"

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#pragma comment(lib,"Winmm.lib")

bool textureMode = true;
bool lightMode = true;

void ayaya(double x1[], double y1[], double z1[], double vecn[])      
	{
		/*double y[] = { x1[0] - y1[0], x1[1] - y1[1], x1[2] - y1[2] };
		double x[] = { z1[0] - y1[0], z1[1] - y1[1], z1[2] - y1[2] };
		
		vecn[0] = y[1] * x[2] - y[1] * x[2];
		vecn[1] = -x[0] * y[2] + y[0] * x[2];
		vecn[2] = x[0] * y[1] - y[0] * x[2];*/
		double qx, qy, qz, px, py, pz;

	qx = x1[0] - y1[0];
	qy = x1[1] - y1[1];
	qz = x1[2] - y1[2];
	px = z1[0] - y1[0];
	py = z1[1] - y1[1];
	pz = z1[2] - y1[2];
	vecn[0] = py*qz - pz*qy;
	vecn[1] = pz*qx - px*qz;
	vecn[2] = px*qy - py*qx;

		double length = sqrt(pow(vecn[0], 2) + pow(vecn[1], 2) + pow(vecn[2], 2));

		vecn[0] /= length;
		vecn[1] /= length;
		vecn[2] /= length;
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
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

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
static bool k = false;
static bool gvn = false;
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
	 if (key == 'J') {
  gvn = !gvn;
  k = !k;
 }
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}




//выполняется перед первым рендером
void initRender(OpenGL *ogl)
{
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
	OpenGL::LoadBMP("KOK.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	GLuint texId;
	//генерируем ИД для текстуры
	glGenTextures(1, &texId);
	//биндим айдишник, все что будет происходить с текстурой, будте происходить по этому ИД
	glBindTexture(GL_TEXTURE_2D, texId);

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
}





void Render(OpenGL *ogl)
{       	
	
	
	
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
	GLfloat amb[] = { 0.0, 0.0, 0.0, 1. };
	GLfloat dif[] = { 0.0, 0.0, 0.0, 1. };
	GLfloat spec[] = { 1.0, 1.0, 1, 1. };
	GLfloat sh = 128;

	GLfloat ambFloor[] = { 0.5, 0.5, 0.0, 1. };
	GLfloat difFloor[] = { 0.5, 0.5, 0.0, 1. };
	GLfloat specFloor[] = { 0.5, 0.5, 0, 1. };
	GLfloat emissFloor[] = {0.5, 0.5, 0, 1};
	GLfloat shFloor = 20;
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.04f);
glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.009f);

	////фоновая
	//glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	////дифузная
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	////зеркальная
	//glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
	////размер блика
	//glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//glEnable(GL_COLOR_MATERIAL);
//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambFloor);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, difFloor);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, specFloor);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, shFloor);
	glMaterialfv(GL_FRONT, GL_EMISSION, emissFloor);

	glEnable(GL_COLOR_MATERIAL);
    //чтоб было красиво, без квадратиков (сглаживание освещения)
	glShadeModel(GL_SMOOTH);
	//===================================
	//Прогать тут  ФИГУРКА!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	
//	float a, a1;
	if(!gvn)
	  PlaySound( "1.wav", NULL, SND_FILENAME | SND_ASYNC);
	static double da = 0, db = 0, dw = 0;
	static bool hj = false, hi = false, hr = false;
	double vecn[3],
		   TAD[] = {(double)0/512,(double)512/512}, TBD[] = {(double)0/512,(double)0/512}, TCD[] = {(double)512/512,(double)0/512},
		   TAF[] = {(double)0/512,(double)512/512}, TBF[] = {(double)512/512,(double)512/512}, TCF[] = {(double)512/512,(double)0/512},
		   BA[]={6,0,0}, BB[]={8,6,0}, BC[]={4,6,0}, BD[]={6,9,5};
		 //  HA[]={7,9,5}, HB[]={5,9,5}, HC[]={6,10,6}, HD[]={6,12,4},
		 //  RLA[]={8,6,0}, RLB[]={6.5,6,0}, RLC[]={7,7.5,2.5}, RLD[]={7,10,0},
		 //  LLA[]={5.5,6,0}, LLB[]={4,6,0}, LLC[]={5,7.5,2.5}, LLD[]={5,10,0},
		 //  RWA[]={6,5.5,3}, RWB[]={8.5,8,4.5}, RWC[]={11,5.5,3}, RWD[]={8.5,6.5,4}, RWE[]={8.5,6.5,3},
		 //  LWA[]={6,5.5,3}, LWB[]={3.5,8,4.5}, LWC[]={1,5.5,3}, LWD[]={3.5,6.5,4}, LWE[]={3.5,6.5,3};
	glColor4f(1.0f,1.0f,0.0f,1.0f);
	glBegin(GL_TRIANGLES);
	glNormal3f(0,0,1);
	glTexCoord2dv(TAF);
glVertex3f(50, -50, 0);
glTexCoord2dv(TBF);
glVertex3f(-50, -50, 0);
glTexCoord2dv(TCF);
glVertex3f(-50, 50, 0);
glEnd();

glBegin(GL_TRIANGLES);
	glNormal3f(0,0,1);
	glTexCoord2dv(TAF);
	glVertex3f(50, -50, 0);
	glTexCoord2dv(TBF);
glVertex3f(50, 50, 0);
glTexCoord2dv(TCF);
glVertex3f(-50, 50, 0);
		glEnd();

		GLfloat amb2[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif2[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec2[] = { 0.0, 0.0, 1, 1. };
	GLfloat sh2 = 0.1f * 256;
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.04f);
glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.009f);

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb2);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif2);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec2);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh2);

	glEnable(GL_COLOR_MATERIAL);
	glColor3f(1.0f,0.0f,0.5f);
	//TELO 
 glBegin(GL_TRIANGLES);
 ayaya(BC, BD, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BB);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
 ayaya(BA, BC, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BB);	
	ayaya(BB, BD, BA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BB);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
	ayaya(BC, BA, BD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);	
 glEnd();
	
	//GOLOVA
	double HA[] = { 2,0,0 }, HB[] = { 0,0,0 }, HC[] = { 1,1,1 }, HD[] = { 1,3,-1 };
 
 
 glPushMatrix();
 glTranslated(5, 9, 5);
 
 if (gvn) {
	
  if(!hj)
   da += 20;
  if (da >= 100||hj) {
   hj = true;
   da -= 20;
   if (da <= 0)
    hj = false;
  }
   
  glRotated(da, 1, 0, 0);
 }
 glBegin(GL_TRIANGLES);
 ayaya(HC, HB, HA, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HD, HB, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HB);
 glTexCoord2dv(TBD);
 glVertex3dv(HD);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HA, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HC);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 ayaya(HA, HB, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 glEnd();
 glPopMatrix();

 double RLA[]={1.5,0,0}, RLB[]={0,0,0}, RLC[]={0.5,1.5,2.5}, RLD[]={0.5,4,0};
 glPushMatrix();
 
 glTranslated(6.5, 6, 0);
 if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 } 
	glBegin(GL_TRIANGLES);
	ayaya(RLC, RLB, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLC, RLD, RLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLB);
	glTexCoord2dv(TBD);
	glVertex3dv(RLD);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLD, RLC, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLC);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	ayaya(RLA, RLB, RLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	glEnd();
	glPopMatrix();
	//LEVY_NOGA
	double LLA[]={1.5,0,0}, LLB[]={0,0,0}, LLC[]={1,1.5,2.5}, LLD[]={1,4,0};
	glPushMatrix();
	
	glTranslated(4, 6, 0);
  if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LLC, LLB, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLC, LLD, LLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLB);
	glTexCoord2dv(TBD);
	glVertex3dv(LLD);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLD, LLC, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLC);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	ayaya(LLA, LLB, LLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	glEnd();
	glPopMatrix();
	//PRAVY_KRYLO
	double   RWA[]={0,0,0}, RWB[]={2.5,3,1.5}, RWC[]={5,0,0}, RWD[]={2.5,1.5,1}, RWE[]={2.5,1.5,0};
	glPushMatrix();
 
 glTranslated(6, 5.5, 3);
 if (gvn) 
 {
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }   
  glRotated(dw, 0, -1, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(RWD, RWB, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWB, RWD, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWA, RWB, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWB, RWC, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWE, RWD, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWD);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWD, RWE, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWC);
	glTexCoord2dv(TBD);
	glVertex3dv(RWE);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	glEnd();
	glPopMatrix();
	//LEVY_KRYLO
	 double   LWA[]={5,0,0}, LWB[]={2.5,3,1.5}, LWC[]={0,0,0}, LWD[]={2.5,1.5,1}, LWE[]={2.5,1.5,0};
	glPushMatrix();

 glTranslated(1, 5.5, 3);
 if (gvn) 
{
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }
  glTranslated(5, 0, 0);
  glRotated(dw, 0, 1, 0);
  glTranslated(-5, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LWA, LWB, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWB, LWC, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWE, LWB, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWE, LWC, LWB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWD, LWE, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWD);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWC, LWE, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWC);
	glTexCoord2dv(TBD);
	glVertex3dv(LWE);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	
	glEnd();

	glPopMatrix();
	//ОГОНОЬ ДА
	glPushMatrix();
	glLoadIdentity();
	GLfloat amb1[] = { 1.0, 1.0, 1.0, 1. };
	GLfloat dif1[] = { 1.0, 1.0, 1.0, 1. };
	GLfloat spec1[] = { 1, 1.0, 0, 1. };
	GLfloat sh1 = 128;
	GLfloat position1[] = {5, 17, 6};
	
	 glMaterialfv(GL_FRONT, GL_POSITION, position1);
  // характеристики излучаемого света
  // фоновое освещение (рассеянный свет)
  glMaterialfv(GL_FRONT, GL_AMBIENT, amb1);
  // диффузная составляющая света
  glMaterialfv(GL_FRONT, GL_DIFFUSE, dif1);
  // зеркально отражаемая составляющая света
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec1);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
  glColor3f(1.0f, 0.5f, 0.0f);  
  
  glPopMatrix();
	double Q1[]={3, 0, 0}, Q2[]={1,3,2}, Q3[]={0,8,0},
		   _Q1[]={3, 0, 0}, _Q2[]={1,2,3}, _Q3[]={2.5,10,2},
		   Q1_[]={3, 0, 0}, Q3_[]={7,7,-1}, Q2_[]={5,5,2};
	static bool f = false;
	static double eh = 0;
	
	if(k)
	{
		glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3, Q2, Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(_Q3, _Q2, _Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(_Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(_Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(_Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3_, Q2_, Q1_, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1_);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2_);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3_);
	glEnd();
	glPopMatrix();
	}






	////////////////////////////////////////////////////////////////////////////
	glPushMatrix();
 glTranslated(8, 12, 5);
 glRotated(60, 1, 0, 1);
	//glPushMatrix();


	GLfloat amb3[] = { 0.2, 0.2, 1.1, 1. };
	GLfloat dif3[] = { 1.4, 0.65, 0.5, 1. };
	GLfloat spec3[] = { 0.5, 1.0, 1, 1. };
	GLfloat sh3 = 0.1f * 256;
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.04f);
glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.009f);

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb3);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif3);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec3);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh3);

	glEnable(GL_COLOR_MATERIAL);
	//glPopMatrix();
	glColor3f(0.1f,1.0f,0.5f);
	//TELO 
 glBegin(GL_TRIANGLES);
 ayaya(BC, BD, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BB);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
 ayaya(BA, BC, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BB);	
	ayaya(BB, BD, BA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BB);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
	ayaya(BC, BA, BD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);	
 glEnd();
	
	//GOLOVA
//	double HA[] = { 2,0,0 }, HB[] = { 0,0,0 }, HC[] = { 1,1,1 }, HD[] = { 1,3,-1 };
 
 
 glPushMatrix();
 glTranslated(5, 9, 5);
 
 if (gvn) 
 {	
  if(!hj)
   da += 20;
  if (da >= 100||hj) 
  {
   hj = true;
   da -= 20;
   if (da <= 0)
    hj = false;
  }
   
  glRotated(da, 1, 0, 0);
 }
 glBegin(GL_TRIANGLES);
 ayaya(HC, HB, HA, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HD, HB, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HB);
 glTexCoord2dv(TBD);
 glVertex3dv(HD);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HA, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HC);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 ayaya(HA, HB, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 glEnd();
 glPopMatrix();

 glPushMatrix();
 //double RLA[]={1.5,0,0}, RLB[]={0,0,0}, RLC[]={0.5,1.5,2.5}, RLD[]={0.5,4,0};
 glTranslated(6.5, 6, 0);
 if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 } 
	glBegin(GL_TRIANGLES);
	ayaya(RLC, RLB, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLC, RLD, RLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLB);
	glTexCoord2dv(TBD);
	glVertex3dv(RLD);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLD, RLC, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLC);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	ayaya(RLA, RLB, RLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	glEnd();
	glPopMatrix();
	//LEVY_NOGA
	glPushMatrix();
	//double LLA[]={1.5,0,0}, LLB[]={0,0,0}, LLC[]={1,1.5,2.5}, LLD[]={1,4,0};
	glTranslated(4, 6, 0);
  if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LLC, LLB, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLC, LLD, LLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLB);
	glTexCoord2dv(TBD);
	glVertex3dv(LLD);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLD, LLC, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLC);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	ayaya(LLA, LLB, LLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	glEnd();
	glPopMatrix();
	//PRAVY_KRYLO
	glPushMatrix();
 //double   RWA[]={0,0,0}, RWB[]={2.5,3,1.5}, RWC[]={5,0,0}, RWD[]={2.5,1.5,1}, RWE[]={2.5,1.5,0};
 glTranslated(6, 5.5, 3);
 if (gvn) 
 {
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }   
  glRotated(dw, 0, -1, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(RWD, RWB, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWB, RWD, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWA, RWB, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWB, RWC, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWE, RWD, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWD);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWD, RWE, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWC);
	glTexCoord2dv(TBD);
	glVertex3dv(RWE);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	glEnd();
	glPopMatrix();
	//LEVY_KRYLO
	glPushMatrix();
 //double   LWA[]={5,0,0}, LWB[]={2.5,3,1.5}, LWC[]={0,0,0}, LWD[]={2.5,1.5,1}, LWE[]={2.5,1.5,0};
 glTranslated(1, 5.5, 3);
 if (gvn) 
{
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }
  glTranslated(5, 0, 0);
  glRotated(dw, 0, 1, 0);
  glTranslated(-5, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LWA, LWB, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWB, LWC, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWE, LWB, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWE, LWC, LWB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWD, LWE, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWD);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWC, LWE, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWC);
	glTexCoord2dv(TBD);
	glVertex3dv(LWE);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	
	glEnd();

	glPopMatrix();
	//ОГОНОЬ ДА
	glPushMatrix();
	glLoadIdentity();
	GLfloat amb4[] = { 1.0, 1.0, 0.0, 1. };
	GLfloat dif4[] = { 1.0, 1.0, 0.0, 1. };
	GLfloat spec4[] = { 1, 1.0, 0, 1. };
	GLfloat sh4 = 128;
	GLfloat position4[] = {5, 17, 6};
	
	 glMaterialfv(GL_FRONT, GL_POSITION, position4);
  // характеристики излучаемого света
  // фоновое освещение (рассеянный свет)
  glMaterialfv(GL_FRONT, GL_AMBIENT, amb4);
  // диффузная составляющая света
  glMaterialfv(GL_FRONT, GL_DIFFUSE, dif4);
  // зеркально отражаемая составляющая света
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec4);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
  glColor3f(1.0f, 0.5f, 0.0f);  
  
  glPopMatrix();
	/*double Q1[]={3, 0, 0}, Q2[]={1,3,2}, Q3[]={0,8,0},
		   _Q1[]={3, 0, 0}, _Q2[]={1,2,3}, _Q3[]={2.5,10,2},
		   Q1_[]={3, 0, 0}, Q3_[]={7,7,-1}, Q2_[]={5,5,2};
	static bool f = false;
	static double eh = 0;*/
	
	if(k)
	{
		glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3, Q2, Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(_Q3, _Q2, _Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(_Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(_Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(_Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3_, Q2_, Q1_, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1_);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2_);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3_);
	glEnd();
	glPopMatrix();
	
	}
	glPopMatrix();
	
	////////////////////////////////////////////////////////////////////////
	glPushMatrix();
 glTranslated(-5, -7, 3);
 glRotated(125, 0, 1, 1);
	//glPushMatrix();


	GLfloat amb5[] = { 0.3, 0.2, 0.1, 1. };
	GLfloat dif5[] = { 0.2, 0.65, 0.5, 1. };
	GLfloat spec5[] = { 0.5, 0.5, 1, 1. };
	GLfloat sh5 = 0.2f * 256;
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.04f);
glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.009f);

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb5);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif5);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec5);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh5);

	glEnable(GL_COLOR_MATERIAL);
	//glPopMatrix();
	glColor3f(1.1f,1.0f,0.1f);
	//TELO 
 glBegin(GL_TRIANGLES);
 ayaya(BC, BD, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BB);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
 ayaya(BA, BC, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BB);	
	ayaya(BB, BD, BA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BB);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
	ayaya(BC, BA, BD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);	
 glEnd();
	
	//GOLOVA
//	double HA[] = { 2,0,0 }, HB[] = { 0,0,0 }, HC[] = { 1,1,1 }, HD[] = { 1,3,-1 };
 
 
 glPushMatrix();
 glTranslated(5, 9, 5);
 
 if (gvn) 
 {	
  if(!hj)
   da += 20;
  if (da >= 100||hj) 
  {
   hj = true;
   da -= 20;
   if (da <= 0)
    hj = false;
  }
   
  glRotated(da, 1, 0, 0);
 }
 glBegin(GL_TRIANGLES);
 ayaya(HC, HB, HA, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HD, HB, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HB);
 glTexCoord2dv(TBD);
 glVertex3dv(HD);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HA, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HC);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 ayaya(HA, HB, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 glEnd();
 glPopMatrix();

 glPushMatrix();
 //double RLA[]={1.5,0,0}, RLB[]={0,0,0}, RLC[]={0.5,1.5,2.5}, RLD[]={0.5,4,0};
 glTranslated(6.5, 6, 0);
 if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 } 
	glBegin(GL_TRIANGLES);
	ayaya(RLC, RLB, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLC, RLD, RLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLB);
	glTexCoord2dv(TBD);
	glVertex3dv(RLD);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLD, RLC, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLC);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	ayaya(RLA, RLB, RLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	glEnd();
	glPopMatrix();
	//LEVY_NOGA
	glPushMatrix();
	//double LLA[]={1.5,0,0}, LLB[]={0,0,0}, LLC[]={1,1.5,2.5}, LLD[]={1,4,0};
	glTranslated(4, 6, 0);
  if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LLC, LLB, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLC, LLD, LLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLB);
	glTexCoord2dv(TBD);
	glVertex3dv(LLD);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLD, LLC, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLC);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	ayaya(LLA, LLB, LLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	glEnd();
	glPopMatrix();
	//PRAVY_KRYLO
	glPushMatrix();
 //double   RWA[]={0,0,0}, RWB[]={2.5,3,1.5}, RWC[]={5,0,0}, RWD[]={2.5,1.5,1}, RWE[]={2.5,1.5,0};
 glTranslated(6, 5.5, 3);
 if (gvn) 
 {
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }   
  glRotated(dw, 0, -1, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(RWD, RWB, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWB, RWD, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWA, RWB, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWB, RWC, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWE, RWD, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWD);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWD, RWE, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWC);
	glTexCoord2dv(TBD);
	glVertex3dv(RWE);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	glEnd();
	glPopMatrix();
	//LEVY_KRYLO
	glPushMatrix();
 //double   LWA[]={5,0,0}, LWB[]={2.5,3,1.5}, LWC[]={0,0,0}, LWD[]={2.5,1.5,1}, LWE[]={2.5,1.5,0};
 glTranslated(1, 5.5, 3);
 if (gvn) 
{
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }
  glTranslated(5, 0, 0);
  glRotated(dw, 0, 1, 0);
  glTranslated(-5, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LWA, LWB, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWB, LWC, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWE, LWB, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWE, LWC, LWB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWD, LWE, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWD);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWC, LWE, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWC);
	glTexCoord2dv(TBD);
	glVertex3dv(LWE);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	
	glEnd();

	glPopMatrix();
	//ОГОНОЬ ДА
	glPushMatrix();
	glLoadIdentity();
	GLfloat amb6[] = { 1.0, 1.0, 0.0, 1. };
	GLfloat dif6[] = { 1.0, 1.0, 0.0, 1. };
	GLfloat spec6[] = { 1, 1.0, 0, 1. };
	GLfloat sh6 = 128;
	GLfloat position6[] = {5, 17, 6};
	
	 glMaterialfv(GL_FRONT, GL_POSITION, position6);
  // характеристики излучаемого света
  // фоновое освещение (рассеянный свет)
  glMaterialfv(GL_FRONT, GL_AMBIENT, amb6);
  // диффузная составляющая света
  glMaterialfv(GL_FRONT, GL_DIFFUSE, dif6);
  // зеркально отражаемая составляющая света
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec6);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
  glColor3f(1.0f, 0.5f, 0.0f);  
  
  glPopMatrix();
	/*double Q1[]={3, 0, 0}, Q2[]={1,3,2}, Q3[]={0,8,0},
		   _Q1[]={3, 0, 0}, _Q2[]={1,2,3}, _Q3[]={2.5,10,2},
		   Q1_[]={3, 0, 0}, Q3_[]={7,7,-1}, Q2_[]={5,5,2};
	static bool f = false;
	static double eh = 0;*/
	
	if(k)
	{
		glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3, Q2, Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(_Q3, _Q2, _Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(_Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(_Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(_Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3_, Q2_, Q1_, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1_);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2_);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3_);
	glEnd();
	glPopMatrix();
	
	}
	glPopMatrix();



	////////////////////////////////////////////////////////////////////////////////////////






	glPushMatrix();
 glTranslated(15, 9, 6);
 glRotated(-40, 1, 1, 1);
	//glPushMatrix();


	GLfloat amb7[] = { 0.3, 0.2, 0.1, 1. };
	GLfloat dif7[] = { 0.2, 0.65, 0.5, 1. };
	GLfloat spec7[] = { 0.5, 0.5, 1, 1. };
	GLfloat sh7 = 0.2f * 256;
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.04f);
glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.009f);

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb7);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif7);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec7);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh7);

	glEnable(GL_COLOR_MATERIAL);
	//glPopMatrix();
	glColor3f(0.1f,1.0f,1.5f);
	//TELO 
 glBegin(GL_TRIANGLES);
 ayaya(BC, BD, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BB);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
 ayaya(BA, BC, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BB);	
	ayaya(BB, BD, BA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BB);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
	ayaya(BC, BA, BD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);	
 glEnd();
	
	//GOLOVA
//	double HA[] = { 2,0,0 }, HB[] = { 0,0,0 }, HC[] = { 1,1,1 }, HD[] = { 1,3,-1 };
 
 
 glPushMatrix();
 glTranslated(5, 9, 5);
 
 if (gvn) 
 {	
  if(!hj)
   da += 20;
  if (da >= 100||hj) 
  {
   hj = true;
   da -= 20;
   if (da <= 0)
    hj = false;
  }
   
  glRotated(da, 1, 0, 0);
 }
 glBegin(GL_TRIANGLES);
 ayaya(HC, HB, HA, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HD, HB, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HB);
 glTexCoord2dv(TBD);
 glVertex3dv(HD);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HA, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HC);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 ayaya(HA, HB, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 glEnd();
 glPopMatrix();

 glPushMatrix();
 //double RLA[]={1.5,0,0}, RLB[]={0,0,0}, RLC[]={0.5,1.5,2.5}, RLD[]={0.5,4,0};
 glTranslated(6.5, 6, 0);
 if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 } 
	glBegin(GL_TRIANGLES);
	ayaya(RLC, RLB, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLC, RLD, RLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLB);
	glTexCoord2dv(TBD);
	glVertex3dv(RLD);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLD, RLC, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLC);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	ayaya(RLA, RLB, RLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	glEnd();
	glPopMatrix();
	//LEVY_NOGA
	glPushMatrix();
	//double LLA[]={1.5,0,0}, LLB[]={0,0,0}, LLC[]={1,1.5,2.5}, LLD[]={1,4,0};
	glTranslated(4, 6, 0);
  if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LLC, LLB, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLC, LLD, LLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLB);
	glTexCoord2dv(TBD);
	glVertex3dv(LLD);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLD, LLC, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLC);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	ayaya(LLA, LLB, LLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	glEnd();
	glPopMatrix();
	//PRAVY_KRYLO
	glPushMatrix();
 //double   RWA[]={0,0,0}, RWB[]={2.5,3,1.5}, RWC[]={5,0,0}, RWD[]={2.5,1.5,1}, RWE[]={2.5,1.5,0};
 glTranslated(6, 5.5, 3);
 if (gvn) 
 {
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }   
  glRotated(dw, 0, -1, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(RWD, RWB, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWB, RWD, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWA, RWB, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWB, RWC, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWE, RWD, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWD);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWD, RWE, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWC);
	glTexCoord2dv(TBD);
	glVertex3dv(RWE);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	glEnd();
	glPopMatrix();
	//LEVY_KRYLO
	glPushMatrix();
 //double   LWA[]={5,0,0}, LWB[]={2.5,3,1.5}, LWC[]={0,0,0}, LWD[]={2.5,1.5,1}, LWE[]={2.5,1.5,0};
 glTranslated(1, 5.5, 3);
 if (gvn) 
{
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }
  glTranslated(5, 0, 0);
  glRotated(dw, 0, 1, 0);
  glTranslated(-5, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LWA, LWB, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWB, LWC, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWE, LWB, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWE, LWC, LWB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWD, LWE, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWD);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWC, LWE, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWC);
	glTexCoord2dv(TBD);
	glVertex3dv(LWE);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	
	glEnd();

	glPopMatrix();
	//ОГОНОЬ ДА
	glPushMatrix();
	glLoadIdentity();
	GLfloat amb8[] = { 1.0, 1.0, 0.0, 1. };
	GLfloat dif8[] = { 1.0, 1.0, 0.0, 1. };
	GLfloat spec8[] = { 1, 1.0, 0, 1. };
	GLfloat sh8 = 128;
	GLfloat position8[] = {5, 17, 6};
	
	 glMaterialfv(GL_FRONT, GL_POSITION, position8);
  // характеристики излучаемого света
  // фоновое освещение (рассеянный свет)
  glMaterialfv(GL_FRONT, GL_AMBIENT, amb8);
  // диффузная составляющая света
  glMaterialfv(GL_FRONT, GL_DIFFUSE, dif8);
  // зеркально отражаемая составляющая света
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec8);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
  glColor3f(1.0f, 0.5f, 0.0f);  
  
  glPopMatrix();
	/*double Q1[]={3, 0, 0}, Q2[]={1,3,2}, Q3[]={0,8,0},
		   _Q1[]={3, 0, 0}, _Q2[]={1,2,3}, _Q3[]={2.5,10,2},
		   Q1_[]={3, 0, 0}, Q3_[]={7,7,-1}, Q2_[]={5,5,2};
	static bool f = false;
	static double eh = 0;*/
	
	if(k)
	{
		glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3, Q2, Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(_Q3, _Q2, _Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(_Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(_Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(_Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3_, Q2_, Q1_, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1_);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2_);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3_);
	glEnd();
	glPopMatrix();
	
	}
	glPopMatrix();




	/////////////////////////////////////////////////////////////////////////////////




	glPushMatrix();
 glTranslated(-7, -10, 0);
 glRotated(189, 0, 0, 1);
	//glPushMatrix();


	GLfloat amb9[] = { 0.3, 0.2, 0.1, 1. };
	GLfloat dif9[] = { 0.2, 0.65, 0.5, 1. };
	GLfloat spec9[] = { 0.5, 0.5, 1, 1. };
	GLfloat sh9 = 0.2f * 256;
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.0f);
glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.04f);
glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.009f);

	//фоновая
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb9);
	//дифузная
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif9);
	//зеркальная
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec9);
	//размер блика
	glMaterialf(GL_FRONT, GL_SHININESS, sh9);

	glEnable(GL_COLOR_MATERIAL);
	//glPopMatrix();
	glColor3f(1.5f,0.0f,0.7f);
	//TELO 
 glBegin(GL_TRIANGLES);
 ayaya(BC, BD, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BB);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
 ayaya(BA, BC, BB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BB);	
	ayaya(BB, BD, BA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BB);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);
	ayaya(BC, BA, BD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(BA);
	glTexCoord2dv(TBD);
	glVertex3dv(BC);
	glTexCoord2dv(TCD);
	glVertex3dv(BD);	
 glEnd();
	
	//GOLOVA
//	double HA[] = { 2,0,0 }, HB[] = { 0,0,0 }, HC[] = { 1,1,1 }, HD[] = { 1,3,-1 };
 
 
 glPushMatrix();
 glTranslated(5, 9, 5);
 
 if (gvn) 
 {	
  if(!hj)
   da += 20;
  if (da >= 100||hj) 
  {
   hj = true;
   da -= 20;
   if (da <= 0)
    hj = false;
  }
   
  glRotated(da, 1, 0, 0);
 }
 glBegin(GL_TRIANGLES);
 ayaya(HC, HB, HA, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HD, HB, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HB);
 glTexCoord2dv(TBD);
 glVertex3dv(HD);
 glTexCoord2dv(TCD);
 glVertex3dv(HC);
 ayaya(HC, HA, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HC);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 ayaya(HA, HB, HD, vecn);
 glNormal3dv(vecn);
 glTexCoord2dv(TAD);
 glVertex3dv(HA);
 glTexCoord2dv(TBD);
 glVertex3dv(HB);
 glTexCoord2dv(TCD);
 glVertex3dv(HD);
 glEnd();
 glPopMatrix();

 glPushMatrix();
 //double RLA[]={1.5,0,0}, RLB[]={0,0,0}, RLC[]={0.5,1.5,2.5}, RLD[]={0.5,4,0};
 glTranslated(6.5, 6, 0);
 if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 } 
	glBegin(GL_TRIANGLES);
	ayaya(RLC, RLB, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLC, RLD, RLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLB);
	glTexCoord2dv(TBD);
	glVertex3dv(RLD);
	glTexCoord2dv(TCD);
	glVertex3dv(RLC);
	ayaya(RLD, RLC, RLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLC);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	ayaya(RLA, RLB, RLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RLA);
	glTexCoord2dv(TBD);
	glVertex3dv(RLB);
	glTexCoord2dv(TCD);
	glVertex3dv(RLD);
	glEnd();
	glPopMatrix();
	//LEVY_NOGA
	glPushMatrix();
	//double LLA[]={1.5,0,0}, LLB[]={0,0,0}, LLC[]={1,1.5,2.5}, LLD[]={1,4,0};
	glTranslated(4, 6, 0);
  if (gvn) 
 {
  if(!hi)
   db += 5;
  if (db >= 20||hi) 
  {
   hi = true;
   db -= 5;
   if (db <= 0)
    hi = false;
  }   
  glRotated(db, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LLC, LLB, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLC, LLD, LLB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLB);
	glTexCoord2dv(TBD);
	glVertex3dv(LLD);
	glTexCoord2dv(TCD);
	glVertex3dv(LLC);
	ayaya(LLD, LLC, LLA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLC);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	ayaya(LLA, LLB, LLD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LLA);
	glTexCoord2dv(TBD);
	glVertex3dv(LLB);
	glTexCoord2dv(TCD);
	glVertex3dv(LLD);
	glEnd();
	glPopMatrix();
	//PRAVY_KRYLO
	glPushMatrix();
 //double   RWA[]={0,0,0}, RWB[]={2.5,3,1.5}, RWC[]={5,0,0}, RWD[]={2.5,1.5,1}, RWE[]={2.5,1.5,0};
 glTranslated(6, 5.5, 3);
 if (gvn) 
 {
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }   
  glRotated(dw, 0, -1, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(RWD, RWB, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWB, RWD, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	ayaya(RWA, RWB, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWB);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWB, RWC, RWE, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWB);
	glTexCoord2dv(TBD);
	glVertex3dv(RWC);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWE, RWD, RWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWA);
	glTexCoord2dv(TBD);
	glVertex3dv(RWD);
	glTexCoord2dv(TCD);
	glVertex3dv(RWE);
	ayaya(RWD, RWE, RWC, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(RWC);
	glTexCoord2dv(TBD);
	glVertex3dv(RWE);
	glTexCoord2dv(TCD);
	glVertex3dv(RWD);
	glEnd();
	glPopMatrix();
	//LEVY_KRYLO
	glPushMatrix();
 //double   LWA[]={5,0,0}, LWB[]={2.5,3,1.5}, LWC[]={0,0,0}, LWD[]={2.5,1.5,1}, LWE[]={2.5,1.5,0};
 glTranslated(1, 5.5, 3);
 if (gvn) 
{
  if(!hr)
   dw += 10;
  if (dw >= 30||hr) 
  {
   hr = true;
   dw -= 10;
   if (dw <= -30)
    hr = false;
  }
  glTranslated(5, 0, 0);
  glRotated(dw, 0, 1, 0);
  glTranslated(-5, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	ayaya(LWA, LWB, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWB, LWC, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	ayaya(LWE, LWB, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWB);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWE, LWC, LWB, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWB);
	glTexCoord2dv(TBD);
	glVertex3dv(LWC);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWD, LWE, LWA, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWA);
	glTexCoord2dv(TBD);
	glVertex3dv(LWD);
	glTexCoord2dv(TCD);
	glVertex3dv(LWE);
	ayaya(LWC, LWE, LWD, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAD);
	glVertex3dv(LWC);
	glTexCoord2dv(TBD);
	glVertex3dv(LWE);
	glTexCoord2dv(TCD);
	glVertex3dv(LWD);
	
	glEnd();

	glPopMatrix();
	//ОГОНОЬ ДА
	glPushMatrix();
	glLoadIdentity();
	GLfloat amb10[] = { 1.0, 1.0, 0.0, 1. };
	GLfloat dif10[] = { 1.0, 1.0, 0.0, 1. };
	GLfloat spec10[] = { 1, 1.0, 0, 1. };
	GLfloat sh10 = 128;
	GLfloat position10[] = {5, 17, 6};
	
	 glMaterialfv(GL_FRONT, GL_POSITION, position6);
  // характеристики излучаемого света
  // фоновое освещение (рассеянный свет)
  glMaterialfv(GL_FRONT, GL_AMBIENT, amb10);
  // диффузная составляющая света
  glMaterialfv(GL_FRONT, GL_DIFFUSE, dif10);
  // зеркально отражаемая составляющая света
  glMaterialfv(GL_FRONT, GL_SPECULAR, spec10);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.0);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.0);
  glColor3f(1.0f, 0.5f, 0.0f);  
  
  glPopMatrix();
	/*double Q1[]={3, 0, 0}, Q2[]={1,3,2}, Q3[]={0,8,0},
		   _Q1[]={3, 0, 0}, _Q2[]={1,2,3}, _Q3[]={2.5,10,2},
		   Q1_[]={3, 0, 0}, Q3_[]={7,7,-1}, Q2_[]={5,5,2};
	static bool f = false;
	static double eh = 0;*/
	
	if(k)
	{
		glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3, Q2, Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(_Q3, _Q2, _Q1, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(_Q1);
	glTexCoord2dv(TBF);
	glVertex3dv(_Q2);
	glTexCoord2dv(TCF);
	glVertex3dv(_Q3);
	glEnd();
	glPopMatrix();
	glPushMatrix();
 glTranslated(3, 12, 5);
 if (gvn) 
 {
  if(!f)
   eh += 10;
  if (eh >= 30||f) 
  {
   f = true;
   eh -= 30;
   if (eh <= -10)
    f = false;
  }
  glRotated(eh, 1, 0, 0);
 }
	glBegin(GL_TRIANGLES);
	
	ayaya(Q3_, Q2_, Q1_, vecn);
	glNormal3dv(vecn);
	glTexCoord2dv(TAF);
	glVertex3dv(Q1_);
	glTexCoord2dv(TBF);
	glVertex3dv(Q2_);
	glTexCoord2dv(TCF);
	glVertex3dv(Q3_);
	glEnd();
	glPopMatrix();
	
	}
	glPopMatrix();
//ФИГУРКА СТОП!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/*
	//Начало рисования квадратика станкина
	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	
	
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();
	//конец рисования квадратика станкина
    */
	
	//текст сообщения вверху слева, если надоест - закоментировать, или заменить =)
	char c[250];  //максимальная длина сообщения
	sprintf_s(c, "J, чтобы пошалить!");
	//	"R - установить камеру и свет в начальное положение\n"
	//	"F - переместить свет в точку камеры", textureMode, lightMode);
	ogl->message = std::string(c);




  } //конец тела функции

