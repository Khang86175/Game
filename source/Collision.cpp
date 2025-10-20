#include "Collision.hpp"
#include "Tank.hpp"      // Need full definitions here
#include "Bullet.hpp"
#include "Obstacle.hpp"
#include "Globals.hpp"

void handleBulletObstacleCollision(std::vector<Bullet>& bullets, std::vector<Obstacle>& obstacles, MyTank& mytank) {
    for (int i=0;i<bullets.size();i++) {
        if (!bullets[i].alive) 
            continue;
        for (int j=0;j<obstacles.size();j++) {
            if (!obstacles[j].alive) 
                continue;
            if (bullets[i].body.isCollidingWith(obstacles[j].body)) {
                bullets[i].body.handleCollisionWith(obstacles[j].body,true,true,0.5);
                if (bullets[i].body.hp <= 0.f){
                    bullets[i].alive = false;
                }
                if (obstacles[j].body.hp <= 0.f) {
                    obstacles[j].alive = false;
                    obstacles[j].timeToRespawn=currentFrame+fps*30;
                    mytank.addScore(obstacles[j].xp_reward);
                    mytank.levelUp();
                }
                break;
            }
        }
    }
    for (int i = (int)bullets.size() - 1; i >= 0; --i) 
        if (!bullets[i].alive) 
            removeBullet(bullets, i);
}
void handleBulletTankCollision(std::vector<Bullet>& bullets, Obj& tank) {
    for (int i=0;i<bullets.size();i++) {
        if (!bullets[i].alive) 
            continue;
        if (bullets[i].body.isCollidingWith(tank) == false) 
            continue;
        bullets[i].body.handleCollisionWith(tank,true,true,0.5);
        if (bullets[i].body.hp <= 0.f){
            bullets[i].alive = false;
        }
    }
    for(int i=0;i<bullets.size();i++)
        if(!bullets[i].alive)
            removeBullet(bullets,i);
}
void handleBullet_BulletCollision(std::vector<Bullet>& my_bullets,std::vector<Bullet>& enemy_bullets){
    for(int i=0;i<my_bullets.size();i++){
        if(my_bullets[i].alive==false)
            continue;
        for(int j=0;j<enemy_bullets.size();j++){
            if(enemy_bullets[j].alive == false)
                continue;
            if(!my_bullets[i].body.isCollidingWith(enemy_bullets[j].body))
                continue;
            my_bullets[i].body.handleCollisionWith(enemy_bullets[j].body,true,false);
            if(my_bullets[i].body.hp == 0){
                my_bullets[i].alive=false;
            }
            if(enemy_bullets[j].body.hp==0){
                enemy_bullets[j].alive=false;
            }
        }
    }
    for(int i=0;i<my_bullets.size();i++)
        if(my_bullets[i].alive == false)
            removeBullet(my_bullets,i);
    for(int i=0;i<enemy_bullets.size();i++)
        if(enemy_bullets[i].alive == false)
            removeBullet(enemy_bullets,i);

}
void handleTankObstacleCollision(MyTank &tank,std::vector<Obstacle> & obs){
    for(int i=0;i<obs.size();i++){
        if(obs[i].alive==false)
            continue;
        if(obs[i].body.isCollidingWith(tank.body)){
            tank.body.handleCollisionWith(obs[i].body,false,true,0.5);
            if(obs[i].body.hp<=0){
                obs[i].alive = false;
                tank.addScore(obs[i].xp_reward);
                tank.levelUp();
                obs[i].timeToRespawn=currentFrame+fps*30;
            }
            return;
        }
    }
}
void handleBotObstacleCollision(EnemyTank &tank, std::vector<Obstacle> &obs){
    for(int i=0;i<obs.size();i++){
        if(obs[i].alive==false)
            continue;
        if(obs[i].body.isCollidingWith(tank.body)){
            tank.body.handleCollisionWith(obs[i].body,false,true,0.5,0);
            if(obs[i].body.hp<=0){
                obs[i].alive = false;
                obs[i].timeToRespawn=currentFrame+fps*30;
            }
            return;
        }
    }
}
void handleTankBotCollision(MyTank& my_tank,EnemyTank& enemy_tank){
    if(!my_tank.body.isCollidingWith(enemy_tank.body))
        return;
    my_tank.body.handleCollisionWith(enemy_tank.body,false,true,1,5);
}
void handle2ObstacleCollision(std::vector<Obstacle>& obs){
    for(int i=0;i<obs.size();i++){
        if(!obs[i].alive)
            continue;
        for(int j=i+1;j<obs.size();j++){
            if(!obs[j].alive)
                continue;
            if(obs[i].body.isCollidingWith(obs[j].body))
                obs[i].body.handleCollisionWith(obs[j].body,false,true,1,0);
        }
    }
}
