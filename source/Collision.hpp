#pragma once
#include <vector>

// Forward declarations to avoid circular dependencies
class MyTank;
class EnemyTank;
class Bullet;
class Obstacle;
class Obj;

void handleBulletObstacleCollision(std::vector<Bullet>& bullets, std::vector<Obstacle>& obstacles, MyTank& mytank);
void handleBulletTankCollision(std::vector<Bullet>& bullets, Obj& tank);
void handleBullet_BulletCollision(std::vector<Bullet>& my_bullets, std::vector<Bullet>& enemy_bullets);
void handleTankObstacleCollision(MyTank &tank, std::vector<Obstacle> &obs);
void handleBotObstacleCollision(EnemyTank &tank, std::vector<Obstacle> &obs);
void handleTankBotCollision(MyTank& my_tank,EnemyTank& enemy_tank);
void handle2ObstacleCollision(std::vector<Obstacle>& obs);