/*
 * CameraOpticalFlow.cpp
 *
 *  Created on: Aug 21, 2015
 *      Author: yankai
 */

#include "_3DFlow.h"

#include "../Base/common.h"
#include "stdio.h"

#include "../Base/cv.h"

namespace kai
{

_3DFlow::_3DFlow()
{
	_ThreadBase();

	m_flowMax = 1e9;
	m_flowAvr = 0.0;

	m_pDF = NULL;
	m_pDepth = NULL;

}

_3DFlow::~_3DFlow()
{
}

bool _3DFlow::init(JSON* pJson, string name)
{
	string presetDir = "";
//	CHECK_INFO(pJson->v("PRESET_DIR", &presetDir));
	string labelFile;
//	CHECK_FATAL(pJson->v("3DFLOW_"+name+"_COLOR_FILE", &labelFile));
	double FPS = DEFAULT_FPS;
//	CHECK_INFO(pJson->v("3DFLOW_"+name+"_FPS", &FPS));

	m_labelColor = imread(presetDir+labelFile, 1);
	m_pGpuLUT = cuda::createLookUpTable(m_labelColor);


	m_ROI.width = 100;
	m_ROI.height = 100;
	m_ROI.x = 0;//m_size.width*0.5 - m_ROI.width*0.5;
	m_ROI.y = 0;//m_size.height*0.5 - m_ROI.height*0.5;

	m_pDepth = new Frame();
	m_pSeg = new Frame();

	this->setTargetFPS(FPS);
	return true;
}


bool _3DFlow::start(void)
{
	m_bThreadON = true;
	int retCode = pthread_create(&m_threadID, 0, getUpdateThread, this);
	if (retCode != 0)
	{
		m_bThreadON = false;
		return false;
	}

	return true;
}

void _3DFlow::update(void)
{
	while (m_bThreadON)
	{
		this->autoFPSfrom();

		findDepthGPU();

		this->autoFPSto();
	}

}

void _3DFlow::findDepth(void)
{
	int i, j;
	Point2f vFlow;
	Mat flowMat;
	Mat depthMat;
	double flowDist;
	double flowTot, newMax;

	if(m_pDF == NULL)return;
	if(m_pDF->m_GFlowMat.empty())return;

	m_pDF->m_GFlowMat.download(flowMat);//m_cFlowMat;
	if(flowMat.empty())return;

	if(flowMat.size() != depthMat.size())
	{
		depthMat = Mat(flowMat.size(), CV_8UC1);
	}

	flowTot = 0.0;
	newMax = 0.0;

	m_flowMax = 15;
	double fInterval = m_flowMax/256;
	uchar segCol;

	for (i = 0; i < flowMat.rows; i++)
	{
		for (j = 0; j < flowMat.cols; j++)
		{
			vFlow = flowMat.at<cv::Point2f>(i, j);
			flowDist = (abs(vFlow.x) + abs(vFlow.y));
//			if(flowDist>1e3)flowDist=0.0;
			flowDist = min(m_flowMax, flowDist);

			segCol = (uchar)(flowDist / fInterval);

			depthMat.at<uchar>(i,j) = segCol;//(uchar)(flowDist - m_flowAvr);

			flowTot += flowDist;
			newMax = max(newMax,flowDist);
		}
	}

	m_flowAvr = flowTot / (flowMat.rows * flowMat.cols);
	m_flowMax = newMax;

	m_pDepth->update(&depthMat);

}

void _3DFlow::findDepthGPU(void)
{
	double flowDist;
	double flowTot, newMax;
	GpuMat pFlowGMat[2];
	GpuMat pGMat[2];
	GpuMat* pDFGmat;
	GpuMat depthGMat;
	GpuMat fGMat;
	GpuMat idxGMat;
	GpuMat segGMat;

	if(m_pDF == NULL)return;
	pDFGmat = &m_pDF->m_GFlowMat;
	if(pDFGmat->empty())return;

	cuda::split(*pDFGmat, pFlowGMat);

	if(pDFGmat->size() != depthGMat.size())
	{
		depthGMat = GpuMat(pDFGmat->size(), CV_8UC1);
	}

	flowTot = 0.0;
	newMax = 0.0;

	m_flowMax = 15;
	double fInterval = m_flowMax/256;
	fInterval = 1.0/fInterval;

	cuda::abs(pFlowGMat[0],pGMat[0]);
	cuda::abs(pFlowGMat[1],pGMat[1]);
	cuda::add(pGMat[0],pGMat[1],fGMat);

//	m_flowMax = cuda::sum(fGMat)[0] / (fGMat.cols*fGMat.rows);
//	fInterval = 1.0/m_flowMax;
//	fInterval *= 50.0;

//	cuda::min(fGMat,Scalar(m_flowMax),pGMat[0]);
//	cuda::multiply(pGMat[0],Scalar(fInterval),fGMat);
	cuda::multiply(fGMat,Scalar(fInterval),pGMat[0]);
	pGMat[0].convertTo(depthGMat,CV_8UC1);

	cv::cuda::cvtColor(depthGMat, idxGMat, CV_GRAY2BGR);
	m_pGpuLUT->transform(idxGMat,segGMat);

	m_pDepth->update(&depthGMat);
	m_pSeg->update(&segGMat);

}



} /* namespace kai */
