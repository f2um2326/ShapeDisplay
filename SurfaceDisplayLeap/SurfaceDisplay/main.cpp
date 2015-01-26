#include <Windows.h>	// for Serial Connection
HANDLE arduino;
bool Ret;
BYTE data[4];
BYTE data_before[16][3] =	{{0, 9, 9}, {0, 9, 9}, {0, 9, 9}, {0, 9, 9}
							, {0, 9, 9}, {0, 9, 9}, {0, 9, 9}, {0, 9, 9}
							, {0, 9, 9}, {0, 9, 9}, {0, 9, 9}, {0, 9, 9}
							, {0, 9, 9}, {0, 9, 9}, {0, 9, 9}, {0, 9, 9}};
BYTE data_this[16][3];
//4.送信用
DWORD dwSendSize;
DWORD dwErrorMask;

#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
//#include <cmath>

#include <opencv2/opencv.hpp>

#include "Leap.h"
using namespace Leap;


#ifdef _DEBUG
#pragma comment(lib, "opencv_core245d.lib")
#pragma comment(lib, "opencv_highgui245d.lib")
#pragma comment(lib, "opencv_calib3d245d.lib")
#pragma comment(lib, "opencv_imgproc245d.lib")
#else
#pragma comment(lib, "opencv_core245.lib")
#pragma comment(lib, "opencv_highgui245.lib")
#pragma comment(lib, "opencv_calib3d245.lib")
#pragma comment(lib, "opencv_imgproc245.lib")
#endif

#define RESOLUTION 8	//8=16pins, 16=64pins, 32=256pins, 64=1024pins


#include "Mouse.h"
#include "OBJLoader.h"
#include "Sample.h"

using namespace std;

int WinID[2];
int WindowPositionX = 100;
int WindowPositionY = 100;
int WindowWidth = 848;	//512->848
int WindowHeight = 480;	//512->480
char WindowTitle[2][15] = {"OBJ file", "SurfaceDisplay"};
GLfloat lightColor[4]  = { 1.0, 1.0, 1.0, 1.0 };
GLfloat lightPosition[4] = { 0.0, 100.0, 0.0, 0.0 };
ViewCamera camera(5.0);
OBJMesh mesh;

GLuint	texture_name;
GLuint	renderbuffer_name;
GLuint	framebuffer_name;
GLint	viewport[ 4 ];

//set material
GLfloat mat_ambient0[] = { 0.1, 0.1, 0.2, 1.0 };
GLfloat mat_diffuse0[] = { 0.3, 0.4, 1.0, 1.0 };
GLfloat mat_specular0[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_shininess0[] = { 50.0 };

bool wireframe_flag = false;

float depth[(RESOLUTION/2)*(RESOLUTION/2)];
double objX[(RESOLUTION/2)*(RESOLUTION/2)];
double objY[(RESOLUTION/2)*(RESOLUTION/2)];
double objZ[(RESOLUTION/2)*(RESOLUTION/2)] = {0};

//デプスを取得したい座標
//position[i][0], 512 - position[i][1]
//x: 0~511, y: 1~512
int position[(RESOLUTION/2)*(RESOLUTION/2)][2];
// memo
/*
**[0,3] [0,2] [0,1] [0,0]
**[1,3] [1,2] [1,1] [1,0]
**[2,3] [2,2] [2,1] [2,0]
**[3,3] [3,2] [3,1] [3,0]
*/

double bar[RESOLUTION/2][RESOLUTION/2];
double bar4pos[RESOLUTION/2][RESOLUTION/2];

//　prototype
void Initialize();
void Display();
void Idle();
void Shutdown();
void RenderToTexture();
void Reshape(int x, int y);
void Keyboard(unsigned char key, int x, int y);
void Special(int key, int x, int y);
void Mouse(int button, int state, int x, int y);
void Motion(int x, int y);
void PassiveMotion(int x, int y);
void SetLighting();
void displayTexPolygon(int pins);

GLfloat RotateAngle = 0.0f;




//----------------------------------------------------------------------------------------------------
//　　main
//　　Desc : メインエントリポイント
//----------------------------------------------------------------------------------------------------
int main( int argc, char **argv )
{

	// Create a sample listener and controller
	SampleListener listener;	//LeapMotion
	Controller controller;		//LeapMotion

	// Have the sample listener receive events from the controller
	controller.addListener(listener);	//LeapMotion


	//----------------------------------
	//		Serial Start
	//----------------------------------
	
	//1.ポートをオープン
	arduino = CreateFile(L"\\\\.\\COM3", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if(arduino == INVALID_HANDLE_VALUE){
	std::cout << "COULD NOT OPEN" << std::endl;
	system("PAUSE");
	exit(0);
	}
	//2.送受信バッファ初期化
	Ret = SetupComm(arduino,1024,1024);
	if(!Ret){
	std::cout << "SET UP FAILED" << std::endl;
	CloseHandle(arduino);
	system("PAUSE");
	exit(0);
	}
	Ret = PurgeComm(arduino,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	if(!Ret){
	std::cout << "CLEAR FAILED" << std::endl;
	CloseHandle(arduino);
	exit(0);
	}
	//3.基本通信条件の設定
	DCB dcb;
	GetCommState(arduino,&dcb);
	dcb.DCBlength = sizeof(DCB);
	dcb.BaudRate = 57600;	//arduino = 9600?
	dcb.fBinary = TRUE;
	dcb.ByteSize = 8;
	dcb.fParity =NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	Ret = SetCommState(arduino,&dcb);
	if(!Ret){
	std::cout << "SetCommState FAILED" << std::endl;
	CloseHandle(arduino);
	system("PAUSE");
	exit(0);
	}
	
	//----------------------------------
	//		Serial END
	//----------------------------------
	glutInit(&argc, argv);

	// WinID[0]
	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitWindowPosition(-500, 400);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
	WinID[0] = glutCreateWindow(WindowTitle[0]);
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);
	glutMotionFunc(Motion);
	glutPassiveMotionFunc(PassiveMotion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(Special);
	Initialize();

	// WinID[1]
	//glutInitWindowSize(WindowWidth, WindowHeight);
	//glutInitWindowPosition (0, 0);
	glutInitDisplayMode (GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
	WinID[1] = glutCreateWindow (WindowTitle[1]);
	glewInit();
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	Initialize();


	// Omit window frame
	//GLのデバイスコンテキストハンドル取得
	HDC glDc = wglGetCurrentDC();
	//ウィンドウハンドル取得
	HWND hWnd = WindowFromDC(glDc);
	//ウィンドウの属性と位置変更
	SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
	SetWindowPos(hWnd, HWND_TOP, 0, 0, WindowWidth, WindowHeight+480, SWP_SHOWWINDOW);

	glutIdleFunc(Idle);
	glutMainLoop();

	CloseHandle(arduino);
	Shutdown();

	// Remove the sample listener when done
	controller.removeListener(listener);	//LeapMotion

	return 0;
}


//----------------------------------------------------------------------------------------------------
//　　SetLighting
//　　Desc : ライティング
//----------------------------------------------------------------------------------------------------
void SetLighting()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightColor);
}

//----------------------------------------------------------------------------------------------------
//　　Initialize
//　　Desc : 初期化処理
//----------------------------------------------------------------------------------------------------
void Initialize()
{
	if(glutGetWindow() == WinID[0]) {
		//　塗りつぶし色
		glClearColor(0.3, 0.3, 1.0, 1.0);
		//　深度テスト有効
		glEnable(GL_DEPTH_TEST);
		//　スムーズシェイディング
		glShadeModel(GL_SMOOTH);
		//　ライティング
		SetLighting();
		//　メッシュファイルの読み込み
		mesh.Load("Mesh/dosei.obj");
		//　メッシュファイルの情報を表示
		mesh.Information();

		/*
		for(int i = 0; i < RESOLUTION/2; i++) {
		for(int j = 0; j < RESOLUTION/2; j++) {
		if(j%2 == 0) {
		position[i*RESOLUTION/2+j][0] = 847/(RESOLUTION/2-1)*j;	//511
		position[i*RESOLUTION/2+j][1] = 479/(RESOLUTION-1)*i*2+1+1;
		}
		if(j%2 == 1) {
		position[i*RESOLUTION/2+j][0] = 847/(RESOLUTION/2-1)*j-1;//-1;
		position[i*RESOLUTION/2+j][1] = 479/(RESOLUTION-1)*(i*2+1)+1;
		}
		}
		}
		*/

		//下の段
		position[0][0] = 847*1/7.0;
		position[0][1] = 38.4;//479*0/3.0;
		position[1][0] = 847*3/7.0;
		position[1][1] = 38.4;//479*0/3.0;
		position[2][0] = 847*5/7.0;
		position[2][1] = 38.4;//479*0/3.0;
		position[3][0] = 847*7/7.0;
		position[3][1] = 38.4;//479*0/3.0;
		//中下段
		position[4][0] = 847*0/7.0;
		position[4][1] = 165.6;//479*1/3.0;
		position[5][0] = 847*2/7.0;
		position[5][1] = 165.6;//479*1/3.0;
		position[6][0] = 847*4/7.0;
		position[6][1] = 165.6;//479*1/3.0;
		position[7][0] = 847*6/7.0;
		position[7][1] = 165.6;//479*1/3.0;
		//中上段
		position[8][0] = 847*1/7.0;
		position[8][1] = 292.8;//479*2/3.0;
		position[9][0] = 847*3/7.0;
		position[9][1] = 292.8;//479*2/3.0;
		position[10][0] = 847*5/7.0;
		position[10][1] = 292.8;//479*2/3.0;
		position[11][0] = 847*7/7.0;
		position[11][1] = 292.8;//479*2/3.0;
		//上段
		position[12][0] = 847*0/7.0;
		position[12][1] = 420;//479*3/3.0;
		position[13][0] = 847*2/7.0;
		position[13][1] = 420;//479*3/3.0;
		position[14][0] = 847*4/7.0;
		position[14][1] = 420;//479*3/3.0;
		position[15][0] = 847*6/7.0;
		position[15][1] = 420;//479*3/3.0;

	} else if(glutGetWindow() == WinID[1]) {
		//　塗りつぶし色
		glClearColor(0.3, 0.3, 1.0, 1.0);
		glEnable( GL_DEPTH_TEST );
		//　スムーズシェイディング
		glShadeModel(GL_SMOOTH);

		//　ライティング
		SetLighting();
		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
		glGenTextures( 1, &texture_name );
		glBindTexture( GL_TEXTURE_2D, texture_name );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
		//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, 256, 256, GL_RGB, GL_UNSIGNED_BYTE, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		//　メッシュファイルの読み込み
		mesh.Load("Mesh/dosei.obj");

		//Renderbuffer
		glGenRenderbuffersEXT( 1, &renderbuffer_name );
		glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, renderbuffer_name );
		glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 1024, 1024 );

		//Framebuffer
		glGenFramebuffersEXT( 1, &framebuffer_name );
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, framebuffer_name );
		glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, texture_name, 0 );
		glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, renderbuffer_name );
		glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	}
}

void RenderToTexture() {
	/* switch to framebuffer object */

	glGetIntegerv( GL_VIEWPORT, viewport );
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, framebuffer_name );

	/* clear buffer */

	glClearColor( 0.2, 0.3, 0.6, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	/* set viewport */

	glViewport( 0, 0, 1024, 1024 );

	/* set projection */

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	//gluPerspective( 40.0, 256 / ( double ) 256,	1.77, 1000.0 );
	gluPerspective( 26.118501205216, 848.0/480.0, 1, 1000.0);

	/* set camera */

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	//gluLookAt( 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 );
	//glRotatef( RotateAngle++, 0.0f, 0.0f, 1.0f );	//auto rotation

	camera.Set2();

	/* set light (in world coordinates) */

	SetLighting();

	/* set material */
	glMaterialfv( GL_FRONT, GL_AMBIENT, mat_ambient0 );
	glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse0 );
	glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular0 );
	glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess0 );

	/* put object */

	glPushMatrix();
	glScalef( -1.0f, 1.0f, 1.0f );

	//　メッシュを描画
	if ( wireframe_flag ) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	mesh.Render(2.0);	
	if ( wireframe_flag ) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glPopMatrix();

	/* execute drawing */

	glFlush();

	/* switch to default buffer */
	glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
	glViewport( viewport[ 0 ], viewport[ 1 ], viewport[ 2 ], viewport[ 3 ] );
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

//---------------------------------------------------------------------------------------------------
//　　Idle
//　　Desc : アイドリング時の処理
//---------------------------------------------------------------------------------------------------
void Idle()
{
	// 複数ウィンドウを出す場合の処理
	for(int loop = 0; loop < 2; ++loop){
		glutSetWindow(WinID[loop]);
		glutPostRedisplay(); //再描画 (※display()関数を呼び出す関数 )
	}
}

//---------------------------------------------------------------------------------------------------
//　　displayTexPolygon
//　　
//---------------------------------------------------------------------------------------------------
void displayTexPolygon(int pins)
{

	bar[0][0] = bar_heights[0][3];	bar[0][1] = bar_heights[0][2];	bar[0][2] = bar_heights[0][1];	bar[0][3] = bar_heights[0][0];
	bar[1][0] = bar_heights[1][3];	bar[1][1] = bar_heights[1][2];	bar[1][2] = bar_heights[1][1];	bar[1][3] = bar_heights[1][0];
	bar[2][0] = bar_heights[2][3];	bar[2][1] = bar_heights[2][2];	bar[2][2] = bar_heights[2][1];	bar[2][3] = bar_heights[2][0];
	bar[3][0] = bar_heights[3][3];	bar[3][1] = bar_heights[3][2];	bar[3][2] = bar_heights[3][1];	bar[3][3] = bar_heights[3][0];

	for(int i = 0; i < RESOLUTION/2; i++) {
		for(int j = 0; j < RESOLUTION/2; j++) {
			if(bar[i][j] < 0) bar[i][j] = 0;
			if(bar[i][j] > 99) bar[i][j] = 99;

			//if(i==0 && j==0) cout << bar[i][j] << endl;

			bar4pos[i][j] = 99 - bar[i][j];

			//data_buffer[0] = (i*4)+j+97;
			data_this[(i*4)+j][0] = bar4pos[i][j]/100 + 48;
			data_this[(i*4)+j][1] = (bar4pos[i][j] - (data_this[(i*4)+j][0]-48))/10 + 48;
			data_this[(i*4)+j][2] = (bar4pos[i][j] - (data_this[(i*4)+j][0]-48)*100 - (data_this[(i*4)+j][1]-48)*10) + 48;
			//cout << bar[i][j] << ", " << data_buffer[0] << data_buffer[1] << data_buffer[2] << data_buffer[3] << endl;

			bar[i][j] = bar[i][j] * (-1) / 10.0;	//シミュレータ上のピンロッドの位置確定
		}
	}

	

	//プレゼン用テクスチャ
	
	cv::Mat src_img = cv::imread("test.jpg");
	cv::Mat img;
	cv::flip(src_img, img, 0);
	cv::flip(img, img, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.size().width, img.size().height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, img.data);
	
	//プレゼン用テクスチャ End
	

	/*
	double pos1 = 0.0;
	double pos2 = 0.0;
	int k = 0;
	for(int loop = 0; loop < pins/2-1; loop++) {
	k = 0;
	glBegin(GL_TRIANGLE_STRIP);

	for(double i = 0; i < pins; i++) {
	if(fmod(i, 2.0) == 0.0) {
	double tmp11 = 1.0-i/(pins-1);
	double tmp12 = -5.0+pos1*30/(pins/2-1)*2;
	double tmp13 = 15.0-i*35/(pins-1);
	double tmp14 = (0.0+pos1*2.0)/(pins/2-1);
	glTexCoord2f(tmp14, tmp11);
	glVertex3f(tmp12-10, tmp13, bar[k][0+(int)pos1*2]);
	} else {
	double tmp21 = 1.0-i/(pins-1);
	double tmp22 = -5.0+30/(pins/2-1)+pos2*30/(pins/2-1)*2; //5.0+pos2*30/(pins/2-1)*2;
	double tmp23 = 15.0-i*35/(pins-1);
	double tmp24 = (1.0+pos2*2.0)/(pins/2-1);
	glTexCoord2f(tmp24, tmp21);
	glVertex3f(tmp22-10, tmp23, bar[k][1+(int)pos2*2]);
	k++;
	}
	}


	glEnd();
	if(loop%2 == 0) pos1++;
	if(loop%2 == 1) pos2++;
	}
	*/


		glBegin(GL_TRIANGLE_STRIP);	//upper
		glTexCoord2f(0.0/7.0, 0.795*3.0/3.0+0.125); glVertex3f(-35.0, 53/2.0/2.0, bar[0][0]);	//縦に倍になったので，/2.0
		glTexCoord2f(1.0/7.0, 0.795*2.0/3.0+0.125); glVertex3f(-25.0, 53/6.0/2.0, bar[1][0]);	//848.480に合わせて横の長さも調節
		glTexCoord2f(2.0/7.0, 0.795*3.0/3.0+0.125); glVertex3f(-15.0, 53/2.0/2.0, bar[0][1]);
		glTexCoord2f(3.0/7.0, 0.795*2.0/3.0+0.125); glVertex3f( -5.0, 53/6.0/2.0, bar[1][1]);
		glTexCoord2f(4.0/7.0, 0.795*3.0/3.0+0.125); glVertex3f(  5.0, 53/2.0/2.0, bar[0][2]);
		glTexCoord2f(5.0/7.0, 0.795*2.0/3.0+0.125); glVertex3f( 15.0, 53/6.0/2.0, bar[1][2]);
		glTexCoord2f(6.0/7.0, 0.795*3.0/3.0+0.125); glVertex3f( 25.0, 53/2.0/2.0, bar[0][3]);
		glTexCoord2f(7.0/7.0, 0.795*2.0/3.0+0.125); glVertex3f( 35.0, 53/6.0/2.0, bar[1][3]);
		glEnd();

		glBegin(GL_TRIANGLE_STRIP);	//center
		glTexCoord2f(0.0/7.0, 0.795*1.0/3.0+0.125); glVertex3f(-35.0, -53/6.0/2.0, bar[2][0]);
		glTexCoord2f(1.0/7.0, 0.795*2.0/3.0+0.125); glVertex3f(-25.0,  53/6.0/2.0, bar[1][0]);
		glTexCoord2f(2.0/7.0, 0.795*1.0/3.0+0.125); glVertex3f(-15.0, -53/6.0/2.0, bar[2][1]);
		glTexCoord2f(3.0/7.0, 0.795*2.0/3.0+0.125); glVertex3f( -5.0,  53/6.0/2.0, bar[1][1]);
		glTexCoord2f(4.0/7.0, 0.795*1.0/3.0+0.125); glVertex3f(  5.0, -53/6.0/2.0, bar[2][2]);
		glTexCoord2f(5.0/7.0, 0.795*2.0/3.0+0.125); glVertex3f( 15.0,  53/6.0/2.0, bar[1][2]);
		glTexCoord2f(6.0/7.0, 0.795*1.0/3.0+0.125); glVertex3f( 25.0, -53/6.0/2.0, bar[2][3]);
		glTexCoord2f(7.0/7.0, 0.795*2.0/3.0+0.125); glVertex3f( 35.0,  53/6.0/2.0, bar[1][3]);
		glEnd();

		glBegin(GL_TRIANGLE_STRIP);	//lower
		glTexCoord2f(0.0/7.0, 0.795*1.0/3.0+0.125); glVertex3f(-35.0, -53/6.0/2.0, bar[2][0]);
		glTexCoord2f(1.0/7.0, 0.795*0.0/3.0+0.125); glVertex3f(-25.0, -53/2.0/2.0, bar[3][0]);
		glTexCoord2f(2.0/7.0, 0.795*1.0/3.0+0.125); glVertex3f(-15.0, -53/6.0/2.0, bar[2][1]);
		glTexCoord2f(3.0/7.0, 0.795*0.0/3.0+0.125); glVertex3f( -5.0, -53/2.0/2.0, bar[3][1]);
		glTexCoord2f(4.0/7.0, 0.795*1.0/3.0+0.125); glVertex3f(  5.0, -53/6.0/2.0, bar[2][2]);
		glTexCoord2f(5.0/7.0, 0.795*0.0/3.0+0.125); glVertex3f( 15.0, -53/2.0/2.0, bar[3][2]);
		glTexCoord2f(6.0/7.0, 0.795*1.0/3.0+0.125); glVertex3f( 25.0, -53/6.0/2.0, bar[2][3]);
		glTexCoord2f(7.0/7.0, 0.795*0.0/3.0+0.125); glVertex3f( 35.0, -53/2.0/2.0, bar[3][3]);
		glEnd();

	/*
	glTexCoord2f(0.22*1/3+0.39, 7.0/7.0); glVertex3f( -2, 28, 0);
	glTexCoord2f(0.22*0/3+0.39, 6.0/7.0); glVertex3f( -6, 20, 0);
	glTexCoord2f(0.22*1/3+0.39, 5.0/7.0); glVertex3f( -2, 12, 0);
	glTexCoord2f(0.22*0/3+0.39, 4.0/7.0); glVertex3f( -6,  4, 0);
	glTexCoord2f(0.22*1/3+0.39, 3.0/7.0); glVertex3f( -2, -4, 0);
	glTexCoord2f(0.22*0/3+0.39, 2.0/7.0); glVertex3f( -6,-12, 0);
	glTexCoord2f(0.22*1/3+0.39, 1.0/7.0); glVertex3f( -2,-20, 0);
	glTexCoord2f(0.22*0/3+0.39, 0.0/7.0); glVertex3f( -6,-28, 0);

	glTexCoord2f(0.22*1/3+0.39, 7.0/7.0); glVertex3f( -2, 28, 0);
	glTexCoord2f(0.22*2/3+0.39, 6.0/7.0); glVertex3f(  2, 20, 0);
	glTexCoord2f(0.22*1/3+0.39, 5.0/7.0); glVertex3f( -2, 12, 0);
	glTexCoord2f(0.22*2/3+0.39, 4.0/7.0); glVertex3f(  2,  4, 0);
	glTexCoord2f(0.22*1/3+0.39, 3.0/7.0); glVertex3f( -2, -4, 0);
	glTexCoord2f(0.22*2/3+0.39, 2.0/7.0); glVertex3f(  2,-12, 0);
	glTexCoord2f(0.22*1/3+0.39, 1.0/7.0); glVertex3f( -2,-20, 0);
	glTexCoord2f(0.22*2/3+0.39, 0.0/7.0); glVertex3f(  2,-28, 0);

	glTexCoord2f(0.22*3/3+0.39, 7.0/7.0); glVertex3f(  6, 28, 0);
	glTexCoord2f(0.22*2/3+0.39, 6.0/7.0); glVertex3f(  2, 20, 0);
	glTexCoord2f(0.22*3/3+0.39, 5.0/7.0); glVertex3f(  6, 12, 0);
	glTexCoord2f(0.22*2/3+0.39, 4.0/7.0); glVertex3f(  2,  4, 0);
	glTexCoord2f(0.22*3/3+0.39, 3.0/7.0); glVertex3f(  6, -4, 0);
	glTexCoord2f(0.22*2/3+0.39, 2.0/7.0); glVertex3f(  2,-12, 0);
	glTexCoord2f(0.22*3/3+0.39, 1.0/7.0); glVertex3f(  6,-20, 0);
	glTexCoord2f(0.22*2/3+0.39, 0.0/7.0); glVertex3f(  2,-28, 0);
	*/


	//----------------------------------
	//		Serial Start
	//----------------------------------
	
	//std::cout << bar[0][0] << std::endl;
	//data = (int)(bar[0][0] * 4);
	for(int i = 0; i < 16; i++) {
		//**********************************
		//* data_thisとdata_beforeを比較
		//* 異なっていればdata = data_this
		//* その後data_before = data_this
		//**********************************
		if((data_this[i][0] != data_before[i][0] || data_this[i][1] != data_before[i][1] || data_this[i][2] != data_before[i][2])) {
		
			data[0] = i+97;
			data[1] = data_this[i][0];
			data[2] = data_this[i][1];
			data[3] = data_this[i][2];
			Ret = WriteFile(arduino, &data, sizeof(data), &dwSendSize, NULL);
			if(!Ret){
				std::cout << "SEND FAILED" << std::endl;
				CloseHandle(arduino);
				system("PAUSE");
				exit(0);
			}
			cout << data[0] << data[1] << data[2] << data[3] << endl;
		}
		data_before[i][0] = data_this[i][0];
		data_before[i][1] = data_this[i][1];
		data_before[i][2] = data_this[i][2];
	}
	
	//----------------------------------
	//		Serial End
	//----------------------------------
}

//---------------------------------------------------------------------------------------------------
//　　Reshape
//　　Desc : サイズ変更
//---------------------------------------------------------------------------------------------------
void Reshape(int x, int y)
{
	if(glutGetWindow() == WinID[0]) {
		WindowWidth = x;
		WindowHeight = y;
		if ( WindowWidth < 1 ) WindowWidth = 1;
		if ( WindowHeight < 1 ) WindowHeight = 1;

	} else if(glutGetWindow() == WinID[1]) {
		glViewport(0, 0, (GLsizei) x, (GLsizei) y);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glFrustum(-5.0, 5.0,-5.0, 5.0, 5.0, 500.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

}

//---------------------------------------------------------------------------------------------------
//　　Display
//　　Desc : ウィンドウへの描画
//---------------------------------------------------------------------------------------------------
void Display()
{
	if(glutGetWindow() == WinID[0]) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, WindowWidth, WindowHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		//gluPerspective(40.0, (double)WindowWidth/(double)WindowHeight, 0.1, 1000.0);
		gluPerspective( 26.118501205216, 848.0/480.0, 1, 1000.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		//glTranslatef(0.0f, 0.0f, -10.0f);
		//gluLookAt(10.0, -2.5, 20.0, 10.0, -2.5, 0.0, 0.0, 1.0, 0.0);
		//glRotatef( RotateAngle++, 0.0f, 0.0f, 1.0f );	//auto rotation
		///
		glPushMatrix();

		if(gesture_flag == LEFT_SWIPE) {

		} else {

		}

		//　視点の描画
		camera.Set(gesture_flag);

		gesture_flag = 2;

		//　ライティング
		SetLighting();

		//　メッシュを描画
		if ( wireframe_flag ) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		mesh.Render(2.0);	
		if ( wireframe_flag ) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		///
		glPopMatrix();

		//　補助軸の描画
		//glPushMatrix();
		//camera.RenderSubAxis(WindowWidth, WindowHeight);
		//glPopMatrix();

		//　ダブルバッファ
		glutSwapBuffers();

		//-------------------------------------------------------------------
		//	get information of depth
		//-------------------------------------------------------------------
		double modelview[16];
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);

		double projection[16];
		glGetDoublev(GL_PROJECTION_MATRIX, projection);

		int viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		// Get Depth buffer
		for(int i = 0; i < pow((RESOLUTION/2), 2.0); i++) {
			glReadPixels(position[i][0],WindowHeight - position[i][1] - 1,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth[i]);
		}
		// ウィンドウ座標からオブジェクト座標を求める
		for(int j = 0; j < pow((RESOLUTION/2), 2.0); j++) {
			gluUnProject(position[j][0],WindowHeight - position[j][1] - 1,depth[j],modelview,projection,viewport,&objX[j],&objY[j],&objZ[j]);
		}


		for(int i = 0; i < RESOLUTION/2; i++) {
			for(int j = RESOLUTION/2 - 1, k = 0; j >= 0; j--, k++) {
				//bar[i][j] = (objZ[i*RESOLUTION/2+k]*(-1)-5)*2;	//+6
				bar[i][j] = floor(objZ[i*RESOLUTION/2+k]*35)+200;	//微調整
			}
		}

		//cout << bar[0][2] << endl;	//outted 20131121
		//cout << "[3,0]:" << bar[3][0] << "\t[3,1]:" << bar[3][1] << "\t[3,2]:" << bar[3][2] << "\t[3,3]:" << bar[3][3] << endl;
		//-------------------------------------------------------------------
		//	end
		//-------------------------------------------------------------------
	} else if(glutGetWindow() == WinID[1]) {
		/* rendering teapot on texture */

		RenderToTexture();

		/* clear buffer */

		glClearColor( 0.0, 0.0, 0.0, 0.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		/* set camera */

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		//gluPerspective( 90, 1, 1, 1000.0);
		//gluLookAt(0.0, 0.0, -40.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		gluLookAt(0.0, -13.25, -40.0, 0.0, -13.25, 0.0, 0.0, 1.0, 0.0);
		//gluLookAt(10.0, -25.0, 12.0, 10.0, -2.5, 0.0, 0.0, 1.0, 0.0);
		// here!

		/* set light (in world coordinates) */

		SetLighting();

		/* set material */

		glEnable( GL_COLOR_MATERIAL );
		glColor3f( 1.0f, 1.0f, 1.0f );

		/* put object */


		glEnable( GL_TEXTURE_2D );
		glBindTexture( GL_TEXTURE_2D, texture_name );
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

		glPushMatrix();

		//glRotatef( 0.0f, 0.0f, 1.0f, 0.0f );
		glEnable(GL_TEXTURE_2D);
		displayTexPolygon(RESOLUTION);
		glDisable( GL_COLOR_MATERIAL );
		glDisable( GL_TEXTURE_2D );




		/* swap buffers */
		glutSwapBuffers();
	}
}

//---------------------------------------------------------------------------------------------------
//　　Mouse
//　　Desc : マウス処理
//---------------------------------------------------------------------------------------------------
void Mouse(int button, int state, int x, int y)
{
	camera.MouseInput(button, state, x, y);
}

//--------------------------------------------------------------------------------------------------
//　　Motion
//　　Desc : マウスドラッグ時
//--------------------------------------------------------------------------------------------------
void Motion(int x, int y)
{
	camera.MouseMotion(x, y);
}

//--------------------------------------------------------------------------------------------------
//　　PassiveMotion
//　　Desc : マウス移動時
//--------------------------------------------------------------------------------------------------
void PassiveMotion(int x, int y)
{
}

//--------------------------------------------------------------------------------------------------
//　　Keyboard
//　　Desc : キーボード処理
//--------------------------------------------------------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
	switch ( key )
	{
	case '\033':
		exit(0);
		break;

	case 'w':
		wireframe_flag ? wireframe_flag = false : wireframe_flag = true;
		cout << "Wire Frame : " << wireframe_flag << endl;
		break;

	default:
		break;
	}
}

//--------------------------------------------------------------------------------------------------
//　　Special
//　　Desc : 特殊キー処理
//--------------------------------------------------------------------------------------------------
void Special(int key, int x, int y)
{
	switch ( key )
	{
	case GLUT_KEY_F1:
		break;

	case GLUT_KEY_F2:
		break;

	case GLUT_KEY_F3:
		break;

	case GLUT_KEY_F4:
		break;

	case GLUT_KEY_F5:
		break;

	case GLUT_KEY_F6:
		break;

	case GLUT_KEY_F7:
		break;

	case GLUT_KEY_F8:
		break;

	case GLUT_KEY_F9:
		break;

	case GLUT_KEY_F10:
		break;

	case GLUT_KEY_F11:
		break;

	case GLUT_KEY_F12:
		break;

	case GLUT_KEY_LEFT:
		break;

	case GLUT_KEY_RIGHT:
		break;

	case GLUT_KEY_UP:
		break;

	case GLUT_KEY_DOWN:
		break;

	case GLUT_KEY_PAGE_UP:
		break;

	case GLUT_KEY_PAGE_DOWN:
		break;

	case GLUT_KEY_HOME:
		break;

	case GLUT_KEY_END:
		break;

	case GLUT_KEY_INSERT:
		break;
	}
}

//----------------------------------------------------------------------------------------------------
//　　Shutdown
//　　Desc : 後片付け
//----------------------------------------------------------------------------------------------------
void Shutdown()
{
	//　メッシュファイルの後片付け
	mesh.Release();
}