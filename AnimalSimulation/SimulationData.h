#ifndef H__SIMULATIONDATA
#define H__SIMULATIONDATA

#include <string>
#include <vector>

#include "gl/glm/glm.hpp"
#include "gl/glm/gtx/transform.hpp"
#include "gl/glm/gtc/type_ptr.hpp"
#include "gl/glm/gtc/matrix_inverse.hpp"

struct SensoryCell
{
	float m_phi, m_theta;
};

struct Structure
{
	float m_size, m_angle;
	std::vector<SensoryCell> m_sensors;
};

struct Food
{
	float m_intensity, m_reset;
	glm::vec3 m_position;
	std::vector<float> m_visibilityFactors;
};

struct Frame
{
	glm::vec3 m_rotation, m_translation;
	std::vector<Food> m_foods;
};

struct Data
{
	Structure m_structure;
	std::vector<Frame> m_frames;
};

class SimulationData
{
public:
	SimulationData(const std::string& filePath);
	~SimulationData();

	const Data& getData() const;

private:
	void loadData();

private:
	std::string m_filePath;
	Data m_data;
};

#endif // !H__SIMULATIONDATA
