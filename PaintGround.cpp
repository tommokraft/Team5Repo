#include "PaintGround.h"
#include "Framework/T5GameObject.h"
#include "Framework/T5PhysicsObject.h"
#include "Framework/T5Global.h"
#include "Painter.h"
PaintGround::~PaintGround()
{
	
}
int PaintGround::GetTeamColour(Painter* painter)
{
	switch (painter->m_team) {
	case PainterTeam::A:
		return 1;
	case PainterTeam::B:
		return 2;
		break;
	case PainterTeam::C:
		return 0;
		break;
	}
}
void PaintGround::AddColourToArray(int index, int colour)
{
	int insertAt = 0;
	if (index < 10000) {
		insertAt = index;
		SamplePoints::Instance->samplePointsColours[insertAt] = colour;
	}
	if (index > 1) {
		insertAt = index - 1;
		SamplePoints::Instance->samplePointsColours[insertAt] = colour;
	}
	if (index < 9998) {
		insertAt = index + 1;
		SamplePoints::Instance->samplePointsColours[insertAt] = colour;
	}
	if (index > 50) {
		insertAt = index - 50;
		SamplePoints::Instance->samplePointsColours[insertAt] = colour;
	}
	if (index < 9950) {
		insertAt = index + 50;
		SamplePoints::Instance->samplePointsColours[insertAt] = colour;
	}
}
void PaintGround::Update(float dt) {
	if (this->GetGameObject()->GetWorldPosition().y < -5) {
		if (this->GetGameObject()->m_name == "Player1" || this->GetGameObject()->m_name == "Player2") {
			//this->GetGameObject()->GetComponent<Player>()->Init(this->GetGameObject()->GetComponent<Player>()->classType);
			player->Init(player->classType);
			this->GetGameObject()->SetWorldPosition(T5Vec3(0, -1, 0));
		}
		else {
			T5Global::sGame->DestroyGameObject(GetGameObject());
		}
	}
	if (isPainting) {
		if (go && painter) {
			auto pos = go->GetWorldPosition();
			pos.y = -3;
			T5Vec3 roundedPos = T5Vec3(floor(pos.x), pos.y, floor(pos.z));
			auto itr = std::find(SamplePoints::Instance->samplePoints.begin(), SamplePoints::Instance->samplePoints.end(), roundedPos);
			if (itr == SamplePoints::Instance->samplePoints.end()) return;
			int index = itr - SamplePoints::Instance->samplePoints.begin();
			int colour = 0;
			SamplePoints::Instance->samplePointsColours[index] = colour;
			T5Vec3 dir = T5Vec3(0, -1, 0);
			painter->PaintOnPaintableObject(paintable, pos, dir);
		}
	}
}

void PaintGround::OnCollisionBegin(T5PhysicsObject* otherObject, const std::vector<T5CollisionPoint>& contactPoints) {
	if (otherObject->GetGameObject() && this->GetGameObject()) {// IF BOTH ARE NOT NULL
		std::string selfname = this->GetGameObject()->m_name; //GET THE NAMES OF THE OBJECTS
		std::string othername = otherObject->GetGameObject()->m_name;

		if (player->GetGameObject()->GetWorldPosition() == otherObject->GetWorldTransform().GetPosition()) return; //CHECK THAT YOU ARENT CHECKING COLLISIONS BETWEEN THE SAME OBJECTS
		if ((selfname == "Bullet" || selfname == "Grenade") && (othername == "Player1" || othername == "Player2")) {
			collisions += selfname.c_str();
			collisions += " : Collided With : ";
			collisions += othername.c_str();
			if (collisionHolder != NULL)
			{
				collisionHolder->push_back(collisions);
			}
			T5Global::sGame->DestroyGameObject(GetGameObject());
			otherObject->GetGameObject()->GetComponent<Player>()->TakeDamage(damage);
		}
		else if ((selfname == "Bullet" || selfname == "Grenade") && othername == "Map") { //BULLET AND MAP COLLISION
			//HUD UPDATE CODE 
			/*collisions += selfname.c_str();
			collisions += " : Collided With : ";
			collisions += othername.c_str();
			if (collisionHolder != NULL)
			{
				collisionHolder->push_back(collisions);
			}
			splashAudio->PlaySound();*/

			//GETTING THE GAME OBJECTS PAINTER COMPONENTS AND POSITION 
			go = GetGameObject();
			assert(go);
			painter = GetGameObject()->GetComponent<Painter>();
			assert(painter);
			paintable = otherObject->GetGameObject()->GetComponent<PaintableObject>();
			T5Vec3 pos = go->GetWorldPosition();
			pos.y = -3;
			T5Vec3 dir = T5Vec3(0, -1, 0);
			T5Vec3 roundedPos = T5Vec3(floor(pos.x), pos.y, floor(pos.z));
			
			//FIND THAT POSITION IN THE SAMPLE POINTS ARRAY AS AN INDEX
			auto itr = std::find(SamplePoints::Instance->samplePoints.begin(), SamplePoints::Instance->samplePoints.end(), roundedPos);
			if (itr == SamplePoints::Instance->samplePoints.end())return;
			int index = itr - SamplePoints::Instance->samplePoints.begin();
				
			//DETERMINE THE PAINT TEAM
			int colour = GetTeamColour(painter);

			//ADD THE PAINTED POSITIONS TO THE COLOURS ARRAY
			if (selfname == "Bullet") AddColourToArray(index, colour);


			else if (selfname == "Grenade") {
				if (index < 10000) {
					SamplePoints::Instance->samplePointsColours[index] = colour;
				}
				if (index < 9996) {
					SamplePoints::Instance->samplePointsColours[index + 1] = colour;
					SamplePoints::Instance->samplePointsColours[index + 2] = colour;
					SamplePoints::Instance->samplePointsColours[index + 3] = colour;
				}
				if (index > 4) {
					SamplePoints::Instance->samplePointsColours[index - 1] = colour;
					SamplePoints::Instance->samplePointsColours[index - 2] = colour;
					SamplePoints::Instance->samplePointsColours[index - 3] = colour;
				}
				if (index > 52) {
					SamplePoints::Instance->samplePointsColours[index - 50] = colour;
					SamplePoints::Instance->samplePointsColours[index - 51] = colour;
					SamplePoints::Instance->samplePointsColours[index - 52] = colour;
				}
				if (index < 9948) {
					SamplePoints::Instance->samplePointsColours[index + 50] = colour;
					SamplePoints::Instance->samplePointsColours[index + 51] = colour;
					SamplePoints::Instance->samplePointsColours[index + 52] = colour;
				}
				if (index > 102) {
					SamplePoints::Instance->samplePointsColours[index - 100] = colour;
					SamplePoints::Instance->samplePointsColours[index - 101] = colour;
					SamplePoints::Instance->samplePointsColours[index - 102] = colour;
				}
				if (index < 9898) {
					SamplePoints::Instance->samplePointsColours[index + 100] = colour;
					SamplePoints::Instance->samplePointsColours[index + 101] = colour;
					SamplePoints::Instance->samplePointsColours[index + 102] = colour;
				}
				if (index > 152) {
					SamplePoints::Instance->samplePointsColours[index - 150] = colour;
					SamplePoints::Instance->samplePointsColours[index - 151] = colour;
					SamplePoints::Instance->samplePointsColours[index - 152] = colour;
				}
				if (index < 9848) {
					SamplePoints::Instance->samplePointsColours[index + 150] = colour;
					SamplePoints::Instance->samplePointsColours[index + 151] = colour;
					SamplePoints::Instance->samplePointsColours[index + 152] = colour;
				}
				if (index > 202) {
					SamplePoints::Instance->samplePointsColours[index - 200] = colour;
					SamplePoints::Instance->samplePointsColours[index - 201] = colour;
					SamplePoints::Instance->samplePointsColours[index - 202] = colour;
				}
				if (index < 9798) {
					SamplePoints::Instance->samplePointsColours[index + 200] = colour;
					SamplePoints::Instance->samplePointsColours[index + 201] = colour;
					SamplePoints::Instance->samplePointsColours[index + 202] = colour;
				}
				if (index > 252) {
					SamplePoints::Instance->samplePointsColours[index - 250] = colour;
					SamplePoints::Instance->samplePointsColours[index - 251] = colour;
					SamplePoints::Instance->samplePointsColours[index - 252] = colour;
				}
				if (index < 9748) {
					SamplePoints::Instance->samplePointsColours[index + 250] = colour;
					SamplePoints::Instance->samplePointsColours[index + 251] = colour;
					SamplePoints::Instance->samplePointsColours[index + 252] = colour;
				}
				if (index > 300) {
					SamplePoints::Instance->samplePointsColours[index - 300] = colour;
				}
				if (index < 9700) {
					SamplePoints::Instance->samplePointsColours[index + 300] = colour;
				}
			}

			//PAINT ON THE GROUND AND DESTROY BULLET
			painter->PaintOnPaintableObject(paintable, pos, dir);
			T5Global::sGame->DestroyGameObject(GetGameObject());
		}
			
		else if ((selfname == "Ai" && othername == "Map") || (othername == "Ai" && selfname == "Map")) {

			go = GetGameObject();
			assert(go);
			painter = GetGameObject()->GetComponent<Painter>();
			assert(painter);
			paintable = otherObject->GetGameObject()->GetComponent<PaintableObject>();
			isPainting = true;
		}
	}
}
