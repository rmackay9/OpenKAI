/*
 * State.cpp
 *
 *  Created on: Aug 27, 2016
 *      Author: root
 */

#include "_Automaton.h"

namespace kai
{

_Automaton::_Automaton()
{
	BASE();

	m_nState = 0;
	m_iState = 0;
}

_Automaton::~_Automaton()
{
	// TODO Auto-generated destructor stub
}

bool _Automaton::init(void* pKiss)
{
	CHECK_F(this->_ThreadBase::init(pKiss)==false);
	Kiss* pK = (Kiss*)pKiss;
	pK->m_pInst = this;

	//create state instances
	Kiss** pItr = pK->getChildItr();

	int i = 0;
	while (pItr[i])
	{
		Kiss* pState = pItr[i++];

		CHECK_F(m_nState >= N_STATE);
		F_ERROR_F(pState->v("state", &m_pStateName[m_nState]));
		m_nState++;
	}

	string startState = "";
	F_ERROR_F(pK->v("startState", &startState));
	m_iState = getStateIdx(&startState);
	CHECK_F(m_iState<0);

	return true;
}

bool _Automaton::link(void)
{
	NULL_F(m_pKiss);
	return true;
}

bool _Automaton::start(void)
{
	return true;
}

int _Automaton::getStateIdx(string* pStateName)
{
	if(pStateName==NULL)return -1;

	for(int i=0;i<m_nState;i++)
	{
		if(m_pStateName[i]==*pStateName)return i;
	}

	return -1;
}

bool _Automaton::transit(string* pNextStateName)
{
	NULL_F(pNextStateName);

	int iNext = getStateIdx(pNextStateName);
	CHECK_F(iNext<0);

	m_iState = iNext;
	return true;
}

bool _Automaton::transit(int nextStateIdx)
{
	CHECK_F(nextStateIdx < 0);
	CHECK_F(nextStateIdx >= m_nState);

	m_iState = nextStateIdx;
	return true;
}

int _Automaton::getCurrentStateIdx(void)
{
	return m_iState;
}

bool _Automaton::draw(Frame* pFrame, vInt4* pTextPos)
{
	NULL_F(pFrame);

	Mat* pMat = pFrame->getCMat();

	putText(*pMat, "Automaton State: " + m_pStateName[m_iState],
			cv::Point(pTextPos->m_x, pTextPos->m_y), FONT_HERSHEY_SIMPLEX, 0.5,
			Scalar(0, 255, 0), 1);
	pTextPos->m_y += pTextPos->m_w;

	return true;
}

} /* namespace kai */
