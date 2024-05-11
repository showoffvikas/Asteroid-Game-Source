#include "Asteroid.h"
#include "Graphics.h"
#include "Random.h"
#include "Maths.h"
#include "ImmediateMode.h"
#include "ImmediateModeVertex.h"

Asteroid::Asteroid(XMVECTOR position,
	XMVECTOR velocity,
	int size) :
	angle_(0.0f),
	size_(size)
{
	SetPosition(position);

	XMStoreFloat3(&velocity_, velocity);

	axis_.x = Random::GetFloat(-1.0f, 1.0f);
	axis_.y = Random::GetFloat(-1.0f, 1.0f);
	axis_.z = Random::GetFloat(-1.0f, 1.0f);
	XMStoreFloat3(&axis_, XMVector3Normalize(XMLoadFloat3(&axis_)));

	const float MAX_ROTATION = 0.3f;
	angularSpeed_ = Random::GetFloat(-MAX_ROTATION, MAX_ROTATION);
}

void Asteroid::Update(System *system)
{
	XMVECTOR position = GetPosition();
	position = XMVectorAdd(position, XMLoadFloat3(&velocity_));
	SetPosition(position);

	angle_ = Maths::WrapModulo(angle_ + angularSpeed_, Maths::TWO_PI);
}

void Asteroid::Render(Graphics *graphics) const
{
	const float RADIUS_MULTIPLIER = 5.0f;

	ImmediateModeVertex square[5] =
	{
		{-1.0f, -1.0f, 0.0f, 0xffffffff},
		{-1.0f,  1.0f, 0.0f, 0xffffffff},
		{ 1.0f,  1.0f, 0.0f, 0xffffffff},
		{ 1.0f, -1.0f, 0.0f, 0xffffffff},
		{-1.0f, -1.0f, 0.0f, 0xffffffff},
	};

	XMMATRIX scaleMatrix = XMMatrixScaling(
		size_ * RADIUS_MULTIPLIER,
		size_ * RADIUS_MULTIPLIER,
		size_ * RADIUS_MULTIPLIER);

	XMMATRIX rotationMatrix = XMMatrixRotationAxis(
		XMLoadFloat3(&axis_),
		angle_);

	XMVECTOR position = GetPosition();
	XMMATRIX translationMatrix = XMMatrixTranslation(
		XMVectorGetX(position),
		XMVectorGetY(position),
		XMVectorGetZ(position));

	XMMATRIX asteroidTransform = scaleMatrix *
		rotationMatrix *
		translationMatrix;

	ImmediateMode *immediateGraphics = graphics->GetImmediateMode();

	immediateGraphics->SetModelMatrix(asteroidTransform);
	immediateGraphics->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		&square[0],
		5);
	immediateGraphics->SetModelMatrix(XMMatrixIdentity());

}

XMVECTOR Asteroid::GetVelocity() const
{
	return XMLoadFloat3(&velocity_);
}

int Asteroid::GetSize() const
{
	return size_;
}
