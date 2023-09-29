#include "Physics.h"





Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{

	CBoundingBox& aBox = a->getComponent<CBoundingBox>();
	CBoundingBox& bBox = b->getComponent<CBoundingBox>();

	CTransform& aTransform = a->getComponent<CTransform>();
	CTransform& bTransform = b->getComponent<CTransform>();

	//check that both entities have the component.
	if (aBox.has && bBox.has)
	{
		Vec2 difference;
		difference.x = abs(aTransform.pos.x - bTransform.pos.x);
		difference.y = abs(aTransform.pos.y - bTransform.pos.y);

		return Vec2(aBox.halfSize.x + bBox.halfSize.x - difference.x, aBox.halfSize.y + bBox.halfSize.y - difference.y);

	}
	return Vec2();
}

bool Physics::AABB(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	Vec2 aPos = a->getComponent<CTransform>().pos;
	Vec2 bPos = b->getComponent<CTransform>().pos;

	Vec2 aSize = a->getComponent<CBoundingBox>().size;
	Vec2 bSize = b->getComponent<CBoundingBox>().size;

	return (
		aPos.x < bPos.x + bSize.x &&
		aPos.x + aSize.x > bPos.x &&
		aPos.y < bPos.y + bSize.y &&
		aPos.y + aSize.y > bPos.y);
}
