#include "LineData.hpp"

LineData::LineData(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color)
{
    this->start = start;
    this->end = end;
    this->color = color;
}

LineData::~LineData()
{
}

LineData::LineData(const LineData &instance)
{
    *this = instance;
}

LineData &LineData::operator=(const LineData &instance)
{
    if (this != &instance)
    {
        start = instance.getStartPoint();
        end = instance.getEndPoint();
        color = instance.getColor();
    }

    return (*this);
}

void LineData::setStartPoint(const glm::vec3 &start)
{
    this->start = start;
}

void LineData::setEndPoint(const glm::vec3 &end)
{
    this->end = end;
}

void LineData::setColor(const glm::vec3 &color)
{
    this->color = color;
}

glm::vec3 LineData::getStartPoint() const
{
    return (start);
}

glm::vec3 LineData::getEndPoint() const
{
    return (end);
}

glm::vec3 LineData::getColor() const
{
    return (color);
}