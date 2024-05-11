#include "Game.h"
#include "System.h"
#include "OrthoCamera.h"
#include "Background.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Explosion.h"
#include "Keyboard.h"
#include "Random.h"
#include "Maths.h"
#include "Bullet.h"
#include "Collision.h"
#include <algorithm>
#include <iostream>
#include "Graphics.h"
#include "FontEngine.h"
#include <ddeml.h>

Game::Game() :
	camera_(0),
	background_(0),
	player_(0),
	collision_(0),
	bullet_(0)
	, firestate(0), FireDelaycnt(0),CurrentScore(0),CurrentDestroyed(0) ,CurrentLevel(0) ,HighScore(0),PlayerHealth(0),PlayerLives(0),Str_FireMode(),UFOFireRate(0)
{
	camera_ = new OrthoCamera();
	camera_->SetPosition(XMFLOAT3(0.0f, 0.0f, 0.0f));
	camera_->SetFrustum(800.0f, 600.0f, -100.0f, 100.0f);
	background_ = new Background(800.0f, 600.0f);
	collision_ = new Collision();
	
}

Game::~Game()
{
	delete camera_;
	delete background_;
	delete player_;
	DeleteBullet(0);
	DeleteAllAsteroids();
	DeleteAllExplosions();
	DeleteAllBullets();
	delete collision_;
	
}

void Game::Update(System *system)
{
	UpdatePlayer(system);
	UpdateAsteroids(system);
	UpdateBullet(system);
	UpdateCollisions();
	UpdateExplosion(system);
	UpdateUFO(system);
	FireDelaycnt += 1;
	if (PlayerHealth < 1) {
		if (PlayerLives > 0 && !player_->PlayerInvicible) {
			PlayerLives -= 1;
			player_->PlayerInvicible = true;
			PlayerHealth = 100 * ((CurrentLevel + 1) / 2);
		}
		else {
			DeletePlayer();
		}
	}
	
}

void Game::RenderBackgroundOnly(Graphics *graphics)
{
	camera_->SetAsView(graphics);
	background_->Render(graphics);
	
	
}

void Game::RenderEverything(Graphics *graphics)
{
	camera_->SetAsView(graphics);

	background_->Render(graphics);

	if (player_)
	{
		player_->Render(graphics);
	}

	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
		++asteroidIt)
	{
		(*asteroidIt)->Render(graphics);
	}
	/*
	if (bullet_)
	{
		bullet_->Render(graphics);
	}
	*/
	for (BulletList::const_iterator itr = bullets_.begin();itr != bullets_.end(); itr++) {
		(*itr)->Render(graphics);
	}

	for (ExplosionList::const_iterator explosionIt = explosions_.begin(),
		end = explosions_.end();
		explosionIt != end;
		++explosionIt)
	{
		(*explosionIt)->Render(graphics);
	}

	for (EnemyUFOList::const_iterator ufoIt = enemies_.begin(),
		end = enemies_.end();
		ufoIt != end;
		++ufoIt)
	{
		(*ufoIt)->Render(graphics);
	}
	if (FireDelaycnt % 5 == 0) {
		RenderText(graphics);
	}
	
	

}

void Game::RenderText(Graphics* graphics)
{
	FontEngine* fontEngine = graphics->GetFontEngine();
	
	fontEngine->BeginFrame();
	char scoreText[256];


	sprintf_s(scoreText, "Total Score %d", CurrentScore);
	int textWidth = fontEngine->CalculateTextWidth(scoreText, FontEngine::FONT_TYPE_SMALL);
	int textX = (200 - textWidth) / 2;
	int textY = (100 - 48) / 2;
	fontEngine->DrawText(scoreText, textX, textY, 0xff00ffff, FontEngine::FONT_TYPE_SMALL);
	
	char charHighscoret[256];
	sprintf_s(charHighscoret, "HighScore %d", HighScore);
	textWidth = fontEngine->CalculateTextWidth(charHighscoret, FontEngine::FONT_TYPE_SMALL);
	textX = (500 - textWidth) / 2;
	textY = (100 - 48) / 2;
	fontEngine->DrawText(charHighscoret, textX, textY, 0xff00ffff, FontEngine::FONT_TYPE_SMALL);

	char currentDestroyedt[256];
	sprintf_s(currentDestroyedt, "Destroyed %d", CurrentDestroyed);
	textWidth = fontEngine->CalculateTextWidth(scoreText, FontEngine::FONT_TYPE_SMALL);
	textX = (800 - textWidth) / 2;
	textY = (100 - 48) / 2;
	fontEngine->DrawText(currentDestroyedt, textX, textY, 0xff00ffff, FontEngine::FONT_TYPE_SMALL);

	char playerlivet[256];
	sprintf_s(playerlivet, "PlayerLives %d", PlayerLives);
	textWidth = fontEngine->CalculateTextWidth(playerlivet, FontEngine::FONT_TYPE_SMALL);
	textX = (1100 - textWidth) / 2;
	textY = (100 - 48) / 2;
	fontEngine->DrawText(playerlivet, textX, textY, 0xff00ffff, FontEngine::FONT_TYPE_SMALL);

	char playerhealtht[256];
	sprintf_s(playerhealtht, "Health %d", PlayerHealth);
	textWidth = fontEngine->CalculateTextWidth(playerhealtht, FontEngine::FONT_TYPE_SMALL);
	textX = (1400 - textWidth) / 2;
	textY = (100 - 48) / 2;
	fontEngine->DrawText(playerhealtht, textX, textY, 0xff00ffff, FontEngine::FONT_TYPE_SMALL);


	char firemodet[256];
	sprintf_s(firemodet, " (SHIFT TO CHANGE)FIRE MODE %s", Str_FireMode);
	textWidth = fontEngine->CalculateTextWidth(firemodet, FontEngine::FONT_TYPE_SMALL);
	textX = (500 - textWidth) / 2;
	textY = (50 - 48) / 2;
	fontEngine->DrawText(firemodet, textX, textY, 0xff00ffff, FontEngine::FONT_TYPE_SMALL);
	

	fontEngine->EndFrame();
	
	
	
}

void Game::InitialiseLevel(int numAsteroids)
{
	CurrentLevel = numAsteroids;
	DeleteAllAsteroids();
	DeleteAllBullets();
	DeleteAllExplosions();
	DeleteAllUFO();
	//collision_->DestroyAllCollider(player_->GetCollider());
	SpawnPlayer();
	SpawnAsteroids(numAsteroids);
	SpawnUFOs(numAsteroids);
	CurrentDestroyed = 0;
	PlayerHealth = 100 * ((numAsteroids + 1) /2);
	UFOFireRate = 200 - (numAsteroids * 2);
	PlayerLives = (CurrentLevel + 3) / 3;
	if (CurrentScore > HighScore) {
		HighScore = CurrentScore;
	}
	if (CurrentLevel == 1) {
		player_->CurrentFireMode = player_->Auto;
		strcpy_s(Str_FireMode, "AUTO");
	}
	player_->CurrentFireMode = player_->Auto;
	player_->MAX_SPEED = (float)(CurrentLevel * 0.2) + 2;
}

bool Game::IsLevelComplete() const
{
	return (asteroids_.empty() && explosions_.empty() && enemies_.empty());
}

bool Game::IsGameOver() const
{
	return (player_ == 0 && explosions_.empty() );
}

void Game::DoCollision(GameEntity *a, GameEntity *b)
{
	Ship *player = static_cast<Ship *>(a == player_ ? a : (b == player_ ? b : 0));
	Bullet *bullet = static_cast<Bullet *>(IsBullet(a) ? a : (IsBullet(b) ? b : 0));
	Asteroid *asteroid = static_cast<Asteroid *>(IsAsteroid(a) ? a : (IsAsteroid(b) ? b : 0));
	EnemyUFO* enemyUFO = static_cast<EnemyUFO*>(IsUFO(a) ? a : (IsUFO(b) ? b : 0));


	
	if (bullet && asteroid && bullet->PawnSpawned)
	{
		AsteroidHit(asteroid);
		DeleteBullet(bullet);
		return;
	}
	if (bullet && enemyUFO && bullet->PawnSpawned) {
		DeleteUFO(enemyUFO);
		DeleteBullet(bullet);
		CurrentScore += 10;
		CurrentDestroyed += 1;
		return;
	}
	if (player && enemyUFO) {
		DeleteUFO(enemyUFO);
		PlayerHealth -= 30;
		CurrentDestroyed += 1;
		return;
	}
	if (player && asteroid)
	{
		PlayerHealth -= 50;
		AsteroidHit(asteroid);
		CurrentLevel += 1;
		return;

	}
	if (bullet && player && !bullet->PawnSpawned) {
		PlayerHealth -= 20;
		DeleteBullet(bullet);
		return;
	}

	
	
	 
	
}



void Game::operator=(const Game&)
{
	
}


void Game::SpawnPlayer()
{
	DeletePlayer();
	player_ = new Ship();
	player_->EnableCollisions(collision_, 10.0f);
}

void Game::DeletePlayer()
{
	
	delete player_;
	player_ = 0;
}


void Game::UpdateUFO(System* system)
{
	for (EnemyUFOList::const_iterator ufoIt = enemies_.begin(),
		end = enemies_.end();
		ufoIt != end;
		++ufoIt)
	{
		(*ufoIt)->Move(FireDelaycnt, CurrentLevel);
		FireUFO(*ufoIt);
		(*ufoIt)->Update(system);
		(*ufoIt)->SetControlInput(1, 0);
		FireUFO(*ufoIt);
		WrapEntity(*ufoIt);
	}
}

void Game::UpdatePlayer(System *system)
{
	if (player_ == 0)
		return;

	Keyboard *keyboard = system->GetKeyboard();

	float acceleration = 0.0f;
	if (keyboard->IsKeyHeld(VK_UP) || keyboard->IsKeyHeld('W'))
	{
		acceleration = 1.0f;
	}
	else if (keyboard->IsKeyHeld(VK_DOWN) || keyboard->IsKeyHeld('S'))
	{
		acceleration = -1.0f;
	}

	float rotation = 0.0f;
	if (keyboard->IsKeyHeld(VK_RIGHT) || keyboard->IsKeyHeld('D'))
	{
		rotation = -1.0f;
	}
	else if (keyboard->IsKeyHeld(VK_LEFT) || keyboard->IsKeyHeld('A'))
	{
		rotation = 1.0f;
	}
	player_->SetControlInput(acceleration, rotation);
	player_->Update(system);
	WrapEntity(player_);
	
	firestate = 0.f;
	if (keyboard->IsKeyHeld(VK_SPACE))
	{
		firestate = 1.f;
		SpawnMultiBullet();
	}
	if (keyboard->IsKeyReleased(VK_SPACE))
	{
		SingleFired = false;
	}
	if (keyboard->IsKeyPressed(VK_LSHIFT)) {
		if (player_->CurrentFireMode == 1) {
			player_->CurrentFireMode = player_->Burst;
			strcpy_s(Str_FireMode,"BURST");
		}
		else if (player_->CurrentFireMode == 2) {
			player_->CurrentFireMode = player_->Single;
		    strcpy_s(Str_FireMode,"SINGLE");
		}
		else if (player_->CurrentFireMode == 3) {
			player_->CurrentFireMode = player_->Auto;
			strcpy_s(Str_FireMode,"AUTO");
		}
		else {
			player_->CurrentFireMode = player_->Auto;
			strcpy_s(Str_FireMode,"AUTO");
		}

	}
	
	
}

void Game::UpdateAsteroids(System *system)
{
	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
	++asteroidIt)
	{
		(*asteroidIt)->Update(system);
		WrapEntity(*asteroidIt);
	}
}

void Game::UpdateBullet(System *system)
{
	/*
	if (bullet_)

	{

    	bullet_->Update(system);
		WrapEntity(bullet_);

	}
	*/
	

	for (BulletList::const_iterator itr = bullets_.begin(); itr != bullets_.end(); itr++) {
		if ((*itr)->Lifetime < 0) {
			
		   (*itr)->DestroyCollider();
			bullets_.remove(*itr);
			itr = bullets_.begin();
			return;
		}
		else {
			(*itr)->Update(system);
			WrapEntity(*itr);
		}
		
	}
	 /*
	for (int i = 0; i < bullets_.size(); i++) {
		BulletExpirecnt[i] += 1;
		if (BulletExpirecnt[i] > 100) {
			bullets_.remove(*bullets_.begin());
			collision_->DestroyCollider(*(collision_->colliders_.begin()));
			BulletExpirecnt.erase(BulletExpirecnt.begin());
			
		}
	}
	*/

}

void Game::UpdateExplosion(System* system)
{
	for (ExplosionList::const_iterator explostionIt = explosions_.begin(),
		end = explosions_.end();
		explostionIt != end;
		++explostionIt)
	{
		if ((*explostionIt)->Lifetime < 0) {
			DeleteExplosion(*explostionIt); 
			return;
		}
		(*explostionIt)->Update(system);
		WrapEntity(*explostionIt);
	}
}

void Game::WrapEntity(GameEntity *entity) const
{
	XMFLOAT3 entityPosition;
	XMStoreFloat3(&entityPosition, entity->GetPosition());
	entityPosition.x = Maths::WrapModulo(entityPosition.x, -400.0f, 400.0f);
	entityPosition.y = Maths::WrapModulo(entityPosition.y, -300.0f, 300.0f);
	entity->SetPosition(XMLoadFloat3(&entityPosition));
}

void Game::DeleteAllAsteroids()
{
	for (AsteroidList::const_iterator asteroidIt = asteroids_.begin(),
		end = asteroids_.end();
		asteroidIt != end;
		++asteroidIt)
	{
		delete (*asteroidIt);
	}
	asteroidmap_.clear();
	asteroids_.clear();
}

void Game::DeleteAllExplosions()
{
	for (ExplosionList::const_iterator explosionIt = explosions_.begin(),
		end = explosions_.end();
		explosionIt != end;
	++explosionIt)
	{
		delete (*explosionIt);
	}

	
}

void Game::DeleteAllBullets()
{
	for (BulletList::const_iterator bulletIt = bullets_.begin(),
		end = bullets_.end();
		bulletIt != end;
		++bulletIt)
	{
		delete (*bulletIt);
	}

	bullets_.clear();
	bulletmap_.clear();
}

void Game::DeleteAllUFO()
{
	for (EnemyUFOList::const_iterator ufoIt = enemies_.begin(),
		end = enemies_.end();
		ufoIt != end;
		++ufoIt)
	{
		delete (*ufoIt);
	}

	enemies_.clear();
	enemiesmap_.clear();
}

void Game::SpawnBullet(XMVECTOR position, XMVECTOR direction)
{
	
	Bullet* temp = new Bullet(position, direction);
	bullets_.push_back(temp);
	BulletExpirecnt.push_back(0);
	bulletmap_.insert({temp,false});
    temp->EnableCollisions(collision_, 3.0f);
	temp->gameref = this;
	temp->PawnSpawned = true;
}

void Game::SpawnMultiBullet()
{
	switch (player_->CurrentFireMode) {
	case 1:
		if (firestate == 1.f) {
			XMVECTOR playerForward = player_->GetForwardVector();
			XMVECTOR bulletPosition = player_->GetPosition() + playerForward * 10.0f;
			if (FireDelaycnt % 10 == 0) {
				SpawnBullet(bulletPosition, playerForward);
			}

		}
	case 2:
		if (firestate == 1.f) {
			XMVECTOR playerForward = player_->GetForwardVector();
			XMVECTOR bulletPosition = player_->GetPosition() + playerForward * 10.0f;
			if ((FireDelaycnt % 10 == 0) && !burstwait) {
				SpawnBullet(bulletPosition, playerForward);
				firestate = 1;
				burstcnt += 1;
				if (burstcnt > 2) {
					burstwait = true;
					burstcnt = 0;
				}
			}
			else if (burstwait) {
				if (FireDelaycnt % 80 == 0) {
					firestate = 0;
					burstwait = false;
					burstcnt = 0;
				}
			}
		}
	case 3:
		if (firestate == 1.f && !SingleFired){
			XMVECTOR playerForward = player_->GetForwardVector();
			XMVECTOR bulletPosition = player_->GetPosition() + playerForward * 10.0f;
				SpawnBullet(bulletPosition, playerForward);
				SingleFired = true;
			

		}
		else if (firestate == 0.f) {
			SingleFired = false;
		}

	}
}

void Game::SpawnExplostion(XMVECTOR postion,int size)
{
	Explosion* AsteroidExplostion = new Explosion(postion,size);
	explosions_.push_back(AsteroidExplostion);
}

void Game::DeleteBullet(Bullet* Bulletref)
{
	Bulletref->DestroyCollider();
	bullets_.remove(Bulletref);
	bulletmap_.erase(Bulletref);
	delete Bulletref;
	
}

void Game::SpawnAsteroids(int numAsteroids)
{
	float halfWidth = 800.0f * 0.5f;
	float halfHeight = 600.0f * 0.5f;
	for (int i = 0; i < numAsteroids; i++)
	{
		float x = Random::GetFloat(-halfWidth, halfWidth);
		float y = Random::GetFloat(-halfHeight, halfHeight);
		XMVECTOR position = XMVectorSet(x, y, 0.0f, 0.0f);
		SpawnAsteroidAt(position, 3);
	}
}

void Game::SpawnAsteroidAt(XMVECTOR position, int size)
{
	const float MAX_ASTEROID_SPEED = 1.0f;

	float angle = Random::GetFloat(Maths::TWO_PI);
	XMMATRIX randomRotation = XMMatrixRotationZ(angle);
	XMVECTOR velocity = XMVectorSet(0.0f, Random::GetFloat(MAX_ASTEROID_SPEED), 0.0f, 0.0f);
	velocity = XMVector3TransformNormal(velocity, randomRotation);

	Asteroid *asteroid = new Asteroid(position, velocity, size);
	asteroid->EnableCollisions(collision_, size * 5.0f);
	asteroids_.push_back(asteroid);
	asteroidmap_.insert({ asteroid,false });
	
}

void Game::SpawnUFOs(int numUFO)
{
	float halfWidth = 800.0f * 0.5f;
	float halfHeight = 600.0f * 0.5f;
	for (int i = 0; i < numUFO; i++)
	{
		float x = Random::GetFloat(-halfWidth, halfWidth);
		float y = Random::GetFloat(-halfHeight, halfHeight);
		XMVECTOR position = XMVectorSet(x, y, 0.0f, 0.0f);
		SpawnUFOAt(position);
	}
}

void Game::SpawnUFOAt(XMVECTOR position)
{

	float angle = Random::GetFloat(Maths::TWO_PI);
	XMMATRIX randomRotation = XMMatrixRotationZ(angle);

	EnemyUFO* enemyUFO = new EnemyUFO(position);
	enemyUFO->EnableCollisions(collision_, 13);
	enemies_.push_back(enemyUFO);
	enemiesmap_.insert({enemyUFO, false});
}

bool Game::IsAsteroid(GameEntity *entity) 
{   
	Asteroid* temp;
	temp = (Asteroid*)entity;
	if (asteroidmap_.find(temp) != asteroidmap_.end()) {
		return true;
	}
	else {
		return false;
	}
}

bool Game::IsBullet(GameEntity* entity)
{
	Bullet* temp;
	temp = (Bullet*)entity;
	if (bulletmap_.find(temp) != bulletmap_.end()) {
		return true;
	}
	else {
		return false;
	}
	
}

bool Game::IsUFO(GameEntity* entity)
{
	EnemyUFO* temp;
	temp = (EnemyUFO*)entity;
	if (enemiesmap_.find(temp) != enemiesmap_.end()) {
		return true;
	}
	else {
		return false;
	}

}




void Game::AsteroidHit(Asteroid *asteroid)
{
	int oldSize = asteroid->GetSize();
	int smallerSize = oldSize - 1;
	XMVECTOR position = asteroid->GetPosition();
	if (oldSize > 1)
	{
		SpawnAsteroidAt(position, smallerSize);
		CurrentScore += (oldSize * 5);
	}
	else {
		SpawnExplostion(position, 10);
		CurrentDestroyed += 1;
	}
		DeleteAsteroid(asteroid);
		
	
}

void Game::DeleteAsteroid(Asteroid *asteroid)
{
	asteroid->DestroyCollider();
	asteroids_.remove(asteroid);
	asteroidmap_.erase(asteroid);
	delete asteroid;
}

void Game::DeleteExplosion(Explosion* explosion)
{
	explosion->DestroyCollider();
	explosions_.remove(explosion);
	delete explosion;
}

void Game::DeleteUFO(EnemyUFO* enemyUFO)
{
	SpawnExplostion(enemyUFO->GetPosition(), 10);
	enemyUFO->DestroyCollider();
	enemies_.remove(enemyUFO);
	enemiesmap_.erase(enemyUFO);
	delete enemyUFO;
}

void Game::FireUFO(EnemyUFO* enemyUFO)
{
	if (FireDelaycnt % UFOFireRate == 0) {
		XMVECTOR playerForward = enemyUFO->GetForwardVector();
		XMVECTOR bulletPosition = enemyUFO->GetPosition() + playerForward * 10.0f;

		Bullet* temp = new Bullet(bulletPosition, playerForward);
		bulletmap_.insert({ temp,false });
		bullets_.push_back(temp);
		BulletExpirecnt.push_back(0);
		temp->EnableCollisions(collision_, 3.0f);
		temp->gameref = this;
		temp->PawnSpawned = false;
	}
}

void Game::UpdateCollisions()
{
	collision_->DoCollisions(this);
}
