// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#ifdef _DEBUG
#undef _DEBUG
#include <Python.h>
#define _DEBUG
#else
#include <Python.h>
#endif

#include <tchar.h>
#include "stdafx.h"
#include<mutex>

#include <tchar.h>
#include "stdafx.h"
#include <vector>
#include <vector>
#include <libusb.h>
#include <iostream>
#include <thread>
#include <queue>
#include <mutex>
#include <libusb.h>

using namespace std;

//Python test objects

typedef struct measurements measurements;
struct measurements {
	int numSamples;
	UCHAR* samples;
};

extern "C" __declspec(dllimport) void __cdecl pySetup(int VID, int PID, int serialno=NULL);
extern "C" __declspec(dllimport) void __cdecl pyClose();
extern "C" __declspec(dllimport) void __cdecl pyStart(int calTime, int maxTime);
extern "C" __declspec(dllimport) void __cdecl pyStop();
extern "C" __declspec(dllimport) UCHAR*  __cdecl pyGetBulkData(int num_numbers, UCHAR *numbers);
extern "C" __declspec(dllimport) int  __cdecl pyQueueCount();
extern "C" __declspec(dllimport) void  __cdecl pySendCommand(unsigned char operation, int value);
extern "C" __declspec(dllimport) int  __cdecl pyGetValue(unsigned char operation, int length);

// TODO: reference additional headers your program requires here
std::vector<unsigned char> intToBytes(int paramInt);
void sendCommand(libusb_device_handle* handle, unsigned char operation, int value);
int getValue(libusb_device_handle* handle, unsigned char operation, int length);
libusb_device_handle*  openDevice(int VID, int PID,int serialno=NULL);
void closeDevice(libusb_device_handle* handle);
void startSampling(libusb_device_handle* handle, int calTime, int maxTime);
void stopSampling(libusb_device_handle* handle);
void init();
void getBulkData(libusb_device_handle* handle);
int getSamples(UCHAR* processingQueue, int maxTransfer);



class LVPM
{
private:
	mutex QueueAccess;
	libusb_device_handle* handle;
	int VendorID = 0x2AB9;
	int ProductID = 0x0001;
	static const int packetLength = 64; //per firmware spec.
	std::queue<std::vector<UCHAR>> ProcessQueue;
	bool running = false;
	static const int QueueSize = 64000;
	UCHAR Packets[QueueSize]; //Space for up to 1000 packets.
	thread swizzleThread;
	thread processThread;
	//Calibration information
	float mainFineScale = 35946.0;
	float mainCoarseScale = 3103.4;

	float mainFineResistor = 0.050;
	float mainCoarseResistor = 0.050;
	float factoryResistor = 0.050;

	//TODO:  Create a data structure that records the last x Cal values and averages them.
	int mainFineRefCal = 0;
	int mainCoarseRefCal = 0;
	int mainFineZeroCal = 0;
	int mainCoarseZeroCal = 0;

	int totalSampleCount = 0;

public:
	LVPM();
	void setup_usb(int serialno=NULL);
	void setVout(float value);
	void Close();
	void Start(int calTime, int maxTime);
	void Stop();
	void SwizzlePackets(UCHAR* Packets, int numPackets);
	void ProcessPackets();
	void getCalValues();
	bool Calibrated();
	void Enque();
};
