#include "engine/physics/physics_world.hpp"
#include <algorithm>
#include <cmath>
namespace origin{void PhysicsWorld::step(EntityRegistry&e,const World&w,double dt){for(auto&x:e.all())if(x.alive&&x.body.dynamic){x.body.velocity.y-=9.81*dt;x.body.position+=x.body.velocity*dt;x.body.position.x=std::clamp(x.body.position.x,0.0,(double)w.width()-1e-4);double floor=w.ground_height(x.body.position.x)+x.body.radius_m;if(x.body.position.y<floor){x.body.position.y=floor;if(std::abs(x.body.velocity.y)>.05)x.body.velocity.y=-x.body.velocity.y*x.body.restitution;else x.body.velocity.y=0;x.body.velocity.x*=.98;}}}}
