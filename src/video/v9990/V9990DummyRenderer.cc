// $Id$

#include "V9990DummyRenderer.hh"

namespace openmsx {

void V9990DummyRenderer::reset(const EmuTime& /*time*/)
{
}

void V9990DummyRenderer::frameStart(const EmuTime& /*time*/)
{
}

void V9990DummyRenderer::frameEnd(const EmuTime& /*time*/)
{
}

void V9990DummyRenderer::renderUntil(const EmuTime& /*time*/)
{
}

void V9990DummyRenderer::setDisplayMode(V9990DisplayMode /*mode*/,
	                                const EmuTime& /*time*/)
{
}

void V9990DummyRenderer::setColorMode(V9990ColorMode /*mode*/,
	                              const EmuTime& /*time*/)
{
}

void V9990DummyRenderer::updatePalette(
	int /*index*/, byte /*r*/, byte /*g*/, byte /*b*/,
	const EmuTime& /*time*/)
{
}

void V9990DummyRenderer::updateBackgroundColor(
	int /*index*/, const EmuTime& /*time*/)
{
}

void V9990DummyRenderer::setImageWidth(int /*width*/)
{
}

} // namespace openmsx
