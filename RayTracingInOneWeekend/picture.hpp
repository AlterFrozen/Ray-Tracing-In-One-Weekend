#pragma once
//STD
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <cassert>
//3rd Party
#include "glm/glm.hpp"

typedef enum { ppm } PictureFormats;
typedef enum { R, G, B, A } ColorChannels;

class Picture
{
public:
#ifdef PIC_PRECISION_FLOAT
	using precision = float;
#else 
	using precision = int;
#endif
	using PictureLayout = std::vector< std::vector< std::vector<precision>>>; // height / width / channels

	Picture() = delete;
	Picture(int width, int height, int channels, PictureFormats format, std::string name)
		: _width{ width }, _height{ height }, _channels{ channels }, _format{ format }, _name{ name },
		 _data(  height, std::vector<std::vector<precision>>(  width, std::vector<precision>(channels)  )  )
	{};

public: // Interfaces
	int height() const { return _height; }
	int width() const { return _width; }

	void write(int row, int col, const glm::vec3& color);

	void defaultPicture();

	void save();
	void save(std::string absPath);

private:
	std::string _name;
	int _width, _height;
	int _channels;
	PictureFormats _format = ppm;
	PictureLayout _data;
};