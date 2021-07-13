#include "iStd.h"

#include "iWindow.h"

iSize devSize;
iRect viewport;

bool* keyBuf;

void appInitialize()
{
	srand(time(NULL));
	keyBuf = new bool[256];
	memset(keyBuf, 0xff, 256);
}

struct Zoom
{
	bool ing;
	float delta, _delta;
	float delay, _delay;
	float scale;
	iPoint position;
};

static Zoom zoom =
{
	false,
	0.5f, 0.5f,
	0.5f, 0.5f,
	1.0f,
	{0,0},
};

bool setZoom(iPoint point, float scale, float deltaZoom, float deltaDelay)
{
	if (zoom.ing)
		return false;

	zoom.ing = true;
	zoom.position = point;
	zoom.scale = scale;
	zoom._delta = deltaZoom;
	zoom.delta = 0.0f;
	zoom._delay = deltaDelay;
	zoom.delay = 0.0f;

	return true;
}

float updateZoom(float dt, iPoint& p)
{
	float s = 1.0f;
	if (zoom.ing == false)
		return s;

	if (zoom.delta < zoom._delta)
	{
		s = linear(zoom.delta / zoom.delta, 1.0f, zoom.scale);

		zoom.delta += dt;
		if (zoom.delta > zoom._delta)
			zoom.delta = zoom._delta;

	}
	else if (zoom.delta == zoom._delta)
	{
		s = zoom.scale;

		zoom.delay += dt;
		if (zoom.delay > zoom._delay)
			zoom.delta += 0.000001f;
	}
	else if (zoom.delta < zoom._delta * 2)
	{
		s = linear((zoom.delta - zoom._delta) / zoom._delta, zoom.scale, 1.0f);

		zoom.delta += dt;
	}
	else
	{
		zoom.ing = false;
	}
	p = zoom.position * (1.0f - s);

	return s;
}

int keyStat = 0, keyDown = 0; //stat : 계속 누름, down : 한번누름
void setKeys(int& keys, iKeyStat stat, unsigned int c);

void setKeyStat(iKeyStat stat, unsigned int c)
{
	setKeys(keyStat, stat, c);
}

void setKeyDown(iKeyStat stat, unsigned int c)
{
	//just one time
	if (stat == iKeyStatBegan)
	{
		if (keyBuf[c])
			return;
		keyBuf[c] = true;
		setKeys(keyDown, stat, c);
	}
	else
	{
		keyBuf[c] = false;
	}
}

void setKeys(int& keys, iKeyStat stat, unsigned int c)
{
	if (stat == iKeyStatBegan)
	{
		if (c == 'a' || c == 'A')
			keys |= keysA;
		else if (c == 's' || c == 'S')
			keys |= keysS;
		else if (c == 'd' || c == 'D')
			keys |= keysD;
		else if (c == 'w' || c == 'W')
			keys |= keysW;
		else if (c == ' ')
			keys |= keysSpace;

		else if (c == VK_LEFT)
			keys |= keysLeft;
		else if (c == VK_RIGHT)
			keys |= keysRight;
		else if (c == VK_UP)
			keys |= keysUp;
		else if (c == VK_DOWN)
			keys |= keysDown;
	}

	else if (stat == iKeyStatEnded)
	{
		if (c == 'a' || c == 'A')
			keys &= ~keysA;
		else if (c == 's' || c == 'S')
			keys &= ~keysS;
		else if (c == 'd' || c == 'D')
			keys &= ~keysD;
		else if (c == 'w' || c == 'W')
			keys &= ~keysW;
		else if (c == ' ')
			keys &= ~keysSpace;

		else if (c == VK_LEFT)
			keys &= ~keysLeft;
		else if (c == VK_RIGHT)
			keys &= ~keysRight;
		else if (c == VK_UP)
			keys &= ~keysUp;
		else if (c == VK_DOWN)
			keys &= ~keysDown;
	}
}

int random()
{
	return rand();
}

float _cos(float degree)
{
	return cos(degree * M_PI / 180);
}
float _sin(float degree)
{
	return sin(degree * M_PI / 180);
}

float uint8ToFloat(uint8 c)
{
	return c / 255.0f;
}

uint8 floatToUint8(float c)
{
	return (uint8)(c * 0xFF);
}

static float _r = 1.0f, _g = 1.0f, _b = 1.0f, _a = 1.0f;

void setRGBA(iColor4f c)
{
	_r = c.r;
	_g = c.g;
	_b = c.b;
	_a = c.a;
}

void setRGBA(iColor4b c)
{
	_r = uint8ToFloat(c.r);
	_g = uint8ToFloat(c.g);
	_b = uint8ToFloat(c.b);
	_a = uint8ToFloat(c.a);
}
void setRGBA(float r, float g, float b, float a)
{
	_r = r;
	_g = g;
	_b = b;
	_a = a;
}

void getRGBA(float& r, float& g, float& b, float& a)
{
	r = _r;
	g = _g;
	b = _b;
	a = _a;
}

static float _lineWidth = 1.0f;
void setLineWidth(float width)
{
	_lineWidth = width;
}
float getLineWidth()
{
	return _lineWidth;
}

void clearRect()
{
	glClearColor(_r, _g, _b, _a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void setClip(float x, float y, float width, float height)
{
	if (width == 0.0f || height == 0.0f)
	{
		glDisable(GL_SCISSOR_TEST);
		//glScisor(0,0,devSize.width, devSize.height);
	}

	else
	{
		glEnable(GL_SCISSOR_TEST);
		// devSize(x,y, width, height) => 
#if 0 //backBuffer 쓸때
		glScissor(x, y, width, height);
#else //backBuffer 안쓸때
		//win_border_width
		glScissor(x, y + 16, width, height);
#endif

	}
}

void drawLine(iPoint sp, iPoint ep)
{
	glLineWidth(_lineWidth);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	iPoint p[2] = { sp,ep };
	glVertexPointer(2, GL_FLOAT, sizeof(iPoint), p);

	iColor4f c[2] = { {_r, _g, _b, _a}, {_r, _g, _b, _a} };
	glColorPointer(4, GL_FLOAT, sizeof(iColor4f), c);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

}
void drawLine(float x0, float y0, float x1, float y1)
{
	glLineWidth(_lineWidth);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	iPoint p[2] = { {x0, y0}, {x1, y1} };
	glVertexPointer(2, GL_FLOAT, sizeof(iPoint), p);

	iColor4f c[2] = { {_r, _g, _b, _a}, {_r, _g, _b, _a} };
	glColorPointer(4, GL_FLOAT, sizeof(iColor4f), c);

	glDrawArrays(GL_LINES, 0, 2);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

void drawRect(iRect rt, float radius)
{
	drawRect(rt.origin.x, rt.origin.y, rt.size.width, rt.size.height, radius);
}

void drawRect(float x, float y, float width, float height, float radius)
{
	drawLine(x, y, x + width, y);
	drawLine(x, y + height, x + width, y + height);
	drawLine(x, y, x, y + height);
	drawLine(x + width, y, x + width, y + height);
}

void fillRect(iRect rt, float radius)
{
	fillRect(rt.origin.x, rt.origin.y, rt.size.width, rt.size.height, radius);
}
void fillRect(float x, float y, float width, float height, float radius)
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	iPoint p[4] = {
		{x, y},			{x + width, y},
		{x, y + height},{x + width, y + height}
	};

	glVertexPointer(2, GL_FLOAT, sizeof(iPoint), p);

	iColor4f c[4] = {
		{_r, _g, _b, _a},	{_r, _g, _b, _a},
		{_r, _g, _b, _a},	{_r, _g, _b, _a},
	};
	glColorPointer(4, GL_FLOAT, sizeof(iColor4f), c);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
}

uint32 nextPot(uint32 x)
{
	x = x - 1;
	x = x | (x >> 1);
	x = x | (x >> 2);
	x = x | (x >> 4);
	x = x | (x >> 8);
	x = x | (x >> 16);
	return x + 1;
}

void copyBmp2RGBA(uint32* src, int stride, int x, int y, int w, int h,
				  uint32* dst, int dw, int dh)
{
	int potWidth = nextPot(dw);
	int potHeight = nextPot(dh);
	memset(dst, 0x00, sizeof(uint32) * potWidth * potHeight);

	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			uint8* d = (uint8*)&dst[potWidth * j + i];
			uint8* s = (uint8*)&src[stride * (y + j) + (x + i)];
			d[0] = s[2]; // r
			d[1] = s[1]; // g
			d[2] = s[0]; // b
			d[3] = s[3]; // a
		}
	}
}

void copyBmp2RGBABgAlpha(uint32* src, int stride, int x, int y, int w, int h,
						uint32* dst, int dw, int dh)
{
	int potWidth = nextPot(dw);
	int potHeight = nextPot(dh);
	memset(dst, 0x00, sizeof(uint32) * potWidth * potHeight);

	for (int j = 0; j < h; j++)
	{
		for (int i = 0; i < w; i++)
		{
			uint8* d = (uint8*)&dst[potWidth * j + i];
			if (src[stride * (y + j) + (x + i)] == src[0])
			{
				d[3] = 0;
				continue;
			}
			uint8* s = (uint8*)&src[stride * (y + j) + (x + i)];
			d[0] = s[2]; // r
			d[1] = s[1]; // g
			d[2] = s[0]; // b
			d[3] = s[3]; // a
		}
	}
}



Texture* createImageWithRGBA(uint8* rgba, int width, int height)
{
	int potWidth = nextPot(width);
	int potHeight = nextPot(height);

	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	applyTextureParms();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, potWidth, potHeight, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, rgba);
	glBindTexture(GL_TEXTURE_2D, 0);

	Texture* tex = new Texture;
	tex->texID = texID;
	tex->width = width;
	tex->height = height;
	tex->potWidth = potWidth;
	tex->potHeight = potHeight;
	tex->retainCount = 1;
#if CHECK_TEXTURE
	texture_num++;
#endif
	return tex;
}

Texture* createTexture(int width, int height)
{
	GLuint texID;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	applyTextureParms();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	Texture* tex = new Texture;
	tex->texID = texID;
	tex->width = width;
	tex->height = height;
	tex->potWidth = width;
	tex->potHeight = height;
	tex->retainCount = 1;
#if CHECK_TEXTURE
	texture_num++;
#endif

	return tex;
}

#if CHECK_TEXTURE
	int texture_num = 0;
#endif

Texture** createImageDivide(int numX, int numY, const char* szFormat, ...)
{
	char szText[1024];
	va_start_end(szText, szFormat);

	wchar_t* ws = utf8_to_utf16(szText);
	Bitmap* bmp = Bitmap::FromFile(ws, false);
	delete ws;

	// lock
	int width = bmp->GetWidth();
	int height = bmp->GetHeight();
	Rect rect(0, 0, width, height);
	BitmapData bmpData;
	bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

	int stride = bmpData.Stride / 4;
	uint32* rgba = (uint32*)bmpData.Scan0;

	// to do...
	int numXY = numX * numY;
	Texture** texs = new Texture * [numXY];
	int w = width / numX;
	int h = height / numY;
	int potW = nextPot(w);
	int potH = nextPot(h);
	uint32* pixel = new uint32[potW * potH];

	for (int j = 0; j < numY; j++)
	{
		for (int i = 0; i < numX; i++)
		{
			copyBmp2RGBA(rgba, stride, w * i, h * j, w, h, pixel, w, h);
			texs[numX * j + i] = createImageWithRGBA((uint8*)pixel, w, h);
		}
	}

	delete pixel;

	// unlock
	bmp->UnlockBits(&bmpData);

	return texs;
}

Texture* createImage(const char* szFormat, ...)
{
	char szText[1024];
	va_start_end(szText, szFormat);

	wchar_t* ws = utf8_to_utf16(szText);
	Bitmap* bmp = Bitmap::FromFile(ws, false);
	delete ws;

	int width = bmp->GetWidth();
	int height = bmp->GetHeight();
	Rect rect(0, 0, width, height);
	BitmapData bmpData;
	bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

	int stride = bmpData.Stride / 4;
	uint32* rgba = (uint32*)bmpData.Scan0;

	int potWidth = nextPot(width);
	int potHeight = nextPot(height);
	uint32* pixel = new uint32[potWidth * potHeight];
	copyBmp2RGBA(rgba, stride, 0, 0, width, height,
		pixel, width, height);

	Texture* tex = createImageWithRGBA((uint8*)pixel, width, height);
	delete pixel;

	bmp->UnlockBits(&bmpData);

	return tex;
}

Texture** createImageAlphaDivide(int numX, int numY, const char* szFormat, ...)
{
	char szText[1024];
	va_start_end(szText, szFormat);

	wchar_t* ws = utf8_to_utf16(szText);
	Bitmap* bmp = Bitmap::FromFile(ws, false);
	delete ws;

	// lock
	int width = bmp->GetWidth();
	int height = bmp->GetHeight();
	Rect rect(0, 0, width, height);
	BitmapData bmpData;
	bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

	int stride = bmpData.Stride / 4;
	uint32* rgba = (uint32*)bmpData.Scan0;

	// to do...
	int numXY = numX * numY;
	Texture** texs = new Texture * [numXY];
	int w = width / numX;
	int h = height / numY;
	int potW = nextPot(w);
	int potH = nextPot(h);
	uint32* pixel = new uint32[potW * potH];

	for (int j = 0; j < numY; j++)
	{
		for (int i = 0; i < numX; i++)
		{
			copyBmp2RGBABgAlpha(rgba, stride, w * i, h * j, w, h, pixel, w, h);
			texs[numX * j + i] = createImageWithRGBA((uint8*)pixel, w, h);
		}
	}

	delete pixel;

	// unlock
	bmp->UnlockBits(&bmpData);

	return texs;
}

Texture* createImageAlpha(const char* szFormat, ...)
{
	char szText[1024];
	va_start_end(szText, szFormat);

	wchar_t* ws = utf8_to_utf16(szText);
	Bitmap* bmp = Bitmap::FromFile(ws, false);
	delete ws;

	int width = bmp->GetWidth();
	int height = bmp->GetHeight();
	Rect rect(0, 0, width, height);
	BitmapData bmpData;
	bmp->LockBits(&rect, ImageLockModeRead, PixelFormat32bppARGB, &bmpData);

	int stride = bmpData.Stride / 4;
	uint32* rgba = (uint32*)bmpData.Scan0;

	int potWidth = nextPot(width);
	int potHeight = nextPot(height);
	uint32* pixel = new uint32[potWidth * potHeight];
	copyBmp2RGBABgAlpha(rgba, stride, 0, 0, width, height,
		pixel, width, height);

	Texture* tex = createImageWithRGBA((uint8*)pixel, width, height);
	delete pixel;

	bmp->UnlockBits(&bmpData);

	return tex;
}

void freeImage(Texture* tex)
{
	if (tex->retainCount > 1)
	{
		tex->retainCount--;
		return;
	}
	glDeleteTextures(1, &tex->texID);
	delete tex;
}

void drawImage(Texture* tex, float x, float y, int anc,
	float ix, float iy, float iw, float ih,
	float rx, float ry,
	int xyz, float degree, int reverse)
{
	//Image* img = (Image*)_img;
	float w = iw * rx, h = ih * ry;

	switch (anc)
	{
	case TOP | LEFT:										break;
	case TOP | RIGHT:			x -= w;						break;
	case TOP | HCENTER:			x -= w / 2;					break;

	case BOTTOM | LEFT:						y -= h;			break;
	case BOTTOM | RIGHT:		x -= w;		y -= h;			break;
	case BOTTOM | HCENTER:		x -= w / 2; y -= h;			break;

	case VCENTER | LEFT:					y -= h / 2;		break;
	case VCENTER | RIGHT:		x -= w;		y -= h / 2;		break;
	case VCENTER | HCENTER:		x -= w / 2;	y -= h / 2;		break;
	}

	iPoint dstPoint[4] = {
		{x, y},			{x + w, y},
		{ x, y + h },	{x + w, y + h},
	};

	if (reverse == REVERSE_NONE);
	else if (reverse == REVERSE_WIDTH) //좌우 반전
	{
		float t = dstPoint[0].x;
		dstPoint[0].x = dstPoint[1].x;
		dstPoint[1].x = t;

		dstPoint[2].x = dstPoint[0].x;
		dstPoint[3].x = dstPoint[1].x;
	}

	else if (reverse == REVERSE_HEIGHT) //상하 반전
	{
		float t = dstPoint[0].y;
		dstPoint[0].y = dstPoint[2].y;
		dstPoint[2].y = t;

		dstPoint[1].y = dstPoint[0].y;
		dstPoint[3].y = dstPoint[2].y;
	}

	iPoint t = iPointMake(x + w / 2, y + h / 2);
	if (xyz == 0)
	{
		dstPoint[0].y =
			dstPoint[1].y = y + h / 2 - h / 2 * _cos(degree);
		dstPoint[2].y =
			dstPoint[3].y = y + h / 2 + h / 2 * _cos(degree);
	}
	else if (xyz == 1)
	{
		dstPoint[0].x =
			dstPoint[2].x = x + w / 2 - w / 2 * _cos(degree);
		dstPoint[1].x =
			dstPoint[3].x = x + w / 2 + w / 2 * _cos(degree);
	}

	else //if(xyz == 2)
	{
		for (int i = 0; i < 4; i++)
			dstPoint[i] = iPointRotate(dstPoint[i], t, degree);
	}

	iPoint st[4] = {
		{ix / tex->potWidth, iy / tex->potHeight},			{(ix + iw) / tex->potWidth, iy / tex->potHeight},
		{ix / tex->potWidth, (iy + ih) / tex->potHeight},	{(ix + iw) / tex->potWidth, (iy + ih) / tex->potHeight},
	};

	iColor4f c[4] = {
		 {_r, _g, _b, _a}, {_r, _g, _b, _a},
		 {_r, _g, _b, _a}, {_r, _g, _b, _a},
	};

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, tex->texID);

	glVertexPointer(2, GL_FLOAT, sizeof(iPoint), dstPoint);
	glColorPointer(4, GL_FLOAT, sizeof(iColor4f), c);
	glTexCoordPointer(2, GL_FLOAT, sizeof(iPoint), st);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D, 0);

}
void drawImage(Texture* tex, float x, float y, int anc)
{
	//Image* img = (Image*)_img;

	drawImage(tex, x, y, anc,
		0, 0, tex->width, tex->height,
		1.0f, 1.0f,
		2, 0);
}

static char* stringName = NULL;
static float stringSize = 20.f;
static float stringLineHeight = 30.0f;
static float stringR = 1.0f, stringG = 1.0f, stringB = 1.0f, stringA = 1.0f;
static float stringBorder = 0.0f;
static float stringBorderR = 1.0f, stringBorderG = 1.0f, stringBorderB = 1.0f, stringBorderA = 1.0f;

const char* getStringName()
{
	return stringName;
}
void setStringName(const char* str)
{
	if (stringName)
	{
		if (strcmp(stringName, str) == 0)
			return;

		delete stringName;
	}

	stringName = new char[strlen(str) + 1];
	strcpy(stringName, str);
}

float getStringSize()
{
	return stringSize;
}
void setStringSize(float size)
{
	stringSize = size;
}

void setStringLineHeight(float height)
{
	stringLineHeight = height;
}

float getStringLineHeight()
{
	return stringLineHeight;
}

void getStringRGBA(float& r, float& g, float& b, float& a)
{
	r = stringR;
	g = stringG;
	b = stringB;
	a = stringA;
}

void setStringRGBA(float r, float g, float b, float a)
{
	stringR = r;
	stringG = g;
	stringB = b;
	stringA = a;
}

float getStringBorder()
{
	return stringBorder;
}
void setStringBorder(float border)
{
	stringBorder = border;
}

void getStringBorderRGBA(float& r, float& g, float& b, float& a)
{
	r = stringBorderR;
	g = stringBorderG;
	b = stringBorderB;
	a = stringBorderA;
}

void setStringBorderRGBA(float r, float g, float b, float a)
{
	stringBorderR = r;
	stringBorderG = g;
	stringBorderB = b;
	stringBorderA = a;
}

iFont* loadFont(const char* strOTFTTF, float height, const char* strUse)
{
	iFont* f = new iFont;
	f->height = height;
	f->interval = 1;
	f->texs = new Texture * [256];
	memset(f->texs, 0x00, sizeof(Texture*) * 256);

	setStringName(strOTFTTF);
	setStringSize(height);
	setStringRGBA(1, 1, 1, 1);
	setStringBorder(0);
	iGraphics* g = iGraphics::share();

	//int i, len = strlen(strUse);
	//for (i = 0; i < len; i++)
	for (int i = 0; strUse[i]; i++)
	{
		char c = strUse[i];
		if (f->texs[c]) continue;

		if (c == ' ')
		{
			g->init(height * 0.6, height);

			f->texs[c] = g->getTexture();
			continue;
		}
		else if (c == ',')
		{
			g->init(height * 0.6, height);

			iRect rt = iGraphics::rectOfString(",");
			g->drawString(0, height - rt.size.height,
				TOP | LEFT, ",");

			f->texs[c] = g->getTexture();
			continue;
		}

		char str[2] = { c, 0 };
		iRect rt = iGraphics::rectOfString(str);
		g->init(rt.size);

		g->drawString(0, 0, TOP | LEFT, str);

		f->texs[c] = g->getTexture();
	}

	return f;
}

void freeFont(iFont* font)
{
	for (int i = 0; i < 256; i++)
	{
		if (font->texs[i])
			freeImage(font->texs[i]);
	}
	delete font->texs;
	delete font;
}

iFont* fontCurr;
void setFont(iFont* font)
{
	fontCurr = font;
}

void drawString(float x, float y, int anc, const char* szFormat, ...)
{
	char szText[128];
	va_start_end(szText, szFormat);
	drawString(x, y, 1.0f, 1.0f, anc, szText);
}

void drawString(float x, float y, float sx, float sy, int anc, const char* szFormat, ...)
{
	char szText[128];
	va_start_end(szText, szFormat);

	float h = fontCurr->height;
	float w = 0;
	int i, len = strlen(szText) - 1;
	for (i = 0; i < len; i++)
		w += (fontCurr->texs[szText[i]]->width + fontCurr->interval);
	w += fontCurr->texs[szText[i]]->width;

	switch (anc) {
	case TOP | LEFT:								break;
	case TOP | RIGHT:		x -= w;					break;
	case TOP | HCENTER:		x -= w / 2;				break;

	case BOTTOM | LEFT:					y -= h;		break;
	case BOTTOM | RIGHT:	x -= w;		y -= h;		break;
	case BOTTOM | HCENTER:	x -= w / 2;	y -= h;		break;

	case VCENTER | LEFT:				y -= h / 2;	break;
	case VCENTER | RIGHT:	x -= w;		y -= h / 2;	break;
	case VCENTER | HCENTER:	x -= w / 2;	y -= h / 2;	break;
	}

	Texture* tex;
	len++;
#if 0
	w = 0;
	for (i = 0; i < len; i++)
	{
		tex = fontCurr->texs[szText[i]];
		drawImage(tex, x + w * sx, y, TOP | LEFT);
		w += (tex->width + fontCurr->interval);
	}
#else 
	for (i = 0; i < len; i++)
	{
		tex = fontCurr->texs[szText[i]];
		drawImage(tex, x, y, TOP | LEFT);
		x += (tex->width + fontCurr->interval);
	}
#endif
}


iRect rectOfString(const char* szFormat, ...)
{
	return iRectMake(0, 0, 0, 0);
}

char* loadFile(const char* fileName, int& length)
{
	FILE* pf = fopen(fileName, "rb");
	if (!pf)
		return NULL;

	fseek(pf, 0, SEEK_END);
	length = ftell(pf);
	char* buf = new char[length + 1];
	fseek(pf, 0, SEEK_SET);
	fread(buf, sizeof(char), length, pf);
	buf[length] = 0;

	fclose(pf);

	return buf;
}

void saveFile(const char* fileName, char* buf, int length)
{
	FILE* pf = fopen(fileName, "wb");
	fwrite(buf, sizeof(char), length, pf);
	fclose(pf);
}
