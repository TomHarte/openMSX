// $Id$

#ifndef __SDLFONT_HH__
#define __SDLFONT_HH__

#include "Font.hh"

struct SDL_Surface;


namespace openmsx {

class File;


class SDLFont : public Font
{
public:
	SDLFont(File* file, SDL_Surface* surface);
	virtual ~SDLFont();

	virtual void drawText(const string& string, int x, int y);

private:
	SDL_Surface* fontSurface;
	SDL_Surface* drawSurface;
};

} // namespace openmsx

#endif // __SDLFONT_HH__
