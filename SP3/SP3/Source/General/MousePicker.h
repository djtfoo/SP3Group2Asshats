#pragma once
#include "Vector3.h"
#include "MatrixStack.h"
#include "ZooCamera.h"
//#include "Application.h"

static const int RECURSION_COUNT = 200;
static const float RAY_RANGE = 600;

class MousePicker
{
public:

    Vector3 currentRay = Vector3();

    Mtx44 projectionMatrix;
    Mtx44 viewMatrix;
    ZooCamera camera;

    MousePicker(ZooCamera cam, Mtx44 projection)
    {
        camera = cam;
        projectionMatrix = projection;

        viewMatrix.SetToLookAt(camera.position.x, camera.position.y, camera.position.z,
            camera.target.x, camera.target.y, camera.target.z,
            camera.up.x, camera.up.y, camera.up.z);
    }

    Vector3 getCurrentRay() 
    {
        return currentRay;
    }

    Vector3 getNormalisedDeviceCoordinates(float mouseX, float mouseY) 
    {
        float x = (2.0f * mouseX) / 1920 - 1.f;
        float y = (2.0f * mouseY) / 1080 - 1.f;
        return Vector3(x, y, -1);
    }

    Vector3 toWorldCoords(Mtx44 eyeCoords) 
    {
        Mtx44 rayWorld = viewMatrix.GetInverse() * eyeCoords;

        Vector3 mouseRay = rayWorld * Vector3(1, 1, 1);
        mouseRay.Normalize();

        return mouseRay;
    }

    Mtx44 toEyeCoords(Mtx44 clipCoords) 
    {
        Mtx44 eyeCoords = clipCoords * projectionMatrix.GetInverse();

        return eyeCoords;
    }

    Vector3 calculateMouseRay() 
    {
        float mouseX = 100;
        float mouseY = 100;
        Vector3 normalizedCoords = getNormalisedDeviceCoordinates(mouseX, mouseY);

        Mtx44 clipCoords;
        clipCoords.SetToPerspective(normalizedCoords.x, normalizedCoords.y, -1.0f, 1.0f);

        Mtx44 eyeCoords = toEyeCoords(clipCoords);
        Vector3 worldRay = toWorldCoords(eyeCoords);

        return worldRay;
    }

    void update() 
    {
        viewMatrix.SetToLookAt(camera.position.x, camera.position.y, camera.position.z,
            camera.target.x, camera.target.y, camera.target.z,
            camera.up.x, camera.up.y, camera.up.z);

        currentRay = calculateMouseRay();
    }

    //**********************************************************//

    Vector3 getPointOnRay(Vector3 ray, float distance) 
    {
        Vector3 start;
        start.Set(camera.position.x, camera.position.y, camera.position.z);

        Vector3 scaledRay;
        scaledRay.Set(ray.x * distance, ray.y * distance, ray.z * distance);

        return (start + scaledRay);
    }

    MousePicker();
    ~MousePicker();

private:

};

