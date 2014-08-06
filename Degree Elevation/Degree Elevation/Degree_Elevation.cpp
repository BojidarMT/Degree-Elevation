#include <iostream>
#include <stdlib.h>
#include <GL/glut.h> 
#include <windows.h>
#include <math.h>
#include <gl/Gl.h>
#include <gl/Glu.h>
#include <vector>
#include <cmath>
#include <GL\glui.h>
using namespace std;
int DegreesCounter = 1;
bool IncreaseDegrees;
GLuint main_window; //  Window
GLUI * glui_window; // Pointer to window for Glui library
bool AddingPoints = false;

class Points {
public:

	float x, y, z;
	void xANDy(float x2, float y2) { x = x2; y = y2; }

	const Points & operator = (const Points &Point) {
		x = Point.x;
		y = Point.y;
		z = Point.z;

		return *this;
	}

};

std::vector<Points> NewPoints; // vector, who saves the coordinates for the new points after increasing the degree 

Points *newPoints;	// pointer to NewPoints

std::vector<Points> meanPoints; // vector, in who are calculated the points after increasing the degree, then they are transferred to NewPoints 

Points *mean;	// pointer to meanPoints

std::vector<Points> polygonPoints; // vector, in who are saves first introduced points with mouse, which are needed for the curve
Points *polygon;	// pointer to polygonPoints

Points DrawBezierCurve(int r, int i, double t) { // De Casteljau Algorithm 
	Points P;

	if (r == 0) return polygonPoints[i];

	Points p1 = DrawBezierCurve(r - 1, i, t);
	Points p2 = DrawBezierCurve(r - 1, i + 1, t);

	P.x = (float)((1 - t)*p1.x + t*p2.x);
	P.y = (float)((1 - t)*p1.y + t*p2.y);
	P.z = (float)((1 - t)*p1.z + t*p2.z);
	return P;
}

void drawLine(Points p1, Points p2) { // method for drawing the curve

	glLineWidth(4);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glBegin(GL_LINE_STRIP);
	glVertex3f(p1.x, p1.y, -0.2);
	glVertex3f(p2.x, p2.y, -0.2);
	glEnd();

	glFlush();
}

int NumberOfPoints = 0; // number of points introduced with the mouse

void DrawNewPoints() // method for drawing the new points after increasing the degree
{
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glPointSize(5);
	glBegin(GL_POINTS);
	for (int i = 0; i < NewPoints.size(); i++)
	{

		if (NewPoints[i].x > 0 || NewPoints[i].y > 0){
			glVertex3f(NewPoints[i].x, NewPoints[i].y, 1);
		}

	}
	glEnd();

	glColor4f(0.137255f, 0.737255f, 0.560784f, 1.0f);
	glLineWidth(2);
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i < NewPoints.size(); i++)
	{
		if (NewPoints[i].x > 0 || NewPoints[i].y > 0){
			glVertex3f(NewPoints[i].x, NewPoints[i].y, 0.6);

		}

	}
	glEnd();
}

void drawPoint()
{
	if (AddingPoints == false)
	{
		if (NumberOfPoints > 2) {

			Points POld = polygonPoints[0];

			for (double t = 0.0; t <= 1.0; t += 0.02) {   // Drawing the Bezier Curve with t = 0,02 for smoothing curve
				Points P = DrawBezierCurve(polygonPoints.size() - 1, 0, t);
				drawLine(POld, P);
				POld = P;

			}

		}

		// Drawing the points introduced with the mouse
		glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_BLEND);
		glPointSize(12);
		glBegin(GL_POINTS);
		for (int i = 0; i < polygonPoints.size(); i++)
		{
			int k = polygonPoints[i].x;
			if (polygonPoints[i].x > 0 || polygonPoints[i].y > 0){
				glVertex3f(polygonPoints[i].x, polygonPoints[i].y, 0.6);
			}

		}
		glEnd();

		// Drawing the lines between the points introduced with the mouse
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glLineWidth(6);
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < polygonPoints.size(); i++)
		{
			if (polygonPoints[i].x > 0 || polygonPoints[i].y > 0){
				glVertex3f(polygonPoints[i].x, polygonPoints[i].y, 0.3);

			}

		}
		glEnd();

	}
	if (IncreaseDegrees == true) // If is pressed the button for increasing the degree: IncreaseDegrees becomes true and the points are drawn for the new polygon
	{
		DrawNewPoints();
	}
}

void PovdiganeNaStepen() // Method used for calculating the new points after increasing the degree with the appropriate formula
{


	int broitochki = 1;
	float newX;
	float newY;
	if (NewPoints.size()<2)
	{
		return;
	}

	mean = new Points;
	mean->xANDy((float)NewPoints.front().x, (float)NewPoints.front().y);
	meanPoints.insert(meanPoints.begin(), *mean);


	for (int i = 1; i < NewPoints.size(); i++)
	{
		float t = ((float)i / (float)NewPoints.size());
		float t1 = 1 - t;
		newX = (t * (NewPoints.at(i - 1)).x) + (t1* (NewPoints.at(i)).x);
		newY = (t * (NewPoints.at(i - 1)).y) + (t1* (NewPoints.at(i)).y);

		mean = new Points;
		mean->xANDy((float)newX, (float)newY);
		meanPoints.insert(meanPoints.begin() + broitochki, *mean);

		broitochki++;
	}

	mean = new Points;
	mean->xANDy((float)NewPoints.back().x, (float)NewPoints.back().y);
	meanPoints.insert(meanPoints.begin() + broitochki, *mean);

	NewPoints = meanPoints;

	meanPoints.clear();

	cout << "Degree : " << DegreesCounter + NumberOfPoints - 1 << endl;  // displaying the current degree
	DrawNewPoints();
}

void CallIncreaseDegree() //method, which is calling when the button is pressed
{
	if (polygonPoints.size() > 2)
	{

		PovdiganeNaStepen();
		IncreaseDegrees = true;
		DegreesCounter++;
		AddingPoints = true;
	}

}



void ClearAll() // Clear all
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	NewPoints.clear();
	polygonPoints.clear();
	meanPoints.clear();
	NumberOfPoints = 0;
	DegreesCounter = 1;
	AddingPoints = false;
	glFlush();
}

void MyGlui() //  Drawing the buttons
{
	GLUI *glui = GLUI_Master.create_glui_subwindow(main_window, GLUI_SUBWINDOW_TOP);

	glui->add_button("Increase Degree", 1, (GLUI_Update_CB)CallIncreaseDegree);
	glui->add_column(true);
	glui->add_button("Clear", 1, (GLUI_Update_CB)ClearAll);
	glui->add_column(true);
	glui->add_button("Quit", 2, (GLUI_Update_CB)exit);
	glui->set_main_gfx_window(main_window);
}

void Mouse(int button, int state, int x, int y) { // Method used for taking the coordinates from the mouse when is introduced a vertex
	if (AddingPoints == false)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			newPoints = new Points;
			newPoints->xANDy((float)x, (float)y);
			NewPoints.insert(NewPoints.begin() + NumberOfPoints, *newPoints);
			polygon = new Points;
			polygon->xANDy((float)x, (float)y);
			polygonPoints.insert(polygonPoints.begin() + NumberOfPoints, *polygon);
			NumberOfPoints++;
		}
	}

}

void Screen() {
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	drawPoint();
	glFlush();

}

void Reshape(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glOrtho(0.0, 640, 480, 0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int main(int argc, char *argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);

	main_window = glutCreateWindow("Degree Elevation");
	glutDisplayFunc(Screen);
	glutReshapeFunc(Reshape);
	glutMouseFunc(Mouse);

	MyGlui();

	glutMainLoop();
}
