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

	void write(int row, int col, const glm::vec3& color)
	{
		assert(_channels >= 3 && "The color channels less than three!");
		assert((row >= 0 && row < _height && col >= 0 && col < _width) && "Illegal position for writing!");
		this->_data[row][col][R] = static_cast<int>(255.99 * color.r);
		this->_data[row][col][G] = static_cast<int>(255.99 * color.g);
		this->_data[row][col][B] = static_cast<int>(255.99 * color.b);
	}

	void defaultPicture()
	{
		assert(_channels >= 3 && "The color channels less than three!");
		if (_format == ppm)
		{
			for (int rcol = _width - 1; rcol >= 0; --rcol)
			{
				for (int row = 0; row < _height; ++row)
				{
					glm::vec3 color{ static_cast<float>(row) / static_cast<float>(_height) ,
												  static_cast<float>(rcol) / static_cast<float>(_width) ,
												  0.2f };
					_data[row][_width - (1 + rcol)][R] = static_cast<int>(255.99 * color[R]);
					_data[row][_width - (1 + rcol)][G] = static_cast<int>(255.99 * color[G]);
					_data[row][_width - (1 + rcol)][B] = static_cast<int>(255.99 * color[B]);
				}
			}
		}
	}

	void save()
	{
		std::string filePath =  _name;
		if (_format == ppm) filePath += ".ppm";

		std::ofstream fout(filePath);
		if (!fout.is_open()) throw std::runtime_error("Failed to save the file!\n");

		fout << "P3\n" << _width << ' ' << _height << '\n' << 255 << '\n'; //P3 means colors are in ASCII
		for (int row = 0; row < _height; ++row)
		{
			for (int col = 0; col < _width; ++col)
			{
				for (int channel = 0; channel < _channels; ++channel)
				{
					fout << _data[row][col][channel] << (channel == _channels - 1 ? '\n' : ' ');
				}
			}
		}
		fout.close();
	}

	void save(std::string absPath)
	{
		std::string filePath = absPath + '\\' + _name;
		if (_format == ppm) filePath += ".ppm";

		std::ofstream fout(filePath);
		if (!fout.is_open()) throw std::runtime_error("Failed to save the file!\n");

		fout << "P3\n" << _width << ' ' << _height << '\n' << 255 << '\n'; //P3 means colors are in ASCII
		for (int row = 0; row < _height; ++row)
		{
			for (int col = 0; col < _width; ++col)
			{
				for (int channel = 0; channel < _channels; ++channel)
				{
					fout << _data[row][col][channel] << (channel == _channels-1? '\n' : ' ');
				}
			}
		}
		fout.close();
	}

private:
	std::string _name;
	int _width, _height;
	int _channels;
	PictureFormats _format = ppm;
	PictureLayout _data;
};