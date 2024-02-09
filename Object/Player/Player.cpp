#include "Player.h"
#include "MyMath.h"

#include <Input.h>
#include <Xinput.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <cassert>
#include "GlobalVariables.h"

void Player::Initialize(const std::vector<Model*>&models) { 
	BaseCharacter::Initialize(models);

	/*models_[kModelIndexBody] = models[kModelIndexBody];
	models_[kModelIndexHead] = models[kModelIndexHead];
	models_[kModelIndexL_arm] = models[kModelIndexL_arm];
	models_[kModelIndexR_arm] = models[kModelIndexR_arm];*/

	worldTransformL_arm_.translation_.x = -1.5f;
	worldTransformR_arm_.translation_.x = 1.5f;
	worldTransformL_arm_.translation_.y = 5.0f;
	worldTransformR_arm_.translation_.y = 5.0f;

	SetParent(&GetWorldTransformBody());
	worldTransformBody_.parent_ = worldTransform_.parent_;

	InitializeFloatingGimmick();

	worldTransformBase_.Initialize();
	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformHammer_.Initialize();

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();

	const char* groupName = "Player";
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	globalVariables->AddItem(groupName, "Testing", 90);
	globalVariables->AddItem(groupName, "Head Translation", worldTransformHead_.translation_);
	globalVariables->AddItem(groupName, "Left Arm Translation", worldTransformL_arm_.translation_);
	globalVariables->AddItem(groupName, "Right Arm Translation", worldTransformR_arm_.translation_);
}

void Player::Update() { 
	BaseCharacter::Update();

	XINPUT_STATE joyState;

	if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
		return;
	}

	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		behaviorRequest_ = Behavior::kAttack;
	}

	if (behaviorRequest_) {
		behavior_ = behaviorRequest_.value();

		switch (behavior_) {
		case Player::Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;

		case Player::Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		}

		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	}

	worldTransformHammer_.UpdateMatrix();
	worldTransformBody_.UpdateMatrix();
	worldTransformHead_.UpdateMatrix();
	worldTransformL_arm_.UpdateMatrix();
	worldTransformR_arm_.UpdateMatrix();

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	ApplyGlobalVariables();

	if (globalVariables->GetInstance()->GetIsSaved()) {
		globalVariables->SaveFile("Player");
	}
}

void Player::Draw(const ViewProjection& viewProjection) {
	models_[kModelIndexBody]->Draw(worldTransformBody_, viewProjection);
	models_[kModelIndexHead]->Draw(worldTransformHead_, viewProjection);
	models_[kModelIndexL_arm]->Draw(worldTransformL_arm_, viewProjection);
	models_[kModelIndexR_arm]->Draw(worldTransformR_arm_, viewProjection);

	if (behavior_ == Behavior::kAttack) {
		models_[kModelIndexWeapon]->Draw(worldTransformHammer_, viewProjection);
	}
}

Vector3 Player::GetWorldPosition() { 
	Vector3 worldPosition;

	worldPosition.x = worldTransform_.matWorld_.m[3][0];
	worldPosition.y = worldTransform_.matWorld_.m[3][1];
	worldPosition.z = worldTransform_.matWorld_.m[3][2];

	return worldPosition;
}

void Player::SetParent(const WorldTransform* parent) { 
	worldTransformHead_.parent_ = parent;
	worldTransformL_arm_.parent_ = parent;
	worldTransformR_arm_.parent_ = parent;
	worldTransformHammer_.parent_ = parent;
}

void Player::InitializeFloatingGimmick() {
	for (int i = 0; i < kMaxMoveModelParts_; i++) {
		floatingParameter_[i] = 0.0f;
	}
}

void Player::UpdateFloatingGimmick() {
	uint16_t floatingCycle[2]{};
	floatingCycle[0] = 120;
	floatingCycle[1] = 120;

	float steps[2]{};

	for (int i = 0; i < kMaxMoveModelParts_; i++) {
		steps[i] = 2.0f * (float)M_PI / floatingCycle[i];
		floatingParameter_[i] += steps[i];
		floatingParameter_[i] = (float)std::fmod(floatingParameter_[i], 2.0f * M_PI);
	}

	const float floatingAmplitude = 0.5f;

	worldTransformBody_.translation_.y = std::sin(floatingParameter_[0]) * floatingAmplitude;

	worldTransformL_arm_.rotation_.x = std::sin(floatingParameter_[1]) * 0.75f;
	worldTransformR_arm_.rotation_.x = std::sin(floatingParameter_[1]) * 0.75f;
}

void Player::BehaviorRootInitialize() { 
	worldTransformL_arm_.rotation_.x = 0.0f;
	worldTransformR_arm_.rotation_.x = 0.0f;
	worldTransformHammer_.rotation_.x = 0.0f;

	InitializeFloatingGimmick();

	worldTransformBody_.Initialize();
	worldTransformHead_.Initialize();
	worldTransformL_arm_.Initialize();
	worldTransformR_arm_.Initialize();
	worldTransformHammer_.Initialize();
}

void Player::BehaviorRootUpdate() { 
	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {
		const float speed = 0.3f;

		Vector3 move{
		    (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
		    (float)joyState.Gamepad.sThumbLY / SHRT_MAX};

		move = Multiply(speed, Normalize(move));

		Matrix4x4 rotateMatrix = MakeRotateMatrix(viewProjection_->rotation_);
		move = TransformNormal(move, rotateMatrix);

		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
		worldTransformBody_.translation_ = worldTransform_.translation_;

		worldTransform_.rotation_.y = std::atan2(move.x, move.z);
		worldTransformBody_.rotation_.y = worldTransform_.rotation_.y;
	}

	UpdateFloatingGimmick();
}

void Player::BehaviorAttackInitialize() { 
	worldTransformL_arm_.rotation_.x = (float)M_PI;
	worldTransformR_arm_.rotation_.x = (float)M_PI;
	worldTransformHammer_.rotation_.x = 0.0f;
	animationFrame = 0;
}

void Player::BehaviorAttackUpdate() { 
	if (animationFrame < 10) {
		worldTransformL_arm_.rotation_.x -= 0.05f;
		worldTransformR_arm_.rotation_.x -= 0.0f;

		worldTransformHammer_.rotation_.x -= 0.05f;
	} else if (worldTransformHammer_.rotation_.x <= 2.0f * (float)M_PI / 4) {
		worldTransformL_arm_.rotation_.x += 0.1f;
		worldTransformR_arm_.rotation_.x += 0.1f;

		worldTransformHammer_.rotation_.x += 0.1f;
	} else {
		behaviorRequest_ = Behavior::kRoot;
	}

	animationFrame++;
}

void Player::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";

	worldTransformHead_.translation_ = globalVariables->GetVector3Value(groupName, "Head Translation");
	worldTransformL_arm_.translation_ = globalVariables->GetVector3Value(groupName, "Left Arm Translation");
	worldTransformR_arm_.translation_ = globalVariables->GetVector3Value(groupName, "Right Arm Translation");
	
	//floatingParameter_[0] = globalVariables->GetFloatValue(groupName, "Floating Cycle Arm");
	//floatingParameter_[1] = globalVariables->GetFloatValue(groupName, "Floating Cycle Body");
}