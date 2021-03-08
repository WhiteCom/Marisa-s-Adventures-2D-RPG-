#pragma once

#define DISPLAY_FPS

class iFPS
{
public: 
	static iFPS* instance();
private:
	iFPS();
public:
	float update();

	void setSlowTime(float duration);

#ifdef DISPLAY_FPS
	int fps(bool apply = false);
#endif //DISPLAY_FPS

public:
	unsigned long now;
#ifdef DISPLAY_FPS
	int num;
	float tickDt;
	int f;
#endif //DISPLAY_FPS
	float lastDt;
};