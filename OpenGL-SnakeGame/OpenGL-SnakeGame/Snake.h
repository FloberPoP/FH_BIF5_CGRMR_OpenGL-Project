#pragma once
class Snake
{
public:
	Snake(float gridFieldSize);
	void ApplyMovement();
	bool CollidesWithBorder();
	bool CollidesWithSelf();

	float posX = 0.0f;
	float posY = 0.0f;
	float dirX = 0.0f;
	float dirY = 0.0f;

private:
	float _gridFieldSize;
};

