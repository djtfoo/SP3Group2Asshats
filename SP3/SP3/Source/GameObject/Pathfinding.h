#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "Vector3.h"
#include <vector>

struct Node
{
    Node* parent;

    const Vector3 position;
    
    float heuristic;
    float dist;
    // F value

    // find current tile player is at
    // open ist
    // closed list
};

class Pathfinding
{
    std::vector<Node> movementNodes;

    Vector3 position;
    Vector3 destination;

public:
    Pathfinding();
    ~Pathfinding();

    void Update(double dt);

    bool hasFinishedMovement();
    void ReceiveCurrentPos(const Vector3& pos);
    void ReceiveDestination(const Vector3& pos);    // when movementNodes is empty or state changes, receive new destination
    void CalculatePath();   // call this when monster changes path (e.g. due to movement nodes being empty or change in state
};

#endif