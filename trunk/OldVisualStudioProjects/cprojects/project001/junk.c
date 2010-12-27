// Put junk in here but stuff that may be worth keeping
	/*
	// Display a dialog box

	if (hwnd_dialog)
	{
		if (GetWindowRect (hwnd_dialog, &rect))
		{
			if (rect.left > (rect.top * 2))
			{
				SetWindowPos (hwnd_dialog, 0,
					(rect.left / 2) - ((rect.right - rect.left) / 2),
					rect.top, 0, 0,
					SWP_NOZORDER | SWP_NOSIZE);
			}
		}

		ShowWindow (hwnd_dialog, SW_SHOWDEFAULT);
		UpdateWindow (hwnd_dialog);
		SetForegroundWindow (hwnd_dialog);
	}*/

/*
#define NUM_PARTICLES 50

void draw (void)
{
	int i;

	rotation += 0.01;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); 
	glPointSize(10);
	glBegin(GL_POINTS); 
	{
		GLfloat x,y, r=1, g=0, b=0, inc=0;
		inc = (float)6/NUM_PARTICLES;

		for (i=0; i < NUM_PARTICLES; i++)
		{
			x = cos(rotation + ((float)i/NUM_PARTICLES) * (M_PI*2))*75;
			y = sin(rotation + ((float)i/NUM_PARTICLES) * (M_PI*2))*75;

			glColor3f(r, g, b);
			if (r >= 1 && g < 1 && b <= 0)
			{
				r = 1;
				g += inc;// need to figure out
			}
			if (g >= 1 && r > 0 && b < 1)
			{
				r -= inc;
				g = 1;
			}
			if (g >= 1 && r <= 0 && b < 1)
			{
				r = 0;
				b += inc;
				g = 1;
			}
			if (b >= 1 && g > 0 && r <= 0)
			{
				r = 0;
				g -= inc;
				b = 1;
			}
			if (b >= 1 && r < 1 && g <= 0)
			{
				r += inc;
				g = 0;
				b = 1;
			}
			if (b > 0 && r >= 1 && g <= 0)
			{
				r = 1;
				g = 0;
				b -= inc;
			}
			glVertex3f(x, cos(i + rotation)*10, y);
		}
	}

	glEnd();
	glFlush();

	SwapBuffers(maindc);
}*/