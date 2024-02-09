#include "FollowCamera.h"
#include "MyMath.h"
#include <Input.h>
#include <Xinput.h>

void FollowCamera::Initialize() { 
	viewProjection_.Initialize(); 
}

void FollowCamera::Update() { 
	if (target_) {
		Vector3 offset = {0.0f, 10.0f, -30.0f};
		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_.rotation_);

		offset = TransformNormal(offset, rotateMatrix);
		viewProjection_.translation_ = Add(target_->translation_, offset);
	}

	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float kRadian = 0.02f;
		viewProjection_.rotation_.y += (float)joyState.Gamepad.sThumbRX / SHRT_MAX * kRadian;
	}

	viewProjection_.UpdateViewMatrix();
	viewProjection_.TransferMatrix();
}