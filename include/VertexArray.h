#pragma once
#ifndef RSOS_VERTARRAY
#define RSOS_VERTARRAY

class VertexArray {
public:
	VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indices, unsigned int numIndices);
	~VertexArray();

	//Activate this vertex array (so we can draw it)
	void setActive();

	unsigned int getNumIndices()const { return mNumIndices; }
	unsigned int getNumVerts()const { return mNumVerts; }
private:
	unsigned int mNumVerts;
	unsigned int mNumIndices;
	//OpenGL ID of vertex buffer
	unsigned int mVertexBuffer;
	//OpenGL ID of index buffer
	unsigned int mIndexBuffer;
	unsigned int mVertexArray;
};

#endif