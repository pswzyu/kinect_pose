/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - User Viewer Sample                                   *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#include "Viewer.h"
#include <iostream>
#include "predictor.h"
#include "testpredict.h"

int main(int argc, char** argv)
{
    // normal code

	openni::Status rc = openni::STATUS_OK;

	SampleViewer sampleViewer("User Viewer");

	rc = sampleViewer.Init(argc, argv);
	if (rc != openni::STATUS_OK)
	{
		return 1;
	}
	sampleViewer.Run();


    // testing code
    //TestPredict t;
    //t.test();

    return 0;
}
