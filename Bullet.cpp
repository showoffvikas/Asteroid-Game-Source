#include "Bullet.h"
#include "Graphics.h"
#include "ImmediateMode.h"
#include "ImmediateModeVertex.h"

Bullet::Bullet(XMVECTOR position,
	XMVECTOR direction) 
{
	const float BULLET_SPEED = 4.0f;

	SetPosition(position);
	XMVECTOR normalisedDirection = XMVector3Normalize(direction);
	XMStoreFloat3(&velocity_, normalisedDirection * BULLET_SPEED);
}

void Bullet::Update(System *system)
{
	XMVECTOR position = GetPosition();
	position = XMVectorAdd(position, XMLoadFloat3(&velocity_));
	SetPosition(position);
	Lifetime -= 1;
	
}

void Bullet::Render(Graphics *graphics) const
{
	const float RADIUS = 3.0f;

	ImmediateModeVertex square[5] =
	{
		{-RADIUS, -RADIUS, 0.0f, 0xffffffff},
		{-RADIUS,  RADIUS, 0.0f, 0xffffffff},
		{ RADIUS,  RADIUS, 0.0f, 0xffffffff},
		{ RADIUS, -RADIUS, 0.0f, 0xffffffff},
		{-RADIUS, -RADIUS, 0.0f, 0xffffffff},
	};
	
	XMVECTOR position = GetPosition();
	XMMATRIX translationMatrix = XMMatrixTranslation(
		XMVectorGetX(position),
		XMVectorGetY(position),
		XMVectorGetZ(position));

	ImmediateMode *immediateGraphics = graphics->GetImmediateMode();

	immediateGraphics->SetModelMatrix(translationMatrix);
	immediateGraphics->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		&square[0],
		5);
	immediateGraphics->SetModelMatrix(XMMatrixIdentity());
}
