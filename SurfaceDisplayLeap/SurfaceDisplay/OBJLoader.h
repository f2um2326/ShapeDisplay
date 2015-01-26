/************************************************************************
　　OBJLoader.h

　　Alias Wavefront OBJ File Loader

　　Date : Nov. 18, 2007
　　Version : 1.0
　　Author : Pocol
*************************************************************************/

#ifndef _OBJ_LOADER_H_INCLUDED_
#define _OBJ_LOADER_H_INCLUDED_

//　Disable Warning C4996
#pragma warning(disable : 4996)

//
//　include
//
#include <iostream>
#include <GL/glut.h>
using namespace std;


//
//　define
//
#define OBJ_BUFFER_LENGTH 1024
#define OBJ_NAME_LENGTH 256


/////////////////////////////////////////////////////////////////////////
//　　OBJVertex class
/////////////////////////////////////////////////////////////////////////
class OBJVertex
{
public:
	union { struct {float x, y, z; }; float v[3]; };
	operator float* () { return (float*)&x; }
	operator const float*() { return (const float*)&x; }
	OBJVertex &operator = (const OBJVertex &vert) { x=vert.x; y=vert.y; z=vert.z; return (*this); }
	OBJVertex operator *(float s) { return OBJVertex(x*s, y*s, z*s); }
	OBJVertex operator /(float s) { float sInv = 1.0f/s; return OBJVertex(x*sInv, y*sInv, z*sInv); }
	OBJVertex(float nx=0.0, float ny=0.0, float nz=0.0) : x(nx), y(ny), z(nz) {}
};

/////////////////////////////////////////////////////////////////////////
//　　OBJColor class
/////////////////////////////////////////////////////////////////////////
class OBJColor
{
public:
	union { struct { float r, g, b, a; }; float c[4]; };
	operator float* () { return (float*)&r; }
	operator const float* () { return (const float*)&r; }
	OBJColor &operator = (const OBJColor &col) { r=col.r; g=col.g; b=col.b; a=col.a; return(*this); }
	OBJColor operator *(float s) { return OBJColor(r*s, g*s, b*s, a*s); }
	OBJColor operator /(float s) { float sInv = 1.0f/s; return OBJColor(r*sInv, g*sInv, b*sInv, a*sInv); }
	OBJColor(float nr=0.0, float ng=0.0, float nb=0.0, float na=0.0) : r(nr), g(ng), b(nb), a(na) {}
};

////////////////////////////////////////////////////////////////////////
//　　OBJFace class
////////////////////////////////////////////////////////////////////////
class OBJFace
{
public:
	GLenum type;
	int element;
	int material_index;
	int *vertex_index;
	int *normal_index;
	bool use_normal;
	OBJFace &operator = (const OBJFace &surf)
	{
		type=surf.type;
		element=surf.element;
		material_index=surf.material_index;
		vertex_index = new int [element];
		normal_index = new int [element];
		use_normal = surf.use_normal;
		for ( int i=0; i<element; i++ )
		{
			vertex_index[i] = surf.vertex_index[i];
			if ( use_normal ) normal_index[i] = surf.normal_index[i];
			else normal_index[i] = -1;
		}
		return (*this);
	}
	OBJFace(GLenum ntype=0, 
		int nelement=0, 
		int *nvertex_index=NULL, 
		int *nnormal_index=NULL, 
		bool nuse_normal=false) 
		: type(ntype),
		element(nelement),
		vertex_index(nvertex_index),
		normal_index(nnormal_index), 
		use_normal(nuse_normal) {}
};

////////////////////////////////////////////////////////////////////////
//　　OBJMaterial
////////////////////////////////////////////////////////////////////////
class OBJMaterial
{
public:
	char name[OBJ_NAME_LENGTH];
	OBJColor color;
	OBJColor ambient;
	OBJColor diffuse;
	OBJColor specular;
	OBJColor emission;
	float shininess;
	OBJMaterial &operator = (const OBJMaterial &mat)
	{ 
		strcpy(name, mat.name); 
		color=mat.color; 
		ambient=mat.ambient;
		diffuse=mat.diffuse;
		specular=mat.specular;
		emission=mat.emission;
		shininess=mat.shininess;
		return (*this); }
	OBJMaterial(char mat_name[OBJ_NAME_LENGTH] = "default",
		OBJColor mat_color = OBJColor(1.0, 1.0, 1.0, 1.0),
		OBJColor amb = OBJColor(1.0, 1.0, 1.0, 1.0),
		OBJColor dif = OBJColor(1.0, 1.0, 1.0, 1.0),
		OBJColor spe = OBJColor(1.0, 1.0, 1.0, 1.0),
		OBJColor emi = OBJColor(0.0, 0.0, 0.0, 0.0),
		float power = 5.0f
		)
		: color(mat_color), 
		ambient(amb),
		diffuse(dif),
		specular(spe),
		emission(emi),
		shininess(power)
	{ strcpy(name, mat_name); }
};

////////////////////////////////////////////////////////////////////////
//　　OBJMesh
////////////////////////////////////////////////////////////////////////
class OBJMesh
{
protected:
	int num_vertex;
	int num_normal;
	int num_face;
	int num_material;
	float size;
	OBJVertex *vertex;
	OBJVertex *normal;
	OBJFace *face;
	OBJMaterial *material;
	bool LoadOBJFile(const char *filename);
	bool LoadMTLFile(const char *filename);
	char objFileName[OBJ_NAME_LENGTH];
	char mtlFileName[OBJ_NAME_LENGTH];
	char directoryName[OBJ_NAME_LENGTH];

public:
	GLuint listID;
	OBJMesh();
	~OBJMesh();
	void Release();
	void Render(float scale=1.0f);
	bool Load(const char *objfilename);
	GLuint MakeDisplayList(float scale=1.0f);
	void AddVertex(OBJVertex &vert);
	void AddNormal(OBJVertex &norm);
	void AddFace(OBJFace &surf);
	void AddMaterial(OBJMaterial &mat);

	int GetVertexSize() { return num_vertex; }
	int GetNormalSize() { return num_normal; }
	int GetFaceSize() { return num_face; }
	int GetMaterialSize() { return num_material; }

	void Information();
};

#endif	//　_OBJ_LOADER_H_INCLUDED_