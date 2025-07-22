#include "GameScene.h"
#include "externels/imgui/imgui.h"
#include "externels/imgui/imgui_impl_dx12.h"
#include "externels/imgui/imgui_impl_win32.h"



void GameScene::Initialize() {
	//BaseScene::Initialize();
	//ModelManager::GetInstance()->LoadModel("Resources/Model/gltf/human", "walkMultiMaterial.gltf", true, true);

	TextureManager::GetInstance()->LoadTexture("Resources/rostock_laage_airport_4k.dds");

	camera = new Camera();
	/*camera->SetRotate(Vector3(SwapRadian(10.0f), 3.14f, 0.0f));
	camera->SetTranslate({ 0.0f, 1.7f, 0.0f });*/

	SkyBox::GetInstance()->SetCamera(camera);
	SkyBox::GetInstance()->SetTexture("Resources/rostock_laage_airport_4k.dds");

	input = Input::GetInstance();
	input->ShowMouseCursor(true);

	ParticleManager::GetInstance()->SetCamera(camera);

	ParticleManager::GetInstance()->CreateParticleGroupFromOBJ("Resources/Debug/obj", "plane.obj", "plane");

	ParticleManager::GetInstance()->CreateParticleGroup(ParticleType::plane, "Resources/Particle/circle2.png", "circle");

	//ParticleManager::GetInstance()->CreateParticleGroup("white", "Resources/Model/obj/circle2.png");

	Object3dBase::GetInstance()->SetDefaultCamera(camera);

	Transform pl = {
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.01f, 0.0f}
	};
	player_ = new Player();
	player_->Initialize(camera, input, pl, true);

	cameraObject = new Object3d();
	cameraObject->Initialize();
	cameraObject->SetModel("Resources/Debug", "CameraCone.obj", false);
	cameraObject->SetTranslate({ 5.0f, 0.0f, 0.0f });

	grid = new Object3d();
	grid->Initialize();
	grid->SetModel("Resources/Debug", "Grid.obj");
	grid->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });

	box = new Object3d();
	box->Initialize();
	//box->SetModel("Resources/Debug/obj", "box.obj");
	box->SetModel("Resources/Model/gltf/human", "walk.gltf", true, true);
	box->SetTranslate({ 0.0f, 1.0f, 0.0f });

	terrain = new Object3d();
	terrain->Initialize();
	terrain->SetModel("Resources/Debug/obj", "terrain.obj");
	terrain->SetTranslate({ 20.0f, 0.01f, 0.0f });
	terrain->SetEnableLighting(true);

	land = new Object3d();
	land->Initialize();
	land->SetModel("Resources/Debug", "LandPlate.obj", false);

	Audio::GetInstance()->LoadMP3("Resources/sekiranun.mp3", "bgm", 0.1f);

}

void GameScene::Update() {

	//ImGui::ShowDemoWindow();
	cameraTransform = camera->GetTransform();


#ifdef _DEBUG
	ImGui::Begin("State");
	ImGui::SetWindowPos(ImVec2{ 0.0f, 0.0f });
	ImGui::SetWindowSize(ImVec2{ 300.0f, float(WinApp::GetInstance()->GetkClientHeight()) });
	if (ImGui::TreeNode("Camera")) {
		ImGui::DragFloat3("Tranlate", &cameraTransform.translate.x, 0.1f);
		ImGui::DragFloat3("Rotate", &cameraTransform.rotate.x, 0.1f);
		ImGui::DragFloat3("Scale", &cameraTransform.scale.x, 0.1f);
		ImGui::TreePop();
	}
	/*if (ImGui::TreeNode("アニメーション")) {
		ImGui::DragFloat("walk", &walkspeed, 0.01f);
		ImGui::DragFloat("sneak", &sneakSpeed, 0.01f);
		ImGui::TreePop();
	}*/
	if (ImGui::Button("デバイス更新"))
	{
		input->UpdateDevice();
	}
	ImGui::DragFloat("カメラ速度", &speed, 0.01f);
	ImGuiIO& io = ImGui::GetIO();
	if (io.Framerate > 45)
	{
		ImGui::TextColored({ 0.0f, 1.0f, 0.0f, 1.0f }, "FPS: %.1f", io.Framerate);
	}
	else if (io.Framerate > 30 && io.Framerate < 45)
	{
		ImGui::TextColored({ 1.0f, 1.0f, 0.0f, 1.0f }, "FPS: %.1f", io.Framerate);
	}
	else
	{
		ImGui::TextColored({ 1.0f, 0.0f, 0.0f, 1.0f }, "FPS: %.1f", io.Framerate);
	}
	ImGui::Checkbox("マウスカーソル表示", &cursorshow);
	ImGui::End();

#endif // _DEBUG

	if (input->TriggerKey(DIK_ESCAPE))
	{
		finished = true;
	}

	if (input->TriggerKey(DIK_F11))
	{
		cursorshow = !cursorshow;
	}
	input->ShowMouseCursor(cursorshow);

	if (input->TriggerKey(DIK_1))
	{
		Audio::GetInstance()->Play2D("bgm", { 0.0f, 0.0f }, false);
	}

	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);
	//camera->SetTranslateParent(human->GetWorldMatrix());
	camera->Update();

	SkyBox::GetInstance()->Update();
	//camera->SetTranslateParent(human->GetWorldMatrix());
	//cameraObject->SetTransform(cameraTransform);
	cameraObject->Update();

	player_->Update();

	grid->Update();

	box->Update();


	terrain->Update();
	land->Update();
	//sprite->TriangleUpdate();

	input->Update();

}

void GameScene::Draw() {

	SpriteBase::GetInstance()->ShaderDraw();

	//sprite->Draw();

	Object3dBase::GetInstance()->ShaderDraw();

	terrain->Draw();

	land->Draw();

	cameraObject->Draw();


	SkinningObject3dBase::GetInstance()->ShaderDraw();

	box->Draw();
	player_->Draw();

	WireFrameObjectBase::GetInstance()->ShaderDraw();

	grid->Draw();

}

void GameScene::Finalize() {

	delete camera;

	delete player_;

	delete box;

	delete cameraObject;

	delete grid;

	delete terrain;

	delete land;

}