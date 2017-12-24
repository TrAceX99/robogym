//JSON.h
/*
	JSON library for Arduino by TrAceX
	Used to work with JSON stringified data in C/C++
    Abandoned
*/
#ifndef JSON_h
#define JSON_h
#include <Arduino.h>

enum Type {integer, string, arrayInt, arrayString};

class JSON {
	int _vars;
	String _json;
	String *_name;
	String *_value;
	Type *_type;
	
	int find (String name);
	String stringify (int index);
	
	
	public:
		JSON (int vars);
		~JSON ();
		void add (String name, int value);
        void add (String name, const char *value);
        void add (String name, int *value, int size);
        void add (String name, String *value, int size);
		void set (String name, int value);
        String toString ();


};

#endif