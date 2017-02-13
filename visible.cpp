#include "Visible.h"
#include "GraphicsSystem.h"
#include "OpenGLUtility.h"
#include "Transform.h"

/*MeshVisible::MeshVisible(Mesh *mesh_)
{
	mesh= mesh_;

	glGenVertexArrays(1, &vertex_array_handle);
	glBindVertexArray(vertex_array_handle);

	cout << glGetError() << endl;

	GLuint vertex_buffer_handle;
	glGenBuffers(1, &vertex_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
	glBufferData(GL_ARRAY_BUFFER, mesh->GetVertexCount()* 3* sizeof(float), &mesh->positions[0], GL_STATIC_DRAW);

	GLint position_attribute= glGetAttribLocation(3, "position");//have to decide how this will get access to program handle
	glVertexAttribPointer(position_attribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 3, nullptr);
	glEnableVertexAttribArray(position_attribute);

	cout << glGetError() << endl;

	GLuint normal_buffer_handle;
	glGenBuffers(1, &normal_buffer_handle);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer_handle);
	glBufferData(GL_ARRAY_BUFFER, mesh->GetVertexCount()* 3* sizeof(float), &mesh->normals[0], GL_STATIC_DRAW);

	cout << glGetError() << endl;

	GLint normal_attribute= glGetAttribLocation(3, "normal");//have to decide how this will get access to program handle
	cout << glGetError() << endl;
	glVertexAttribPointer(normal_attribute, 3, GL_FLOAT, GL_FALSE, sizeof(float)* 3, nullptr);
	cout << glGetError() << endl;
	glEnableVertexAttribArray(normal_attribute);

	cout << glGetError() << endl;
	
	glGenBuffers(1, &element_buffer_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_handle);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->GetTriangleCount()* 3* sizeof(float), &mesh->position_indices[0], GL_STATIC_DRAW);

	cout << glGetError() << endl;
}

void MeshVisible::SetDisplacement(Vec3f displacement_)
{
	displacement= displacement_;
}

void MeshVisible::SetRotation(float rotation_)
{
	rotation= rotation_;
}

void MeshVisible::Draw()
{
	cout << glGetError() << endl;

	glBindVertexArray(vertex_array_handle);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_handle);

	cout << glGetError() << endl;

	Transform foo= Transform().RotateAboutY(0).Translate(displacement).c;
	glUniformMatrix4fv(glGetUniformLocation(3, "model_transform"), 1, GL_TRUE, foo.c);

	cout << glGetError() << endl;
	
	glDrawElements(GL_TRIANGLES, mesh->GetTriangleCount()* 3, GL_UNSIGNED_INT, nullptr);

	cout << glGetError() << endl;
}*/
