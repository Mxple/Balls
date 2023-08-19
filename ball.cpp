#include "ball.h"
#include "curve.h"


Ball::Ball() {
    pos = Vec2f();
    vel = Vec2f();
}

void Ball::step(const double& acc, const int timestep) {
    vel.y += acc;
    if (!collide(pos.x + vel.x, pos.y + vel.y)) {
        pos += vel;
        return;
    }

    for (int i = 0; i < timestep; i++) {
        // collision handling
        if (collide(pos.x + vel.x / timestep, pos.y + vel.y / timestep)) {
            // https://physics.stackexchange.com/questions/678984/elastic-collision-of-a-ball-against-a-container-wall
            Vec2f wall_tan = Vec2f(1, curve_derivative(pos.x)).normalize();
      //       Vec2f vel_parl = wall_tan.scale(wall_tan.dot(vel));
      //       Vec2f vel_perp = vel - vel_parl; 
      //
      //       // apply collision
      //       vel_perp.scale(-1);
      // 
      //       // convert back to cartesian
      //       vel = vel_perp + vel_parl;

            vel = wall_tan.scale(2 * wall_tan.dot(vel)) - vel;
        }
        pos += vel * (1. / timestep);
    }
}

// fast step does not check for collisions. used for when balls are outside chunks with curves
void Ball::fast_step(const double& acc) {
    vel.y += acc;
    pos += vel;
}
