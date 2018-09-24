#include "VertexArray.h"

VertexArray::VertexArray() : m_attributeCount(0)
{
	GLCall(glGenVertexArrays(1, &m_RendererID));
	GLCall(glBindVertexArray(m_RendererID));
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::AddBuffer(const VertexBuffer & vb, const VertexBufferLayout & layout)
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for (size_t i = 0; i < elements.size(); i++)
	{
		const auto& element = elements[i];
		unsigned int attributeIndex = m_attributeCount + i;

		GLCall(glEnableVertexAttribArray(attributeIndex));
		GLCall(glVertexAttribPointer(
			attributeIndex,
			element.count,
			element.type,
			element.normalized,
			layout.GetStride(),
			(const void*)offset
		));
		offset += VertexBufferElement::GetSizeOfType(element.type);
	}
	m_attributeCount += elements.size();
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}
