/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0 - User Viewer Sample                                   *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#ifndef _NITE_USER_VIEWER_H_
#define _NITE_USER_VIEWER_H_

#include "NiTE.h"

#include <armadillo>
#include "settings.h"
#include <deque>
#include "predictor.h"

#define MAX_DEPTH 10000

class SampleViewer
{
public:
	SampleViewer(const char* strSampleName);
	virtual ~SampleViewer();

	virtual openni::Status Init(int argc, char **argv);
	virtual openni::Status Run();	//Does not return

    std::deque<arma::mat> pose_stream; // store the poses
    void processPose(nite::UserTracker* pUserTracker, const nite::UserData& userData);
    Predictor predictor;

    bool should_record; // the status of mouse press down
    bool last_should_record; // the last status, if the status changes , we should so something
    std::deque<arma::mat> pose_sequence; // store the pose sequence, write to disk if needed
    int n_recorded;
    void savePoseSequenceToDisk();
    void mouseEventCallback(int button, int state, int x, int y);
    static void glutMouseEventCallback(int button, int state, int x, int y);


protected:
	virtual void Display();
	virtual void DisplayPostDraw(){};	// Overload to draw over the screen image

	virtual void OnKey(unsigned char key, int x, int y);

	virtual openni::Status InitOpenGL(int argc, char **argv);
	void InitOpenGLHooks();

	void Finalize();

private:
	SampleViewer(const SampleViewer&);
	SampleViewer& operator=(SampleViewer&);

	static SampleViewer* ms_self;
	static void glutIdle();
	static void glutDisplay();
	static void glutKeyboard(unsigned char key, int x, int y);

	float				m_pDepthHist[MAX_DEPTH];
	char			m_strSampleName[ONI_MAX_STR];
	openni::RGB888Pixel*		m_pTexMap;
	unsigned int		m_nTexMapX;
	unsigned int		m_nTexMapY;

	openni::Device		m_device;
	nite::UserTracker* m_pUserTracker;

	nite::UserId m_poseUser;
	uint64_t m_poseTime;
};


#endif // _NITE_USER_VIEWER_H_
