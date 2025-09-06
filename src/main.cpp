#include "game.h"

#include "global.h"
#include "solver.h"
#include "render.h"
#include "simulation.h"

using namespace std;
mt19937 rng;

int main(){
    rng.seed(time(nullptr));
    run();
    return 0;
}