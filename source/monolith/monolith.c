
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>

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
	
	
	
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_COLOR_MATERIAL);
	
	glutSwapBuffers(); // the magic actually happens here....
}

int main(int argc, char *argv[])
{
	return 0;
}
