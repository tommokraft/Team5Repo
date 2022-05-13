#include "Player.h"
#include "Framework/T5Global.h"
#include "Framework/T5GameObject.h"
#include "Framework/T5PhysicsObject.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h> 
#include "PaintGround.h"

void Player::Init(Class playerClass) {
	srand(time(NULL));
	gunBase->player = this;
	collisionHolder =  vector<string>();
	switch (playerClass) {
	case Player::Class::SCOUT: {
		movementSpeed = 16;
		health = 75;
		maxHealth = 75;
		shield = 75;
		maxShield = 75;
		jumpHeight = 750;
		ability1Cooldown = 5;
		ability2Cooldown = 15;
		ability3Cooldown = 60;
		classType = playerClass;
		gunBase->weaponName = "Shotgun";
		gunBase->weaponType = 1;
		gunBase->ammo = 8;
		gunBase->maxAmmo = 8;
		gunBase->damage = 14;
		gunBase->gunForce = 100.0f;
		gunBase->shotCount = 8;
		gunBase->fireRate = 100.0f;
		gunBase->isAutomatic = false;
		gunBase->canShoot = true;
		gunBase->shotCooldown = false;
		gunBase->gunForce = 125;
		gunBase->reloadSpeed = 800.0f;
		canDoubleJump = true;
		hasDoubleJump = true;
		break;
	}
	case Player::Class::TANK: {
		movementSpeed = 10;
		health = 100;
		maxHealth = 100;
		shield = 100;
		maxShield = 100;
		jumpHeight = 350;
		ability1Cooldown = 15;
		ability2Cooldown = 30;
		ability3Cooldown = 60;
		classType = playerClass;
		gunBase->weaponName = "LMG";
		gunBase->weaponType = 2;
		gunBase->ammo = 50;
		gunBase->maxAmmo = 50;
		gunBase->damage = 35;
		gunBase->gunForce = 250.0f;
		gunBase->shotCount = 1;
		gunBase->fireRate = 250.0f;
		gunBase->recoilAmount = 5;
		gunBase->recoilTime = 800.0f; //time between shots for recoil to reset
		gunBase->isAutomatic = true;
		gunBase->canShoot = true;
		gunBase->shotCooldown = false;
		gunBase->gunForce = 125;
		gunBase->reloadSpeed = 1200.0f;
		break;
	}
	case Player::Class::HEALER: {
		movementSpeed = 12;
		health = 50;
		maxHealth = 50;
		shield = 75;
		maxShield = 75;
		jumpHeight = 500;
		ability1Cooldown = 15;
		ability2Cooldown = 30;
		ability3Cooldown = 60;
		classType = playerClass;
		gunBase->weaponName = "Pistol";
		gunBase->weaponType = 3;
		gunBase->ammo = 15;
		gunBase->maxAmmo = 15;
		gunBase->damage = 21;
		gunBase->shotCount = 1;
		gunBase->fireRate = 5.0f;
		gunBase->isAutomatic = false;
		gunBase->canShoot = true;
		gunBase->shotCooldown = false;
		gunBase->gunForce = 125;
		gunBase->reloadSpeed = 500.0f;
		break;
	}
	case Player::Class::GRENADIER: {
		movementSpeed = 12;
		health = 100;
		maxHealth = 100;
		shield = 100;
		maxShield = 100;
		jumpHeight = 400;
		ability1Cooldown = 15;
		ability2Cooldown = 30;
		ability3Cooldown = 60;
		classType = playerClass;
		gunBase->weaponName = "AssaultRifle";
		gunBase->weaponType = 4;
		gunBase->ammo = 30;
		gunBase->maxAmmo = 30;
		gunBase->damage = 37;
		gunBase->shotCount = 1;
		gunBase->fireRate = 200.0f;
		gunBase->isAutomatic = true;
		gunBase->canShoot = true;
		gunBase->shotCooldown = false;
		gunBase->gunForce = 125;
		gunBase->reloadSpeed = 600.0f;

		break;
	}
	}
	
	if (!init) {
		m_hud.reset(new PlayerHUD());
		m_hud->player = this;
		T5HudManager::GetInstance()->PushHud(m_hud);
	}

	collisionRemoveTimer = chrono::high_resolution_clock::now();
	keyboard = T5Global::sMainWindow->GetKeyboard();
	mouse = T5Global::sMainWindow->GetMouse();
	init = true;
	//gamepad = T5Global::sMainWindow->GetGamePad(0);
}

void Player::CreateBullet() {
	
	auto bullet = T5Global::sGame->CreateGameObject();

	auto bulletPC = T5CreateComponent<T5PrimitiveComponent>();
	bullet->m_name = "Bullet";	
	auto painter = T5CreateComponent<Painter>();
	if (GetGameObject()->m_name == "Player1") {
		painter->m_team = PainterTeam::A;
	}
	else {
		painter->m_team = PainterTeam::B;
	}
	painter->m_painterProperties.radius = 1;
	painter->m_painterProperties.hardness = 1;
	painter->m_painterProperties.paintStrength = 1;
	painter->m_painterProperties.normalClampAngle = 90;
	bullet->AddComponent(painter);
	T5PxMaterial bulletPhysicsMat;
	T5PhysicsObject* bulletPhysics = T5CreateComponent<T5PhysicsObject>();
	bulletPhysics->SetColliderAsSphere(0.15, bulletPhysicsMat);
	bulletPhysics->SetMass(0.01);
	bulletPhysics->SetAffectedByGravity(true);
	bulletPhysics->SetCollisionChannel(CC_Dynamic, CC_Static | CC_Dynamic);
	bullet->AddComponent(bulletPhysics);
	auto splashAudio = T5CreateComponent<T5AudioPlayer>();
	splashAudio->SetSound(T5Global::sAssetManager->GetSound("Splat"), false);
	bullet->AddComponent(splashAudio);

	bulletPC->SetMesh(bulletMesh);
	bulletPC->SetMaterial(bulletMat);
	bullet->AddComponent(bulletPC);
	auto bulletWorld = bullet->GetWorldTransform();

	if (gunBase->weaponType == 1) {
		float rotX = rand() % 200 + -100;
		float rotY = rand() % 200 + -100;
		float rotZ = rand() % 150 + -50;
		rotX = rotX / 100;
		rotY = rotY / 100;
		rotZ = rotZ / 100;
		bulletWorld.SetPosition(T5Vec3(playerCam->GetLocalPosition().x + rotX, playerCam->GetLocalPosition().y + rotY, playerCam->GetLocalPosition().z) + (playerCam->GetLocalTransform().GetForward() * 0.5f * rotZ));
	}
	else {
		bulletWorld.SetPosition(T5Vec3(playerCam->GetWorldPosition().x, playerCam->GetWorldPosition().y, playerCam->GetWorldPosition().z) + (playerCam->GetLocalTransform().GetForward() * 0.5f));
	}
	bulletWorld.SetScale(T5Vec3(0.15, 0.15, 0.15));
	bullet->SetWorldTransform(bulletWorld);

	PaintGround* paint = T5CreateComponent<PaintGround>();
	paint->collisionHolder = &collisionHolder;
	//paint->exampleGame = T5Global::sGame;
	bullet->AddComponent(paint);
	paint->splashAudio = splashAudioPlayer;
	gunBase->bullet = bulletPC;

	bulletPhysics->AddForce(T5Vec3(playerCam->GetWorldTransform().GetForward() * gunBase->gunForce));
	
	paint->radius = 0.5;
	paint->damage = gunBase->damage;
	paint->player = this;
}

void Player::SquidMode() 
{
	T5Vec3 pos = GetGameObject()->GetWorldPosition();
	T5Vec3 roundedPos = T5Vec3(floor(pos.x), -3, floor(pos.z));
	int teamColour;
	if (this->GetGameObject()->GetComponent<Painter>()->m_team == PainterTeam::A) {
		teamColour = 1;
	}
	else if(this->GetGameObject()->GetComponent<Painter>()->m_team == PainterTeam::B) {
		teamColour = 2;
	}
	if (controllerId >= 0) {
		if (gamepad->KeyHeld(T5GamePadKey::Down))
		{
			//CHECK IF THE POSITION THAT THE PLAYER IS CURRENTLY IN IS A PAINTED POSITION
			auto itr = std::find(SamplePoints::Instance->samplePoints.begin(), SamplePoints::Instance->samplePoints.end(), roundedPos);
			if (itr == SamplePoints::Instance->samplePoints.end()) return;
			int index = itr - SamplePoints::Instance->samplePoints.begin();


			if (SamplePoints::Instance->samplePointsColours[index] == teamColour) {
				squidEntered = true;
				auto end = chrono::high_resolution_clock::now();
				ms_double = end - fireRateTimer;
				//movespeed ++
				squidMulti = 1.75;

				//ScaleY * 0.75
				gunBase->reloading = true;
				gunBase->canShoot = false;

				GetGameObject()->SetScale(T5Vec3(1, 0.5, 1));

				if (ms_double.count() >= 0.1)
				{
					reloadTimer = chrono::high_resolution_clock::now();
					gunBase->ReloadWeapon();
				}
			}

			else {
				squidMulti = 1;
				GetGameObject()->SetScale(T5Vec3(1, 2, 1));
			}
		}
		else
		{
			if (squidEntered) {
				gunBase->canShoot = true;
				gunBase->reloading = false;
				squidMulti = 1;
				GetGameObject()->SetScale(T5Vec3(1, 2, 1));
			}
			GetGameObject()->SetScale(T5Vec3(1, 2, 1));
			squidMulti = 1;
		}
	}

	if (controllerId < 0) {
		/*if (keyboard->KeyDown(T5KeyboardKeys::SHIFT))
		{
			auto itr = std::find(SamplePoints::Instance->samplePoints.begin(), SamplePoints::Instance->samplePoints.end(), roundedPos);
			if (itr == SamplePoints::Instance->samplePoints.end()) return;
			int index = itr - SamplePoints::Instance->samplePoints.begin();
			if (SamplePoints::Instance->samplePointsColours[index] == 1 || SamplePoints::Instance->samplePointsColours[index] == 2) {
				squidEntered = true;
				reloadTimer = chrono::high_resolution_clock::now();
			}
			else {
				squidEntered = false;
			}
		}*/
		if (keyboard && keyboard->KeyHeld(T5KeyboardKeys::SHIFT))
		{
			auto itr = std::find(SamplePoints::Instance->samplePoints.begin(), SamplePoints::Instance->samplePoints.end(), roundedPos);
			if (itr == SamplePoints::Instance->samplePoints.end()) return;
			int index = itr - SamplePoints::Instance->samplePoints.begin();
			if (SamplePoints::Instance->samplePointsColours[index] == teamColour) {
				squidEntered = true;
				gunBase->reloading = false;
				squidMulti = 1;
				GetGameObject()->SetScale(T5Vec3(1, 2, 1));
				auto end = chrono::high_resolution_clock::now();
				ms_double = end - fireRateTimer;
				//movespeed ++
				squidMulti = 1.75;

				//ScaleY * 0.75
				gunBase->reloading = true;
				gunBase->canShoot = false;

				GetGameObject()->SetScale(T5Vec3(1, 0.5, 1));


				if (ms_double.count() >= 0.1)
				{
					reloadTimer = chrono::high_resolution_clock::now();
					gunBase->ReloadWeapon();
				}
			}

			else {
				squidMulti = 1;
				GetGameObject()->SetScale(T5Vec3(1, 2, 1));
			}

		}
		else
		{
			if (squidEntered) {
				gunBase->canShoot = true;
				gunBase->reloading = false;
				squidMulti = 1;
				GetGameObject()->SetScale(T5Vec3(1, 2, 1));
			}
			GetGameObject()->SetScale(T5Vec3(1, 2, 1));
			squidMulti = 1;
		}

	}
	squidEntered = false;
}

void Player::TakeDamage(float damage) {
	if (shield <= 0)
	{
		health -= damage;
	}
	else
	{
		shield -= damage;
		if (shield<= 0)
		{
			health += shield;
			shield = 0;
		}
	}
}

bool Player::ReceiveMovementInput(T5Vec2& dir, bool& jump)
{
	/*T5Vec3 camForward = playerCam->GetLocalTransform().GetForward();
	T5Vec3 camRight = playerCam->GetLocalTransform().GetRight();
	T5Vec2 moveForward = T5Vec2(camForward.x, camForward.z).Normalised();
	T5Vec2 moveRight = T5Vec2(camRight.x, camRight.z).Normalised();*/

	dir = T5Vec2();
	jump = false;

	if (controllerId < 0) {
		if (!keyboard) return false;

		if (keyboard->KeyPressed(T5KeyboardKeys::SPACE))
		{
			jump = true;
		}
		if (keyboard && keyboard->KeyHeld(T5KeyboardKeys::W))
		{
			//dir += moveForward;
			dir += T5Vec2(0, 1);
		}
		if (keyboard && keyboard->KeyHeld(T5KeyboardKeys::S))
		{
			//dir -= moveForward;
			dir += T5Vec2(0, -1);
		}
		if (keyboard && keyboard->KeyHeld(T5KeyboardKeys::A))
		{
			//dir -= moveRight;
			dir += T5Vec2(-1, 0);
		}
		if (keyboard && keyboard->KeyHeld(T5KeyboardKeys::D))
		{
			//dir += moveRight;
			dir += T5Vec2(1, 0);
		}
		dir.Normalise();

	}
	else {
		if (!gamepad) return false;

		if (gamepad->KeyPressed(T5GamePadKey::A))
		{
			jump = true;
		}
		if (std::abs(gamepad->GetAxisValue(T5GamePadAxis::LeftX)) > 0.1f)
		{
			dir.x = gamepad->GetAxisValue(T5GamePadAxis::LeftX);
			//dir += moveRight * gamepad->GetAxisValue(T5GamePadAxis::LeftX);
		}
		if (std::abs(gamepad->GetAxisValue(T5GamePadAxis::LeftY)) > 0.1f)
		{
			dir.y = gamepad->GetAxisValue(T5GamePadAxis::LeftY);
			//dir += moveForward * gamepad->GetAxisValue(T5GamePadAxis::LeftY);
		}
		dir.Normalise();
		/*T5Vec3 totalMovement;
		if (std::abs(moveForward.x) > std::abs(moveForward.z)) {
			totalMovement = T5Vec2(0, gamepad->GetAxisValue(T5GamePadAxis::LeftX)) * moveRight + T5Vec2(gamepad->GetAxisValue(T5GamePadAxis::LeftY), 0) * moveForward;
		}
		else {
			totalMovement = T5Vec2(gamepad->GetAxisValue(T5GamePadAxis::LeftX), 0) * moveRight + T5Vec2(0, gamepad->GetAxisValue(T5GamePadAxis::LeftY)) * moveForward;
		}*/
	}
	return true;
}

void Player::Movement() {
	
	auto po = GetGameObject()->GetComponent<T5PhysicsObject>();
	if (!po) return;

	playerCam->SetWorldPosition(GetGameObject()->GetWorldPosition() + T5Vec3(0, 1, 0));
	GetGameObject()->SetWorldRotation(T5Vec3(0, playerCam->GetWorldRotation().y, 0));

	T5Vec3 camForward = playerCam->GetLocalTransform().GetForward();
	T5Vec3 camRight = playerCam->GetLocalTransform().GetRight();
	T5Vec2 moveForward = T5Vec2(camForward.x, camForward.z).Normalised();
	T5Vec2 moveRight = T5Vec2(camRight.x, camRight.z).Normalised();
	
	float vUp = po->GetLinearVelocity().y;
	T5Vec2 localDir(0,0);
	bool moveJump = false;

	if (!ReceiveMovementInput(localDir, moveJump)) return;

	T5Vec2 moveDir = moveForward * localDir.y + moveRight * localDir.x;
	moveDir *= squidMulti * movementSpeed;
	float moveSpeed = moveDir.Length();

	if (moveJump)
	{
		if (canMove && std::abs(vUp) < 0.1)
		{
			po->AddForce(T5Vec3(0, jumpHeight, 0));
			isJumping = true;
			jumpAudioPlayer->PlaySound();
			skeleton->StopAnimation(12);
		}
		else if (canDoubleJump && hasDoubleJump)
		{
			po->AddForce(T5Vec3(0, jumpHeight, 0));
			canDoubleJump = false;
			jumpAudioPlayer->PlaySound();
			skeleton->StopAnimation(12);
		}
	}
	if (canMove /*&& !isJumping*/)
	{
		po->SetLinearVelocity(T5Vec3(moveDir.x, vUp, moveDir.y));
		if (moveSpeed > 0)
		{
			float localLength = localDir.Length();
			float angle = -1 * std::asin(localDir.x / localLength) / 3.1415 * 180.0f;
			if (localDir.y < 0)
				angle = angle > 0 ? 180.f - angle : -180.0f - angle;
			skeletonObj->SetLocalRotation(T5Vec3(0, angle, 0));
		}
		if (!isMoving && moveSpeed > 0)
		{
			footAudioPlayer->PlaySound();
			skeleton->PlayAnimation(true);
		}
		else if(moveSpeed == 0)
		{
			footAudioPlayer->PauseSound();
			skeleton->StopAnimation();
		}
	}
	isMoving = moveSpeed > 0 || moveJump;

	if (std::abs(vUp) < 0.1) {
		if (isMoving)skeleton->PlayAnimation(true);
		isJumping = false;
		canDoubleJump = true;
	}
	
}

void Player::Abilities() {
	auto physicsObject = GetGameObject()->GetComponent<T5PhysicsObject>();
	if (controllerId < 0) {
		
		if (keyboard && keyboard->KeyPressed(T5KeyboardKeys::Q) && !startAbility1 && !ability1OnCooldown)
		{
			ability1OnCooldown = true;

			switch (classType) {
			case Class::SCOUT:
				break;
			case Class::TANK:
				shieldAudioPlayer->PlaySound();
				break;
			case Class::GRENADIER:
				break;
			case Class::HEALER:
				healPackAudioPlayer->PlaySound();
				break;

			}

			//audioSystem->playSound(false, shieldAudio);
			gunBase->canShoot = false;
			startAbility1 = true;
			startAbilityCooldown1 = chrono::high_resolution_clock::now();
			startAbility1Timer = chrono::high_resolution_clock::now();
		}
		if (keyboard && keyboard->KeyPressed(T5KeyboardKeys::Q) && !startAbility1 && ability1OnCooldown)
		{
			std::cout << "ability 1 on cooldown" << std::endl;
		}

		if (keyboard && keyboard->KeyPressed(T5KeyboardKeys::E) && !startAbility2 && !ability2OnCooldown)
		{
			ability2OnCooldown = true;

			switch (classType) {
			case Class::SCOUT:
				dashAudioPlayer->PlaySound();
				break;
			case Class::TANK:
				channelAudioPlayer->PlaySound();
				break;
			case Class::GRENADIER:
				break;
			case Class::HEALER:
				AOEHealAudioPlayer->PlaySound();
				break;
			}

			startAbility2 = true;
			startAbilityCooldown2 = chrono::high_resolution_clock::now();
			startAbility2Timer = chrono::high_resolution_clock::now();
		}
		if (keyboard && keyboard->KeyPressed(T5KeyboardKeys::E) && ability2OnCooldown) {
			std::cout << "ability 2 on cooldown" << std::endl;
		}

		if (keyboard && keyboard->KeyPressed(T5KeyboardKeys::V) && !startAbility3 && !ability3OnCooldown)
		{
			ability3OnCooldown = true;

			switch (classType) {
			case Class::SCOUT:
				break;
			case Class::TANK:
				gunChangeAudioPlayer->PlaySound();
				break;
			case Class::GRENADIER:
				gunChangeAudioPlayer->PlaySound();
				break;
			case Class::HEALER:
				teamHealAudioPlayer->PlaySound();
				break;
			}

			startAbility3 = true;
			startAbilityCooldown3 = chrono::high_resolution_clock::now();
			startAbility3Timer = chrono::high_resolution_clock::now();

		}
		if (keyboard && keyboard->KeyPressed(T5KeyboardKeys::V) && ability3OnCooldown) {
			std::cout << "ability 3 on cooldown" << std::endl;
		}
	}
	else {
		if (gamepad && gamepad->KeyPressed(T5GamePadKey::B) && !startAbility1 && !ability1OnCooldown)
		{
			ability1OnCooldown = true;

			switch (classType) {
			case Class::SCOUT:
				break;
			case Class::TANK:
				shieldAudioPlayer->PlaySound();
				break;
			case Class::GRENADIER:
				break;
			case Class::HEALER:
				healPackAudioPlayer->PlaySound();
				break;

			}

			startAbility1 = true;
			startAbilityCooldown1 = chrono::high_resolution_clock::now();
			startAbility1Timer = chrono::high_resolution_clock::now();
		}
		if (gamepad && gamepad->KeyPressed(T5GamePadKey::B) && !startAbility1 && ability1OnCooldown)
		{
			std::cout << "ability 1 on cooldown" << std::endl;
		}

		if (gamepad && gamepad->KeyPressed(T5GamePadKey::Y) && !startAbility2 && !ability2OnCooldown)
		{
			ability2OnCooldown = true;

			switch (classType) {
			case Class::SCOUT:
				dashAudioPlayer->PlaySound();
				break;
			case Class::TANK:
				channelAudioPlayer->PlaySound();
				break;
			case Class::GRENADIER:
				break;
			case Class::HEALER:
				AOEHealAudioPlayer->PlaySound();
				break;
			}

			startAbility2 = true;
			startAbilityCooldown2 = chrono::high_resolution_clock::now();
			startAbility2Timer = chrono::high_resolution_clock::now();
		}
		if (gamepad && gamepad->KeyPressed(T5GamePadKey::Y) && ability2OnCooldown) {
			std::cout << "ability 2 on cooldown" << std::endl;
		}
		
		if (gamepad && gamepad->KeyPressed(T5GamePadKey::ShoulderL) && !startAbility3 && !ability3OnCooldown)
		{
			ability3OnCooldown = true;

			switch (classType) {
			case Class::SCOUT:
				break;
			case Class::TANK:
				gunChangeAudioPlayer->PlaySound();
				break;
			case Class::GRENADIER:
				gunChangeAudioPlayer->PlaySound();
				break;
			case Class::HEALER:
				teamHealAudioPlayer->PlaySound();
				break;
			}

			startAbility3 = true;
			startAbilityCooldown3 = chrono::high_resolution_clock::now();
			startAbility3Timer = chrono::high_resolution_clock::now();

		}
		if (gamepad && gamepad->KeyPressed(T5GamePadKey::ShoulderL) && ability3OnCooldown) {
			std::cout << "ability 3 on cooldown" << std::endl;
		}
	}

	if (startAbility1) {
		auto end = chrono::high_resolution_clock::now();
		duration<double, std::milli> ms_double = end - startAbility1Timer;
		
		switch (classType)
		{
		case Player::Class::SCOUT:
			break;
		case Player::Class::TANK:

			if (ms_double.count() < 5000) {
				UseAbility(1);
			}
			else {
				startAbility1 = false;
				gunBase->canShoot = true;
				shield = 100;
			}
			break;
		case Player::Class::HEALER:
			if (healpackmade) {
				if (ms_double.count() < 5000) {
					UseAbility(1);
				}
				else {
					startAbility1 = false;
					auto healPackObj = T5Global::sGame->GetScene()->FindGameObject("HealPack");
					T5Global::sGame->DestroyGameObject(healPackObj);
					healpackmade = false;
				}
			}
			else {
				auto healPackPC = T5CreateComponent<T5PrimitiveComponent>();
				T5PhysicsObject* healPackPhysics = T5CreateComponent<T5PhysicsObject>();
				T5PxMaterial healpackMat;
				auto healPack = T5Global::sGame->CreateGameObject();
				auto healAudio = T5CreateComponent<T5AudioPlayer>();
				healAudio->SetSound(T5Global::sAssetManager->GetSound("HealHum"), false);
				healPackPhysics->SetColliderAsBox(1,0.5,1, healpackMat);
				healPackPhysics->SetCollisionChannel(CC_Dynamic, CC_Static | CC_Dynamic);
				healPack->AddComponent(healPackPhysics);
				healPack->AddComponent(healAudio);
				healAudio->PlaySound();
				healPackPC->SetMesh(cubeMesh);
				healPackPC->SetMaterial(healthMat);
				healPack->AddComponent(healPackPC);
				healPack->SetWorldPosition(GetGameObject()->GetWorldPosition() + T5Vec3(0, 1, -2));
				healPack->SetScale(T5Vec3(1, 0.5, 1));
				healPackPhysics->SetMass(0.5);
				healPackPhysics->SetAffectedByGravity(true);
				healPack->m_name = "HealPack";
				healpackmade = true;
			}
			
			break;
		case Player::Class::GRENADIER:
			UseAbility(1);
			startAbility1 = false;
			break;
		}
	}
	if (startAbility2) {
		auto end = chrono::high_resolution_clock::now();
		duration<double, std::milli> ms_double = end - startAbility2Timer;
		
		switch (classType)
		{
		case Player::Class::SCOUT:
			if (ms_double.count() < 500) {
				UseAbility(2);
			}
			else {
				startAbility2 = false;
				movementSpeed = 16;
			}
			break;
		case Player::Class::TANK:

			if (ms_double.count() < 5000) {
				UseAbility(2);
			}
			else {
				startAbility2 = false;
				gunBase->fireRate = 250.0f;
				shield = 100;
			}
			break;
		case Player::Class::HEALER:
			if (ms_double.count() < 10000) {
				UseAbility(2);
			}
			else {
				startAbility2 = false;
				gunBase->canShoot = true;
			}
			break;
		case Player::Class::GRENADIER:
			UseAbility(2);
			startAbility2 = false;
			break;
		}

	}
	if (startAbility3) {
		auto end = chrono::high_resolution_clock::now();
		duration<double, std::milli> ms_double = end - startAbility3Timer;
		switch (classType)
		{
		case Player::Class::SCOUT:
			if (ms_double.count() < 10000) {
				UseAbility(3);
			}
			else {
				startAbility3 = false;
				jetpackAudioPlayer->PauseSound();
			}
			break;
		case Player::Class::TANK:

			if (ms_double.count() < 15000) {
				UseAbility(3);
			}
			else {
				startAbility3 = false;
				gunBase->fireRate = 250.0f;
				gunBase->ammo = 50;
				canMove = true;
			}
			break;
		case Player::Class::HEALER:
			if (ms_double.count() < 15000) {
				UseAbility(3);
			}
			else {
				startAbility3 = false;
				gunBase->canShoot = true;
			}
			break;
		case Player::Class::GRENADIER:
			if (shotsLeftOnGL > 0) {
				gunBase->canShoot = false;
				UseAbility(3);
			}
			else {
				gunBase->canShoot = true;
				startAbility3 = false;
				shotsLeftOnGL = 6;
			}
			break;
		}
	}
	if (ability1OnCooldown) {
		auto timeNow = chrono::high_resolution_clock::now();
		duration<double, std::milli> ms_double = timeNow - startAbilityCooldown1;
		if (ms_double.count() > ability1Cooldown * 1000) {
			ability1OnCooldown = false;
		}
	}
	if (ability2OnCooldown) {
		auto timeNow = chrono::high_resolution_clock::now();
		duration<double, std::milli> ms_double = timeNow - startAbilityCooldown2;
		if (ms_double.count() > ability2Cooldown * 1000) {
			ability2OnCooldown = false;
		}
	}
	if (ability3OnCooldown) {
		auto timeNow = chrono::high_resolution_clock::now();
		duration<double, std::milli> ms_double = timeNow - startAbilityCooldown3;
		if (ms_double.count() > ability3Cooldown * 1000) {
			ability3OnCooldown = false;
		}
	}
}

void Player::Shooting()
{
	if (controllerId < 0) {
		if (mouse && mouse->ButtonHeld(T5MouseButtons::LEFT) && gunBase->isAutomatic && !gunBase->shotCooldown && gunBase->canShoot && gunBase->ammo != 0 && !gunBase->reloading)
		{
			for (int i = 0; i < gunBase->shotCount; i++) {
				CreateBullet();
			}
			gunBase->shotCooldown = true;
			gunBase->FireWeapon();
			fireRateTimer = chrono::high_resolution_clock::now();
		}
		if (mouse && mouse->ButtonPressed(T5MouseButtons::LEFT) && !gunBase->isAutomatic && !gunBase->shotCooldown && gunBase->canShoot && gunBase->ammo != 0 && !gunBase->reloading)
		{
			for (int i = 0; i < gunBase->shotCount; i++) {
				CreateBullet();
			}
			gunBase->shotCooldown = true;
			gunBase->FireWeapon();
			fireRateTimer = chrono::high_resolution_clock::now();
		}	
		/*if (keyboard && keyboard->KeyPressed(T5KeyboardKeys::R) && !gunBase->reloading) {
			gunBase->reloading = true;
			gunBase->canShoot = false;
			reloadTimer = chrono::high_resolution_clock::now();
			gunBase->ReloadWeapon();
		}*/
	}
	else {
		if (gamepad && gamepad->KeyHeld(T5GamePadKey::ShoulderR) && gunBase->isAutomatic && !gunBase->shotCooldown && gunBase->canShoot && gunBase->ammo != 0 && !gunBase->reloading)
		{
			for (int i = 0; i < gunBase->shotCount; i++) {
				CreateBullet();
			}
			gunBase->shotCooldown = true;
			gunBase->FireWeapon();
			fireRateTimer = chrono::high_resolution_clock::now();
		}
		if (gamepad && gamepad->KeyPressed(T5GamePadKey::ShoulderR) && !gunBase->isAutomatic && !gunBase->shotCooldown && gunBase->canShoot && gunBase->ammo != 0 && !gunBase->reloading)
		{
			for (int i = 0; i < gunBase->shotCount; i++) {
				CreateBullet();
			}
			gunBase->shotCooldown = true;
			gunBase->FireWeapon();
			fireRateTimer = chrono::high_resolution_clock::now();
		}
		/*if (gamepad && gamepad->KeyPressed(T5GamePadKey::X) && !gunBase->reloading) {
			gunBase->reloading = true;
			gunBase->canShoot = false;
			reloadTimer = chrono::high_resolution_clock::now();
			gunBase->ReloadWeapon();
		}*/

	}
	if (gunBase->reloading) {
		auto end = chrono::high_resolution_clock::now();
		ms_double = end - reloadTimer;

		if (ms_double.count() >= gunBase->reloadSpeed) {
			//std::cout << ms_double.count() << std::endl;
			gunBase->reloading = false;
			gunBase->canShoot = true;
		}
	}
	if (gunBase->shotCooldown) {
		auto end = chrono::high_resolution_clock::now();
		ms_double = end - fireRateTimer;

		if (ms_double.count() >= gunBase->fireRate) {
		//	std::cout << ms_double.count() << std::endl;
			gunBase->shotCooldown = false;
		}

	}
}

void Player::Update(float dt) {

	auto end = chrono::high_resolution_clock::now();
	ms_double = end - collisionRemoveTimer;
	if (collisionHolder.size() > 0 )
	{
		if (ms_double.count() >= 5000/collisionHolder.size())
		{
			collisionHolder.erase(collisionHolder.begin()+ collisionHolder.size()-1);
			collisionRemoveTimer = chrono::high_resolution_clock::now();
		}
	}

	if (controllerId >= 0)
	{
		gamepad = T5Global::sMainWindow->GetGamePad(controllerId);
	}
	

	if (health <= 0) {
		this->Init(classType);
		if (GetGameObject()->GetComponent<Painter>()->m_team == PainterTeam::A) {
			GetGameObject()->SetWorldPosition(T5Vec3(35, -1, 35));
		}
		if (GetGameObject()->GetComponent<Painter>()->m_team == PainterTeam::B) {
			GetGameObject()->SetWorldPosition(T5Vec3(-35, -1, -35));
		}
	}

	auto camController = playerCam->GetComponent<PlayerCameraController>();
	if(camController)
	{
		camController->controllerId = controllerId;
	}
	
	Movement();
	Shooting();
	Abilities();
	SquidMode();

	/*if (keyboard->KeyPressed(T5KeyboardKeys::C))
	{
		auto camComp = playerCam->GetComponent<T5CameraComponent>();
		if (camComp)
		{
			if (camComp->IsActive())
			{
				camComp->Deactivate();
			}
			else
			{
				camComp->Activate();
			}
		}
		
	}*/

	//std::cout << "Health: "<< health << " Shield: "<< shield << std::endl;
}

void Player::UseAbility(int ability) {
	auto healPack = T5Global::sGame->GetScene()->FindGameObject("HealPack");
	switch (classType)
	{
	case Player::Class::SCOUT:
		switch (ability) {
		case 1:
			std::cout << "scout ability 1" << std::endl;
			//double jump so passive ?
			break;
		case 2:
			std::cout << "scout ability 2" << std::endl;
			movementSpeed = 50;
			//audioSystem->playSound(Dash->GetFModSound(), false, footSteps);
			
			//dash
			break;
		case 3:
			
			if (controllerId < 0) {
				if (keyboard->KeyPressed(T5KeyboardKeys::SPACE)) {
					jetstartAudioPlayer->PlaySound();
					jetpackAudioPlayer->PlaySound();
				}

				if (keyboard->KeyHeld(T5KeyboardKeys::SPACE)) {
					GetGameObject()->GetComponent<T5PhysicsObject>()->AddForce(T5Vec3(0, 50, 0));
				}
				
				if(keyboard->KeyReleased(T5KeyboardKeys::SPACE))
					jetpackAudioPlayer->PauseSound();
			}
			else {
				if (gamepad->KeyPressed(T5GamePadKey::A)) {
					jetstartAudioPlayer->PlaySound();
					jetpackAudioPlayer->PlaySound();
				}

				if (gamepad->KeyHeld(T5GamePadKey::A)) {
					GetGameObject()->GetComponent<T5PhysicsObject>()->AddForce(T5Vec3(0, 50, 0));
				}

				if (gamepad->KeyReleased(T5GamePadKey::A))
					jetpackAudioPlayer->PauseSound();
			}
			


			std::cout << "scout ability 3" << std::endl;
			//jetpack
			break;
		}
		break;
	case Player::Class::TANK:
		switch (ability) {
		case 1:
			//std::cout << "overshield: " <<shield<< std::endl;
			if (shield < 150) {
				shield = shield + 1;
			}
			//overheal cant shoot
			break;
		case 2:
			//std::cout << "firerate = "<<gunBase->fireRate << " shield = "<< shield << std::endl;
			gunBase->fireRate = 175.0f; //increased fire rate for ability
			if (shield > 0) {
				shield = shield - 1;
			}
			//increase fire rate lower shield
			
			break;
		case 3:
			//std::cout << "minigun fire rate: " << gunBase->fireRate << std::endl;
			gunBase->fireRate = 100.0f;
			gunBase->ammo = 500;
			GetGameObject()->GetComponent<T5PhysicsObject>()->SetLinearVelocity(T5Vec3(0, 0, 0));
			canMove = false;
			//minigun cant move
			break;
		}
		break;
	case Player::Class::HEALER:
		
		switch (ability) {
		case 1:
			std::cout << "healer ability 1" << std::endl;
		
			for (int i = 0; i < players.size(); i++) {
				if (players[i]->GetGameObject()->GetWorldPosition() != GetGameObject()->GetWorldPosition()) {
					if ((players[i]->GetGameObject()->GetWorldPosition() - healPack->GetWorldPosition()).Length() < 10) {
						players[i]->health = players[i]->health + 0.5;
					}
				}
			}
			//heal pack
			break;
		case 2:
			std::cout << "healer ability 2" << std::endl;
			gunBase->canShoot = false;
			//AOE heal all players in area
			for (int i = 0; i < players.size(); i++) {
				if (players[i]->GetGameObject()->GetWorldPosition() != GetGameObject()->GetWorldPosition()) {
					if ((players[i]->GetGameObject()->GetWorldPosition() - GetGameObject()->GetWorldPosition()).Length() < 10) {
						if (players[i]->health < players[i]->maxHealth) {
							players[i]->health = players[i]->health + 0.5;
							std::cout << (players[i]->GetGameObject()->GetWorldPosition() - GetGameObject()->GetWorldPosition()).Length() << std::endl;
						}
					}
				}
			}
			break;
		case 3:
			std::cout << "healer ability 3" << std::endl;
			gunBase->canShoot = false;
			for (int i = 0; i < players.size(); i++) {
				if (players[i]->GetGameObject()->GetWorldPosition() != GetGameObject()->GetWorldPosition()) {
					if (players[i]->health < players[i]->maxHealth) {
						players[i]->health = players[i]->health + 1;
					}
				}
			}
			//HEAL ALL PLAYERS
			break;
		}
		break;
	case Player::Class::GRENADIER:
		switch (ability) {
		case 1:
			std::cout << "grenadier ability 1" << std::endl;
			CreateGrenade(1);
			//Paint grenade
			break;
		case 2:
			std::cout << "grenadier ability 2" << std::endl;
			CreateGrenade(0);
			//water grenade
			break;
		case 3:
			if (controllerId < 0) {
				if (mouse->ButtonPressed(T5MouseButtons::LEFT)) {
					gunBase->canShoot = false;
					CreateGrenade(1);
					shotsLeftOnGL--;
				}
			}
			else {
				if (gamepad->KeyPressed(T5GamePadKey::ShoulderR)) {
					gunBase->canShoot = false;
					CreateGrenade(1);
					shotsLeftOnGL--;
				}
			}
			//grenade launcher
			break;
		}
		break;
	}
}

void Player::CreateGrenade(int i) {
	auto grenadePC = T5CreateComponent<T5PrimitiveComponent>();
	T5PhysicsObject* grenadePhysics = T5CreateComponent<T5PhysicsObject>();
	T5PxMaterial bulletgrenadeMat;
	PaintGround* paint = T5CreateComponent<PaintGround>();
	//paint->exampleGame = T5Global::sGame;
	paint->splashAudio = grenadeAudioPlayer;
	auto grenade = T5Global::sGame->CreateGameObject();
	auto painter = T5CreateComponent<Painter>();
	if (i == 0) {
		painter->m_team = PainterTeam::C;
	}
	else {
		if (GetGameObject()->m_name == "Player1") {
			painter->m_team = PainterTeam::A;
		}
		else {
			painter->m_team = PainterTeam::B;
		}
	}
	
	painter->m_painterProperties.radius = 3;
	painter->m_painterProperties.hardness = 1;
	painter->m_painterProperties.paintStrength = 1;
	painter->m_painterProperties.normalClampAngle = 90;
	grenade->AddComponent(painter);
	grenadePhysics->SetColliderAsSphere(0.5, bulletgrenadeMat);
	grenadePhysics->SetCollisionChannel(CC_Dynamic, CC_Static | CC_Dynamic);
	grenade->AddComponent(grenadePhysics);
	grenadePC->SetMesh(grenadeMesh);
	grenadePC->SetMaterial(grenadeMat);
	grenade->AddComponent(grenadePC);
	auto grenadeAudio = T5CreateComponent<T5AudioPlayer>();
	grenadeAudio->SetSound(T5Global::sAssetManager->GetSound("grenade"), false);
	grenade->AddComponent(grenadeAudio);
	grenade->SetWorldPosition(playerCam->GetWorldPosition() + T5Vec3(0, 1, 0));
	grenade->SetScale(T5Vec3(0.5, 0.5, 0.5));
	grenadePhysics->SetMass(0.01);
	grenadePhysics->SetAffectedByGravity(true);
	grenadePhysics->AddForce(playerCam->GetWorldTransform().GetForward() * 100.0f);
	grenade->m_name = "Grenade";
	grenade->AddComponent(paint);
	paint->damage = 50;
	paint->player = this;
}

Player::Player()
{
}

Player::~Player()
{
	T5HudManager::GetInstance()->RemoveHud(m_hud);
	m_hud.reset();
}

