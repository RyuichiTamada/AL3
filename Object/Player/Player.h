#pragma once

#include "Model.h"
#include "WorldTransform.h"
#include "BaseCharacter.h"

#include <optional>
#include <memory>

/// <summary>
/// 自キャラ
/// </summary>
class Player : public BaseCharacter {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name= "model">モデル</param>
	/// <param name= "textureHandle">テクスチャハンドル</param>
	void Initialize(const std::vector<Model*>& models) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name= "viewProjection">ビュープロジェクション（参照渡し）</param>
	void Draw(const ViewProjection& viewProjection) override;

	void InitializeFloatingGimmick();
	void UpdateFloatingGimmick();

	void BehaviorRootInitialize();
	void BehaviorRootUpdate();
	void BehaviorAttackInitialize();
	void BehaviorAttackUpdate();

	Vector3 GetWorldPosition();

	const WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }
	const WorldTransform& GetWorldTransformBase() { return worldTransform_; }

	void SetViewProjection(const ViewProjection* viewProjection) {viewProjection_ = viewProjection; }

	void SetParent(const WorldTransform* parent);

	void ApplyGlobalVariables();

private:
	enum class Behavior {
		kRoot,
		kAttack
	};

	Behavior behavior_ = Behavior::kRoot;
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	int animationFrame;

	// ワールド変換データ
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;

	WorldTransform worldTransformHammer_;

	const ViewProjection* viewProjection_ = nullptr;

	// モデル
	Model* modelBody_;
	Model* modelHead_;
	Model* modelL_arm_;
	Model* modelR_arm_;

	const uint16_t kMaxMoveModelParts_ = 2;

	float floatingParameter_[2];
};