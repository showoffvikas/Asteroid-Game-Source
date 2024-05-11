#include "Ship.h"
#include "Graphics.h"
#include "Maths.h"
#include "ImmediateMode.h"
#include "ImmediateModeVertex.h"
#include <algorithm>

Ship::Ship() :
	accelerationControl_(0.0f),
	rotationControl_(0.0f),
	velocity_(XMFLOAT3(0.0f, 0.0f, 0.0f)),
	forward_(XMFLOAT3(0.0f, 1.0f, 0.0f)),
	rotation_(0.0f) //**TODO: Candidate for crash
{
	
}

void Ship::SetControlInput(float acceleration,
	float rotation)
{
	accelerationControl_ = acceleration;
	rotationControl_ = rotation;
}

void Ship::Update(System *system)
{
	
	const float RATE_OF_ROTATION = 0.1f;
	const float VELOCITY_TWEEN = 0.05f;

	rotation_ = Maths::WrapModulo(rotation_ + rotationControl_ * RATE_OF_ROTATION,
		Maths::TWO_PI);

	XMMATRIX rotationMatrix = XMMatrixRotationZ(rotation_);
	XMVECTOR newForward = XMVector3TransformNormal(XMVectorSet(0.f, 1.0f, 0.0f, 0.0f), rotationMatrix);
	newForward = XMVector3Normalize(newForward);
	XMStoreFloat3(&forward_, newForward);

	XMVECTOR idealVelocity = XMVectorScale(XMLoadFloat3(&forward_), accelerationControl_ * MAX_SPEED);
	XMVECTOR newVelocity = XMVectorLerp(XMLoadFloat3(&velocity_), idealVelocity, VELOCITY_TWEEN);
	XMStoreFloat3(&velocity_, newVelocity);

	XMVECTOR position = GetPosition();
	position = XMVectorAdd(position, XMLoadFloat3(&velocity_));
	SetPosition(position);
	if (PlayerInvicible) {
		CountTime += 1;
		if (CountTime > 200) {
			PlayerInvicible = false;
			CountTime = -1;
		}
	}
	else {
		CountTime = 0;
		
	}

}

void Ship::Render(Graphics *graphics) const
{
	if ((PlayerInvicible && (CountTime % 40 == 0)) || !PlayerInvicible) {
		ImmediateModeVertex axis[8] =
		{
			{0.0f, -5.0f, 0.0f, 0xffffffff}, {0.0f, 10.0f, 0.0f, 0xffffffff},
			{-5.0f, 0.0f, 0.0f, 0xffffffff}, {5.0f, 0.0f, 0.0f, 0xffffffff},
			{0.0f, 10.0f, 0.0f, 0xffffffff}, {-5.0f, 5.0f, 0.0f, 0xffffffff},
			{0.0f, 10.0f, 0.0f, 0xffffffff}, {5.0f, 5.0f, 0.0f, 0xffffffff},
		};

		XMMATRIX rotationMatrix = XMMatrixRotationZ(rotation_);

		XMVECTOR position = GetPosition();
		XMMATRIX translationMatrix = XMMatrixTranslation(
			XMVectorGetX(position),
			XMVectorGetY(position),
			XMVectorGetZ(position));

		XMMATRIX shipTransform = rotationMatrix * translationMatrix;

		ImmediateMode* immediateGraphics = graphics->GetImmediateMode();

		immediateGraphics->SetModelMatrix(shipTransform);
		immediateGraphics->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			&axis[0],
			8);
		immediateGraphics->SetModelMatrix(XMMatrixIdentity());
	
	}
	
}

XMVECTOR Ship::GetForwardVector() const
{
	return XMLoadFloat3(&forward_);
}

XMVECTOR Ship::GetVelocity() const
{
	return XMLoadFloat3(&velocity_);
}



void Ship::Reset()
{
	accelerationControl_ = 0.0f;
	rotationControl_ = 0.0f;

	velocity_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
	forward_ = XMFLOAT3(0.0f, 1.0f, 0.0f);
	rotation_ = 0.0f;

	SetPosition(XMVectorZero());
}
