#include "DebugBuffer.h"
#include <scene/Scene.h>
#include <model/SkinnedMesh.h>
#include <component/UIComponent.h>


void DebugBuffer::Initialize()
{
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	// create floating point color buffer
	glGenTextures(1, &m_FBOTexture);
	glBindTexture(GL_TEXTURE_2D, m_FBOTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_FBOTexture, 0);
	// create depth buffer (renderbuffer)
	glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	// attach buffers
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_Width, m_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		CHROMA_WARN("Framebuffer Not Complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// build Line VAO
	GeneratePointVAO();
}


void DebugBuffer::GeneratePointVAO()
{
	ChromaVertex singleVert;

	// Generate buffers
	// Vertex Array Object Buffer
	glGenVertexArrays(1, &pointVAO);
	// Vertex Buffer and Element Buffer
	glGenBuffers(1, &pointVBO);

	// Bind buffers
	glBindVertexArray(pointVAO);
	glBindBuffer(GL_ARRAY_BUFFER, pointVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(ChromaVertex), &singleVert, GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChromaVertex), (void*)0);

}

void DebugBuffer::DrawShapes()
{
	Bind();
	// OVERLAY 
	CopyColor(m_PostFXBuffer->GetFBO(), m_FBO);
	DrawOverlayShapes();

	// Set to alpha blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Disable Back Face Culling to allow interior of transparent objects to be seen
	glDisable(GL_CULL_FACE);
	for (UID const& componentUID : Chroma::Scene::GetUIComponentUIDs())
	{
		((UIComponent*)Chroma::Scene::GetComponent(componentUID))->Draw();
	}
	// set to default blending
	glBlendFunc(GL_ONE, GL_ZERO);
	// Re enable backface culling for preventing unecessary rendering
	glEnable(GL_CULL_FACE);

	// DEPTH RESPECTING
	CopyDepth(m_PostFXBuffer->GetFBO(), m_FBO);
	DrawDepthCulledShapes();

	UnBind();
}

void DebugBuffer::DrawOverlayShapes()
{
	// skeletons
	if (m_DebugSkeletons)
	{
		DrawSceneSkeletons();
	}
	// lines
	for (LineShape line : m_OverlayLines)
		RenderLine(line);
	// spheres
	for (SphereShape sphere : m_OverlaySpheres)
		RenderSphere(sphere);
	// boxes
	for (BoxShape box : m_OverlayBoxes)
		RenderBox(box);
	// joints
	for (JointShape joint : m_OverlayJoints)
		RenderJoint(joint);
	// coordinates
	for (CoordinatesShape coordinate : m_OverlayCoordinates)
		RenderCoordinate(coordinate);

}

void DebugBuffer::DrawDepthCulledShapes()
{
	// lines
	for (LineShape line : m_lines)
		RenderLine(line);
	// spheres
	for (SphereShape sphere : m_spheres)
		RenderSphere(sphere);
	// boxes
	for (BoxShape box : m_boxes)
		RenderBox(box);
	// coordinates
	for (CoordinatesShape coordinate : m_Coordinates)
		RenderCoordinate(coordinate);
}

void DebugBuffer::RenderLine(LineShape line)
{
	m_LineShader.Use();
	m_LineShader.SetUniform("Start", line.start);
	m_LineShader.SetUniform("End", line.end);
	m_LineShader.SetUniform("view", Chroma::Scene::GetRenderCamera()->GetViewMatrix());
	m_LineShader.SetUniform("projection", Chroma::Scene::GetRenderCamera()->GetProjectionMatrix());
	m_LineShader.SetUniform("model", glm::mat4(1.0f));
	m_LineShader.SetUniform("color", line.color);

	BindPointVAO();
}

void DebugBuffer::RenderSphere(SphereShape sphere)
{
	m_SphereShader.Use();
	m_SphereShader.SetUniform("VPMat", Chroma::Scene::GetRenderCamera()->GetViewProjMatrix());
	m_SphereShader.SetUniform("model", sphere.transform);
	m_SphereShader.SetUniform("radius", sphere.m_Radius);
	m_SphereShader.SetUniform("color", sphere.color);

	BindPointVAO();
}

void DebugBuffer::RenderBox(BoxShape box)
{
	m_BoxShader.Use();
	m_BoxShader.SetUniform("BBoxMin", box.bbox_min);
	m_BoxShader.SetUniform("BBoxMax", box.bbox_max);
	m_BoxShader.SetUniform("VPMat" , Chroma::Scene::GetRenderCamera()->GetViewProjMatrix());
	m_BoxShader.SetUniform("model", box.transform);
	m_BoxShader.SetUniform("color", box.color);

	BindPointVAO();
}

void DebugBuffer::RenderJoint(JointShape joint)
{
	m_JointShader.Use();
	m_JointShader.SetUniform("JointPos", joint.jointPos);
	m_JointShader.SetUniform("ChildPos", joint.childPos);
	m_JointShader.SetUniform("Size", joint.size);
	m_JointShader.SetUniform("VPMat", Chroma::Scene::GetRenderCamera()->GetViewProjMatrix());
	m_JointShader.SetUniform("transform", joint.transform);
	m_JointShader.SetUniform("color", joint.color);
	BindPointVAO();
}

void DebugBuffer::RenderCoordinate(CoordinatesShape coordinate)
{
	// Coordinate reference 
	m_CoordinatesShader.Use();
	m_CoordinatesShader.SetUniform("VPMat", Chroma::Scene::GetRenderCamera()->GetViewProjMatrix());
	m_CoordinatesShader.SetUniform("transform", coordinate.transform);
	m_CoordinatesShader.SetUniform("Size", coordinate.size);

	BindPointVAO();
}

void DebugBuffer::BindPointVAO()
{
	glBindVertexArray(pointVAO);
	glDrawArrays(GL_POINTS, 0, 1);
	glBindVertexArray(0);
}


void DebugBuffer::DrawCoordinates(const glm::mat4& transform, const float& size)
{
	CoordinatesShape newCoordinate;
	newCoordinate.transform = transform;
	newCoordinate.size = size;
	m_Coordinates.push_back(newCoordinate);
}

void DebugBuffer::DrawOverlayCoordinates(const glm::mat4& transform, const float& size)
{
	CoordinatesShape newCoordinate;
	newCoordinate.transform = transform;
	newCoordinate.size = size;
	m_OverlayCoordinates.push_back(newCoordinate);
}

void DebugBuffer::DrawLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color)
{
	LineShape new_line;
	new_line.start = from;
	new_line.end = to;
	new_line.color = color;
	m_lines.push_back(new_line);
}

void DebugBuffer::DrawOverlayLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color)
{
	LineShape new_line;
	new_line.start = from;
	new_line.end = to;
	new_line.color = color;
	m_OverlayLines.push_back(new_line);
}

void DebugBuffer::DrawBox(const glm::vec3& bbMin, const glm::vec3& bbMax, const glm::vec3& color)
{
	BoxShape new_box;
	new_box.bbox_min = bbMin;
	new_box.bbox_max = bbMax;
	new_box.color = color;
	m_boxes.push_back(new_box);
}

void DebugBuffer::DrawBox(const glm::vec3& bbMin, const glm::vec3& bbMax, const glm::mat4& trans, const glm::vec3& color)
{
	BoxShape new_box;
	new_box.bbox_min = bbMin;
	new_box.bbox_max = bbMax;
	new_box.color = color;
	new_box.transform =  trans;
	m_boxes.push_back(new_box);
}

void DebugBuffer::DrawOverlayBox(const glm::vec3& bbMin, const glm::vec3& bbMax, const glm::vec3& color)
{
	BoxShape new_box;
	new_box.bbox_min = bbMin;
	new_box.bbox_max = bbMax;
	new_box.color = color;
	m_OverlayBoxes.push_back(new_box);
}

void DebugBuffer::DrawSphere(const glm::vec3& center, const float& m_Radius, const glm::vec3& color)
{
	SphereShape new_sphere;
	new_sphere.transform = glm::translate(new_sphere.transform, center);
	new_sphere.m_Radius = m_Radius;
	new_sphere.color = color;
	m_spheres.push_back(new_sphere);
}

void DebugBuffer::DrawOverlaySphere(const glm::vec3& center, const float& m_Radius, const glm::vec3& color)
{
	SphereShape new_sphere;
	new_sphere.transform = glm::translate(new_sphere.transform, center);
	new_sphere.m_Radius = m_Radius;
	new_sphere.color = color;
	m_OverlaySpheres.push_back(new_sphere);
}

void DebugBuffer::DrawOverlayJoint(const glm::vec3& originPosition, const glm::vec3 childPosition, const glm::mat4 jointTransform, const float& size, const glm::vec3& color)
{
	JointShape newJoint;
	newJoint.transform = jointTransform;
	newJoint.jointPos = originPosition;
	newJoint.childPos = childPosition;
	newJoint.size = size;
	newJoint.color = color;
	m_OverlayJoints.push_back(newJoint);
}

void DebugBuffer::ToggleDrawSkeletons()
{
	m_DebugSkeletons = m_DebugSkeletons ? false : true;
}

void DebugBuffer::DrawSceneSkeletons()
{
	for (UID const& uid : Chroma::Scene::GetAnimatedEntityUIDs())
	{
		for (UID const& component : Chroma::Scene::GetEntity(uid)->getMeshComponentUIDs())
		{
			// check if mesh skinned
			if (((MeshComponent*)Chroma::Scene::GetComponent(component))->GetIsSkinned())
			{
				((SkinnedMesh*)Chroma::Scene::GetComponent(component))->GetSkeleton()->DebugDraw();
			}
		}
	}
}


void DebugBuffer::ClearColorAndDepth()
{
	m_OverlayLines.clear();
	m_OverlayBoxes.clear();
	m_OverlaySpheres.clear();
	m_OverlayJoints.clear();
	m_OverlayCoordinates.clear();

	m_lines.clear();
	m_spheres.clear();
	m_boxes.clear();
	m_Coordinates.clear();
}

void DebugBuffer::Draw()
{
	// 1. Draw shapes to debugbuffer after fetching postFX color and depth
	DrawShapes();

	// 2. Copy New Color and Depth back to postFX Buffer
	CopyColorAndDepth(m_FBO, m_PostFXBuffer->GetFBO());
}





