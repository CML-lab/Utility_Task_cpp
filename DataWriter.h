#ifndef DATAWRITER_H
#define DATAWRITER_H
#pragma once

#include <fstream>
#include "TargetFrame.h"
#include "InputFrame.h"

// Records data
class DataWriter
{
private:
	std::ofstream file;
public:
	/* filename defines a custom name to save the file as. If filename is NULL,
	 * a name will be generated automatically.
	 */
	DataWriter(TargetFrame Target, char* filename = NULL);
	~DataWriter();
	// Record a new line of data
	// deviceNo is a number identifying the device sending the data
	// frame is the frame of input data
	void Record(int deviceNo, TargetFrame Target);
};

#endif
