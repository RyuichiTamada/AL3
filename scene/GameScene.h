#pragma once

#include "Audio.h"
#include "DebugCamera.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Model.h"
#include "Player.h"
#include "SafeDelete.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
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
	// テクスチャハンドル

	DirectXCommon* dxCommon_ = nullptr;
	Input* input_ = nullptr;
	Audio* audio_ = nullptr;
	DebugCamera* debugCamera_ = nullptr;
	uint32_t textureHandle_ = 0;
	uint32_t voiceHandle_ = 0;
	
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;
	float inputFloat3[3] = {0, 0, 0};
	// 自キャラの生成
	std::unique_ptr<Player> player_;
	// モデルデータ
	std::unique_ptr<Model> model_;

	/// <summary>
	/// ゲームシーン用
	/// </summary>
};