// $Id$

#ifndef __GLCONSOLE_HH__
#define __GLCONSOLE_HH__

#include <memory>
#include "OSDConsoleRenderer.hh"
#include "GLUtil.hh"

using std::auto_ptr;

struct SDL_Surface;

namespace openmsx {

class Console;

class GLConsole : public OSDConsoleRenderer
{
public:
	GLConsole(Console& console);
	virtual ~GLConsole();

	virtual bool loadFont(const string& filename);
	virtual bool loadBackground(const string& filename);
	virtual void drawConsole();
	virtual void updateConsole();

private:
	int powerOfTwo(int a);
	bool loadTexture(const string& filename, GLuint& texture,
			int& width, int& height, GLfloat* texCoord);

	GLuint backgroundTexture;
	auto_ptr<BackgroundSetting> backgroundSetting;
	auto_ptr<FontSetting> fontSetting;
	GLfloat backTexCoord[4];
	int consoleWidth;
	int consoleHeight;
	int dispX;
	int dispY;
	Console& console;
	void updateConsoleRect(SDL_Surface* screen);
};

} // namespace openmsx

#endif // __GLCONSOLE_HH__
