#include "Zombie.h"


constexpr auto BodySize = NAS2D::Vector<int>{14, 46};
constexpr auto BodyOffset = NAS2D::Vector<int>{-10, -40};
constexpr auto HeadSize = NAS2D::Vector<int>{8, 8};
constexpr auto HeadOffset = NAS2D::Vector<int>{-7, -50};
constexpr auto HealthMeterSize = NAS2D::Vector<int>{24, 2};
constexpr auto HealthMeterOffset = NAS2D::Vector<int>{-HealthMeterSize.x / 2, -55};


Zombie::Zombie(NAS2D::Point_2df position, float speed) :
	mSprite("zombie_0.xml"),
	mPosition(position),
	mHealth(100),
	mMaxHealth(mHealth),
	mDirection(0.0f),
	mSpeed(speed),
	mBodyRect(NAS2D::Rectangle<int>::Create(mPosition.to<int>() + BodyOffset, BodySize)),
	mHeadRect(NAS2D::Rectangle<int>::Create(mPosition.to<int>() + HeadOffset, HeadSize))
{
	mSprite.play("WalkWest");
}


void Zombie::update(int timeDelta, NAS2D::Point_2df playerPosition)
{
	mSprite.update(mPosition.x(), mPosition.y());

	if(dead())
		return;

	// Ultra basic bee-line AI
	mDirection = NAS2D::angleFromPoints(mPosition.x(), mPosition.y(), playerPosition.x(), playerPosition.y());
	mPosition += NAS2D::getDirectionVector(mDirection) * (timeDelta * mSpeed);

	// Update bounding boxes.
	mBodyRect = NAS2D::Rectangle<int>::Create(mPosition.to<int>() + BodyOffset, BodySize);
	mHeadRect = NAS2D::Rectangle<int>::Create(mPosition.to<int>() + HeadOffset, HeadSize);

	// Health bar
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	renderer.drawBoxFilled(NAS2D::Rectangle<float>::Create(mPosition + HealthMeterOffset, HealthMeterSize), NAS2D::Color::Black);
	renderer.drawBoxFilled(NAS2D::Rectangle<float>::Create(mPosition + HealthMeterOffset, NAS2D::Vector<int>{HealthMeterSize.x * mHealth / mMaxHealth, HealthMeterSize.y}), NAS2D::Color::Yellow);

	renderer.drawBox(mHeadRect, NAS2D::Color::White);
	renderer.drawBox(mBodyRect, NAS2D::Color::White);
}


bool Zombie::hit(NAS2D::Point_2d pt)
{
	return mBodyRect.contains(pt) || mHeadRect.contains(pt);
}


void Zombie::damage(int d, NAS2D::Point_2d pt)
{
	if(dead())
		return;

	if(mHeadRect.contains(pt))
	{
		mHealth = 0;
		mSprite.play("Dead2West");
		mDeadTimer.reset();
		return;
	}

	mHealth -= d;
	if(mHealth <= 0)
	{
		mHealth = 0;
		mSprite.play("Dead1West");
		mDeadTimer.reset(); // reset timer so we know how long it's been since the zombie died.
	}
}


unsigned int Zombie::deadTime()
{
	if(!dead())
		return 0;

	return mDeadTimer.accumulator();
}
