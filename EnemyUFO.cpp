#include "EnemyUFO.h"
#include "Graphics.h"
#include "Maths.h"
#include "ImmediateMode.h"
#include "ImmediateModeVertex.h"
#include <algorithm>

EnemyUFO::EnemyUFO(XMVECTOR position) {
	SetPosition(position);
}

void EnemyUFO::Render(Graphics* graphics) 
{
	Ship::Render(graphics);
	ImmediateModeVertex axis[8] =
	{
		
		{-10.0f, 0.0f, 0.0f, 0xffffffff}, {10.0f, 0.0f, 0.0f, 0xffffffff},
		{0.0f, -10.0f, 0.0f, 0xffffffff}, {0.0f, 10.0f, 0.0f, 0xffffffff},
		{0.0f, 20.0f, 0.0f, 0xffffffff}, {10.0f, 10.0f, 0.0f, 0xffffffff},
		{0.0f, 20.0f, 0.0f, 0xffffffff}, {-10.0f, 10.0f, 0.0f, 0xffffffff},
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
	DrawRect(graphics);
}

void EnemyUFO::DrawRect(Graphics* graphics)
{
	const float RADIUS = 10.0f;

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

	ImmediateMode* immediateGraphics = graphics->GetImmediateMode();

	immediateGraphics->SetModelMatrix(translationMatrix);
	immediateGraphics->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
		&square[0],
		5);
	immediateGraphics->SetModelMatrix(XMMatrixIdentity());
}

void EnemyUFO::Move(int Delaycnt, int level)
{
	if (level == 0) {
		level = 1;
	}
	switch (level)
	{
	case 1: {
		MAX_SPEED = 0.5;
		bool randbool = rand() & 1;
		if (Delaycnt % 500 == 0) {
			if (rot == 1) {
				rot = -1;
			}
			else {
				rot = 1;
			}
		}
		if ( Delaycnt % 2 == 0) {
			SetControlInput(1, 0);
		}
		else if(randbool && Delaycnt % 5 == 0) {
			SetControlInput(1, rot);
		}
		break;
	}
	case 2: {
		MAX_SPEED = 0.8;
		bool randbool = rand() & 1;
		if (Delaycnt % 200 == 0) {
			if (rot == 1) {
				rot = -1;
			}
			else {
				rot = 1;
			}
		}
		if (Delaycnt % 2 == 0) {
			SetControlInput(1, 0);
		}
		else if (randbool && Delaycnt % 3 == 0) {
			SetControlInput(1, rot);
		}
		break;
	}
	case 3: {
		MAX_SPEED = 1.2;
		bool randbool = rand() & 1;
		if (randbool) {
			if (Delaycnt % 50 == 0) {
				bool randbool = rand() & 1;
				if (randbool) {
					randbool = rand() & 1;
					if (randbool) {
						SetControlInput(1, -1);
					}
					else {
						SetControlInput(1, 0);
					}
				}
				else {
					randbool = rand() & 1;
					if (randbool) {
						SetControlInput(1, 1);
					}
					else {
						SetControlInput(1, 0);
					}
				}
			}
			else {
				bool randbool1 = rand() & 1;
				if (Delaycnt % 200 == 0) {
					if (rot == 1) {
						rot = -1;
					}
					else {
						rot = 1;
					}
				}
				if (Delaycnt % 2 == 0) {
					SetControlInput(1, 0);
				}
				else if (randbool1 && Delaycnt % 3 == 0) {
					SetControlInput(1, rot);
				}
			}

		}
	}
	case 4: {
		MAX_SPEED = 1.4;
		bool randbool = rand() & 1;
		if (randbool) {
			if (Delaycnt % 50 == 0) {
				bool randbool = rand() & 1;
				if (randbool) {
					randbool = rand() & 1;
					if (randbool) {
						SetControlInput(1, -1);
					}
					else {
						SetControlInput(1, 0);
					}
				}
				else {
					randbool = rand() & 1;
					if (randbool) {
						SetControlInput(1, 1);
					}
					else {
						SetControlInput(1, 0);
					}
				}
			}
			else {
				bool randbool1 = rand() & 1;
				if (Delaycnt % 200 == 0) {
					if (rot == 1) {
						rot = -1;
					}
					else {
						rot = 1;
					}
				}
				if (Delaycnt % 2 == 0) {
					SetControlInput(1, 0);
				}
				else if (randbool1 && Delaycnt % 3 == 0) {
					SetControlInput(1, rot);
				}
			}

		}
	}
	default:
		MAX_SPEED = 1 + (level * 0.2);
		bool randbool = rand() & 1;
		if (randbool) {
			if (Delaycnt % 500 == 0) {
				bool randbool = rand() & 1;
				if (randbool) {
					randbool = rand() & 1;
					if (randbool) {
						SetControlInput(1, -1);
					}
					else {
						SetControlInput(1, 0);
					}
				}
				else {
					randbool = rand() & 1;
					if (randbool) {
						SetControlInput(1, 1);
					}
					else {
						SetControlInput(1, 0);
					}
				}
			}
			else {
				bool randbool1 = rand() & 1;
				if (Delaycnt % 200 == 0) {
					if (rot == 1) {
						rot = -1;
					}
					else {
						rot = 1;
					}
				}
				if (Delaycnt % 2 == 0) {
					SetControlInput(1, 0);
				}
				else if (randbool1 && Delaycnt % 2 == 0) {
					SetControlInput(1, rot);
				}
			}

		}
	}
}
