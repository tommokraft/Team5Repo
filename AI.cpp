#include "AI.h"
#include "Framework/T5Global.h"
#include "Framework/T5GameObject.h"
#include "Framework/T5PhysicsObject.h"
#include "SamplePoints.h"

void AI::Update(float dt) {
	if (!GetGameObject())return;
	auto ao = GetGameObject()->GetComponent<T5PhysicsObject>();
	if (!ao) return;
	//auto aw = GetGameObject()->GetWorldPosition();

	T5Vec3 distance;
	T5Vec3 currentPos = GetGameObject()->GetWorldTransform().GetPosition();
	T5Vec3 currentPosRounded = T5Vec3(floor(currentPos.x), -3, floor(currentPos.z));//ROUNDED POSITION OF THE AI OBJECT
	

	distance = targetPos - currentPos;
	if (distance.Length() <= 1) {
		ao->SetLinearVelocity(GetGameObject()->GetWorldTransform().GetPosition());

		//FIND ITERATOR OF CURRENT POSITION IN ARRAY AS INDEX
		auto itr = std::find(SamplePoints::Instance->samplePoints.begin(), SamplePoints::Instance->samplePoints.end(), currentPosRounded);
		if (itr == SamplePoints::Instance->samplePoints.end()) return;
		int index = itr - SamplePoints::Instance->samplePoints.begin();

		bool foundPainted = false;
		int nextIndex = 0;
		while (!foundPainted)
		{
			//LOOP THROUGH ARRAY 5000 POSITIONS FORWARD AND BACKWARDS LOOKING FOR PAINTED POSITIONS IN THE ARRAY
			for (int i = 0; i < 5000; i++) {
				if (index + i < 9999) {
					nextIndex = index + i;
					if (SamplePoints::Instance->samplePointsColours[nextIndex] == 1 || SamplePoints::Instance->samplePointsColours[nextIndex] == 2) {
						
						targetPos = SamplePoints::Instance->samplePoints[nextIndex];

						//SET THE TARGET POSITION OF THE AI OBJECT
						targetPos = T5Vec3(targetPos.x, -1.7, targetPos.z);
						foundPainted = true;

					}
				}
				if (index - i > 1) {
					nextIndex = index - i;
					if (SamplePoints::Instance->samplePointsColours[nextIndex] == 1 || SamplePoints::Instance->samplePointsColours[nextIndex] == 2) {
						
						targetPos = SamplePoints::Instance->samplePoints[nextIndex];
						targetPos = T5Vec3(targetPos.x, -1.7, targetPos.z);
						foundPainted = true;
					}
				}
			}
			/*curr = rand() % 4;
			targetPos = aiLocations[curr];*/
			break;
		}	
	}
	distance = targetPos - currentPos;
	T5Vec3 dir = distance.Normalised();
	ao->SetLinearVelocity(dir * aiSpeed);
}