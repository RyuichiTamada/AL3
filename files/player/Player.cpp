#include "Player.h"
#include <cassert>

void Player::Initialize(Model* model) { 
	assert(model);
	model_ = model;

	worldTransform_.Initialize();
	worldTransform_.scale_ = {0.0f, 0.0f, 0.0f};
}

void Player::Update() { 
	worldTransform_.TransferMatrix();
}

void Player::Draw(ViewProjection& viewProjection) { 
	model_->Draw(worldTransform_, viewProjection);
}