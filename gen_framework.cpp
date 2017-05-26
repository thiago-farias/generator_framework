#include <stdio.h> 
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "gen_framework.h"

 namespace gen {
 
Field::Field(Type type, string identifier, bool pk, bool unique) : type(type), identifier(identifier), pk(pk), unique(unique) {}

Type Field::getType() const {
	return this->type;
}

string Field::getIdentifier() const {
	return this->identifier;
}

bool Field::isPk() const {
	return this->pk;
}

bool Field::isUnique() const {
	return this->unique;
}


vector<Field*> &Fields::getFields() {
	return this->fields;
}

Field *Fields::operator[] (int i) {
	return this->fields[i];
}

Entity::Entity(string name, Fields *fields) : name(name), fields(fields){}

Fields *Entity::getFields(){
	return this->fields;
}

string Entity::getName() {
	return this->name;
}

vector<Relationship *> &Entity::getRelationship() {
	return this->relationship;
}

Relationship::Relationship(string name, Entity *from, Entity *to, Arity arity, bool bilateral, string opositeName, bool weak) 
		: name(name), from(from), to(to), arity(arity), bilateral(bilateral), opositeName(opositeName), weak(weak) {}

string Relationship::getName() const {
	return this->name;
}

Entity *Relationship::getFrom() const {
	return this->from;
}

Entity *Relationship::getTo() const{
	return this->to;
}

Relationship::Arity Relationship::getArity() const {
	return this->arity;
}

bool Relationship::isBilateral() const {
	return this->bilateral;
}

bool Relationship::isWeak() const {
	return this->weak;
}

void Relationship::setWeak(bool weak) {
	this->weak = weak;
}

string Relationship::getOpositeName() const {
	return this->opositeName;
}


vector<Entity*> entities;
vector<Relationship*> relationship;
string projectName;

Relationship::Arity flipArity(Relationship::Arity arity) {
	switch(arity) {
		case Relationship::ONE_TO_ONE:
		case Relationship::MANY_TO_MANY:
			return arity;
		case Relationship::MANY_TO_ONE:
			return Relationship::ONE_TO_MANY;
		case Relationship::ONE_TO_MANY:
			return Relationship::MANY_TO_ONE;
		default:
			return Relationship::ONE_TO_MANY;
	}
}


};

extern int yyparse();

using namespace std;

string getJavaType(gen::Type type) {
	switch(type) {
		case gen::INT:
			return "int";
		case gen::REAL:
			return "double";
		case gen::LONG:
			return "Long";
		case gen::TEXT:
			return "String";
		case gen::DATE:
			return "java.util.Date";
		case gen::TIME:
			return "java.util.Date";
		case gen::FILE:
			return "byte[]";
		case gen::BOOL:
			return "boolean";
	}
	return "int";
}

bool isNumeric(gen::Type type) {
	return (type == gen::INT || type == gen::LONG || type == gen::REAL);
}

bool isJavaTypeObject(gen::Type type) {
	switch(type) {
		case gen::INT:
			return false;
		case gen::REAL:
			return false;
		case gen::LONG:
			return true;
		case gen::TEXT:
			return true;
		case gen::DATE:
			return true;
		case gen::TIME:
			return true;
		case gen::FILE:
			return true;
		case gen::BOOL:
			return false;
	}
	return false;
}

string toLower(const string &str) {
	string res = str;
	for(int i=0;i<res.length();i++) {
		res[i] = tolower(res[i]);
	}
	
	return res;
}

string camelCase(const string &str) {
	string res = str;
	res[0] = toupper(res[0]);
	
	return res;
}

void generateJavaEntities(){
	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		string path = toLower(gen::projectName) + "\\" + toLower(entity->getName()) + "\\";
		ofstream out((path + entity->getName() + ".java").c_str());
		
		out << "package " << toLower(gen::projectName) << "." << toLower(entity->getName()) << ";" << endl << endl;
		out << "import java.io.Serializable;" << endl;
		out << "import javax.persistence.*;" << endl;
		out << "import java.util.List;" << endl;
		vector<gen::Relationship*> &rels = entity->getRelationship();
		for(int j=0;j<rels.size();j++) {
			gen::Relationship *r = rels[j];
			out << "import " << toLower(gen::projectName) << "." << toLower(r->getTo()->getName()) << "." << r->getTo()->getName() << ";" << endl;
		}
		out << endl;
		
		out << "@Entity" << endl;
		out << "public class " << entity->getName() << " implements Serializable {" << endl;
		
		out << "\tprivate static final long serialVersionUID = 1L;" << endl;
		for(int j=0;j<entity->getFields()->getFields().size(); j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			out << "\tprivate " << getJavaType(field->getType()) << " " << field->getIdentifier() << ";" << endl;
		}
		for(int j=0;j<gen::relationship.size();j++) {
			gen::Relationship *rel = gen::relationship[j];
			if(rel->getFrom()->getName() == entity->getName()) {
				out << "\tprivate ";
				if(rel->getArity() == gen::Relationship::ONE_TO_MANY || rel->getArity() == gen::Relationship::MANY_TO_MANY) {
					out << "List<" << rel->getTo()->getName() << "> " << rel->getName() << ";" << endl;
				} else {
					out << rel->getTo()->getName() << " " << rel->getName() << ";" << endl;
				}
			} else if(rel->getTo()->getName() == entity->getName() && rel->isBilateral()) {
				out << "\tprivate ";
				if(rel->getArity() == gen::Relationship::ONE_TO_MANY || rel->getArity() == gen::Relationship::ONE_TO_ONE) {
					out << rel->getFrom()->getName() << " " << rel->getOpositeName() << ";" << endl;
				} else {
					out << "List<" << rel->getFrom()->getName() << "> " << rel->getOpositeName() << ";" << endl;
				}
			}
		}
		out << endl;
		
		out << "\tpublic " << entity->getName() << "() {" << endl; 
		
		for(int j=0;j<gen::relationship.size();j++) {
			gen::Relationship *rel = gen::relationship[j];
			if(rel->getFrom()->getName() == entity->getName()) {
				
				if(rel->getArity() == gen::Relationship::ONE_TO_MANY || rel->getArity() == gen::Relationship::MANY_TO_MANY) {
					out << "\t\tthis." << rel->getName() << " = new java.util.ArrayList<" << rel->getTo()->getName() << ">();" << endl;
				}
			} else if(rel->getTo()->getName() == entity->getName() && rel->isBilateral()) {
				
				if(rel->getArity() == gen::Relationship::MANY_TO_ONE || rel->getArity() == gen::Relationship::MANY_TO_MANY) {
					out << "\t\tthis." << rel->getOpositeName() << " = new java.util.ArrayList<" << rel->getFrom()->getName() << ">();" << endl;
				}
			}
		}
		
		
		out << "\t}" << endl << endl;
		
		out << "\tpublic " << entity->getName() << "(";
		out << getJavaType(entity->getFields()->getFields()[0]->getType()) << " " << entity->getFields()->getFields()[0]->getIdentifier();
		for(int j=1;j<entity->getFields()->getFields().size(); j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			out << ", " << getJavaType(field->getType()) << " " << field->getIdentifier(); 
		}
		
		for(int j=0;j<gen::relationship.size();j++) {
			gen::Relationship *rel = gen::relationship[j];
			if(rel->getFrom()->getName() == entity->getName()) {
				
				if(rel->getArity() == gen::Relationship::ONE_TO_ONE || rel->getArity() == gen::Relationship::MANY_TO_ONE) {
					out << ", " << rel->getTo()->getName() << " " << rel->getName();
				}
			} else if(rel->getTo()->getName() == entity->getName() && rel->isBilateral()) {
				
				if(rel->getArity() == gen::Relationship::ONE_TO_ONE || rel->getArity() == gen::Relationship::ONE_TO_MANY) {
					out << ", " << rel->getFrom()->getName() << " " << rel->getOpositeName();
				}
			}
		}
		out << ") {" << endl;
		for(int j=0;j<entity->getFields()->getFields().size(); j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			out << "\t\tthis." << field->getIdentifier() << " = " << field->getIdentifier() << ";" << endl; 
		}
		for(int j=0;j<gen::relationship.size();j++) {
			gen::Relationship *rel = gen::relationship[j];
			if(rel->getFrom()->getName() == entity->getName()) {
				
				if(rel->getArity() == gen::Relationship::ONE_TO_MANY || rel->getArity() == gen::Relationship::MANY_TO_MANY) {
					out << "\t\tthis." << rel->getName() << " = new java.util.ArrayList<" << rel->getTo()->getName() << ">();" << endl;
				} else {
					out << "\t\tthis." << rel->getName() << " = " << rel->getName() << ";" << endl;
				}
			} else if(rel->getTo()->getName() == entity->getName() && rel->isBilateral()) {
				
				if(rel->getArity() == gen::Relationship::MANY_TO_ONE || rel->getArity() == gen::Relationship::MANY_TO_MANY) {
					out << "\t\tthis." << rel->getOpositeName() << " = new java.util.ArrayList<" << rel->getFrom()->getName() << ">();" << endl;
				} else {
					out << "\t\tthis." << rel->getOpositeName() << " = " << rel->getOpositeName() << ";" << endl;
				}
			}
		}
		out << "\t}" << endl << endl;
		
		
		for(int j=0;j<entity->getFields()->getFields().size(); j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			out << "\tpublic void set" << camelCase(field->getIdentifier()) << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") {" << endl;
			out << "\t\tthis." << field->getIdentifier() << " = " << field->getIdentifier() << ";" << endl;
			out << "\t}" << endl << endl;
			
			if(field->isPk()) {
				out << "\t@Id" << endl;
				if(isNumeric(field->getType())) {
					out << "\t@GeneratedValue(strategy = GenerationType.AUTO)" << endl;
				}
			}
			if(field->getType() == gen::DATE) {
				out << "\t@Temporal(TemporalType.DATE)" << endl;
			} else if(field->getType() == gen::TIME) {
				out << "\t@Temporal(TemporalType.TIME)" << endl;
			}
			out << "\tpublic " << getJavaType(field->getType()) << " get" << camelCase(field->getIdentifier()) << "() {" << endl;
			out << "\t\treturn this." << field->getIdentifier() << ";" << endl;
			out << "\t}" << endl << endl;
		}
		
		for(int j=0;j<gen::relationship.size();j++) {
			gen::Relationship *rel = gen::relationship[j];
			if(rel->getFrom()->getName() == entity->getName()) {
				if(rel->getArity() == gen::Relationship::ONE_TO_MANY || rel->getArity() == gen::Relationship::MANY_TO_MANY) {
					out << "\tpublic void set" << camelCase(rel->getName()) << "(List<" << rel->getTo()->getName() << "> " << rel->getName() << ") {" << endl;
					out << "\t\tthis." << rel->getName() << " = " << rel->getName() << ";" << endl;
					out << "\t}" << endl << endl;
					
					if(rel->getArity() == gen::Relationship::ONE_TO_MANY) {
						out << "\t@OneToMany" << endl;
					} else {
						out << "\t@ManyToMany" << endl;
					}
					out << "\tpublic List<" << rel->getTo()->getName() << "> get" << camelCase(rel->getName()) << "() {" << endl;
					out << "\t\treturn this." << rel->getName() << ";" << endl;
					out << "\t}" << endl << endl;
					
				} else {
					out << "\tpublic void set" << camelCase(rel->getName()) << "(" << rel->getTo()->getName() << " " << rel->getTo()->getName() << ") {" << endl;
					out << "\t\tthis." << rel->getName() << " = " << rel->getName() << ";" << endl;
					out << "\t}" << endl << endl;
					
					if(rel->getArity() == gen::Relationship::ONE_TO_ONE) {
						out << "\t@OneToOne" << endl;
					} else {
						out << "\t@ManyToOne" << endl;
					}
					out << "\tpublic " << rel->getTo()->getName() << " get" << camelCase(rel->getName()) << "() {" << endl;
					out << "\t\treturn this." << rel->getName() << ";" << endl;
					out << "\t}" << endl << endl;
				}
			} else if(rel->getTo()->getName() == entity->getName() && rel->isBilateral()) {
				if(rel->getArity() == gen::Relationship::MANY_TO_ONE || rel->getArity() == gen::Relationship::MANY_TO_MANY) {
					out << "\tpublic void set" << camelCase(rel->getOpositeName()) << "(List<" << rel->getFrom()->getName() << "> " << rel->getOpositeName() << ") {" << endl;
					out << "\t\tthis." << rel->getOpositeName() << " = " << rel->getOpositeName() << ";" << endl;
					out << "\t}" << endl << endl;
					
					if(rel->getArity() == gen::Relationship::MANY_TO_ONE) {
						out << "\t@OneToMany" << endl;
					} else {
						out << "\t@ManyToMany" << endl;
					}
					out << "\tpublic List<" << rel->getFrom()->getName() << "> get" << camelCase(rel->getOpositeName()) << "() {" << endl;
					out << "\t\treturn this." << rel->getOpositeName() << ";" << endl;
					out << "\t}" << endl << endl;
					
				} else {
					out << "\tpublic void set" << camelCase(rel->getOpositeName()) << "(" << rel->getFrom()->getName() << " " << rel->getFrom()->getName() << ") {" << endl;
					out << "\t\tthis." << rel->getOpositeName() << " = " << rel->getOpositeName() << ";" << endl;
					out << "\t}" << endl << endl;
					
					if(rel->getArity() == gen::Relationship::ONE_TO_ONE) {
						out << "\t@OneToOne" << endl;
					} else {
						out << "\t@ManyToOne" << endl;
					}
					out << "\tpublic " << rel->getFrom()->getName() << " get" << camelCase(rel->getOpositeName()) << "() {" << endl;
					out << "\t\treturn this." << rel->getOpositeName() << ";" << endl;
					out << "\t}" << endl << endl;
				}
			}
		}
		
		out << "\tpublic boolean equals(Object o) {" << endl
			<< "\t\tif(o == null)" << endl
			<< "\t\t\treturn false;" << endl
			<< "\t\tif(o.getClass() != " << entity->getName() << ".class)" << endl
			<< "\t\t\treturn false;" << endl
			<< "\t\t" << entity->getName() << " " << toLower(entity->getName()) << " = (" << entity->getName() << ") o;" << endl
			<< "\t\treturn true";
		for(int j=0;j<entity->getFields()->getFields().size(); j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			if(field->isUnique() && isJavaTypeObject(field->getType()))
				out << " && (this." << field->getIdentifier() << ".equals(" << toLower(entity->getName()) << "." << field->getIdentifier() << "))";
			else if(field->isUnique()) {
				out << " && (this." << field->getIdentifier() << " == " << toLower(entity->getName()) << "." << field->getIdentifier() << ")";
			}
		}
		out << ";" << endl;
		out << "\t}" << endl;
		
		
		out << "}" << endl;
		out.close();
	}
}

void generateJavaPersistenceInterfaces() {
	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		string path = toLower(gen::projectName) + "\\" + toLower(entity->getName()) + "\\";
		ofstream out((path + string("Repositorio") + entity->getName() + ".java").c_str());
		
		out << "package " << toLower(gen::projectName) << "." << toLower(entity->getName()) << ";" << endl << endl;
		out << "import java.io.Serializable;" << endl;
		out << "import javax.ejb.Local;" << endl;
		out << "import java.util.List;" << endl;
		out << "import " << toLower(gen::projectName) << ".ErroInternoException;" << endl;
		vector<gen::Relationship*> &rels = entity->getRelationship();
		for(int j=0;j<rels.size();j++) {
			gen::Relationship *r = rels[j];
			out << "import " << toLower(gen::projectName) << "." << toLower(r->getTo()->getName()) << "." << r->getTo()->getName() << ";" << endl;
		}
		out << endl;
		
		out << "@Local" << endl;
		out << "public interface Repositorio" << entity->getName() << " extends Serializable {" << endl;
		out << "\tpublic void adicionar(" << entity->getName() << " " << toLower(entity->getName()) << ") throws ErroInternoException;" << endl;
		out << "\tpublic void atualizar(" << entity->getName() << " " << toLower(entity->getName()) << ") throws ErroInternoException, " << entity->getName() << "InexistenteException;" << endl;
		out << "\tpublic List<" << entity->getName() << "> listarTodos() throws ErroInternoException;" << endl;
		
		for(int j=0;j<entity->getFields()->getFields().size();j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			if(field->isUnique()) {
				out << "\tpublic void remover(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException, " << entity->getName() << "InexistenteException;" << endl;
				out << "\tpublic " << entity->getName() << " buscar" << camelCase(field->getIdentifier()) << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException, " << entity->getName() << "InexistenteException;" << endl;
			} else {
				out << "\tpublic List<" << entity->getName() << "> buscar" << camelCase(field->getIdentifier()) << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException;" << endl;
			}
			
		}
		
		out << "}" << endl;
		out.close();
	}
}

void generateJavaExceptions() {
	string path = toLower(gen::projectName) + "\\";
	ofstream eie((path + "ErroInternoException.java").c_str());
	
	eie << "package " << toLower(gen::projectName) << ";" << endl << endl;
	eie << "public class ErroInternoException extends Exception {" << endl;
	eie << "\tpublic ErroInternoException(Exception causa) {" << endl;
	eie << "\t\tsuper(causa.getMessage());" << endl;
	eie << "\t}" << endl;
	eie << "}" << endl;
	eie.close();

	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		path = toLower(gen::projectName) + "\\" + toLower(entity->getName()) + "\\";
		ofstream out((path + entity->getName() + "InexistenteException.java").c_str());
		
		out << "package " << toLower(gen::projectName) << "." << toLower(entity->getName()) << ";" << endl << endl;
		
		out << "public class " << entity->getName() << "InexistenteException extends Exception {" << endl;
		out << "\tpublic " << entity->getName() << "InexistenteException() {" << endl;
		out << "\t\tsuper(\"" << entity->getName() << " não encontrado\");" << endl;
		out << "\t}" << endl;
		out << "}" << endl;
		out.close();
		
		ofstream out2((path + entity->getName() + "ExistenteException.java").c_str());
		
		out2 << "package " << toLower(gen::projectName) << "." << toLower(entity->getName()) << ";" << endl << endl;
		
		out2 << "public class " << entity->getName() << "ExistenteException extends Exception {" << endl;
		out2 << "\tpublic " << entity->getName() << "ExistenteException() {" << endl;
		out2 << "\t\tsuper(\"" << entity->getName() << " já existe\");" << endl;
		out2 << "\t}" << endl;
		out2 << "}" << endl;
		out2.close();
		
		ofstream out3((path + entity->getName() + "RelacionadoException.java").c_str());
		
		out3 << "package " << toLower(gen::projectName) << "." << toLower(entity->getName()) << ";" << endl << endl;
		
		out3 << "public class " << entity->getName() << "RelacionadoException extends Exception {" << endl;
		out3 << "\tpublic " << entity->getName() << "RelacionadoException() {" << endl;
		out3 << "\t\tsuper(\"" << entity->getName() << " está associado a um relacionamento\");" << endl;
		out3 << "\t}" << endl;
		out3 << "}" << endl;
		out3.close();
		
	}
}

class EntityJavaFormat {
	private:
		gen::Entity *entity;
		gen::Field *pk;
	public:
		EntityJavaFormat(gen::Entity *entity) : entity(entity) {
			for(int i=0;i<entity->getFields()->getFields().size();i++) {
				gen::Field *field = entity->getFields()->getFields()[i];
				if(field->isPk()) {
					pk = field;
					break;
				}
			}
		}
		
		inline string typeName() const {
			return entity->getName();
		}
		
		inline string varName() const {
			return toLower(entity->getName());
		}
		
		inline string getPk(string var = "") const {
			return get(pk, var);
		}
		
		inline string get(gen::Field *field, string var="") const {
			return ((var == "")?varName():var) + ".get" + camelCase(field->getIdentifier()) + "()";
		}
		inline string set(gen::Field *field, string value, string var="") const {
			return ((var == "")?varName():var) + ".set" + camelCase(field->getIdentifier()) + "(" + value + ")";
		}
		
		inline string declareVar(string name="") const {
			return typeName() + " " + ((name == "")?varName():name);
		}
		
		inline string declareVar(gen::Field *field, string name="") const {
			return getJavaType(field->getType()) + " " + ((name == "")?field->getIdentifier():name);
		}
		
		inline string pkName() const {
			return this->pk->getIdentifier();
		}
		
		inline string pkType() const {
			return getJavaType(this->pk->getType());
		}
		
};

void generateJavaJpaPersistenceImplementation() {
	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		EntityJavaFormat format(entity);
		string path = toLower(gen::projectName) + "\\" + toLower(entity->getName()) + "\\";
		ofstream out((path + string("Repositorio") + format.typeName() + "Jpa.java").c_str());
		
		out << "package " << toLower(gen::projectName) << "." << format.varName() << ";" << endl << endl;
		out << "import javax.persistence.*;" << endl;
		out << "import javax.ejb.Stateless;" << endl;
		out << "import java.util.List;" << endl;
		out << "import " << toLower(gen::projectName) << ".ErroInternoException;" << endl;
		
		vector<gen::Relationship*> &rels = entity->getRelationship();
		for(int j=0;j<rels.size();j++) {
			gen::Relationship *r = rels[j];
			out << "import " << toLower(gen::projectName) << "." << toLower(r->getTo()->getName()) << "." << r->getTo()->getName() << ";" << endl;
		}
		out << endl;
		
		out << "@Stateless" << endl;
		out << "public class Repositorio" << format.typeName() << "Jpa implements Repositorio" << format.typeName() << " {" << endl;
		out << "\t@PersistenceContext" << endl;
		out << "\tprivate EntityManager em;" << endl << endl;
		
		//Adicionar
		out << "\tpublic void adicionar(" << format.typeName() << " " << format.varName() << ") throws ErroInternoException {" << endl
			<< "\t\ttry {" << endl
			<< "\t\t\tthis.em.persist(" << format.varName() << ");" << endl
			<< "\t\t} catch (Exception e) {" << endl
			<< "\t\t\tthrow new ErroInternoException(e);" << endl
			<< "\t\t}" << endl
			<< "\t}" << endl << endl;
		
		//Atualizar
		out << "\tpublic void atualizar(" << format.typeName() << " " << format.varName() << ") throws ErroInternoException, " << format.typeName() << "InexistenteException {" << endl
			<< "\t\t" << format.typeName() << " in = buscar" << camelCase(format.pkName()) << "(" << format.getPk() << ");" << endl;
		
		for(int j=0;j<entity->getFields()->getFields().size();j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			if(!field->isUnique()) {
				out << "\t\t" << format.set(field, format.get(field), "in") << ";" << endl;
			}
		}
		
		out << "\t\ttry {" << endl
			<< "\t\t\tthis.em.merge(in);" << endl
			<< "\t\t}" << endl
			<< "\t\tcatch(Exception e) {" << endl
			<< "\t\t\tthrow new ErroInternoException(e);" << endl
			<< "\t\t}" << endl			
			<< "\t}" << endl << endl;
			
		//Listar Todos
		out << "\tpublic List<" << format.typeName() << "> listarTodos() throws ErroInternoException {" << endl
			<< "\t\ttry {" << endl
			<< "\t\t\tTypedQuery<" << format.typeName() << "> query = this.em.createQuery(\"select " << format.varName() << " from " << format.typeName() << "\", " << format.typeName() << ".class);" << endl
			<< "\t\t\treturn query.getResultList();" << endl
			<< "\t\t}" << endl
			<< "\t\tcatch(Exception e) {" << endl
			<< "\t\t\tthrow new ErroInternoException(e);" << endl
			<< "\t\t}" << endl
			<< "\t}" << endl << endl;
		
		for(int j=0;j<entity->getFields()->getFields().size();j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			if(field->isUnique()) {
				//Remover
				out << "\tpublic void remover(" << format.declareVar(field) << ") throws ErroInternoException, " << entity->getName() << "InexistenteException {" << endl
					<< "\t\ttry{" << endl
					<< "\t\t\t" << format.declareVar() << " = buscar" << camelCase(field->getIdentifier()) << "(" << field->getIdentifier() <<");" << endl
					<< "\t\t\tthis.em.remove(" << format.varName() << ");" << endl
					<< "\t\t}" << endl
					<< "\t\tcatch(Exception e) {" << endl
					<< "\t\t\tthrow new ErroInternoException(e);" << endl
					<< "\t\t}" << endl
					<< "\t}" << endl << endl;
				//Buscar
				out << "\tpublic " << entity->getName() << " buscar" << camelCase(field->getIdentifier()) << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException, " << entity->getName() << "InexistenteException {" << endl
					<< "\t\ttry{" << endl;
				if(field->isPk()) {
					out << "\t\t\t" << format.declareVar() << " = this.em.find(" << format.typeName() << ".class, " << field->getIdentifier() << ");" << endl
						<< "\t\t\treturn " << format.varName() << ";" << endl
						<< "\t\t}" << endl
						<< "\t\tcatch(Exception e) {" << endl
						<< "\t\t\tthrow new ErroInternoException(e);" << endl
						<< "\t\t}" << endl
						<< "\t}" << endl << endl;
				} else {
					out << "\t\t\tTypedQuery<" << format.typeName() << "> query = this.em.createQuery(\"select " << format.varName() << " from " << format.typeName() << " where " << format.varName() << "." << field->getIdentifier() << " = :" << field->getIdentifier() << "\", " << format.typeName() << ".class);" << endl
						<< "\t\t\tquery.setParameter(\"" << field->getIdentifier() << "\", " << field->getIdentifier() << ");" << endl
						<< "\t\t\treturn query.getSingleResult();" << endl
						<< "\t\t}" << endl
						<< "\t\tcatch(NoResultException ex) {" << endl
						<< "\t\t\tthrow new " << format.typeName() << "InexistenteException();" << endl
						<< "\t\t}" << endl
						<< "\t\tcatch(Exception e) {" << endl
						<< "\t\t\tthrow new ErroInternoException(e);" << endl
						<< "\t\t}" << endl
						<< "\t}" << endl << endl;
				}
			} else {
				//Buscar
				out << "\tpublic List<" << entity->getName() << "> buscar" << camelCase(field->getIdentifier()) << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException {" << endl
					<< "\t\ttry {" << endl
					<< "\t\t\tTypedQuery<" << format.typeName() << "> query = this.em.createQuery(\"select " << format.varName() << " from " << format.typeName() << " where " << format.varName() << "." << field->getIdentifier() << ((field->getType() == gen::TEXT)?" LIKE ":" = ") << ":" << field->getIdentifier() << "\", " << format.typeName() << ".class);" << endl
					<< "\t\t\tquery.setParameter(\"" << field->getIdentifier() << "\", " << ((field->getType() == gen::TEXT)?"\"%\" + ":"") << field->getIdentifier() << ((field->getType() == gen::TEXT)?" + \"%\"":"") << ");" << endl
					<< "\t\t\treturn query.getResultList();" << endl
					<< "\t\t}" << endl
					<< "\t\tcatch(Exception e) {" << endl
					<< "\t\t\tthrow new ErroInternoException(e);" << endl
					<< "\t\t}" << endl
					<< "\t}" << endl << endl;
			}
			
		}
		for(int j=0;j<gen::relationship.size();j++) {
			gen::Relationship *rel = gen::relationship[j];
			
			if(rel->getFrom()->getName() == entity->getName()) {  
				EntityJavaFormat formatRel(rel->getTo());

				if((rel->getArity() == gen::Relationship::ONE_TO_MANY || rel->getArity() == gen::Relationship::MANY_TO_MANY)) {
					out << "\tpublic List<" << format.typeName() << "> buscar" << formatRel.typeName() << "(" << formatRel.pkType() << " " << formatRel.pkName() << ") throws ErroInternoException {" << endl
						<< "\t\ttry {" << endl
						<< "\t\t\tTypedQuery<" << format.typeName() << "> query = this.em.createQuery(\"select " << format.varName() << " from " << format.typeName() << " join " << format.varName() << "." << rel->getName() << " el where el." << formatRel.pkName() << " = :" << formatRel.pkName() << "\", " << format.typeName() << ".class);" << endl
						<< "\t\t\tquery.setParameter(\"" << formatRel.pkName() << "\", " << formatRel.pkName() << ");" << endl
						<< "\t\t\treturn query.getResultList();" << endl
						<< "\t\t}" << endl
						<< "\t\tcatch(Exception e) {" << endl
						<< "\t\t\tthrow new ErroInternoException(e);" << endl
						<< "\t\t}" << endl
						<< "\t}" << endl << endl;
				}
			} else if(rel->getTo()->getName() == entity->getName() && rel->isBilateral()) {
				EntityJavaFormat formatRel(rel->getFrom());

				if((rel->getArity() == gen::Relationship::MANY_TO_ONE || rel->getArity() == gen::Relationship::MANY_TO_MANY)) {
					out << "\tpublic List<" << format.typeName() << "> buscar" << formatRel.typeName() << "(" << formatRel.pkType() << " " << formatRel.pkName() << ") throws ErroInternoException {" << endl
						<< "\t\ttry {" << endl
						<< "\t\t\tTypedQuery<" << format.typeName() << "> query = this.em.createQuery(\"select " << format.varName() << " from " << format.typeName() << " join " << format.varName() << "." << rel->getName() << " el where el." << formatRel.pkName() << " = :" << formatRel.pkName() << "\", " << format.typeName() << ".class);" << endl
						<< "\t\t\tquery.setParameter(\"" << formatRel.pkName() << "\", " << formatRel.pkName() << ");" << endl
						<< "\t\t\treturn query.getResultList();" << endl
						<< "\t\t}" << endl
						<< "\t\tcatch(Exception e) {" << endl
						<< "\t\t\tthrow new ErroInternoException(e);" << endl
						<< "\t\t}" << endl
						<< "\t}" << endl << endl;
				}
			}
			
		}
		
		
		out << "}" << endl;
		out.close();
	}
}

void generateJavaBusinessClasses() {
	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		EntityJavaFormat format(entity);
		string path = toLower(gen::projectName) + "\\" + toLower(entity->getName()) + "\\";
		ofstream out((path + string("Cadastro") + format.typeName() + ".java").c_str());
		
		out << "package " << toLower(gen::projectName) << "." << format.varName() << ";" << endl << endl;
		out << "import javax.ejb.Stateless;" << endl
			<< "import java.io.Serializable;" << endl
			<< "import javax.ejb.EJB;" << endl
			<< "import " << toLower(gen::projectName) << ".ErroInternoException;" << endl;
		out << "import java.util.List;" << endl;
		vector<gen::Relationship*> &rels = entity->getRelationship();
		for(int j=0;j<rels.size();j++) {
			gen::Relationship *r = rels[j];
			out << "import " << toLower(gen::projectName) << "." << toLower(r->getTo()->getName()) << "." << r->getTo()->getName() << ";" << endl
				<< "import " << toLower(gen::projectName) << "." << toLower(r->getTo()->getName()) << ".Repositorio" << r->getTo()->getName() << ";" << endl
				<< "import " << toLower(gen::projectName) << "." << toLower(r->getTo()->getName()) << "." << r->getTo()->getName() << "InexistenteException;" << endl;
		}
		out << endl;
		
		out << "@Stateless" << endl
			<< "public class Cadastro" << format.typeName() << " implements Serializable {" << endl
			<< "\t@EJB" << endl
			<< "\tprivate Repositorio" << format.declareVar("rep" + format.typeName()) << ";" << endl;
		
		for(int j=0;j<gen::relationship.size();j++) {
			gen::Relationship *rel = gen::relationship[j];
			
			if(rel->getFrom()->getName() == entity->getName()) {  
				out << "\t@EJB" << endl
					<< "\tprivate Repositorio" << rel->getTo()->getName() << " rep" << rel->getTo()->getName() << ";" << endl;
					
			} else if(rel->getTo()->getName() == entity->getName() && rel->isBilateral()) {
				out << "\t@EJB" << endl
					<< "\tprivate Repositorio" << rel->getFrom()->getName() << " rep" << rel->getFrom()->getName() << ";" << endl;
			}
			
		}
		out << endl << endl;
		
		//Adicionar
		out << "\tpublic void adicionar(" << entity->getName() << " " << toLower(entity->getName()) << ") throws ErroInternoException, " << format.typeName() << "ExistenteException {" << endl;			
		
		bool tested = false;
		for(int j=0;j<entity->getFields()->getFields().size();j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			if(field->isUnique() && !isNumeric(field->getType())) {
				tested = true;
			}
		}
		if(tested)
			out << "\t\ttry {" << endl;
		for(int j=0;j<entity->getFields()->getFields().size();j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			if(field->isUnique() && !isNumeric(field->getType())) {
				out << "\t\t\tbuscar" << camelCase(field->getIdentifier()) << "(" << format.get(field) << ");" << endl;
			}
		}
		
		if(tested) {		
		out	<< "\t\t\tthrow new " << format.typeName() << "ExistenteException();" << endl
			<< "\t\t}" << endl
			<< "\t\tcatch(" << format.typeName() << "InexistenteException ex) {" << endl
			<< "\t\t\tthis.rep" << format.typeName() << ".adicionar(" << format.varName() << ");" << endl
			<< "\t\t}" << endl;
			
		} else {
			out << "\t\t\tthis.rep" << format.typeName() << ".adicionar(" << format.varName() << ");" << endl;
		}
		out << "\t}" <<  endl << endl;
		//Atualizar
		out << "\tpublic void atualizar(" << entity->getName() << " " << toLower(entity->getName()) << ") throws ErroInternoException, " << entity->getName() << "InexistenteException {" << endl
			<< "\t\tthis.rep" << format.typeName() << ".atualizar(" << format.varName() << ");" << endl
			<< "\t}" << endl << endl;
			
		out << "\tpublic List<" << entity->getName() << "> listarTodos() throws ErroInternoException {" << endl
			<< "\t\treturn this.rep" << format.typeName() << ".listarTodos();" << endl
			<< "\t}" << endl;
		
		for(int j=0;j<entity->getFields()->getFields().size();j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			if(field->isUnique()) {
				bool withWeakRelationship = false;
				for(int k=0;k<gen::relationship.size();k++) {
					gen::Relationship *rel = gen::relationship[k];
					if(rel->getTo()->getName() == entity->getName() || rel->getFrom()->getName() == entity->getName()) {
						withWeakRelationship = !rel->isWeak();
						break;
					}
				}
				out << "\tpublic void remover(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException, " << entity->getName() << "InexistenteException" << (withWeakRelationship?(string(", ") + entity->getName() + "RelacionadoException"):"") << " {" << endl
					<< "\t\t" << format.declareVar("interno") << " = this.rep" << format.typeName() << ".buscar" << camelCase(field->getIdentifier()) << "(" << field->getIdentifier() << ");" << endl;
				
				for(int k=0;k<gen::relationship.size();k++) {
					gen::Relationship *rel = gen::relationship[k];
					gen::Entity *related = NULL;
					gen::Relationship::Arity arity;
					string relName;
					string relOpositeName;
					
					bool trocado = false;
					if(rel->getTo()->getName() == entity->getName()) {  
						related = rel->getFrom();
						trocado = true;
						switch(rel->getArity()) {
							case gen::Relationship::MANY_TO_ONE:
								arity = gen::Relationship::ONE_TO_MANY;
								break;
							case gen::Relationship::ONE_TO_MANY:
								arity = gen::Relationship::MANY_TO_ONE;
								break;
							default:
								arity = rel->getArity();
						}
						if(rel->isBilateral())
							relName = rel->getOpositeName();
						relOpositeName = rel->getName();
					} else if(rel->getFrom()->getName() == entity->getName()) {
						related = rel->getTo();
						arity = rel->getArity();
						relName = rel->getName();
						if(rel->isBilateral())
							relOpositeName = rel->getOpositeName();
					} else continue;
					
					EntityJavaFormat formatRel(related);
					
					if(rel->isWeak()) {
						out << "\t\ttry {" << endl;
						if(arity == gen::Relationship::MANY_TO_ONE && rel->isBilateral()) {
							out << "\t\t" << formatRel.declareVar() << " = this.rep" << formatRel.typeName() << ".buscar" << camelCase(formatRel.pkName()) << "(" << formatRel.getPk(string("interno.get") + camelCase(relName) + "()") << ");" << endl
								<< "\t\t" << formatRel.varName() << ".get" << camelCase(relOpositeName) << "().remove(interno);" << endl
								<< "\t\t" << "this.rep" << formatRel.typeName() << ".atualizar(" << formatRel.varName() << ");" << endl;
								
						} else if(arity == gen::Relationship::ONE_TO_ONE) {
							out << "\t\tthis.rep" << formatRel.typeName() << ".remover" /*<< camelCase(formatRel.pkName()) */<< "(" << formatRel.getPk(string("interno.get") + camelCase(relName)) << ");" << endl;
						} else if(arity == gen::Relationship::ONE_TO_MANY && rel->isBilateral()) {
							out << "\t\tfor(int i=0;i<interno.get" << camelCase(relName) << "().size();i++) {" << endl
								<< "\t\t\t" << formatRel.declareVar() << " = interno.get" << camelCase(relName) << "().get(i);" << endl
								<< "\t\t\tthis.rep" << formatRel.typeName() << ".remover" /*<< camelCase(formatRel.pkName()) */ << "(" << formatRel.getPk() << ");" << endl
								<< "\t\t}" << endl;
						} else if(arity == gen::Relationship::MANY_TO_MANY && rel->isBilateral()) {
							out << "\t\tfor(int i=0;i<interno.get" << camelCase(relName) << "().size();i++) {" << endl
								<< "\t\t\t" << formatRel.declareVar() << " = interno.get" << camelCase(relName) << "().get(i);" << endl
								<< "\t\t\t" << formatRel.varName() << ".get" << camelCase(relOpositeName) << "().remove(interno);" << endl
								<< "\t\t\t" << "this.rep" << formatRel.typeName() << ".atualizar(" << formatRel.varName() << ");" << endl
								<< "\t\t}" << endl;
						}
						out << "\t\t} catch(" << formatRel.typeName() << "InexistenteException e) {" << endl
							<< "\t\t}" << endl;
					} else {						
						if(rel->isBilateral()) {
							if(arity == gen::Relationship::MANY_TO_ONE) {
								out << "\t\tif(interno.get" << camelCase(relName) << "() != null) {"  << endl
									<< "\t\t\tthrow new " << format.typeName() << "RelacionadoException();" << endl
									<< "\t\t}" << endl;
							} else if(arity == gen::Relationship::ONE_TO_MANY || arity == gen::Relationship::MANY_TO_MANY) {
								out << "\t\tif(!interno.get" << camelCase(relName) << "().isEmpty()) {"  << endl
									<< "\t\t\tthrow new " << format.typeName() << "RelacionadoException();" << endl
									<< "\t\t}" << endl;
							}	
						} else if(trocado){
							out << "\t\tif(!this.rep" << formatRel.typeName() << ".buscar" << format.typeName() << "(" << format.getPk("interno") << ").isEmpty()) {" << endl
								<< "\t\t\tthrow new " << format.typeName() << "RelacionadoException();" << endl
								<< "\t\t}" << endl;
						}
					}
				}
				out << "\t\tthis.rep" << format.typeName() << ".remover" << /*camelCase(format.pkName()) << */"(" << format.getPk("interno") << ");" << endl;
				out << "\t}" << endl;
				
				out << "\tpublic " << entity->getName() << " buscar" << camelCase(field->getIdentifier()) << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException, " << entity->getName() << "InexistenteException {" << endl 
					<< "\t\treturn this.rep" << format.typeName() << ".buscar" << camelCase(field->getIdentifier()) << "(" << field->getIdentifier() << ");" << endl
					<< "\t}" << endl;
			} else {
				out << "\tpublic List<" << entity->getName() << "> buscar" << camelCase(field->getIdentifier()) << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException {" << endl
					<< "\t\treturn this.rep" << format.typeName() << ".buscar" << camelCase(field->getIdentifier()) << "(" << field->getIdentifier() << ");" << endl
					<< "\t}" << endl;
			}
			
		}
		out	<< "}" << endl;
			
		out.close();
	}
}

void generateJavaFacade() {
	string path = toLower(gen::projectName) + "\\" ;
	ofstream out((path + string("Fachada") + gen::projectName + ".java").c_str());
	
	out << "package " << toLower(gen::projectName) << ";" << endl << endl;
	out << "import javax.ejb.Stateless;" << endl
		<< "import java.io.Serializable;" << endl
		<< "import javax.ejb.EJB;" << endl;
	out << "import java.util.List;" << endl;
	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		out << "import " << toLower(gen::projectName) << "." << toLower(entity->getName()) << ".*;" << endl;
	}
	
	out << endl;
	
	out << "@Stateless" << endl
		<< "public class Fachada" << gen::projectName << " implements Serializable {" << endl;
	
	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		EntityJavaFormat format(entity);
		
		out << "\t@EJB" << endl
			<< "\tprivate Cadastro" << format.typeName() << " cad" << format.typeName() << ";" << endl; 
	}
	out << endl;
	
	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		EntityJavaFormat format(entity);
		
		out << "\tpublic void adicionar(" << entity->getName() << " " << toLower(entity->getName()) << ") throws ErroInternoException, " << format.typeName() << "ExistenteException {" << endl
			<< "\t\tthis.cad" << format.typeName() << ".adicionar(" << format.varName() << ");" << endl
			<< "\t}" << endl;
		
			
		//Atualizar
		out << "\tpublic void atualizar(" << entity->getName() << " " << toLower(entity->getName()) << ") throws ErroInternoException, " << entity->getName() << "InexistenteException {" << endl
			<< "\t\tthis.cad" << format.typeName() << ".atualizar(" << format.varName() << ");" << endl
			<< "\t}" << endl;
			
		out << "\tpublic List<" << entity->getName() << "> listarTodos" << format.typeName() << "() throws ErroInternoException {" << endl
			<< "\t\treturn this.cad" << format.typeName() << ".listarTodos();" << endl
			<< "\t}" << endl;
		
		for(int j=0;j<entity->getFields()->getFields().size();j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			if(field->isUnique()) {
				bool withWeakRelationship = false;
				for(int k=0;k<gen::relationship.size();k++) {
					gen::Relationship *rel = gen::relationship[k];
					if(rel->getTo()->getName() == entity->getName() || rel->getFrom()->getName() == entity->getName()) {
						withWeakRelationship = !rel->isWeak();
						break;
					}
				}
				out << "\tpublic void remover" << format.typeName() << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException, " << entity->getName() << "InexistenteException" << (withWeakRelationship?(string(", ") + entity->getName() + "RelacionadoException"):"") << " {" << endl
					<< "\t\tthis.cad" << format.typeName() << ".remover(" << field->getIdentifier() << ");" << endl
					<< "\t}" << endl;
				
				out << "\tpublic " << entity->getName() << " buscar" << format.typeName() << camelCase(field->getIdentifier()) << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException, " << entity->getName() << "InexistenteException {" << endl 
					<< "\t\treturn this.cad" << format.typeName() << ".buscar" << camelCase(field->getIdentifier()) << "(" << field->getIdentifier() << ");" << endl
					<< "\t}" << endl;
			} else {
				out << "\tpublic List<" << entity->getName() << "> buscar" << format.typeName() << camelCase(field->getIdentifier()) << "(" << getJavaType(field->getType()) << " " << field->getIdentifier() << ") throws ErroInternoException {" << endl
					<< "\t\treturn this.cad" << format.typeName() << ".buscar" << camelCase(field->getIdentifier()) << "(" << field->getIdentifier() << ");" << endl
					<< "\t}" << endl;
			}
		}
	}
	
	out << "}" << endl;
	out.close();
		
		
}

void createDirectoryTree() {
	string command = "mkdir " + toLower(gen::projectName);
	system(command.c_str());
	command = "mkdir " + toLower(gen::projectName) + "\\ui";
	system(command.c_str());
	command = "mkdir " + toLower(gen::projectName) + "\\paginas";
	system(command.c_str());
	for(int i=0; i < gen::entities.size();i++) {
		command = "mkdir " + toLower(gen::projectName) + "\\" + toLower(gen::entities[i]->getName());
		system(command.c_str());
		command = "mkdir " + toLower(gen::projectName) + "\\paginas\\" + toLower(gen::entities[i]->getName());
		system(command.c_str());
	}
	
}


void generateFaceletsListar() {
	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		EntityJavaFormat format(entity);
		string path = toLower(gen::projectName) + "\\paginas\\" + toLower(entity->getName()) + "\\";
		ofstream out((path + string("listar") + format.typeName() + ".xhtml").c_str());
		
		out << "<?xml version='1.0' encoding='UTF-8' ?>" << endl
			<< "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl
			<< "<html xmlns=\"http://www.w3.org/1999/xhtml\"" << endl
			<< "\t\txmlns:h=\"http://java.sun.com/jsf/html\"" << endl
			<< "\t\txmlns:f=\"http://java.sun.com/jsf/core\"" << endl
			<< "\t\txmlns:p=\"http://primefaces.org/ui\"" << endl
			<< "\t\txmlns:ui=\"http://java.sun.com/jsf/facelets\">" << endl
			<< "\t<h:head><title>Cadastrar " << format.typeName() << "</title>" << endl
			<< "\t\t<script type=\"text/javascript\">" << endl
			<< "PrimeFaces.locales['pt_BR'] = {  " << endl
			<< "	closeText: 'Fechar',  " << endl
			<< "	prevText: 'Anterior',  "<< endl
			<< "	nextText: 'Próximo',  " << endl
			<< "	monthNames: ['Janeiro','Fevereiro','Março','Abril','Maio','Junho','Julho','Agosto','Setembro','Outubro','Novembro','Dezembro'],  " << endl
			<< "	monthNamesShort: ['Jan','Fev','Mar','Abr','Mai','Jun', 'Jul','Ago','Set','Out','Nov','Dez'],  " << endl
			<< "	dayNames: ['Domingo','Segunda','Terça','Quarta','Quinta','Sexta','Sábado'],  " << endl
			<< "	dayNamesShort: ['Dom','Seg','Ter','Qua','Qui','Sex','Sáb'],  " << endl
			<< "	dayNamesMin: ['D','S','T','Q','Q','S','S'],  " << endl
			<< "	weekHeader: 'Semana',  " << endl
			<< "	firstDay: 1,  " << endl
			<< "	isRTL: false, " << endl 
			<< "	showMonthAfterYear: false,  " << endl
			<< "	yearSuffix: '',  " << endl
			<< "	timeOnlyTitle: 'Só Horas',  " << endl
			<< "	timeText: 'Tempo',  " << endl
			<< "	hourText: 'Hora', " << endl
			<< "	minuteText: 'Minuto', " << endl
			<< "	secondText: 'Segundo',  " << endl
			<< "	currentText: 'Data Atual',  " << endl
			<< "	ampm: false,  " << endl
			<< "	month: 'Mês',  " << endl
			<< "	week: 'Semana',  " << endl
			<< "	day: 'Dia',  " << endl
			<< "	allDayText : 'Todo Dia'  " << endl
			<< "};  " << endl
			<< "\t</script>" << endl
			<< "</h:head>" << endl
			<< "\t<h:body>" << endl;
			
		out << "\t\t<h:form>" << endl
			<< "\t\t\t<p:panel header=\"Listar " << format.typeName() << "\">" << endl;
			//<< "\t\t\t\t<h:panelGrid columns=\"2\">" << endl;
			
		out << "\t\t\t\t<p:dataTable value=\"#{managedBean" << format.typeName() << ".listaCompleta}\">" << endl;
		for(int j=0;j<entity->getFields()->getFields().size(); j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			out << "\t\t\t\t\t<p:column headerText=\"" << camelCase(field->getIdentifier()) << "\">" << endl;
			
			switch(field->getType()) {
				case gen::INT:
				case gen::REAL:
				case gen::LONG:
				case gen::TEXT:
					out << "\t\t\t\t\t<h:outputText value=\"#{managedBean" << format.typeName() << "." << format.varName() << "." << field->getIdentifier() << "}\"/>" << endl;
					break;
				case gen::DATE:
					out << "\t\t\t\t\t<h:outputText value=\"#{managedBean" << format.typeName() << "." << format.varName() << "." << field->getIdentifier() << "}\">" << endl
						<< "\t\t\t\t\t\t<f:convertDateTime pattern=\"dd/MM/yyyy\"/>" << endl
						<< "\t\t\t\t\t</h:outputText>" << endl;
					break;
				case gen::TIME:
					out << "\t\t\t\t\t<h:outputText value=\"#{managedBean" << format.typeName() << "." << format.varName() << "." << field->getIdentifier() << "}\">" << endl
						<< "\t\t\t\t\t\t<f:convertDateTime pattern=\"HH:mm\"/>" << endl
						<< "\t\t\t\t\t</h:outputText>" << endl;
					break;
				case gen::FILE:
					out << "\t\t\t\t\t<h:outputText value=\"Not Supported Yet\" />" << endl;
					break;
				case gen::BOOL:
					out << "\t\t\t\t\t<p:selectBooleanCheckbox value=\"#{managedBean" << format.typeName() << "." << format.varName() << "." << field->getIdentifier() << "}\"/>" << endl;
					break;
			}
			out << "\t\t\t\t\t</p:column>" << endl;
			
		}
		out << "\t\t\t\t</p:dataTable>" << endl;
		//out << "\t\t\t\t</h:panelGrid>" << endl;
		out //<< "\t\t\t\t<p:commandButton id=\"btnCadastrar\" value=\"Cadastrar\" ajax=\"false\" action=\"#{managedBean" << format.typeName() << ".cadastrar()}\"/>" << endl
            << "\t\t\t\t<p:messages showDetail=\"true\"/>" << endl		
			<< "\t\t\t</p:panel>" << endl		
			<< "\t\t</h:form>" << endl
			<< "\t</h:body>" << endl
			<< "</html>";

		
		out.close();
	}
}

void generateFaceletsCadastrar() {
	for(int i=0; i<gen::entities.size();i++) {
		gen::Entity *entity = gen::entities[i];
		EntityJavaFormat format(entity);
		string path = toLower(gen::projectName) + "\\paginas\\" + toLower(entity->getName()) + "\\";
		ofstream out((path + string("cadastrar") + format.typeName() + ".xhtml").c_str());
		
		out << "<?xml version='1.0' encoding='UTF-8' ?>" << endl
			<< "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">" << endl
			<< "<html xmlns=\"http://www.w3.org/1999/xhtml\"" << endl
			<< "\t\txmlns:h=\"http://java.sun.com/jsf/html\"" << endl
			<< "\t\txmlns:f=\"http://java.sun.com/jsf/core\"" << endl
			<< "\t\txmlns:p=\"http://primefaces.org/ui\"" << endl
			<< "\t\txmlns:ui=\"http://java.sun.com/jsf/facelets\">" << endl
			<< "\t<h:head><title>Cadastrar " << format.typeName() << "</title>" << endl
			<< "\t\t<script type=\"text/javascript\">" << endl
			<< "PrimeFaces.locales['pt_BR'] = {  " << endl
			<< "	closeText: 'Fechar',  " << endl
			<< "	prevText: 'Anterior',  "<< endl
			<< "	nextText: 'Próximo',  " << endl
			<< "	monthNames: ['Janeiro','Fevereiro','Março','Abril','Maio','Junho','Julho','Agosto','Setembro','Outubro','Novembro','Dezembro'],  " << endl
			<< "	monthNamesShort: ['Jan','Fev','Mar','Abr','Mai','Jun', 'Jul','Ago','Set','Out','Nov','Dez'],  " << endl
			<< "	dayNames: ['Domingo','Segunda','Terça','Quarta','Quinta','Sexta','Sábado'],  " << endl
			<< "	dayNamesShort: ['Dom','Seg','Ter','Qua','Qui','Sex','Sáb'],  " << endl
			<< "	dayNamesMin: ['D','S','T','Q','Q','S','S'],  " << endl
			<< "	weekHeader: 'Semana',  " << endl
			<< "	firstDay: 1,  " << endl
			<< "	isRTL: false, " << endl 
			<< "	showMonthAfterYear: false,  " << endl
			<< "	yearSuffix: '',  " << endl
			<< "	timeOnlyTitle: 'Só Horas',  " << endl
			<< "	timeText: 'Tempo',  " << endl
			<< "	hourText: 'Hora', " << endl
			<< "	minuteText: 'Minuto', " << endl
			<< "	secondText: 'Segundo',  " << endl
			<< "	currentText: 'Data Atual',  " << endl
			<< "	ampm: false,  " << endl
			<< "	month: 'Mês',  " << endl
			<< "	week: 'Semana',  " << endl
			<< "	day: 'Dia',  " << endl
			<< "	allDayText : 'Todo Dia'  " << endl
			<< "};  " << endl
			<< "\t</script>" << endl
			<< "</h:head>" << endl
			<< "\t<h:body>" << endl;
			
		out << "\t\t<h:form>" << endl
			<< "\t\t\t<p:panel header=\"Cadastrar " << format.typeName() << "\">" << endl
			<< "\t\t\t\t<h:panelGrid columns=\"2\">" << endl;
			
		for(int j=0;j<entity->getFields()->getFields().size(); j++) {
			gen::Field *field = entity->getFields()->getFields()[j];
			if(field->isPk()) continue;
			out << "\t\t\t\t\t<h:outputText value=\"" << camelCase(field->getIdentifier()) << "\"/>" << endl;
			switch(field->getType()) {
				case gen::INT:
				case gen::REAL:
				case gen::LONG:
				case gen::TEXT:
					out << "\t\t\t\t\t<p:inputText value=\"#{managedBean" << format.typeName() << "." << format.varName() << "." << field->getIdentifier() << "}\"/>" << endl;
					break;
				case gen::DATE:
					out << "\t\t\t\t\t<p:calendar navigator=\"true\" pattern=\"dd/MM/yyyy\" locale=\"pt_BR\" value=\"#{managedBean" << format.typeName() << "." << format.varName() << "." << field->getIdentifier() << "}\"/>" << endl;
					break;
				case gen::TIME:
					out << "\t\t\t\t\t<p:calendar pattern=\"HH:mm\" timeOnly=\"true\" value=\"#{managedBean" << format.typeName() << "." << format.varName() << "." << field->getIdentifier() << "}\"/>" << endl;
					break;
				case gen::FILE:
					out << "\t\t\t\t\t<p:fileUpload fileUploadListener=\"#{managedBean" << format.typeName() << ".handleFileUpload}\" mode=\"advanced\" dragDropSupport=\"true\" fileLimit=\"1\" />" << endl;
					break;
				case gen::BOOL:
					out << "\t\t\t\t\t<p:selectBooleanCheckbox value=\"#{managedBean" << format.typeName() << "." << format.varName() << "." << field->getIdentifier() << "}\"/>" << endl;
					break;
			}
			
		}
		
		out << "\t\t\t\t</h:panelGrid>" << endl;
		out << "\t\t\t\t<p:commandButton id=\"btnCadastrar\" value=\"Cadastrar\" ajax=\"false\" action=\"#{managedBean" << format.typeName() << ".cadastrar()}\"/>" << endl
            << "\t\t\t\t<p:messages showDetail=\"true\"/>" << endl		
			<< "\t\t\t</p:panel>" << endl		
			<< "\t\t</h:form>" << endl
			<< "\t</h:body>" << endl
			<< "</html>";

		
		out.close();
	}
}

int main(int argc, char **argv) {
	extern FILE *yyin;
	if(argc != 2) {
		printf("Uso: %s arquivo_de_entrada\n", argv[0]);
		return 1;
	}
	
	if ((yyin = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Erro ao tentar abrir o arquivo %s!\n", argv[1]);
		return 1;
	}
	
	if (yyparse()) {
		fprintf(stderr, "Não foi possível compilar %s!\n", argv[1]);
		return 1;
	}
	printf("Projeto:%s\nsucesso!\n\n", gen::projectName.c_str());

	for(int i=0; i < gen::entities.size();i++) {
		printf("%s\n", gen::entities[i]->getName().c_str());
	}
	
	createDirectoryTree();
	
	generateJavaEntities();
	generateJavaPersistenceInterfaces();
	generateJavaExceptions();
	generateJavaJpaPersistenceImplementation();
	generateJavaBusinessClasses();
	generateJavaFacade();
	generateFaceletsCadastrar();
	generateFaceletsListar();
	return 0;
}
