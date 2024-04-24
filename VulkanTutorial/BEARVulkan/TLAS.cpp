#include "BEARHeaders/TLAS.h"


TLAS::TLAS(const std::vector<TlasInstanceData*>& levelData)
{

}

TLAS::~TLAS()
{

}

void TLAS::SetInstanceTransform(const glm::mat4 newTransform, const uint32_t id)
{
	assert(id < m_LevelData.size() && "ID out of bounds");
	m_LevelData[id]->m_Transform = newTransform;
}

glm::mat4& TLAS::GetInstanceTransformRef(const uint32_t id) const
{
	assert(id < m_LevelData.size() && "ID out of bounds");
	return m_LevelData[id]->m_Transform;
}

void TLAS::Update()
{

}
