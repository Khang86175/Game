#pragma once
#include <vector>

// Forward declarations to avoid circular dependencies
class MyTank;
class EnemyTank;
class Bullet;
class Obstacle;

void handleBulletObstacleCollision(std::vector<Bullet>& bullets, std::vector<Obstacle>& obstacles, MyTank& mytank);
void handleBulletTankCollision(std::vector<Bullet>& enemy_bullets, MyTank& tank);
void handleBulletTankCollision(std::vector<Bullet>& my_bullets,EnemyTank& tank);
void handleBullet_BulletCollision(std::vector<Bullet>& my_bullets, std::vector<Bullet>& enemy_bullets);
void handleTankObstacleCollision(MyTank &tank, std::vector<Obstacle> &obs);
void handle2TankCollision(MyTank& my_tank,EnemyTank& enemy_tank);
void handle2ObstacleCollision(std::vector<Obstacle>& obs);