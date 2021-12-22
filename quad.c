/*
 * Create a window in linux.  Taken from:
 * https://www.khronos.org/opengl/wiki/Programming_OpenGL_in_Linux_GLX_and_Xlib
 *
 *
 * The following comment is taken from https://en.wikipedia.org/wiki/GLX#
 * 
 * GLX consists of three parts:
 * 1. An API that provides OpenGL functions to an XWindow System application
 * 2. An extension of the X protocol, which allows the client (the OpenGL
 * application) to send 3D rendering commands to the X server (the software
 * responsible for the display).  The client and server software may run on 
 * different computers.
 * 3. An extension of the X server that receives the rendering commands from the
 * client and passes them on to the installed OpenGL library.
 *
 * If client and server are running on the same computer and an accelerarted 3D
 * graphics card using a suitable driver is available, the former two components can be
 * bypassed by DRI (Direct Rendering Infrastructure).  In this case, the client
 * application is then allowed to directly access the video hardware through several 
 * API layers.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/X.h>       /* included by X11/Xlib automatically */
#include <X11/Xlib.h>
#include <GL/gl.h>       /* included by X11/Xlib automatically */
#include <GL/glx.h>      /* Manages interaction with X Window System and encodes OpenGL onto the X protocol stream for remote rendering. */
#include <GL/glu.h>      /* the OpenGL utility library.  Provides functions for texture mipmaps, map coords between screen and object space, quadric surfaces and NURBS. */

Display                *dpy;
Window                 root;
GLint                  att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
XVisualInfo            *vi;
Colormap               cmap;
XSetWindowAttributes   swa;
Window                 win;
GLXContext             glc;
XWindowAttributes      gwa;
XEvent                 xev;

void
DrawAQuad()
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1., 1., -1., 1., 1., 20.);	

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0., 0., 10., 0., 0., 0., 0., 1., 0.);

	glBegin(GL_QUADS);
	glColor3f(1., 0., 0.); glVertex3f(-.75, -.75, 0);
	glColor3f(0., 1., 0.); glVertex3f(.75, -.75, 0);
	glColor3f(0., 0., 1.); glVertex3f(.75, .75, 0);
	glColor3f(1., 1., 0.); glVertex3f(-.75, .75, 0);
	glEnd();
}

int
main(int argc, char *argv[])
{

	dpy = XOpenDisplay(NULL);
	
	if(dpy == NULL) {
		printf("\n\tcannot connect to X server \n\n");
		exit(0);
	}

	root = DefaultRootWindow(dpy);

	vi = glXChooseVisual(dpy, 0, att);

	if (vi == NULL) {
		printf("\n\tno appropriate visual found\n\n");
		exit(0);
	} else {
		printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
	}

	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask;

	win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

	XMapWindow(dpy, win);

	XStoreName(dpy, win, "VERY SIMPLE APPLICATION");

	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);

	glEnable(GL_DEPTH_TEST);


	while(1) {
		XNextEvent(dpy, &xev);

		if (xev.type == Expose) {
			XGetWindowAttributes(dpy, win, &gwa);
			glViewport(0, 0, gwa.width, gwa.height);
			DrawAQuad();
			glXSwapBuffers(dpy, win);
		} else if (xev.type == KeyPress) {
			glXMakeCurrent(dpy, None, NULL);
			glXDestroyContext(dpy, glc);
			XDestroyWindow(dpy, win);
			XCloseDisplay(dpy);
			exit(0);
		}
	}
	return EXIT_SUCCESS;
}
