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
double max_radius = 20, cur_radius = 0, cur_square_side = 30;

double wheel_angle_x = 30, bar_angle_z = 30;
double wheel_radius = 20, wheel_width = 10;
double perimeter = 2 * pi * wheel_radius;
double increment = perimeter / 25;
double angle_increment = 360 / 25;
double rotated_angle = 0;

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
struct point wheel_pos(5, 3, 0);
struct point initial_pos(5, 3, 0);

double dist2D(struct point x, struct point y)    {return sqrt((x.x - y.x) * (x.x - y.x) + (x.y - y .y) * (x.y - y.y));}

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
	if(true)
	{
		glColor3f(0.5, 0.5, 0.5);	//grey
		glBegin(GL_LINES);{
			for(i=-10;i<=10;i++){

				//if(i==0)
					//continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, 120, 0);
				glVertex3f(i*10,  -120, 0);

				//lines parallel to X-axis
				glVertex3f(-120,i *10, 0);
				glVertex3f( 120,i * 10, 0);
			}
		}glEnd();
	}
}

void drawRectangle(double length, double width)
{
    length /= 2;
    width /= 2;
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( length, width, 0);
		glVertex3f( -length, width, 0);
		glVertex3f(-length,-width,0);
		glVertex3f(length, -width,0);
	}glEnd();
}



void drawCylinder(double radius, double height, int slices, int stacks)
{
    struct point points[100][100];

    for (int i = 0; i <= stacks; i++)
    {
        double stack_height = height * sin((double) i / stacks * (pi / 2));
        for (int j = 0; j <= slices; j++)
        {
            points[i][j].x=radius*cos(((double)j/(double)slices)* (pi * 2));
			points[i][j].y=radius*sin(((double)j/(double)slices)* (pi * 2));
			points[i][j].z=stack_height - height / 2;
        }
    }

    for(int i=0;i<stacks;i++)
	{
		for(int j=0;j<slices;j++)
        {
            //printf("%d\n", start_color);



            glColor3f(j * 1.0 / slices, 1 - j* 1.0 / slices, 0);


			glBegin(GL_QUADS);{


                //glColor3f(1, 0, 1);
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                //glColor3f(1 - start_color / 360.0, start_color / 360.0, 0);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				 //glColor3f(1, 0, 1);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);

				 ///glColor3f(1 - start_color / 360.0, start_color / 360.0, 0);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawWheel()
{
    glColor3f(0, 1, 1);

    glPushMatrix();
    glTranslatef(wheel_pos.x, wheel_pos.y, wheel_radius);
    glRotatef(wheel_angle_x, 0, 0, 1);
    glRotatef(bar_angle_z, 0, 1, 0);
    drawRectangle(wheel_radius * 2, wheel_width / 2);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(wheel_pos.x, wheel_pos.y, wheel_radius);
    glRotatef(wheel_angle_x, 0, 0, 1);
    glRotatef(bar_angle_z + 90, 0, 1, 0);
    drawRectangle(wheel_radius * 2, wheel_width / 2);
    glPopMatrix();

    glColor3f(1, 0, 1);
    glPushMatrix();

    glTranslatef(wheel_pos.x, wheel_pos.y, wheel_radius);
    glRotatef(wheel_angle_x, 0, 0, 1);
    glRotatef(90, 1, 0, 0);
    glRotatef(-rotated_angle, 0, 0, 1);
    drawCylinder(wheel_radius, wheel_width, 90, 90);

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
        case 'a':
            wheel_angle_x += 5;
            break;
        case 'd':
            wheel_angle_x -= 5;
            break;
        case 'w':

            bar_angle_z += angle_increment;
            wheel_pos.x += increment * cos(deg_to_rad(wheel_angle_x));
            wheel_pos.y += increment * sin(deg_to_rad(wheel_angle_x ));
            rotated_angle += angle_increment;
            if (rotated_angle > 360) rotated_angle -= 360;

            break;
        case 's':
            bar_angle_z -= angle_increment;
            wheel_pos.x -= increment * cos(deg_to_rad(wheel_angle_x));
            wheel_pos.y -= increment * sin(deg_to_rad(wheel_angle_x));

            rotated_angle -= angle_increment;
            if (rotated_angle < 360) rotated_angle += 360;

            break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
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
	gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	//gluLookAt(200,200,200,	0,0,0,	0, 1, 0);
	//gluLookAt(pos.x, pos.y, pos.z,    pos.x + look.x, pos.y + look.y, pos.z + look.z,   up.x, up.y, up.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    drawWheel();



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
