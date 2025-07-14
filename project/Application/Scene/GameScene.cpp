#include "GameScene.h"
#include "externels/imgui/imgui.h"
#include "externels/imgui/imgui_impl_dx12.h"
#include "externels/imgui/imgui_impl_win32.h"



void GameScene::Initialize() {

	//ModelManager::GetInstance()->LoadModel("Resources/Model/gltf/human", "walkMultiMaterial.gltf", true, true);

	camera = new Camera();
	camera->SetRotate(Vector3(SwapRadian(10.0f), 3.14f, 0.0f));
	camera->SetTranslate({ 0.0f, 2.8f, 8.0f });

	input = Input::GetInstance();
	input->ShowMouseCursor(true);

	ParticleManager::GetInstance()->SetCamera(camera);

	ParticleManager::GetInstance()->CreateParticleGroupFromOBJ("Resources/Model/obj", "plane.obj", "plane");

	ParticleManager::GetInstance()->CreateParticleGroup(ParticleType::plane, "Resources/Particle/circle2.png", "circle");

	//ParticleManager::GetInstance()->CreateParticleGroup("white", "Resources/Model/obj/circle2.png");

	Object3dBase::GetInstance()->SetDefaultCamera(camera);

	cameraObject = new Object3d();
	cameraObject->Initialize();
	cameraObject->SetModel("Resources/Debug", "CameraCone.obj", false);
	cameraObject->SetTranslate({ 5.0f, 0.0f, 0.0f });

	object3d = new Object3d();
	object3d->Initialize();
	object3d->SetModel("Resources/Model/gltf/human", "walkMultiMaterial.gltf", true, true);
	object3d->AddAnimation("Resources/Model/gltf/human", "sneakWalk.gltf", "sneak");
	object3d->SetTranslate({ 1.0f, 0.1f, 0.0f });
	object3d->SetEnableLighting(true);

	human = new Object3d();
	human->Initialize();
	human->SetModel("Resources/Model/gltf/char", "walk.gltf", true, true);
	human->AddAnimation("Resources/Model/gltf/char", "sneak.gltf", "sneak");
	human->SetTranslate({ 0.0f, 0.1f, 0.0f });

	grid = new Object3d();
	grid->Initialize();
	grid->SetModel("Resources/Debug", "Grid.obj");
	grid->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });

	terrain = new Object3d();
	terrain->Initialize();
	terrain->SetModel("Resources/Model/obj", "terrain.obj");
	terrain->SetTranslate({ 20.0f, 0.01f, 0.0f });
	terrain->SetEnableLighting(true);

	land = new Object3d();
	land->Initialize();
	land->SetModel("Resources/Debug", "LandPlate.obj", false);

	Audio::GetInstance()->LoadMP3("Resources/sekiranun.mp3", "bgm", 0.1f);

	sprite = new Sprite();
	sprite->Initialize("Resources/Debug/white1x1.png");
	leftTop = { 0.0f, 0.0f };
	transformSprite = {
		{200.0f, 200.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		{640.0f, 360.0f, 1.0f}
	};
	sprite->SetTransform(transformSprite);
	sprite->SetTextureLeftTop(leftTop);


	modelTransform = object3d->GetTransform();
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
	float walkspeed = human->GetAnimationSpeed();
	float sneakSpeed = object3d->GetAnimationSpeed();
	if (ImGui::TreeNode("アニメーション")) {
		ImGui::DragFloat("walk", &walkspeed, 0.01f);
		ImGui::DragFloat("sneak", &sneakSpeed, 0.01f);
		ImGui::TreePop();
	}
	if (ImGui::Button("デバイス更新"))
	{
		input->UpdateDevice();
	}
	human->SetAnimationSpeed(walkspeed);
	object3d->SetAnimationSpeed(sneakSpeed);
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
	ImGui::End();

#endif // _DEBUG


	if (input->TriggerKey(DIK_ESCAPE))
	{
		finished = true;
	}
	Vector3 velocity(0.0f, 0.0f, speed);
	velocity = TransformNormal(velocity, camera->GetWorldMatrix());
	if (input->PushKey(DIK_W)) {
		cameraTransform.translate += velocity;
	}
	if (input->PushKey(DIK_S)) {
		cameraTransform.translate -= velocity;
	}
	velocity = { speed, 0.0f, 0.0f };
	velocity = TransformNormal(velocity, camera->GetWorldMatrix());
	if (input->PushKey(DIK_A)) {
		cameraTransform.translate -= velocity;
	}
	if (input->PushKey(DIK_D)) {
		cameraTransform.translate += velocity;
	}
	if (input->PushKey(DIK_SPACE)) {
		cameraTransform.translate.y += speed;
	}
	if (input->PushKey(DIK_LSHIFT)) {
		cameraTransform.translate.y -= speed;
	}
	if (input->PushKey(DIK_LEFT)) {
		cameraTransform.rotate.y -= 0.03f;
	}
	if (input->PushKey(DIK_RIGHT)) {
		cameraTransform.rotate.y += 0.03f;
	}
	if (input->PushKey(DIK_UP)) {
		cameraTransform.rotate.x -= 0.03f;
	}
	if (input->PushKey(DIK_DOWN)) {
		cameraTransform.rotate.x += 0.03f;
	}
	if (input->PushKey(DIK_Q)) {
		cameraTransform.rotate.z -= 0.01f;
	}
	if (input->PushKey(DIK_E)) {
		cameraTransform.rotate.z += 0.01f;
	}
	if (input->TriggerKey(DIK_LCONTROL))
	{
		cursorshow = !cursorshow;
	}
	if (input->PushKey(DIK_F1))
	{
		speed -= 0.005f;
	}
	if (input->PushKey(DIK_F2))
	{
		speed += 0.005f;
	}
	if (input->TriggerKey(DIK_F4))
	{
		object3d->ToggleStartAnimation();
		human->ToggleStartAnimation();
	}
	if (input->TriggerKey(DIK_F3))
	{
		object3d->ResetAnimationTime();
		human->ResetAnimationTime();
	}
	if (input->TriggerKey(DIK_F5))
	{
		object3d->ResetAnimationSpeed();
		human->ResetAnimationSpeed();
		//object3d->PlayDefaultAnimation();
	}
	if (input->TriggerKey(DIK_1))
	{
		human->ChangePlayAnimation("sneak");
		object3d->ChangePlayAnimation("sneak");
	}
	if (input->TriggerKey(DIK_2))
	{
		human->ChangePlayAnimation();
		object3d->ChangePlayAnimation();
	}
	if (input->TriggerButton(Controller::X))
	{
		sneak = !sneak;
		if (sneak)
		{
			human->ChangePlayAnimation("sneak");
		}
		else
		{
			human->ChangePlayAnimation();
		}
	}
	input->ShowMouseCursor(cursorshow);
	speed = max(0.0f, speed);

	Vector2 pos = input->GetMouseVel2();

	cameraTransform.rotate.y += SwapRadian(pos.x) / 4.0f;
	cameraTransform.rotate.x += SwapRadian(pos.y) / 4.0f;

	//Vector3 normHand = Normalize(object3d->GetJointNormal("LeftHand"));

	//normHand = normHand * Vector3{ 0.0f, 1.0f, 0.0f };

	if (input->TriggerKey(DIK_Y)) {
		ParticleManager::GetInstance()->Emit("circle", { 0.0f, 2.0f, 0.0f }, 10);
	}
	if (input->TriggerKey(DIK_T)) {
		ParticleManager::GetInstance()->Emit("plane", { 0.0f, 2.0f, 0.0f }, 10);
	}

	/*if (input->TriggerKey(DIK_1))
	{
		Audio::GetInstance()->Play2D("bgm", { 0.0f, 0.0f }, false);
	}*/
	// ジョイスティックを使ったプレイヤーの移動
	Transform humanTransform = human->GetTransform();
	Vector3 joyPos = input->GetLeftJoyStickPos3(0.0f);
	Vector3 rotate = human->GetRotate();
	rotate.y = input->GetJoyStickDirection3(joyPos) + camera->GetRotate().y;
	velocity = { 0.0f, 0.0f, 0.031f};
	if (sneak)
	{
		velocity.z = 0.032f;
	}
	Transform cam = camera->GetTransform();
	cam.rotate.x = 0.0f;
	cam.rotate.z = 0.0f;
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cam.scale, cam.rotate, cam.translate);
	velocity = TransformNormal(velocity, cameraMatrix);

	ImGui::Begin("Joy");
	ImGui::Text("%f", input->GetJoyStickLength(joyPos));
	ImGui::End();
	if (input->IsMoveLeftJoyStick())
	{
		humanTransform.rotate.y = rotate.y;
		humanTransform.translate += input->GetJoyStickVelocity(joyPos, velocity);
		// ジョイスティックが動いているときのみアニメーションさせる
		human->SetStartAnimation(true);
		// ジョイスティックの倒す大きさによってアニメーションの速度を変更する
		human->SetAnimationSpeed(input->GetJoyStickLength(joyPos));
	}
	else
	{
		human->SetStartAnimation(false);
	}
	human->SetTransform(humanTransform);

	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);
	//camera->SetTranslateParent(human->GetWorldMatrix());
	camera->Update();
	//cameraObject->SetTransform(cameraTransform);
	cameraObject->Update();

	object3d->SetTransform(modelTransform);
	//object3d->SetEnableLighting(enableLighting);
	object3d->Update();

	human->Update();

	aabb = object3d->GetAABB();
	sprite->SetTransform(transformSprite);
	sprite->SetTextureLeftTop(leftTop);

	grid->Update();

	terrain->Update();
	land->Update();
	//sprite->TriangleUpdate();

	input->Update();

	ParticleManager::GetInstance()->Update();

}

void GameScene::Draw() {

	SpriteBase::GetInstance()->ShaderDraw();

	//sprite->Draw();

	Object3dBase::GetInstance()->ShaderDraw();

	terrain->Draw();

	land->Draw();

	cameraObject->Draw();

	SkinningObject3dBase::GetInstance()->ShaderDraw();

	object3d->Draw();
	human->Draw();

	WireFrameObjectBase::GetInstance()->ShaderDraw();
	

	grid->Draw();

	//ParticleManager::GetInstance()->Draw();

}

void GameScene::Finalize() {

	delete camera;

	delete cameraObject;

	delete object3d;

	delete human;

	delete grid;
	delete terrain;
	delete land;

	delete sprite;

}