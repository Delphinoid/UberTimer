#ifndef VERTEX3D_HPP
#define VERTEX3D_HPP

#include <glm/glm.hpp>

struct vertex3D{
	glm::vec3 pos;
	float u, v;
	float nx, ny, nz;
	vertex3D(){ pos = glm::vec3(0.f); u = 0.f; v = 0.f; nx = 0.f; ny = 0.f; nz = 0.f; }
};

#endif
