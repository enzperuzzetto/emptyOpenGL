#include "oculusvr.h"
#include "meshController.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#pragma region INITIALISATION
OculusVR::OculusVR() :
	m_hmdSession(nullptr),
	m_hmdDesc(),
	m_eyeLayer(),
	m_mirrorTexture(),
	m_frameIndex(0),
	m_sensorSampleTime(0.0)
{
	memset(m_eyeRenderDesc, 0, ovrEye_Count * sizeof(ovrEyeRenderDesc));
	memset(m_eyeRenderPose, 0, ovrEye_Count * sizeof(ovrPosef));
	memset(m_hmdToEyePose, 0, ovrEye_Count * sizeof(ovrPosef));
	memset(m_buffers, 0, ovrEye_Count * sizeof(OVRBuffer*));
	memset(&m_mirrorDesc, 0, sizeof(ovrMirrorTextureDesc));
	memset(m_projection, 0, ovrEye_Count * sizeof(OVR::Matrix4f));
}

OculusVR::~OculusVR()
{
	m_hmdSession = nullptr;
	for (auto& buffer : m_buffers)
		delete(buffer);
	ovr_Destroy(m_hmdSession);
	ovr_Shutdown();
}

bool OculusVR::initVR()
{
	ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, NULL, 0, 0 };
	ovrResult result = ovr_Initialize(&initParams);
	ovrGraphicsLuid luid; // as of SDK 0.7.0.0 luid is not supported with OpenGL

	if (result != ovrSuccess)
	{
		assert(("Failed to initialize LibOVR", false));
		return false;
	}

	result = ovr_Create(&m_hmdSession, &luid);

	if (result != ovrSuccess)
	{
		assert(("Failed to create OVR device", OVR_SUCCESS(result)));
		return false;
	}

	if (CompareLuid(luid, GetDefaultAdapterLuid())) // If luid that the Rift is on is not the default adapter LUID...
	{
		assert(("OpenGL supports only the default graphics adapter", false));
		return false;
	}

	m_hmdDesc = ovr_GetHmdDesc(m_hmdSession);

	m_controllers = OVRController();
	m_controllers.init();

	//ovr_SetTrackingOriginType(m_hmdSession, ovrTrackingOrigin_FloorLevel);

	return OVR_SUCCESS(result);
}

bool OculusVR::initBufferVR(int width, int height)
{
	for (int i = 0; i < ovrEye_Count; i++)
	{
		m_buffers[i] = new OVRBuffer(m_hmdSession, i);
	}

	m_mirrorDesc.Width = width;
	m_mirrorDesc.Height = height;
	m_mirrorDesc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

	ovrResult result = ovr_CreateMirrorTextureWithOptionsGL(m_hmdSession, &m_mirrorDesc, &m_mirrorTexture);
	if (!OVR_SUCCESS(result))
	{
		assert(("Failed to create mirror texture", false));
		return false;
	}

	m_eyeLayer.Header.Type = ovrLayerType_EyeFovDepth;
	m_eyeLayer.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft; //Because OpenGL.

	GLuint texId;
	ovr_GetMirrorTextureBufferGL(m_hmdSession, m_mirrorTexture, &texId);

	glGenFramebuffers(1, &m_mirrorFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_mirrorFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
	glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	return true;
}

#pragma endregion

#pragma region RENDER
void OculusVR::tracking()
{
	double displayMidpointSeconds = ovr_GetPredictedDisplayTime(m_hmdSession, m_frameIndex);
	ovrTrackingState trackState = ovr_GetTrackingState(m_hmdSession, displayMidpointSeconds, ovrTrue);
	
	for (unsigned short i = 0; i < 2; i++)
	{
		if (trackState.HandStatusFlags[i] == 15)
		{
			m_controllers.isTrack(i, true);
			m_controllers.setPosOrientation(trackState.HandPoses[i].ThePose, i);
		}
		else
			m_controllers.isTrack(i, false);
	}
	
	m_headPose = trackState.HeadPose.ThePose;

	for (unsigned int i = 0; i < ovrEye_Count; i++)
	{
		m_eyeRenderDesc[i] = ovr_GetRenderDesc(m_hmdSession, (ovrEyeType)i, m_hmdDesc.DefaultEyeFov[i]);
		m_hmdToEyePose[i] = m_eyeRenderDesc[i].HmdToEyePose;
	}

	ovr_GetEyePoses(m_hmdSession, m_frameIndex, ovrTrue, m_hmdToEyePose, m_eyeRenderPose, &m_sensorSampleTime);
}

void OculusVR::onRender(unsigned int eyeIndex)
{
	int curIndex;
	ovr_GetTextureSwapChainCurrentIndex(m_hmdSession, m_buffers[eyeIndex]->m_colorTextureChain, &curIndex);
	ovr_GetTextureSwapChainBufferGL(m_hmdSession, m_buffers[eyeIndex]->m_colorTextureChain, curIndex, &m_buffers[eyeIndex]->m_eyeTexId);

	ovr_GetTextureSwapChainCurrentIndex(m_hmdSession, m_buffers[eyeIndex]->m_depthTextureChain, &curIndex);
	ovr_GetTextureSwapChainBufferGL(m_hmdSession, m_buffers[eyeIndex]->m_depthTextureChain, curIndex, &m_buffers[eyeIndex]->m_depthBuffer);

	m_buffers[eyeIndex]->OnRender();
}

void OculusVR::onRenderFinish(unsigned int eyeIndex)
{
	m_buffers[eyeIndex]->OnRenderFinish();

	ovr_CommitTextureSwapChain(m_hmdSession, m_buffers[eyeIndex]->m_colorTextureChain);
	ovr_CommitTextureSwapChain(m_hmdSession, m_buffers[eyeIndex]->m_depthTextureChain);
}

void OculusVR::updateControllers(const glm::mat4& viewMatrix)
{	
	m_controllers.update(ovr::toGlm(m_headPose.Position), viewMatrix[3]);
}

void OculusVR::activateAim(bool activate)
{
	m_controllers.setRenderLine(activate);
}


void OculusVR::renderControllers(const glm::mat4& viewProjection)
{
	m_controllers.render(viewProjection);
}

void OculusVR::blitMirror(int width, int height)
{
	GLint w = width;
	GLint h = height;
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_mirrorFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, h, w, 0, 0, 0, w, h, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void OculusVR::submitFrame()
{
	//m_eyeLayer.ProjectionDesc = m_posTimewarpProjectionDesc;
	m_eyeLayer.SensorSampleTime = m_sensorSampleTime;

	for (int i = 0; i < ovrEye_Count; i++)
	{
		m_eyeLayer.ColorTexture[i] = m_buffers[i]->m_colorTextureChain;
		m_eyeLayer.DepthTexture[i] = m_buffers[i]->m_depthTextureChain;
		m_eyeLayer.Viewport[i] = OVR::Recti(m_buffers[i]->m_size);
		m_eyeLayer.Fov[i] = m_hmdDesc.DefaultEyeFov[i];
		m_eyeLayer.RenderPose[i] = m_eyeRenderPose[i];
	}

	ovrLayerHeader* layerList = &m_eyeLayer.Header;

	ovrResult result = ovr_SubmitFrame(m_hmdSession, m_frameIndex, nullptr, &layerList, 1);

	if (!OVR_SUCCESS(result))
	{
		assert(("Failed to submit frame", false));
	}
	m_frameIndex++;
}
#pragma endregion

#pragma region BEHAVIOR

bool OculusVR::inputControllers(ovrInputState& inputState)
{
	return false;//m_controllers->callback(m_hmdSession, inputState);
}

bool OculusVR::reset()
{
	ovrResult result = ovr_RecenterTrackingOrigin(m_hmdSession);
	return OVR_SUCCESS(result);
}
#pragma endregion

#pragma region GETTER&SETTER
ovrSessionStatus OculusVR::getSessionStatus()
{
	ovrSessionStatus sessionStatus;
	ovr_GetSessionStatus(m_hmdSession, &sessionStatus);
	return sessionStatus;
}

const glm::mat4 OculusVR::getEyeInSpace(unsigned int eyeIndex)
{
	return ovr::toGlm(m_eyeRenderPose[eyeIndex]);
}

const glm::vec3 OculusVR::getPupilaryDistance(unsigned int eyeIndex)
{
	m_eyeRenderDesc[eyeIndex] = ovr_GetRenderDesc(m_hmdSession, (ovrEyeType)eyeIndex, m_hmdDesc.DefaultEyeFov[eyeIndex]);
	m_hmdToEyePose[eyeIndex] = m_eyeRenderDesc[eyeIndex].HmdToEyePose;
	return ovr::toGlm(m_hmdToEyePose[eyeIndex].Position);
}

glm::mat4 OculusVR::getProjection(unsigned int eyeIndex)
{
	m_eyeLayer.ProjectionDesc = ovrTimewarpProjectionDesc_FromProjection(m_projection[eyeIndex], ovrProjection_None);
	return ovr::toGlm(m_projection[eyeIndex]);
}

void OculusVR::setProjection(float Near, float Far)
{
	for (unsigned int i = 0; i < ovrEye_Count; i++)
	{
		m_projection[i] = ovrMatrix4f_Projection(m_hmdDesc.DefaultEyeFov[i], Near, Far, ovrProjection_None);
	}
}
#pragma endregion

/////////////////// OVRBuffer ///////////////////////////
#pragma region OVRBUFFER

OculusVR::OVRBuffer::OVRBuffer(const ovrSession& session, int eyeIdx)
{
	ovrHmdDesc hmdDesc = ovr_GetHmdDesc(session);
	m_size = ovr_GetFovTextureSize(session, (ovrEyeType)eyeIdx, hmdDesc.DefaultEyeFov[eyeIdx], 1.0f);

	ovrTextureSwapChainDesc desc = {};
	desc.Type = ovrTexture_2D;
	desc.ArraySize = 1;
	desc.Width = m_size.w;
	desc.Height = m_size.h;
	desc.MipLevels = 1;
	desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;
	desc.SampleCount = 1;
	desc.StaticImage = ovrFalse;

	ovrResult result;
	int textureCount = 0;

	{
		result = ovr_CreateTextureSwapChainGL(session, &desc, &m_colorTextureChain);

		ovr_GetTextureSwapChainLength(session, m_colorTextureChain, &textureCount);

		if (OVR_SUCCESS(result))
		{
			for (int j = 0; j < textureCount; ++j)
			{
				GLuint chainTexId;
				ovr_GetTextureSwapChainBufferGL(session, m_colorTextureChain, j, &chainTexId);
				glBindTexture(GL_TEXTURE_2D, chainTexId);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
		}
	}

	desc.Format = OVR_FORMAT_D32_FLOAT;

	{
		result = ovr_CreateTextureSwapChainGL(session, &desc, &m_depthTextureChain);

		textureCount = 0;
		ovr_GetTextureSwapChainLength(session, m_depthTextureChain, &textureCount);

		if (OVR_SUCCESS(result))
		{
			for (int j = 0; j < textureCount; ++j)
			{
				GLuint chainTexId;
				ovr_GetTextureSwapChainBufferGL(session, m_depthTextureChain, j, &chainTexId);
				glBindTexture(GL_TEXTURE_2D, chainTexId);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			}
		}
	}

	glGenFramebuffers(1, &m_eyeFbo);
}

void OculusVR::OVRBuffer::OnRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_eyeFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_eyeTexId, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthBuffer, 0);

	glViewport(0, 0, m_size.w, m_size.h);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_FRAMEBUFFER_SRGB);
}

void OculusVR::OVRBuffer::OnRenderFinish()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_eyeFbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, 0, 0);
}

void OculusVR::OVRBuffer::Destroy(const ovrSession& session)
{
	if (glIsFramebuffer(m_eyeFbo))
		glDeleteFramebuffers(1, &m_eyeFbo);

	if (glIsTexture(m_depthBuffer))
		glDeleteTextures(1, &m_depthBuffer);

	ovr_DestroyTextureSwapChain(session, m_colorTextureChain);
	ovr_DestroyTextureSwapChain(session, m_depthTextureChain);
}
#pragma endregion

/////////////////// OVRControllers //////////////////////

#pragma region OVRCONTROLLER
OculusVR::OVRController::OVRController()
{
	memset(m_hand, 0, 2 * sizeof(ovrPosef));
	memset(m_meshControllers, 0, 2 * sizeof(MeshController*));
	memset(m_handIsTrack, 0, 2 * sizeof(bool));
}

void OculusVR::OVRController::init()
{
	for (int i = 0; i < 2; i++)
	{
		m_meshControllers[i] = new MeshController();
		m_meshControllers[i]->init();
	}
}

void OculusVR::OVRController::render(const glm::mat4& VP)
{
	for (unsigned int i=0; i<2; i++)
	{
		if(m_handIsTrack[i])
			m_meshControllers[i]->render(VP);
	}
}

void OculusVR::OVRController::update(const glm::vec3& headPosePhysic, const glm::vec3& headPoseVr)
{
	for (unsigned short i = 0; i < 2; i++)
	{
		glm::mat4 translationMat = glm::translate(glm::mat4(1.f), glm::vec3(0.f, 0.f, 20.f));
		glm::mat4 handPose = ovr::toGlm(m_hand[i]);
		glm::mat4 orientationMat = glm::rotate(glm::mat4(1.f), glm::radians(180.f), glm::vec3(0.f, 1.f, 0.f));
		m_meshControllers[i]->update(translationMat * handPose * orientationMat);
	}
}

bool OculusVR::OVRController::callback(const ovrSession& session, ovrInputState& inputState)
{
	return OVR_SUCCESS(ovr_GetInputState(session, ovrControllerType_Touch, &inputState));
}

void OculusVR::OVRController::setPosOrientation(const ovrPosef& pose, unsigned int handIndex)
{
	m_hand[handIndex] = pose;
}
void OculusVR::OVRController::setRenderLine(bool activate)
{
	for (auto& mesh : m_meshControllers)
		mesh->setRenderLine(activate);
}
void OculusVR::OVRController::isTrack(unsigned int handIndex, bool istrack)
{
	m_handIsTrack[handIndex] = istrack;
}
#pragma endregion

/////////////////// Static function /////////////////////

#pragma region STATIC_FUNCTION
ovrGraphicsLuid OculusVR::GetDefaultAdapterLuid()
{
	ovrGraphicsLuid luid = ovrGraphicsLuid();

#if defined(_WIN32)
	IDXGIFactory* factory = nullptr;

	if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
	{
		IDXGIAdapter* adapter = nullptr;

		if (SUCCEEDED(factory->EnumAdapters(0, &adapter)))
		{
			DXGI_ADAPTER_DESC desc;

			adapter->GetDesc(&desc);
			memcpy(&luid, &desc.AdapterLuid, sizeof(luid));
			adapter->Release();
		}

		factory->Release();
	}
#endif

	return luid;
}

int OculusVR::CompareLuid(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs)
{
	return memcmp(&lhs, &rhs, sizeof(ovrGraphicsLuid));
}
#pragma endregion