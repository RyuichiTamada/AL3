#include "GameScene.h"
#include "TextureManager.h"
#include "ImGuiManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() 
{ 
	
}

void GameScene::Initialize() {

	dxCommon_ = DirectXCommon::GetInstance();
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();

	// ワールドトランスフォーム
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();

	// 敵
	enemy_ = std::make_unique<Enemy>();
	enemyModel_.reset(Model::CreateFromOBJ("Enemy", true));

	std::vector<Model*> enemyModels = {enemyModel_.get()};
	enemy_->Initialize(enemyModels);

	// 自キャラの生成
	player_ = std::make_unique<Player>();

	modelFighterBody_.reset(Model::CreateFromOBJ("float_Body", true));
	modelFighterHead_.reset(Model::CreateFromOBJ("float_Head", true));
	modelFighterL_arm_.reset(Model::CreateFromOBJ("float_L_arm", true));
	modelFighterR_arm_.reset(Model::CreateFromOBJ("float_R_arm", true));

	modelFighterHammer_.reset(Model::CreateFromOBJ("Hammer", true));

	std::vector<Model*> playerModels = {
	    modelFighterBody_.get(), modelFighterHead_.get(), 
		modelFighterL_arm_.get(), modelFighterR_arm_.get(), modelFighterHammer_.get()};

	player_->Initialize(playerModels);

	// 天球
	skyDome_ = std::make_unique<SkyDome>();
	skyDomeModel_.reset(Model::CreateFromOBJ("skydome", true));
	skyDome_->Initialize(skyDomeModel_.get(), {0, 0, 0});

	// 地面
	ground_ = std::make_unique<Ground>();
	groundModel_.reset(Model::CreateFromOBJ("ground", true));
	ground_->Initialize(groundModel_.get(), {0, 0, 0});

	// デバッグカメラ
	debugCamera_ = std::make_unique<DebugCamera>(1280, 720);

	// カメラ
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&player_->GetWorldTransformBase());

	player_->SetViewProjection(&followCamera_->GetViewProjection());

	// スプライト
	titleTexture_ = TextureManager::Load("title.png");
	titleSprite_ = Sprite::Create(titleTexture_, {motionX_,motionY_}, {1, 1, 1, 1}, {0.5f, 0.5f});

	fadeTexture_ = TextureManager::Load("fade.png");
	fadeSprite_ = Sprite::Create(fadeTexture_, {640, 360}, {1, 1, 1, alpha_}, {0.5f, 0.5f});
}

void GameScene::Update() { 
	switch (sceneNumber_) {
	case 0:
		XINPUT_STATE joyState;

		if (isFlip == false) {
			motionY_ += 0.3f;
		} else {
			motionY_ -= 0.3f;
		}

		if (motionY_ > 370.0f) {
			isFlip = true;
		} else if (motionY_ < 350.0f) {
			isFlip = false;
		}

		titleSprite_->SetPosition({motionX_, motionY_});

		if (!Input::GetInstance()->GetJoystickState(0, joyState)) {
			return;
		}

		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			isTrigger_ = true;
		}

		if (isTrigger_ == true) {
			alpha_ += 0.01f;
		}

		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha_});

		if (alpha_ > 1.0f) {
			sceneNumber_ = 1;
		}

		break;

	case 1:
		if (isGameBegin_ == false) {
			alpha_ -= 0.01f;
		}

		if (alpha_ < 0.0f) {
			isGameBegin_ = true;
		}

		fadeSprite_->SetColor({1.0f, 1.0f, 1.0f, alpha_});

		player_->Update();
		enemy_->Update();
		skyDome_->Update();
		ground_->Update();

		viewProjection_.UpdateMatrix();
		followCamera_->Update();

		viewProjection_.matView = followCamera_->GetViewProjection().matView;
		viewProjection_.matProjection = followCamera_->GetViewProjection().matProjection;
		viewProjection_.TransferMatrix();

		AxisIndicator::GetInstance()->SetVisible(false);
		AxisIndicator::GetInstance()->SetTargetViewProjection(&debugCamera_->GetViewProjection());
		break;
	}
}

void GameScene::Draw() {

	// コマンドリストの取得
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

#pragma region 背景スプライト描画
	// 背景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに背景スプライトの描画処理を追加できる
	/// </summary>

	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon_->ClearDepthBuffer();
#pragma endregion

#pragma region 3Dオブジェクト描画
	// 3Dオブジェクト描画前処理
	Model::PreDraw(commandList);

	/// <summary>
	/// ここに3Dオブジェクトの描画処理を追加できる
	/// </summary>
	
	switch (sceneNumber_) {
	case 0:
		
		break;

	case 1:
		player_->Draw(viewProjection_);
		enemy_->Draw(viewProjection_);
		skyDome_->Draw(viewProjection_);
		ground_->Draw(viewProjection_);
		break;
	}

	// 3Dオブジェクト描画後処理
	Model::PostDraw();
#pragma endregion

#pragma region 前景スプライト描画
	// 前景スプライト描画前処理
	Sprite::PreDraw(commandList);

	/// <summary>
	/// ここに前景スプライトの描画処理を追加できる
	/// </summary>
	
	switch (sceneNumber_) {
	case 0:
		titleSprite_->Draw();
		fadeSprite_->Draw();
		break;

	case 1:
		fadeSprite_->Draw();
		break;
	}

	// スプライト描画後処理
	Sprite::PostDraw();

#pragma endregion
}
