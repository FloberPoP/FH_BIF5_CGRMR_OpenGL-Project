#pragma once
#include <glm/glm.hpp>

class SnakePart
{
public:
	glm::vec2 pos;
	SnakePart* prev;

	SnakePart(glm::vec2 pos, SnakePart* prev = nullptr)
	{
		this->pos = pos;
		this->prev = prev;
	}

	~SnakePart()
	{
		delete prev;
	}
};
