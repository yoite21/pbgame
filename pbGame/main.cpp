#include "pbGameClient.h"

#define MOVE_TIME 70

pbGameClient gameClient;

void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	gameClient.render();

	glutSwapBuffers();
}

void move(int)
{


	gameClient.move();
	gameClient.updateNetwork();

	glutPostRedisplay();
	glutTimerFunc(MOVE_TIME, move, 0);
}


void changeSize(int w, int h)
{
	if( h == 0 )
		h = 1;

	float ratio = (float)w / h;
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45,ratio, 0.1, 1000);
}


void normalKeyboardPress(unsigned char key, int, int)
{
	gameClient.normalKeyboardPress(key);
}

void normalKeyboardUp(unsigned char key, int, int)
{
	gameClient.normalKeyboardUp(key);
}

void mouseMove(int x, int y)
{
	if((x == 100 && y == 100))
		return;

	SetCursorPos(100, 100);

	gameClient.mouseMove((x - 100)*0.1, (y - 100)*0.1);

}

void idleFunc()
{
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
	
	glutInitWindowSize(1000,1080);
	glutCreateWindow("Prisoner Battle");
	glutFullScreen();

	glutKeyboardFunc(normalKeyboardPress);
	glutKeyboardUpFunc(normalKeyboardUp);
	glutIgnoreKeyRepeat(1);
	glutPassiveMotionFunc(mouseMove);
	glutDisplayFunc(render);
	glutReshapeFunc(changeSize);
	glutTimerFunc(MOVE_TIME, move, 0);
	
	gameClient.init();

	glutIdleFunc(idleFunc);

	glutMainLoop();

}
