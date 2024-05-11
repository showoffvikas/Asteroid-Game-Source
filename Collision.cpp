#include "Collision.h"
#include "Collider.h"
#include "Game.h"
#include <functional>

Collision::Collision()
{
}

Collision::~Collision()
{
	for (ColliderList::iterator colliderIt = colliders_.begin(), end = colliders_.end();
		colliderIt != end;
		++colliderIt)
	{
		delete *colliderIt;
	}
}

Collider *Collision::CreateCollider(GameEntity *entity)
{
	Collider *collider = new Collider();

	collider->position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	collider->radius = 0.0f;
	collider->entity = entity;
	collider->enabled = true;
	colliders_.push_back(collider);

	return collider;
}

void Collision::DestroyCollider(Collider *collider)
{
	colliders_.remove_if(std::bind1st((std::equal_to<Collider *>()), collider));
	delete collider;
}

void Collision::UpdateColliderPosition(Collider *collider, const XMFLOAT3 &position)
{
	collider->position = position;
}

void Collision::UpdateColliderRadius(Collider *collider, float radius)
{
	collider->radius = radius;
}

void Collision::EnableCollider(Collider *collider)
{
	collider->enabled = true;
}

void Collision::DisableCollider(Collider *collider)
{
	collider->enabled = false;
}

void Collision::DoCollisions(Game *game) 
{
	

	



	


	

	
	
	for (ColliderList::iterator ItrColl = colliders_.begin(); ItrColl != colliders_.end(); ItrColl++) {
	
		ColliderList::iterator ItrColl2;
		//if (ItrColl != colliders_.end()) {
			 ItrColl2 = ItrColl;
			 ItrColl2 ++;
	//	}
		//else {
		//	return;
		//}
	    
		
		while(ItrColl2!=colliders_.end()) {
		
			Collider* colliderA = *ItrColl;
			Collider* colliderB = *ItrColl2;
			if (CollisionTest(colliderA, colliderB))
			{
				game->DoCollision(colliderA->entity, colliderB->entity);
				
				return;
				
				
			}
			else {
				ItrColl2++;
			}
		}
	}
	
}

bool Collision::CollisionTest(Collider *a, Collider *b)
{
	if (a->enabled == false)
		return false;
	if (b->enabled == false)
		return false;

	XMVECTOR diff = XMVectorSubtract(XMLoadFloat3(&a->position), XMLoadFloat3(&b->position));
	float distance = XMVectorGetX(XMVector3Length(diff));
	if (distance < (a->radius + b->radius))
	{
		return true;
	}

	return false;
}

void Collision::DestroyAllCollider(Collider* collider)
{
	for (ColliderList::iterator colliderIt = colliders_.begin(), end = colliders_.end();
		colliderIt != end;
		++colliderIt)
	{
		if ((*colliderIt) != collider) {
			delete* colliderIt;
		}

	}
}
