#ifndef GEN_FRAMEWORK_H
#define GEN_FRAMEWORK_H
#include <vector>
#include <string>

using namespace std;

namespace gen {
	
	enum Type {
		INT,
		REAL,
		TEXT,
		DATE,
		TIME,
		FILE,
		LONG,
		BOOL
	
	};

	class Field {
		private:
			Type type;
			string identifier;
			bool pk;
			bool unique;
		public:
			Field(Type type, string identifier, bool pk, bool unique);
			Type getType() const;
			string getIdentifier() const;
			bool isPk() const;
			bool isUnique() const;
			
	};
	
	class Fields {
		private:
			vector<Field*> fields;
		public:
			vector<Field*> &getFields();
			Field *operator[] (int i);
	};

	class Relationship;
	
	class Entity {
		private:
			string name;
			Fields *fields;
			vector<Relationship *> relationship;
		public:
			Entity(string name, Fields *fields);
			Fields *getFields();
			string getName();
			vector<Relationship*> &getRelationship();
	};
	
	class Relationship {
		public:
			enum Arity {
				ONE_TO_ONE,
				ONE_TO_MANY,
				MANY_TO_ONE,
				MANY_TO_MANY
			};
		private:
			string name;
			Entity *from;
			Entity *to;
			Arity arity;
			bool bilateral;
			bool weak;
			string opositeName;
		public:
			Relationship(string name, Entity *from,	Entity *to,	Arity arity, bool bilateral=false, string opositeName="", bool weak=false);
			string getName() const;
			Entity *getFrom() const;
			Entity *getTo() const;
			Arity getArity() const;
			bool isBilateral() const;
			string getOpositeName() const;
			bool isWeak() const;
			void setWeak(bool weak);
	};
	
	Relationship::Arity flipArity(Relationship::Arity arity);
	
	extern vector<Entity*> entities;
	extern vector<Relationship*> relationship;
	extern string projectName;

	
};

#endif
