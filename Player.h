#pragma once
#include "..\Common\Framework\T5Component.h"
#include "Framework/T5AudioPlayer.h"
#include "Framework/T5CameraComponent.h"
#include "Framework/T5SkeletonComponent.h"
#include <chrono>
#include <ctime>
#include "PlayerCameraController.h"
#include "GunBase.h"
#include "PlayerHUD.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

class GunBase;
class ExampleGame;


class Player : public T5Component
{
	T5COMPONENT_BODY(Player);
public:
	enum class Class {
		SCOUT,
		TANK,
		HEALER,
		GRENADIER
	};
	Player();
	virtual ~Player();

	virtual void Init(Class classType);
	virtual void CreateBullet();
	virtual void CreateGrenade(int i);
	virtual void Update(float dt) override;

	bool ReceiveMovementInput(T5Vec2& dir, bool& jump);
	void Movement();
	void Abilities();
	void Shooting();
	void SquidMode();
	void TakeDamage(float damage);
	void UseAbility(int ability);
	float movementSpeed;
	float jumpHeight;
	Class classType;
	float health;
	float maxHealth;
	float shield;
	float maxShield;


	float squidMulti;

	double channelTime;
	bool startAbility1 = false;
	bool startAbility2 = false;
	bool startAbility3 = false;
	bool ability1OnCooldown = false;
	bool ability2OnCooldown = false;
	bool ability3OnCooldown = false;

	float ability1Cooldown;
	float ability2Cooldown;
	float ability3Cooldown;


	int controllerId = -1;
	bool canMove = true;
	bool isMoving = false;
	bool isJumping = false;
	bool canDoubleJump = false;
	bool hasDoubleJump = false;

	T5GameObjectPtr playerCam;

	duration<double, std::milli> ms_double;
	chrono::time_point<chrono::high_resolution_clock> fireRateTimer;
	chrono::time_point<chrono::high_resolution_clock> reloadTimer;

	chrono::time_point<chrono::high_resolution_clock> collisionRemoveTimer;

	chrono::time_point<chrono::high_resolution_clock> startAbility1Timer;
	chrono::time_point<chrono::high_resolution_clock> startAbility2Timer;
	chrono::time_point<chrono::high_resolution_clock> startAbility3Timer;
	chrono::time_point<chrono::high_resolution_clock> startAbilityCooldown1;
	chrono::time_point<chrono::high_resolution_clock> startAbilityCooldown2;
	chrono::time_point<chrono::high_resolution_clock> startAbilityCooldown3;
	
	T5Keyboard* keyboard = nullptr;
	T5Mouse* mouse = nullptr;
	T5GamePad* gamepad = nullptr;

	T5GameObjectPtr skeletonObj;
	T5SkeletonComponent* skeleton;

	T5AudioPlayer* footAudioPlayer;
	T5AudioPlayer* jumpAudioPlayer;
	T5AudioPlayer* shieldAudioPlayer;
	T5AudioPlayer* splashAudioPlayer;
	T5AudioPlayer* healPackAudioPlayer;
	T5AudioPlayer* dashAudioPlayer;
	T5AudioPlayer* channelAudioPlayer;
	T5AudioPlayer* AOEHealAudioPlayer;
	T5AudioPlayer* gunChangeAudioPlayer;
	T5AudioPlayer* teamHealAudioPlayer;
	T5AudioPlayer* jetpackAudioPlayer;
	T5AudioPlayer* jetstartAudioPlayer;
	T5AudioPlayer* grenadeAudioPlayer;

	GunBase* gunBase;
	T5MeshPtr bulletMesh;
	T5MeshPtr cubeMesh;
	T5MeshPtr grenadeMesh;
	T5MaterialPtr bulletMat;
	T5MaterialPtr grenadeMat;
	T5MaterialPtr healthMat;
	T5PhysicsObject* bulletPhysics;
	std::vector<Player*> players;
	int shotsLeftOnGL = 6;
	bool healpackmade = false;
	bool init = false;
	bool squidEntered = false;
	std::shared_ptr<PlayerHUD> m_hud = nullptr;
	vector<string> collisionHolder;

};
