
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

/* OPENCL INCLUDES */
#ifndef DARWIN
#include <CL/cl.h>
#else
#include <OpenCL/opencl.h>
#endif

/* OPENCL_ENVIRONMENT */
#include <clenvironment.h>
#ifndef CL_BUILD_RUNTIME
#include <kernel_spacetime.h>
#endif

/* OPENGL INCLUDES */
#ifdef DARWIN
#include <glut.h>
#else
#include <GL/glut.h>
#endif

#define X 0
#define Y 1
#define Z 2
#define W 3
#define R X
#define G Y
#define B Z
#define A W

#define HOST
#include <clspacetime.h>

typedef struct _camera_t {
    cl_double4 				m_from;			/**< position of camera */
    cl_double4 				m_at;         	/**< where camera points */
    cl_double4 				m_up;         	/**< orientation of camera */
    cl_double4 				m_pan;        	/**< Vector to pan to new object */
    double 					m_pan_reps;   	/**< Number of times the pan will be calculated to reach the specified target */
    double 					m_radius;     	/**< distance away from the target */
    double 					m_theta;      	/**< in radians */
    double 					m_phi;        	/**< in radians */
    double 					m_accel;		/**< */
    int32_t 				m_state;		/**< */
	float					m_ratio;		/**< ratio of projection width to height */
	float 					m_fov;			/**< degrees of field of view */
	float					m_near;			/**< near clipping plane */	
	float					m_far;			/**< far clipping plane */
} camera_t;

typedef struct _light_t {
	GLint					m_light;
	cl_body_t				m_body;
	cl_float4				m_ambient;
	cl_float4				m_diffuse;
	cl_float4				m_specular;
} light_t;

typedef struct _spacetime_t {
	cl_environment_t       *m_pEnv;
	cl_spacetime_grid_t     m_grid;
	cl_body_t 			   	m_body;	
	float					m_fps;
	camera_t				m_camera;
	light_t					m_lights[1];
	int32_t 				m_width;
	int32_t 				m_height;
	int32_t 				m_depthmax;
} spacetime_t;

// file local variable
static spacetime_t *spacetime;

void glut_display(void)
{
	GLenum err = 0;
	printf("Draw Called!\n");
	// clear the fb
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glScalef(1.0,1.0,1.0);
    //glDepthRange(-1.0,1.0);

	// place the camera
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(spacetime->m_camera.m_fov,spacetime->m_camera.m_ratio, spacetime->m_camera.m_near, spacetime->m_camera.m_far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(spacetime->m_camera.m_from[X],spacetime->m_camera.m_from[Y],spacetime->m_camera.m_from[Z],
              spacetime->m_camera.m_at[X],  spacetime->m_camera.m_at[Y],  spacetime->m_camera.m_at[Z],
              spacetime->m_camera.m_up[X],  spacetime->m_camera.m_up[Y],  spacetime->m_camera.m_up[Z]);

	glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_DEPTH_TEST);

	glLineWidth(1.0f);
	glEnable(GL_POINT_SMOOTH);
    glColor4ub(255,255,255,0);
    err = glGetError();
    if (err) 
		printf("Error before glBegin/glEnd\n");
	glBegin(GL_POINTS);
	for (int z = 0; z < GRID_DIM; z++) {
		for (int y = 0; y < GRID_DIM; y++) {
			for (int x = 0; x < GRID_DIM; x++) {
				uint32_t i = (z*GRID_DIM*GRID_DIM) + (y*GRID_DIM) + (x);
				cl_float4 position;
				memcpy(position, spacetime->m_grid.m_position[i], sizeof(cl_float4));
				glVertex3f(position[X],position[Y],position[Z]);
			}
		}
	}
	glEnd();
	glDisable(GL_LINE_SMOOTH);

	// now turn on the lights...
    glEnable(GL_LIGHTING);
	for (int i = 0; i < dimof(spacetime->m_lights); i++)
		glEnable(spacetime->m_lights[i].m_light);
	
    //draw the Mass
    glColor3ub(100,106,187);
    glPushMatrix();
    glTranslated(spacetime->m_body.m_position[X],spacetime->m_body.m_position[Y],spacetime->m_body.m_position[Z]);
    glutSolidSphere(spacetime->m_body.m_radius,20,20);
    glPopMatrix();
		
	for (int i = 0; i < dimof(spacetime->m_lights); i++)
		glDisable(spacetime->m_lights[i].m_light);
	glDisable(GL_LIGHTING);
	
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
	
	glutSwapBuffers(); // the magic actually happens here....
}

void glut_reshape(int width, int height)
{
	printf("reshape(%d,%d) called\n",width,height);
	spacetime->m_width = width;
	spacetime->m_height = height;
	glViewport(0, 0, width, height);
	spacetime->m_camera.m_ratio = (float)width/height;
	// place the camera
	glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(spacetime->m_camera.m_fov,spacetime->m_camera.m_ratio, spacetime->m_camera.m_near, spacetime->m_camera.m_far);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(spacetime->m_camera.m_from[X],spacetime->m_camera.m_from[Y],spacetime->m_camera.m_from[Z],
              spacetime->m_camera.m_at[X],  spacetime->m_camera.m_at[Y],  spacetime->m_camera.m_at[Z],
              spacetime->m_camera.m_up[X],  spacetime->m_camera.m_up[Y],  spacetime->m_camera.m_up[Z]);
}

void glut_passive(int x, int y)
{
	//printf("mouse over %d,%d\n",x,y);
}

void ocl_notify(cl_program program, void *arg)
{
    printf("Program %p Arg %p\n",program, arg);
}

cl_int kernel_spacetime(cl_environment_t *pEnv,
                	   cl_spacetime_grid_t *grid,
					   cl_body_t *body)
{
    cl_int err;
	cl_float distance[GRID_SIZE];
	
    cl_kernel_param_t params[] = {
		{CL_KPARAM_BUFFER_1D, sizeof(distance), distance, NULL, CL_MEM_WRITE_ONLY},
        {CL_KPARAM_BUFFER_1D, sizeof(cl_float4)*GRID_SIZE, grid->m_position, NULL, CL_MEM_READ_WRITE},
		{CL_KPARAM_BUFFER_1D, sizeof(cl_float4), &body->m_position, NULL, CL_MEM_READ_ONLY},
    };
    cl_kernel_call_t call = {
        "kernel_distance",
        params, dimof(params),
		1, 
		{0, 0, 0},
        {GRID_SIZE, 0, 0},
        {1, 1, 1},
        CL_SUCCESS
    };

    err = clCallKernel(pEnv, &call, 1);
    if (err != CL_SUCCESS)
        return err;
    else
        return call.err;
}

void glut_timer(int value)
{
	GLenum err = glGetError();
    const GLubyte *pErr = gluErrorString(err);
	float distance[GRID_SIZE];

	printf("timer fired %d! (err=%d, %s)\n", value, err, pErr);
	fflush(stdout);
	
	// move the body to the new position
	spacetime->m_body.m_position[X] += spacetime->m_body.m_velocity[X];
	spacetime->m_body.m_position[Y] += spacetime->m_body.m_velocity[Y];
	spacetime->m_body.m_position[Z] += spacetime->m_body.m_velocity[Z];
	// reset the body if it moves out of the view
	if (spacetime->m_body.m_position[X] > GRID_DIM/2)
		spacetime->m_body.m_position[X] = -GRID_DIM/2;
		
	// call the OpenCL Kernels to compute the gravity distortion
	kernel_spacetime(spacetime->m_pEnv, &spacetime->m_grid, &spacetime->m_body);
	
	// move the camera to the new position
    //take the "at" location and add the sphereical translated coordinates
    spacetime->m_camera.m_from[X] = spacetime->m_camera.m_at[X] + 
		spacetime->m_camera.m_radius*cos(M_PI/2 - spacetime->m_camera.m_phi)*cos(spacetime->m_camera.m_theta);
    spacetime->m_camera.m_from[Y] = spacetime->m_camera.m_at[Y] + 
		spacetime->m_camera.m_radius*cos(M_PI/2 - spacetime->m_camera.m_phi)*sin(spacetime->m_camera.m_theta);
    spacetime->m_camera.m_from[Z] = spacetime->m_camera.m_at[Z] + spacetime->m_camera.m_radius*cos(spacetime->m_camera.m_phi);
	
	printf("camera from={%lf,%lf,%lf} at={%lf,%lf,%lf}\n",
		spacetime->m_camera.m_from[X],
		spacetime->m_camera.m_from[Y],
		spacetime->m_camera.m_from[Z],
		spacetime->m_camera.m_at[X],
		spacetime->m_camera.m_at[Y],
		spacetime->m_camera.m_at[Z]);
	
	// tell glut to call display after this function.
	glutPostRedisplay(); 
	
	// reschedule this timer 
    glutTimerFunc(1000/spacetime->m_fps,glut_timer, value+1);
}

int main(int argc, char *argv[])
{	
    spacetime = (spacetime_t *)calloc(1, sizeof(spacetime_t));
	if (spacetime)
	{
	    char title[80];

		// initialize the spacetime info
		spacetime->m_fps = 30;
		spacetime->m_body.m_mass = pow(7.32f,22.0f); // the mass of the moon in kg
		spacetime->m_body.m_radius = 2.0f;
		spacetime->m_body.m_position[X] = -GRID_DIM/2;
		spacetime->m_body.m_velocity[X] = 1;
		spacetime->m_camera.m_radius = 20.0f;
		spacetime->m_camera.m_theta = M_PI/2;
		spacetime->m_camera.m_phi = M_PI/2;
		spacetime->m_camera.m_up[Z] = 1.0f;
		spacetime->m_width = 640;
		spacetime->m_height = 480;
		spacetime->m_camera.m_fov = 40.0f;
		spacetime->m_camera.m_ratio = (float)spacetime->m_width/spacetime->m_height;
		spacetime->m_camera.m_near = 1.0f;
		spacetime->m_camera.m_far = 7000.0f; 
		// initialize the grid points (from -GRID_DIM/2 to GRID_DIM/2)
		for (int iz = 0; iz < GRID_DIM; iz++) {
			for (int iy = 0; iy < GRID_DIM; iy++) {
				for (int ix = 0; ix < GRID_DIM; ix++) {
					uint32_t i = (iz*GRID_DIM*GRID_DIM) + (iy*GRID_DIM) + (ix);
					spacetime->m_grid.m_position[i][X] = (cl_float)(ix - GRID_DIM/2);
					spacetime->m_grid.m_position[i][Y] = (cl_float)(iy - GRID_DIM/2);
					spacetime->m_grid.m_position[i][Z] = (cl_float)(iz - GRID_DIM/2);
				}
			}
		}
		
#ifdef CL_BUILD_RUNTIME
		spacetime->m_pEnv = clCreateEnvironment(KDIR "kernel_spacetime.cl", CL_DEVICE_TYPE_GPU, 1, ocl_notify, CL_ARGS);
#else	
	    spacetime->m_pEnv = clCreateEnvironmentFromBins(&gKernelBins, ocl_notify, CL_ARGS);
#endif
		if (!spacetime->m_pEnv)
		{
			printf("OpenCL Environment failure!\n");
			free(spacetime);
			return -1;
		}
		
		
		sprintf(title,"Space-Time Warping Due to Mass; Simulation by Erik Rainey");
		glutInitDisplayMode(GLUT_RGBA|GLUT_DEPTH|GLUT_DOUBLE|GLUT_MULTISAMPLE);
	    glutInitWindowPosition(100, 100);
	    glutInitWindowSize(spacetime->m_width, spacetime->m_height);
	    glutInit(&argc, argv);
	    glutCreateWindow(title);
	
		// initialize the display settings
		glEnable(GL_CULL_FACE);
		glClearColor(0,0,0,0); // set the background color to the blackness of space ... !
		
		// initialize each light
		for (int i = 0; i < dimof(spacetime->m_lights); i++) {
			spacetime->m_lights[i].m_ambient[A] = 1.0f;
			spacetime->m_lights[i].m_diffuse[R] = 1.0f;
			spacetime->m_lights[i].m_diffuse[G] = 1.0f;
			spacetime->m_lights[i].m_diffuse[B] = 1.0f;
			spacetime->m_lights[i].m_diffuse[A] = 1.0f;
			spacetime->m_lights[i].m_body.m_position[X] = 20.0f;
			spacetime->m_lights[i].m_body.m_position[Y] = 20.0f;
			spacetime->m_lights[i].m_body.m_position[Z] = 20.0f;
			spacetime->m_lights[i].m_light = GL_LIGHT0 + i;
			glLightfv(spacetime->m_lights[i].m_light, GL_AMBIENT, spacetime->m_lights[i].m_ambient);
		    glLightfv(spacetime->m_lights[i].m_light, GL_DIFFUSE, spacetime->m_lights[i].m_diffuse);
		    glLightfv(spacetime->m_lights[i].m_light, GL_POSITION, spacetime->m_lights[i].m_body.m_position);
		}
		// install the hooks
		glutDisplayFunc(glut_display);
		glutReshapeFunc(glut_reshape);
		glutIdleFunc(NULL);
		glutTimerFunc(1000/spacetime->m_fps,glut_timer,0); // give initial count as zero
		glutPassiveMotionFunc(glut_passive);
		
		// install the main loop, I don't think we return from this until killed
		glutMainLoop();
		
		free(spacetime);
		clDeleteEnvironment(spacetime->m_pEnv);
	}
	return 0;
}
