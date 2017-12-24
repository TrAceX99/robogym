/*
	JSON.cpp - JSON library for Arduino
*/

#include <Arduino.h>
#include "JSON.h"

JSON::JSON (int size) {
	_vars = 0;
	_name = new String[size];
	_value = new String[size];
	_type = new Type[size];
	_json = "{";
}

JSON::~JSON () {
	delete[] _name;
	delete[] _value;
	delete[] _type;
}

// Nalazi indeks varijable imena name
int JSON::find (String name) {
	for (int i = 0; i < _vars; i++) {
		if (_name[i] == name) return i;
	}
}

// Pretvara u JSON string
String JSON::stringify (int index) {
	switch (_type[index]) {
	case string:
		return "\"" + _name[index] + "\":\"" + _value[index] + "\"";
	default:
		return "\"" + _name[index] + "\":" + _value[index];
	}
}

// Dodaje novu varijablu u JSON niz
void JSON::add (String name, int value) {
    _name[_vars] = name;
    _value[_vars] = String(value);
    _vars++;
}
void JSON::add (String name, const char *value) {
    _name[_vars] = name;
    _value[_vars] = value;
    _type[_vars] = string;
    _vars++;
}
void JSON::add (String name, int *value, int size) {
	_name[_vars] = name;
	_value[_vars] = "[";
    for (int i = 0; i < size; i++) {
        _value[_vars] += String(value[i]);
        if (i < size - 1) _value[_vars] += ",";
    }
    _value[_vars] += "]";
    _type[_vars] = arrayString;
	_vars++;
}
void JSON::add (String name, String *value, int size) {
	_name[_vars] = name;
	_value[_vars] = "[";
    for (int i = 0; i < size; i++) {
        _value[_vars] += String(value[i]);
        if (i < size - 1) _value[_vars] += ",";
    }
    _value[_vars] += "]";
    _type[_vars] = arrayString;
	_vars++;
}

// Menja vrednost varijable
void JSON::set (String name, int value) {
	_value[JSON::find(name)] = String(value);
}

// Za slanje
String JSON::toString () {
	for (int i = 0; i < _vars; i++) {
		_json += JSON::stringify(i);
		if (i < _vars - 1) _json += ",";
	}
	_json += "}";
	return _json;
}