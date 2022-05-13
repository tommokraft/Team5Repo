#pragma once
#include "..\Common\Framework\T5Component.h"
#include <Framework\T5Collider.h>
#include <Framework\T5Global.h>
//#include "GameScene.h"


class GameScene;

class AI: public T5Component
{
	T5COMPONENT_BODY(AI);
public:

	void Init();
	virtual void Update(float dt) override;

	T5Vec3 aiLocations[4] = { T5Vec3(20, -1.74, 20), T5Vec3(20, -1.74, -20), T5Vec3(-20, -1.74, 20), T5Vec3(-20, -1.74, -20) };
	int curr = 0;
	int aiSpeed = 5;
	T5Vec3 targetPos = T5Vec3(20, -1.7, 20);

//	GameScene* gameScene;
};

