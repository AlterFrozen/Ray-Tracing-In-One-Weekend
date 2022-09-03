#include "picture.hpp"

void Picture::write(int row, int col, const glm::vec3& color)
{
	assert(_channels >= 3 && "The color channels less than three!");
	assert((row >= 0 && row < _height&& col >= 0 && col < _width) && "Illegal position for writing!");
	this->_data[row][col][R] = static_cast<int>(255.99 * color.r);
	this->_data[row][col][G] = static_cast<int>(255.99 * color.g);
	this->_data[row][col][B] = static_cast<int>(255.99 * color.b);
}

void Picture::defaultPicture()
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

void Picture::save()
{
	std::string filePath = _name;
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

void Picture::save(std::string absPath)
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
				fout << _data[row][col][channel] << (channel == _channels - 1 ? '\n' : ' ');
			}
		}
	}
	fout.close();
}