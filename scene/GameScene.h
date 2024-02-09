#pragma once

#include "Audio.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "DebugCamera.h"
#include "AxisIndicator.h"
#include "Ground.h"
#include "FollowCamera.h"

#include <memory>

/// <summary>
/// ゲームシーン
/// </summary>
class GameScene {

public: // メンバ関数
	/// <summary>
	/// コンストクラタ
	/// </summary>
	GameScene();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private: // メンバ変数
	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;

	//デバッグカメラ
	std::unique_ptr<DebugCamera> debugCamera_;
	bool isDebugCameraActive_ = false;

	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	
	// ビュープロジェクション
	ViewProjection viewProjection_;
	
	// 敵
	std::unique_ptr<Enemy> enemy_;
	std::unique_ptr<Model> enemyModel_;

	// 自キャラ
	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
	std::unique_ptr<Model> modelFighterHammer_;

	std::unique_ptr<Model> playerModel_;
	std::unique_ptr<Player> player_;
	
	// 天球
	std::unique_ptr<Model> skyDomeModel_;
	std::unique_ptr<SkyDome> skyDome_;
	
	// 地面
	std::unique_ptr<Model> groundModel_;
	std::unique_ptr<Ground> ground_;
	
	// カメラ
	std::unique_ptr<FollowCamera> followCamera_;

	int sceneNumber_ = 0;

	// スプライト
	Sprite* titleSprite_ = nullptr;
	uint32_t titleTexture_ = 0u;

	float motionX_ = 640.0f;
	float motionY_ = 360.0f;

	bool isFlip = false;

	float alpha_ = 0.0f;

	bool isTrigger_ = false;

	bool isGameBegin_ = false;

	Sprite* fadeSprite_ = nullptr;
	uint32_t fadeTexture_ = 0u;
};
