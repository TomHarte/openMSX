// $Id$

#ifndef __MSXMAPPERIO_HH__
#define __MSXMAPPERIO_HH__

#include <iostream>
#include <fstream>
#include <string>
#include "emutime.hh"
#include "MSXDevice.hh"

class MSXMapperIODevice
{
	public:
		virtual byte convert(byte value) = 0;
		virtual void registerMapper(int blocks) = 0;
};

class MSXMapperIO : public MSXDevice
{
	public:
		~MSXMapperIO();
		static MSXMapperIO *instance();
		
		byte readIO(byte port, Emutime &time);
		void writeIO(byte port, byte value, Emutime &time);
		
		void init();
		void reset();
		
		//void saveState(std::ofstream &writestream);
		//void restoreState(std::string &devicestring, std::ifstream &readstream);

		void registerMapper(int blocks);
		byte getPageNum(int page);
	
	private:
		MSXMapperIO();
		static MSXMapperIO *oneInstance;

		MSXMapperIODevice *device;
		byte pageNum[4];
};

#endif //__MSXMAPPERIO_HH__

