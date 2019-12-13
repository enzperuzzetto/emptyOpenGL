#pragma once

#include <glm\glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ovr
{
	inline glm::mat4 toGlm(const ovrMatrix4f& om) {
		return glm::transpose(glm::make_mat4(&om.M[0][0]));
	}

	inline glm::mat4 toGlm(const ovrFovPort& fovport, float nearPlane = 0.01f, float farPlane = 10000.0f) {
		return toGlm(ovrMatrix4f_Projection(fovport, nearPlane, farPlane, true));
	}

	inline glm::vec3 toGlm(const ovrVector3f& ov) {
		return glm::make_vec3(&ov.x);
	}

	inline glm::vec2 toGlm(const ovrVector2f& ov) {
		return glm::make_vec2(&ov.x);
	}

	inline glm::uvec2 toGlm(const ovrSizei& ov) {
		return glm::uvec2(ov.w, ov.h);
	}

	inline glm::quat toGlm(const ovrQuatf& oq) {
		return glm::make_quat(&oq.x);
	}

	inline glm::mat4 toGlm(const ovrPosef& op) {
		glm::mat4 orientation = glm::mat4_cast(toGlm(op.Orientation));
		glm::mat4 translation = glm::translate(glm::mat4(1.f), ovr::toGlm(op.Position));
		return translation * orientation;
	}

	inline ovrMatrix4f fromGlm(const glm::mat4& m) {
		ovrMatrix4f result;
		glm::mat4 transposed(glm::transpose(m));
		memcpy(result.M, &(transposed[0][0]), sizeof(float) * 16);
		return result;
	}

	inline ovrVector3f fromGlm(const glm::vec3& v) {
		ovrVector3f result;
		result.x = v.x;
		result.y = v.y;
		result.z = v.z;
		return result;
	}

	inline ovrVector2f fromGlm(const glm::vec2& v) {
		ovrVector2f result;
		result.x = v.x;
		result.y = v.y;
		return result;
	}

	inline ovrSizei fromGlm(const glm::uvec2& v) {
		ovrSizei result;
		result.w = v.x;
		result.h = v.y;
		return result;
	}

	inline ovrQuatf fromGlm(const glm::quat& q) {
		ovrQuatf result;
		result.x = q.x;
		result.y = q.y;
		result.z = q.z;
		result.w = q.w;
		return result;
	}
}
