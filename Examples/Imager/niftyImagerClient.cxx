/*=========================================================================

  Program:   Open IGT Link -- Example for Imager Client Program
  Module:    $RCSfile: $
  Language:  C++
  Date:      $Date: $
  Version:   $Revision: $

  Copyright (c) Insight Software Consortium. All rights reserved.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstdio>

#include "igtlOSUtil.h"
#include "igtlImageMessage.h"
#include "igtlTrackingDataMessage.h"
#include "igtlStringMessage.h"
#include "igtlClientSocket.h"

int GetTestImage(igtl::ImageMessage::Pointer& msg, const char* dir, int i);
int GetTestMatrix(igtl::TrackingDataElement::Pointer& msg, const char* dir, int i);
void SetMotorPosition (igtl::Matrix4x4& matrix, float pos = 47.25);
// Modified imager client, designed as a command line mimic of 
// an ultrasonix imager client
enum {
	IMAGE_MESSAGE = 0 ,
	TRACKING_MESSAGE = 1
};
void usage ( char * argv[]) 
{
    // If not correct, print usage
    std::cerr << "Usage: " << argv[0]  << std::endl;
    std::cerr << "    --hostname <hostname>  : IP or host name, default to localhost"                    << std::endl;
    std::cerr << "    --port <port>          : Port # (3200 in Slicer default)"   << std::endl;
    std::cerr << "    --fps  <fps>           : Frequency (fps) to send coordinate [1]" << std::endl;
    std::cerr << "    --filedir <filedir>    : file directory, where \"igtlTestImage[1-5].raw\" are placed." << std::endl;
    std::cerr << "                             (usually, in the Examples/Imager/img directory.)" << std::endl;
    std::cerr << "    --repeats <repeats>    : Number of messages to send [20]" << std::endl;
    std::cerr << "    --messagetype <type>   : Type of message to send (image or trackingdata)" << std::endl;
    exit(0);
}
int main(int argc, char** argv)
{


  char hostname[255];
  int    port     = 3200;
  double fps      = 1;
  char  filedir[512];
	int    messagetype = IMAGE_MESSAGE;
	int    repeats = 20;
	strcpy ( hostname , "localhost"); 
	strcpy (filedir,  "../Examples/Imager/img/");																
	bool filedirSet=false;
	while ( argc > 1 ) 
	{
		bool ok = false; 
	  if ((ok ==false) && (strcmp(argv[1], "--hostname" ) == 0 ) )
		{
			argc--;
			argv++;
			strcpy(hostname,argv[1]);
			argc--;
			argv++;
			ok=true;
		}
		if ((ok ==false) && (strcmp(argv[1], "--port" ) == 0 ) )
		{
			argc--;
			argv++;
			port = atoi (argv[1]);
			argc--;
			argv++;
			ok=true;
		}
		if ((ok ==false) && (strcmp(argv[1], "--fps" ) == 0 ) )
		{
			argc--;
			argv++;
			fps = strtod (argv[1],NULL);
			argc--;
			argv++;
			ok=true;
		}
   if ((ok ==false) && (strcmp(argv[1], "--filedir" ) == 0 ) )
		{
			argc--;
			argv++;
			strcpy(filedir,argv[1]);
			filedirSet = true;
			argc--;
			argv++;
			ok=true;
		}
		if ((ok ==false) && (strcmp(argv[1], "--repeats" ) == 0 ) )
		{
			argc--;
			argv++;
			repeats = atoi (argv[1]);
			argc--;
			argv++;
			ok=true;
		}
	  if ((ok ==false) && (strcmp(argv[1], "--messagetype" ) == 0 ) )
		{
			argc--;
			argv++;
			if ( strcmp ( argv[1],"image" ) == 0 )
				messagetype=IMAGE_MESSAGE;
			if ( strcmp ( argv[1],"trackingdata" ) == 0 ) 
				messagetype=TRACKING_MESSAGE;
			argc--;
			argv++;
			ok=true;
		}
		if ( ok == false ) 
			usage (argv) ;
	}

	if ( ! filedirSet && ( messagetype == TRACKING_MESSAGE ) )
		strcpy (filedir,  "../Examples/Imager/matrices/");																

		
  int    interval = (int) (1000.0 / fps);
  //------------------------------------------------------------
  // Establish Connection
  igtl::ClientSocket::Pointer socket;
  socket = igtl::ClientSocket::New();
  int r = socket->ConnectToServer(hostname, port);

  if (r != 0)
    {
    std::cerr << "Cannot connect to the server." << std::endl;
    exit(0);
    }

	// Create a string message to initiate the connection
	igtl::StringMessage::Pointer infoMsg = igtl::StringMessage::New();
	switch ( messagetype ) 
	{
		case IMAGE_MESSAGE:
			infoMsg->SetString("<ClientDescriptor><Device CommunicationType=\"Porta Dummy\" DeviceName=\"Ultrasonix\" PortName=\"\" DeviceType=\"Imager\"/><Client ClientIP=\"UNKNOWN\" ClientPort=\"3200\"/></ClientDescriptor>");
			break;
		case TRACKING_MESSAGE:
			infoMsg->SetString("<ClientDescriptor><Device CommunicationType=\"Serial\" DeviceName=\"NDI Polaris Vicra\" PortName=\"\" DeviceType=\"Tracker\"/><Client ClientIP=\"UNKNOWN\" ClientPort=\"3200\"/></ClientDescriptor>");
			break;
		default:
			infoMsg->SetString("<ClientDescriptor><Device CommunicationType=\"UNKNOWN\" DeviceName=\"UNKNOWN\" PortName=\"\" DeviceType=\"UNKNOWN\"/><Client ClientIP=\"UNKNOWN\" ClientPort=\"3200\"/></ClientDescriptor>");
	}

  infoMsg->Pack();
  socket->Send(infoMsg->GetPackPointer(), infoMsg->GetPackSize());
  //------------------------------------------------------------
  // loop
  for (int i = 0; i < repeats; i ++)
    {

		switch ( messagetype ) 
		{
		case IMAGE_MESSAGE:
			{	//------------------------------------------------------------
			// size parameters
			int   size[]     = {256, 256, 1};       // image dimension
			float spacing[]  = {1.0, 1.0, 5.0};     // spacing (mm/pixel)
			int   svsize[]   = {256, 256, 1};       // sub-volume size
			int   svoffset[] = {0, 0, 0};           // sub-volume offset
			int   scalarType = igtl::ImageMessage::TYPE_UINT8;// scalar type

			//------------------------------------------------------------
			// Create a new IMAGE type message
			igtl::ImageMessage::Pointer imgMsg = igtl::ImageMessage::New();
			imgMsg->SetDimensions(size);
			imgMsg->SetSpacing(spacing);
			imgMsg->SetScalarType(scalarType);
			imgMsg->SetDeviceName("ImagerClient");
			imgMsg->SetSubVolume(svsize, svoffset);
			imgMsg->AllocateScalars();

			//------------------------------------------------------------
			// Set image data (See GetTestImage() bellow for the details)
			GetTestImage(imgMsg, filedir, i % 5);

			//------------------------------------------------------------
			// Get random orientation matrix and set it.
			igtl::Matrix4x4 matrix;
			SetMotorPosition(matrix);
			imgMsg->SetMatrix(matrix);

			//------------------------------------------------------------
			// Pack (serialize) and send
			imgMsg->Pack();
			socket->Send(imgMsg->GetPackPointer(), imgMsg->GetPackSize());
			break;
			}
		case TRACKING_MESSAGE:
			{
			igtl::TrackingDataMessage::Pointer trkMsg = igtl::TrackingDataMessage::New();
			igtl::TrackingDataElement::Pointer trkEl = igtl::TrackingDataElement::New();
			
			GetTestMatrix ( trkEl, filedir, i % 1000 );
			trkEl->SetName("8700338.rom");
			trkEl->SetType(3);
			trkMsg->AddTrackingDataElement(trkEl);

			trkMsg->Pack();
			socket->Send(trkMsg->GetPackPointer(),trkMsg->GetPackSize());
			break;
			}
		}


    igtl::Sleep(interval); // wait

    }

  //------------------------------------------------------------
  // Close connection
  socket->CloseSocket();

}


//------------------------------------------------------------
// Function to read test image data
int GetTestImage(igtl::ImageMessage::Pointer& msg, const char* dir, int i)
{

  //------------------------------------------------------------
  // Check if image index is in the range
  if (i < 0 || i >= 5) 
    {
    std::cerr << "Image index is invalid." << std::endl;
    return 0;
    }

  //------------------------------------------------------------
  // Generate path to the raw image file
  char filename[128];
  sprintf(filename, "%s/igtlTestImage%d.raw", dir, i+1);
  std::cerr << "Reading " << filename << "...";

  //------------------------------------------------------------
  // Load raw data from the file
  FILE *fp = fopen(filename, "rb");
  if (fp == NULL)
    {
    std::cerr << "File opening error: " << filename << std::endl;
    return 0;
    }
  int fsize = msg->GetImageSize();
  size_t b = fread(msg->GetScalarPointer(), 1, fsize, fp);

  fclose(fp);

  std::cerr << "done." << std::endl;

  return 1;
}
int GetTestMatrix(igtl::TrackingDataElement::Pointer& msg, const char* dir, int i)
{

  //------------------------------------------------------------
  // Check if image index is in the range
  if (i < 0 || i >= 999) 
    {
    std::cerr << "Matrix index is invalid." << std::endl;
    return 0;
    }

  //------------------------------------------------------------
  // Generate path to the raw image file
  char filename[128];
  sprintf(filename, "%s/igtlTestMatrix%04d.matrix", dir, i+1);
  std::cerr << "Reading " << filename << "...";

  //------------------------------------------------------------
  // Load raw data from the file
  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
    {
    std::cerr << "File opening error: " << filename << std::endl;
    return 0;
    }
	igtl::Matrix4x4 matrix;
		for ( int row = 0 ; row < 4 ; row ++)
	{
		for ( int col = 0 ; col < 4 ; col ++)
			fscanf(fp , "%f", &matrix[row][col]);
	}

  fclose(fp);
	msg->SetMatrix(matrix);	

  std::cerr << "done." << std::endl;

  return 1;
}


//------------------------------------------------------------
// Function to generate random matrix.
void SetMotorPosition(igtl::Matrix4x4& matrix, float pos)
{
  matrix[0][0] = 1.0;  matrix[1][0] = 0.0;  matrix[2][0] = 0.0; matrix[3][0] = 0.0;
  matrix[0][1] = 0.0;  matrix[1][1] = 1.0;  matrix[2][1] = 0.0; matrix[3][1] = 0.0;
  matrix[0][2] = 0.0;  matrix[1][2] = 0.0;  matrix[2][2] = 1.0; matrix[3][2] = 0.0;
  matrix[0][3] = pos;  matrix[1][3] = 0.0;  matrix[2][3] = 0.0; matrix[3][3] = 1.0;
  
  igtl::PrintMatrix(matrix);
}

