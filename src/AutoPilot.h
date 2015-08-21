#pragma once

#include "common.h"
#include "CameraVision.h"
#include "MavlinkInterface.h"
#include "util.h"

//
// Operation mode
//
#define OPE_SERIAL_BRIDGE 0
#define OPE_MANUAL 1
#define OPE_ALT_HOLD 2
#define OPE_LOITER 3
#define OPE_AUTO 4
#define OPE_RC_BRIDGE 5
#define OPE_BOOT 6

#define NUM_RC_CHANNEL 8

//FALCON COMMANDS
#define CMD_RC_UPDATE 0
#define CMD_OPERATE_MODE 1

struct PID_SETTING
{
	double m_P;
	double m_I;
	double m_D;
	double m_Z;	//Depth value for the setting
};

struct CONTROL_AXIS
{
	double m_pos;
	double m_targetPos;
	double m_accel;

	double m_cvErr;
	double m_cvErrOld;
	double m_cvErrInteg;

	double m_P;
	double m_I;
	double m_Imax;
	double m_D;

	unsigned int m_pwm;
	unsigned int m_pwmLow;
	unsigned int m_pwmCenter;
	unsigned int m_pwmHigh;
	unsigned int m_RCChannel;
};

class AutoPilot
{
public:
	AutoPilot();
	~AutoPilot();

	bool init(JSON* json);
	PID_SETTING getRollFarPID(void);
	PID_SETTING getRollNearPID(void);
	PID_SETTING getAltFarPID(void);
	PID_SETTING getAltNearPID(void);
	PID_SETTING getPitchFarPID(void);
	PID_SETTING getPitchNearPID(void);
	PID_SETTING getYawFarPID(void);
	PID_SETTING getYawNearPID(void);

	fVector3 getRollPID(void);
	fVector3 getAltPID(void);
	fVector3 getPitchPID(void);
	fVector3 getYawPID(void);

	void setRollFarPID(PID_SETTING pid);
	void setRollNearPID(PID_SETTING pid);
	void setAltFarPID(PID_SETTING pid);
	void setAltNearPID(PID_SETTING pid);
	void setPitchFarPID(PID_SETTING pid);
	void setPitchNearPID(PID_SETTING pid);
	void setYawFarPID(PID_SETTING pid);
	void setYawNearPID(PID_SETTING pid);

	int* getPWMOutput(void);
	CONTROL_AXIS* getRollAxis(void);
	CONTROL_AXIS* getPitchAxis(void);
	CONTROL_AXIS* getAltAxis(void);

	void setDelayTime(double dT);
	double getDelayTime(void);

//	void setRollPID(fVector3 pid);
//	void setAltPID(fVector3 pid);
//	void setPitchPID(fVector3 pid);

	void setRC(int channelID, int pwmCenter, int pwmFrom, int pwmTo);

	void setCameraVision(CameraVision* pCV);
	void setMavlink(MavlinkInterface* pMavlink);

	void markerLock(void);
	void flowLock(void);

	void setTargetPosCV(fVector3 pos);
	fVector3 getTargetPosCV(void);

	void remoteMavlinkMsg(MESSAGE* pMsg);

	SYSTEM m_hostSystem;
	SYSTEM m_remoteSystem;
	MESSAGE* m_pRecvMsg;

private:
	//Common
	double	 m_dT;

	//Marker Lock
	int		 m_lockLevel;
	bool	 m_bAutoCalcDelay;
	fVector3 m_cvPos;
	fVector3 m_cvPosOld;
	fVector3 m_cvAtt;

	//Flow Lock
	int		 m_iFlowFrame;
	int		 m_resetFlowFrame;
	double	 m_resetFactor;
	fVector4 m_flowTotal;

	//Control
	CONTROL_AXIS m_roll;
	CONTROL_AXIS m_pitch;
	CONTROL_AXIS m_yaw;
	CONTROL_AXIS m_alt;

	PID_SETTING m_rollFar;
	PID_SETTING m_rollNear;
	PID_SETTING m_altFar;
	PID_SETTING m_altNear;
	PID_SETTING m_pitchFar;
	PID_SETTING m_pitchNear;
	PID_SETTING m_yawFar;
	PID_SETTING m_yawNear;

	int m_RC[NUM_RC_CHANNEL];

	CameraVision* m_pCV;
	MavlinkInterface* m_pMavlink;

};
