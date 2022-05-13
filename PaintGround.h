#pragma once
#include "..\Common\Framework\T5Component.h"
#include "Player.h"
#include "Painter.h"
#include "PaintableObject.h"
#include "SamplePoints.h"
class PaintGround : public T5Component
{
	T5COMPONENT_BODY(PaintGround);
public:
	PaintGround() { bRequirePxEvents = true; };
	~PaintGround();
	int GetTeamColour(Painter* painter);
	void AddColourToArray(int index, int colour);
	std::vector<T5GameObjectPtr> objHolder;
	std::vector<T5GameObjectPtr> playerObjects;
	virtual void Update(float dt) override;
	virtual void OnCollisionBegin(T5PhysicsObject* otherObject, const std::vector<T5CollisionPoint>& contactPoints);
	float radius = 0.0f;
	float damage = 0.0f;
	Player* player = nullptr;
	//T5Game* exampleGame = nullptr;

	bool isPainting = false;
	bool isPainting2 = false;
	T5GameObjectPtr go = nullptr;
	Painter* painter = nullptr;
	PaintableObject* paintable = nullptr;
	T5GameObjectPtr go2 = nullptr;
	Painter* painter2 = nullptr;
	PaintableObject* paintable2 = nullptr;

	T5AudioPlayer* splashAudio = nullptr;

	string collisions = " ";
	vector<string> * collisionHolder = nullptr;

};