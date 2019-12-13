#pragma once

#include "opengl.h"
#include "OVR_CAPI_GL.h"
#include "OVR_CAPI.h"
#include "Extras/OVR_StereoProjection.h"
#include "Extras/OVR_Math.h"
#include "ovrUtils.h"

#include <glm\glm.hpp>

#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
#pragma comment(lib, "dxgi.lib")
#endif

class MeshController;

class OculusVR
{
public:
	OculusVR();
	~OculusVR();

	// init VR and Controllers
	bool initVR();
	bool initBufferVR(int width, int height);

	// Render in headset and pc
	void tracking();
	void onRender(unsigned int eyeIndex);
	void onRenderFinish(unsigned int eyeIndex);
	void updateControllers(const glm::mat4& viewMatrix);
	void activateAim(bool activate);
	void renderControllers(const glm::mat4& viewProjection);
	void blitMirror(int width, int height);
	void submitFrame();

	// interaction
	bool inputControllers(ovrInputState& inputState);
	bool reset();

	//getter & setter
	ovrSessionStatus getSessionStatus();
	const glm::mat4 getEyeInSpace(unsigned int eyeIndex);
	const glm::vec3 getPupilaryDistance(unsigned int eyeIndex);
	glm::mat4  getProjection(unsigned int eyeIndex);
	void setProjection(float Near, float Far);

private:
	static ovrGraphicsLuid GetDefaultAdapterLuid();
	static int CompareLuid(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs);

	// buffer OpenGL
	struct OVRBuffer
	{
		OVRBuffer(const ovrSession& session, int eyeIdx);
		void OnRender();
		void OnRenderFinish();
		void Destroy(const ovrSession& session);

		ovrSizei   m_size;
		GLuint     m_eyeFbo = 0;
		GLuint     m_eyeTexId = 0;
		GLuint     m_depthBuffer = 0;

		ovrTextureSwapChain m_colorTextureChain = nullptr;
		ovrTextureSwapChain m_depthTextureChain = nullptr;
	};

	// behavior Controllers
	struct OVRController
	{
		OVRController();
		void init();
		void render(const glm::mat4& VP);
		void update(const glm::mat4& mat);
		bool callback(const ovrSession& session, ovrInputState& inputState);
		void setPosOrientation(const ovrPosef& pose, unsigned int handIndex);
		void setRenderLine(bool activate);
		void isTrack(unsigned int handIndex, bool istrack);

		bool m_handIsTrack[2];
		ovrPosef m_hand[2];
		MeshController* m_meshControllers[2];
	};
	
	ovrLayerEyeFovDepth	    m_eyeLayer;
	ovrSession				m_hmdSession;
	ovrHmdDesc				m_hmdDesc;
	ovrTrackingState		m_trackingState;
	ovrMirrorTexture	    m_mirrorTexture;
	ovrMirrorTextureDesc	m_mirrorDesc;
	GLuint					m_mirrorFBO;
	long long				m_frameIndex;
	double					m_sensorSampleTime;

	OVR::Matrix4f m_projection[ovrEye_Count];
	ovrEyeRenderDesc  m_eyeRenderDesc[ovrEye_Count];
	ovrPosef		  m_hmdToEyePose[ovrEye_Count];
	ovrPosef m_eyeRenderPose[ovrEye_Count];
	OVRBuffer* m_buffers[ovrEye_Count];

	OVRController m_controllers;
};