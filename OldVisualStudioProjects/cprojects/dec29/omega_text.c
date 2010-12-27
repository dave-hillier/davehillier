#include "omega_common.h"
#include "omega_gl.h"
#include <math.h>


float tPoints[257][2];// where are all the characters ??
int		font_texture;
int		blockCol = 16, blockRow = 16;
int		fontTabSpace = 4;
float	fontITOF = 1.0f/255.0f;
int		fontItalic = 8;

static float white[4] = {1.0, 1.0, 1.0, 1.0};
static float gray[4] = {0.5, 0.5, 0.5, 1.0};

float fgColor[4] = {1.0, 1.0, 1.0, 1.0};
float bgColor[4] = {0.5, 0.5, 0.5, 1.0};
float gdColor[4] = {0.5, 0.5, 0.5, 1.0};
int gradient = 0, italic = 0, bold = 0, shadow = 0, size=12;
GLfloat tIncX;
GLfloat tIncY;

void TextMap ()
{
#define vCopy(d,x,y) {d[0] = x; d[1] = y;}

   GLint i = 0;
   GLfloat x, y;

   tIncX = (GLfloat)pow (blockCol, -1);
   tIncY = (GLfloat)pow (blockRow, -1);


   for (y = 1 - tIncY; y >= 0; y -= tIncY)
      for (x = 0; x <= 1 - tIncX; x += tIncX, i ++)
         vCopy(tPoints[i], x, y);
#undef vCopy
	  // Build the points of each character
}

enum { backup, restore };
void TextSetMode(int mode)
{
   static int matrixMode;
   static int polyMode[2];
   static int lightingOn;
   static int blendOn;
   static int depthOn;
   static int textureOn;
   static int scissorOn;
   static int blendSrc;
   static int blendDst;

   /* grab the modes that we might need to change */
   if (mode == backup)
   {
      glGetIntegerv(GL_POLYGON_MODE, polyMode);

      if (polyMode[0] != GL_FILL)
         glPolygonMode(GL_FRONT, GL_FILL);
      if (polyMode[1] != GL_FILL)
         glPolygonMode(GL_BACK, GL_FILL);

      textureOn = glIsEnabled(GL_TEXTURE_2D);
         
      if (!textureOn)
         glEnable(GL_TEXTURE_2D);

      depthOn = glIsEnabled(GL_DEPTH_TEST);
   
      if (depthOn)
         glDisable(GL_DEPTH_TEST);

      lightingOn = glIsEnabled(GL_LIGHTING);        

      if (lightingOn) 
         glDisable(GL_LIGHTING);

      scissorOn= glIsEnabled(GL_SCISSOR_TEST);        

      if (!scissorOn) 
         glEnable(GL_SCISSOR_TEST);

      glGetIntegerv(GL_MATRIX_MODE, &matrixMode); 

      /* i don't know if this is correct */
      blendOn= glIsEnabled(GL_BLEND);        
      glGetIntegerv(GL_BLEND_SRC, &blendSrc);
      glGetIntegerv(GL_BLEND_DST, &blendDst);
   
      if (!blendOn)
         glEnable(GL_BLEND);

      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   }
   else if (mode == restore)
   {
      /* put everything back where it was before */
      if (polyMode[0] != GL_FILL)
         glPolygonMode(GL_FRONT, polyMode[0]);
      if (polyMode[1] != GL_FILL)
         glPolygonMode(GL_BACK, polyMode[1]);

      if (lightingOn)
         glEnable(GL_LIGHTING);

      /* i don't know if this is correct */
      if (!blendOn)
      {
         glDisable(GL_BLEND);
         glBlendFunc(blendSrc, blendDst);
      }
      else glBlendFunc(blendSrc, blendDst);

      if (depthOn)
         glEnable(GL_DEPTH_TEST);

      if (!textureOn)
         glDisable(GL_TEXTURE_2D);

      if (!scissorOn) 
         glDisable(GL_SCISSOR_TEST);

      glMatrixMode(matrixMode);
   }
}

void DrawChar(char c, int x, int y, int size, int shadow)
{
   if (!gradient && !shadow)
      glColor4fv(fgColor);
   else if (!gradient && shadow)
      glColor4fv(bgColor);
   else if (gradient && !shadow)
      glColor4fv(gdColor);

   glBegin (GL_QUADS);
      glTexCoord2f(tPoints[(int) c][0], tPoints[(int) c][1]);
      glVertex2i(x, y);

      glTexCoord2f(tPoints[(int) c][0] + tIncX, tPoints[(int) c][1]);
      glVertex2i(x + size, y);

      if (!shadow)
         glColor4fv(fgColor);
      else 
		  glColor4fv(bgColor);

      glTexCoord2f(tPoints[(int) c][0] + tIncX, tPoints[(int) c][1] + tIncY);
      glVertex2i(x + italic + size, y + size);

      glTexCoord2f(tPoints[(int) c][0], tPoints[(int) c][1] + tIncY);
      glVertex2i(x + italic, y + size);
   glEnd ();
}
void RenderChar (char c, int x, int y, int size)
{
   if (shadow)
   {
      if (!bold)
         DrawChar(c, x + 1,  y + 1, size, 1);
      else DrawChar(c, x + 2,  y + 1, size, 1);
   }

   DrawChar(c, x, y, size, 0);

   if (bold)
      DrawChar(c, x + 1, y, size, 0);
}
void TextInit ()
{
	TextMap ();
}
int SetColorFromToken (char *s)
{
   int clr[4];
   int ret = 1;

   if (*s == 'c')
   {
      s += 1;
      if (sscanf(s, "(%d %d %d)", &clr[0], &clr[1], &clr[2]) != 3)
         return -1;
      fgColor[0] = clr[0] * fontITOF;
	  fgColor[1] = clr[1] * fontITOF;
	  fgColor[2] = clr[2] * fontITOF;
   }
   else if (*s == 'a')
   {
      s += 1;
      if (sscanf(s, "(%d %d %d %d)", &clr[0], &clr[1], &clr[2], &clr[3]) != 4)
         return -1;
      fgColor[0] = clr[0] * fontITOF;
	  fgColor[1] = clr[1] * fontITOF; 
	  fgColor[2] = clr[2] * fontITOF;
	  fgColor[3] = clr[3] * fontITOF;
   }

   while (*s != ')' && ret ++)
      s += 1;

   return ret + 1;
}

int ItalicsMode (char *s)
{
   s += 1;

   if (*s == '+')
      italic = fontItalic;
   else if (*s == '-')
      italic = 0;
   else return -1;

   return 2;
}

int BoldMode (char *s)
{
   s += 1;

   if (*s == '+')
      bold = 1;
   else if (*s == '-')
      bold = 0;
   else return -1;

   return 2;
}
int SlashParser(char *buffPtr, int *x, int *y)
{
   int ret = 0;

   *buffPtr++;

   if (!*buffPtr)
      return ret;

   switch (*buffPtr)
   {
      case 'a':
      case 'c':
         *x -= size;
         return SetColorFromToken(buffPtr);
      break;
      case 'i':
         *x -= size;
         return ItalicsMode(buffPtr);
      break;
      case 'b':
         *x -= size;
         return BoldMode(buffPtr);
      break;
      default:
         *buffPtr--;
         RenderChar(*buffPtr, *x, *y, size);
         return ret;
      break;
   }
}
void WalkString (int xpos, int ypos, char *buffPtr)
{
	int x = xpos, y = ypos;

	for ( ; *buffPtr; *buffPtr++, x += size)
	{	
		switch (*buffPtr)
		{
		case '\n':
			y -= size;
			x = xpos - size;
			continue; 
			break;

		case '\t':
			x += (size * fontTabSpace);
			continue; 
			break;
	    case '\\':
            buffPtr += SlashParser(buffPtr, &x, &y);
            if (*buffPtr == '\n' || *buffPtr == '\t')
            {
               buffPtr -= 1;
               continue;
            }
			break;
// TODO: Colours, bold, italics
		default:
			RenderChar(*buffPtr, x, y, size);
		break;
		}

	}
}
void TextPrint (int x, int y, char *format, ...)
{
	va_list		argptr;
	char		buffer[1024];
	
	va_start (argptr, format);
	vsprintf (buffer, format,argptr);
	va_end (argptr);

	TextSetMode(backup);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, current_vidmode.width, 0, current_vidmode.height, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glBindTexture(GL_TEXTURE_2D, font_texture);

	// draw the string 

	WalkString (x, y, buffer);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();   
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	TextSetMode (restore);

}

	
