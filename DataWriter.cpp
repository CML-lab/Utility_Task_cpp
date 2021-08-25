//Modified to add target inputs, 9/10/2012 AW

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "DataWriter.h"


using namespace std;

DataWriter::DataWriter(TargetFrame Target, char* filename)
{

	//record current date/time
	time_t current_time = time(0);
	tm* ltm = localtime(&current_time);
	stringstream ss1,ss2;
	ss1 << setw(4) << setfill('0') << ltm->tm_year + 1900;
	ss1 << setw(2) << setfill('0') << ltm->tm_mon + 1;
	ss1 << setw(2) << setfill('0') << ltm->tm_mday;
	//ss << "_";
	ss2 << setw(2) << setfill('0') << ltm->tm_hour;
	ss2 << setw(2) << setfill('0') << ltm->tm_min;
	ss2 << setw(2) << setfill('0') << ltm->tm_sec;


	// If no filename was supplied, use the date and time
	if (filename == NULL)
	{
		stringstream ss3;
		ss3 << ss1.str() << "_" << ss2.str() << ".txt";
		file.open(ss3.str(), ios::out);
	}
	else
	{
		//append timestamp to prevent overwrites
		stringstream ss3;
		ss3 << ss1.str() << ss2.str() << "_" << filename;
		file.open(ss3.str(), ios::out);
	}

	// Write headers to file
	if (file.is_open())
	{

		//write current date
		file << "Date: " << ss1.str() << endl;
		file << "Time: " << ss2.str() << endl;
		file << "File: " << Target.fname << endl;
		
		file << "--" << endl;
		
		file << "Device_Num "
			<< "SystemTime "
			<< "Trial "
			<< "SureBet "
			<< "Gamble1 "
			<< "Gamble2 "
			<< "Lat "
			<< "Choice "
			<< "Outcome "
			<< "Score "
			<< endl;

		file << "-----" << endl;  //flag designator for finding start of the data stream.  everything above is header

	}
}

DataWriter::~DataWriter()
{
	file.close();
}

void DataWriter::Record(int deviceNo, TargetFrame Target)
{
	// Write data
	if (file.is_open())
	{
		file << deviceNo << " "
			<< Target.time << " "
			<< Target.trial << " "
			<< Target.surebet << " "
			<< Target.gamble1 << " "
			<< Target.gamble2 << " "
			<< Target.lat << " "
			<< Target.choice << " "
			<< Target.outcome << " "
			<< Target.score
			<< endl;
	}
}