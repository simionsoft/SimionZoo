#pragma once
#include <string>
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include "CNTKWrapper.h"

using namespace std;
enum class ActivationFunction
{
	sigmoid = 0,
	elu,
	selu,
	softplus,
	softsign,
	relu,
	tanh,
	hard_sigmoid,
	softmax,
	linear
};
class CIntTuple
{
protected:
	int m_dimension;
public:
	CIntTuple() {};
	~CIntTuple() {};
	CIntTuple(int dimension) { m_dimension = dimension; }
	int getDimension() { return m_dimension; }

	static CIntTuple* getInstance(tinyxml2::XMLElement* pNode);
	virtual CNTK::NDShape getNDShape() = 0;
};

class CIntTuple1D : public CIntTuple
{
protected:
	int m_x1;
	CIntTuple1D(tinyxml2::XMLElement* pNode);

public:
	CIntTuple1D() : CIntTuple(1) {}
	CIntTuple1D(int x1) : CIntTuple1D() {
		m_x1 = x1;
	}

	CNTK::NDShape getNDShape()
	{
		return { (size_t)m_x1 };
	}

	static CIntTuple1D* getInstance(tinyxml2::XMLElement* pNode);

	int getX1() { return m_x1; }
	void setX1(int x1) { m_x1 = x1; }
};

class CIntTuple2D : public CIntTuple
{
protected:
	int m_x1;
	int m_x2;
	CIntTuple2D(tinyxml2::XMLElement* pNode);
public:
	CIntTuple2D() : CIntTuple(2) {}
	CIntTuple2D(int x1, int x2) : CIntTuple2D() {
		m_x1 = x1;
		m_x2 = x2;
	}

	CNTK::NDShape getNDShape()
	{
		return { (size_t)m_x1, (size_t)m_x2 };
	}

	static CIntTuple2D* getInstance(tinyxml2::XMLElement* pNode);

	int getX1() { return m_x1; }
	void setX1(int x1) { m_x1 = x1; }
	int getX2() { return m_x2; }
	void setX2(int x2) { m_x2 = x2; }
};

class CIntTuple3D : public CIntTuple
{
protected:
	int m_x1;
	int m_x2;
	int m_x3;
	CIntTuple3D(tinyxml2::XMLElement* pNode);
public:
	CIntTuple3D() : CIntTuple(3) {}
	CIntTuple3D(int x1, int x2, int x3) : CIntTuple3D() {
		m_x1 = x1;
		m_x2 = x2;
		m_x3 = x3;
	}

	CNTK::NDShape getNDShape()
	{
		return { (size_t)m_x1, (size_t)m_x2, (size_t)m_x3 };
	}

	static CIntTuple3D* getInstance(tinyxml2::XMLElement* pNode);

	int getX1() { return m_x1; }
	void setX1(int x1) { m_x1 = x1; }
	int getX2() { return m_x2; }
	void setX2(int x2) { m_x2 = x2; }
	int getX3() { return m_x3; }
	void setX3(int x3) { m_x3 = x3; }
};

class CIntTuple4D : public CIntTuple
{
protected:
	int m_x1;
	int m_x2;
	int m_x3;
	int m_x4;
	CIntTuple4D(tinyxml2::XMLElement* pNode);

public:
	CIntTuple4D() : CIntTuple(4) {}
	CIntTuple4D(int x1, int x2, int x3, int x4) : CIntTuple4D() {
		m_x1 = x1;
		m_x2 = x2;
		m_x3 = x3;
		m_x4 = x4;
	}

	CNTK::NDShape getNDShape()
	{
		return { (size_t)m_x1, (size_t)m_x2, (size_t)m_x3, (size_t)m_x4 };
	}

	static CIntTuple4D* getInstance(tinyxml2::XMLElement* pNode);

	int getX1() { return m_x1; }
	void setX1(int x1) { m_x1 = x1; }
	int getX2() { return m_x2; }
	void setX2(int x2) { m_x2 = x2; }
	int getX3() { return m_x3; }
	void setX3(int x3) { m_x3 = x3; }
	int getX4() { return m_x4; }
	void setX4(int x4) { m_x4 = x4; }
};

class CInputDataValue
{
protected:
	string m_name;
	string m_id;
	CIntTuple* m_pShape;

	CInputDataValue(tinyxml2::XMLElement* pNode);
public:
	CInputDataValue() {};

	static CInputDataValue* getInstance(tinyxml2::XMLElement* pNode);

	string getID() { return m_id; }
	void setID(string id) { m_id = id; }

	string getName() { return m_name; }
	void setName(string name) { m_name = name; }

	CIntTuple* getShape() { return m_pShape; }
	void setShape(CIntTuple* shape) { m_pShape = shape; }
};

class CLinkConnection
{
protected:
	string m_targetID;
	CLinkConnection(tinyxml2::XMLElement* pNode);
public:
	CLinkConnection();
	~CLinkConnection();
	const string& getTargetId() const { return m_targetID; }

	static CLinkConnection* getInstance(tinyxml2::XMLElement* pNode);
};