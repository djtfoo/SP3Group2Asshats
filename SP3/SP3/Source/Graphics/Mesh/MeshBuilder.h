#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "../Vertex/Vertex.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);
	static Mesh* GenerateQuad(const std::string &meshName, Color color, float length = 1.f);
	static Mesh* GenerateCube(const std::string &meshName, Color color, float length = 1.f);
	static Mesh* GenerateRing(const std::string &meshName, Color color, unsigned numSlice, float outerR = 1.f, float innerR = 0.f);
	static Mesh* GenerateSphere(const std::string &meshName, Color color, unsigned numStack, unsigned numSlice, float radius = 1.f);
	static Mesh* GenerateCone(const std::string &meshName, Color color, unsigned numSlice, float radius, float height);
	static Mesh* GenerateOBJ(const std::string &meshName, const std::string &file_path);
	static Mesh* GenerateText(const std::string &meshName, unsigned row, unsigned col);

    static Mesh* GenerateDiamond(const std::string &meshName, Color color, float length);
    static Mesh* GenerateTriangle(const std::string &meshName, Color color, float lengthX, float lengthY);
    static Mesh* GenerateRightAngledTriangle(const std::string &meshName, Color color, float lengthX, float lengthY);
    static Mesh* GenerateSemicircle(const std::string &meshName, Color color, float radius, unsigned numSlices = 36);
    static Mesh* GenerateQuadrant(const std::string &meshName, Color color, float radius, unsigned numSlices = 36);
    static Mesh* GenerateHexagon(const std::string &meshName, Color color);
    static Mesh* GenerateStar(const std::string &meshName, Color color);
};

#endif