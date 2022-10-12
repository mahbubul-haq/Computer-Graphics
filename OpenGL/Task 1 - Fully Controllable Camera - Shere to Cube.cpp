#include<stdio.h>
#include<stdlib.h>
#include<math.h>


#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
int translate_distance = 5;
double rotation_angle = 1;
double max_radius = 30, cur_radius = 0, cur_square_side = 30;

struct point
{
    double x,y,z;
	point()
	{
        x = 0, y = 0, z = 0;
    }
    point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }
    void print()
    {
        printf("%.2lf %.2lf %.2lf\n", x, y, z);
    }
};


struct point pos(0, -100, 0), up(0, 0, 1), right(1, 0, 0), look(0, 1, 0);

void print_camera_position()
{
    pos.print();
    up.print();
    right.print();
    look.print();
}

double deg_to_rad(double degree)
{
    return (pi / 180) * degree;
}

struct point multiply_vector_scalar(struct point &vec, double val)
{
    return point(vec.x * val, vec.y * val, vec.z * val);
}

struct point add_vectors(struct point & vec1, struct point &vec2)
{
    return point(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
}

struct point rotate_(struct point vec, struct point perp, double angle)
{
    struct point vec_cos = multiply_vector_scalar(vec, cos(deg_to_rad(angle)));
    struct point perp_sin = multiply_vector_scalar(perp, sin(deg_to_rad(angle)));
    return add_vectors(vec_cos, perp_sin);
}

void drawAxes()
{
	if(drawaxes==1)
	{

		glBegin(GL_LINES);{
		    glColor3f(1.0,0, 0);
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

            glColor3f(0, 1,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);
			glColor3f(0, 0, 1);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.5, 0.5, 0.5);	//grey
		glBegin(GL_LINES);{
			for(i=-10;i<=10;i++){

				//if(i==0)
					//continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, 0, 120);
				glVertex3f(i*10,  0, -120);

				//lines parallel to X-axis
				glVertex3f(-120, 0,i *10);
				glVertex3f( 120, 0,i * 10);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}

void drawSphere(double radius,int slices,int stacks)
{

	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)* (pi / 2));
			points[i][j].y=r*sin(((double)j/(double)slices)* (pi / 2));
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawCylinder(double radius, double height, int slices, int stacks)
{
    struct point points[100][100];

    for (int i = 0; i <= stacks; i++)
    {
        double stack_height = height * sin((double) i / stacks * (pi / 2));
        for (int j = 0; j <= slices; j++)
        {
            points[i][j].x=radius*cos(((double)j/(double)slices)* (pi / 2));
			points[i][j].y=radius*sin(((double)j/(double)slices)* (pi / 2));
			points[i][j].z=stack_height;
        }
    }

    for(int i=0;i<stacks;i++)
	{
		for(int j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}

void cubeToSphere()
{
    ///squares
    glColor3f(1, 1, 1);

    glPushMatrix();
    glTranslatef(max_radius, 0, 0);
    glRotatef(90, 0, 1, 0);
    drawSquare(cur_square_side);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-max_radius, 0, 0);
    glRotatef(90, 0, 1, 0);
    drawSquare(cur_square_side);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, max_radius, 0);
    glRotatef(90, 1, 0, 0);
    drawSquare(cur_square_side);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -max_radius, 0);
    glRotatef(90, 1, 0, 0);
    drawSquare(cur_square_side);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, max_radius);
    drawSquare(cur_square_side);
    glTranslatef(0, 0, -2 * max_radius);
    drawSquare(cur_square_side);
    glPopMatrix();

    ///cylinders
    glColor3f(0, 1, 0);
    ///vertical
    glPushMatrix();
    glTranslatef(cur_square_side, cur_square_side, -cur_square_side);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-cur_square_side, cur_square_side, -cur_square_side);
    glRotatef(90, 0, 0, 1);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-cur_square_side, -cur_square_side, -cur_square_side);
    glRotatef(180, 0, 0, 1);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(cur_square_side, -cur_square_side, -cur_square_side);
    glRotatef(-90, 0, 0, 1);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();

    ///horizontal

    glPushMatrix();
    glTranslatef(cur_square_side, cur_square_side, cur_square_side);
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();
    glRotatef(90, 1, 0, 0);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(cur_square_side, cur_square_side, -cur_square_side);
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    glRotatef(90, 0, 0, 1);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();
    glRotatef(90, 1, 0, 0);
    glRotatef(-90, 0, 0, 1);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-cur_square_side, -cur_square_side, cur_square_side);
    glRotatef(180, 0, 0, 1);
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();
    glRotatef(90, 1, 0, 0);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();


    glPushMatrix();
    glTranslatef(-cur_square_side, -cur_square_side, -cur_square_side);
    glRotatef(180, 0, 0, 1);
    glPushMatrix();
    glRotatef(-90, 0, 1, 0);
    glRotatef(90, 0, 0, 1);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();
    glRotatef(90, 1, 0, 0);
    glRotatef(-90, 0, 0, 1);
    drawCylinder(cur_radius, 2 * cur_square_side, 90, 90);
    glPopMatrix();


    ///spheres
    glColor3f(1, 0, 0);

    glPushMatrix();
    glTranslatef(cur_square_side, cur_square_side, cur_square_side);
    drawSphere(cur_radius, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-cur_square_side, cur_square_side, cur_square_side);
    glRotatef(90, 0, 0, 1);
    drawSphere(cur_radius, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-cur_square_side, -cur_square_side, cur_square_side);
    glRotatef(180, 0, 0, 1);
    drawSphere(cur_radius, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(cur_square_side, -cur_square_side, cur_square_side);
    glRotatef(-90, 0, 0, 1);
    drawSphere(cur_radius, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glRotatef(180, 1, 0, 0);

    glPushMatrix();
    glTranslatef(cur_square_side, cur_square_side, cur_square_side);
    drawSphere(cur_radius, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-cur_square_side, cur_square_side, cur_square_side);
    glRotatef(90, 0, 0, 1);
    drawSphere(cur_radius, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-cur_square_side, -cur_square_side, cur_square_side);
    glRotatef(180, 0, 0, 1);
    drawSphere(cur_radius, 90, 90);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(cur_square_side, -cur_square_side, cur_square_side);
    glRotatef(-90, 0, 0, 1);
    drawSphere(cur_radius, 90, 90);
    glPopMatrix();

    glPopMatrix();

}



void keyboardListener(unsigned char key, int x,int y){
    struct point temp;
	switch(key){

		case '0':
			drawgrid=1-drawgrid;
			break;
        case '1':
            temp = rotate_(look, multiply_vector_scalar(right, -1), rotation_angle);
            right = rotate_(right, look, rotation_angle);
            look = temp;
            break;
        case '2':
            temp = rotate_(look, right, rotation_angle);
            right = rotate_(right, multiply_vector_scalar(look, -1), rotation_angle);
            look = temp;
            break;
        case '3':
                temp = rotate_(up, multiply_vector_scalar(look, -1), rotation_angle);
                look = rotate_(look, up, rotation_angle);
                up = temp;
            break;
        case '4':
            temp = rotate_(up, look, rotation_angle);
            look = rotate_(look, multiply_vector_scalar(up, -1), rotation_angle);
            up = temp;
            break;
        case '5':
            temp = rotate_(right, multiply_vector_scalar(up, -1), rotation_angle);
            up = rotate_(up, right, rotation_angle);
            right = temp;
            break;
        case '6':
            temp = rotate_(right, up, rotation_angle);
            up = rotate_(up, multiply_vector_scalar(right, -1), rotation_angle);
            right = temp;
            break;
        case 'p':
            print_camera_position();
            break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
		    pos.x -= translate_distance * look.x;
		    pos.y -= translate_distance * look.y;
		    pos.z -= translate_distance * look.z;
			break;
		case GLUT_KEY_UP:		// up arrow key
			pos.x += translate_distance * look.x;
		    pos.y += translate_distance * look.y;
		    pos.z += translate_distance * look.z;
			break;

		case GLUT_KEY_RIGHT:
			pos.x += translate_distance * right.x;
		    pos.y += translate_distance * right.y;
		    pos.z += translate_distance * right.z;
			break;
		case GLUT_KEY_LEFT:
			pos.x -= translate_distance * right.x;
		    pos.y -= translate_distance * right.y;
		    pos.z -= translate_distance * right.z;
			break;

		case GLUT_KEY_PAGE_UP:
		    pos.x += translate_distance * up.x;
		    pos.y += translate_distance * up.y;
		    pos.z += translate_distance * up.z;
			break;
		case GLUT_KEY_PAGE_DOWN:
		    pos.x -= translate_distance * up.x;
		    pos.y -= translate_distance * up.y;
		    pos.z -= translate_distance * up.z;
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
		    cur_square_side = max(0, cur_square_side - 1);
		    cur_radius = min(max_radius, cur_radius + 1);
			break;
		case GLUT_KEY_END:
		    cur_radius = max(0, cur_radius - 1);
		    cur_square_side = min(max_radius, cur_square_side + 1);
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}


void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	//gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(200,200,200,	0,0,0,	0, 1, 0);
	gluLookAt(pos.x, pos.y, pos.z,    pos.x + look.x, pos.y + look.y, pos.z + look.z,   up.x, up.y, up.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

	cubeToSphere();



	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(800, 800);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}
