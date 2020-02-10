//
// Created by tim on 2/2/20.
//

#include "SpeedValue.h"

#define SpeedValue(value) SpeedValue(value, _speedUnit)

bool SpeedValue::operator==(SpeedValue &other) {
	return toNativeUnits() == other.toNativeUnits();
}

bool SpeedValue::operator==(double other) {
	return toNativeUnits() == other;
}

bool SpeedValue::operator!=(SpeedValue &other) {
    return toNativeUnits() != other.toNativeUnits();
}

bool SpeedValue::operator!=(double other) {
    return toNativeUnits() != other;
}

bool SpeedValue::operator<(SpeedValue &other) {
    return toNativeUnits() < other.toNativeUnits();
}

bool SpeedValue::operator<(double other) {
    return toNativeUnits() < other;
}

bool SpeedValue::operator<=(SpeedValue &other) {
    return toNativeUnits() <= other.toNativeUnits();
}

bool SpeedValue::operator<=(double other) {
    return toNativeUnits() <= other;
}

bool SpeedValue::operator>(SpeedValue &other) {
	return toNativeUnits() > other.toNativeUnits();
}

bool SpeedValue::operator>(double other) {
    return toNativeUnits() > other;
}

bool SpeedValue::operator>=(SpeedValue &other) {
    return toNativeUnits() >= other.toNativeUnits();
}

bool SpeedValue::operator>=(double other) {
    return toNativeUnits() >= other;
}

SpeedValue SpeedValue::operator*(const SpeedValue &other) {
	return SpeedValue(value * other.value);
}

SpeedValue SpeedValue::operator*(double other) {
	return SpeedValue(value * other);
}

SpeedValue SpeedValue::operator/(const SpeedValue &other) {
	return SpeedValue(value / other.value);
}

SpeedValue SpeedValue::operator/(double other) {
	return SpeedValue(value / other);
}

SpeedValue SpeedValue::operator+(const SpeedValue &other) {
	return SpeedValue(value + other.value);
}

SpeedValue SpeedValue::operator+(double other) {
	return SpeedValue(value + other);
}

SpeedValue SpeedValue::operator-(const SpeedValue &other) {
	return SpeedValue(value - other.value);
}

SpeedValue SpeedValue::operator-(double other) {
	return SpeedValue(value - other);
}

SpeedValue SpeedValue::operator*=(const SpeedValue &other) {
	this->value *= other.value;
	return *this;
}

SpeedValue SpeedValue::operator*=(double other) {
	this->value *= other;
	return *this;
}

SpeedValue SpeedValue::operator/=(const SpeedValue &other) {
	this->value /= other.value;
	return *this;
}

SpeedValue SpeedValue::operator/=(double other) {
	this->value /= other;
	return *this;
}

SpeedValue SpeedValue::operator+=(const SpeedValue &other) {
	this->value += other.value;
	return *this;
}

SpeedValue SpeedValue::operator+=(double other) {
	this->value += other;
	return *this;
}

SpeedValue SpeedValue::operator-=(const SpeedValue &other) {
	this->value -= other.value;
	return *this;
}

SpeedValue SpeedValue::operator-=(double other) {
	this->value -= other;
	return *this;
}

#undef SpeedValue
