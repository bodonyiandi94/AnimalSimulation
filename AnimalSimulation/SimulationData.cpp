#include "SimulationData.h"
#include <fstream>
#include <sstream>
#include "rapidjson\document.h"
#include "rapidjson\error/en.h"
#include <iostream>

#include "sdl2/SDL.h"

SimulationData::SimulationData(const std::string& filePath):
	m_filePath(filePath)
{
	loadData();
}

SimulationData::~SimulationData()
{

}


const Data& SimulationData::getData() const
{
	return m_data;
}

void SimulationData::loadData()
{
	std::ifstream fin(m_filePath);

	if (!fin.good())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error parsing simulation data");
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "  cause: unable to open '%s'", m_filePath.c_str());
		return;
	}

	std::stringstream ss;
	ss << fin.rdbuf();
	std::string content = ss.str();

	rapidjson::Document document;
	document.Parse(content.c_str());

	if (document.HasParseError())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"error parsing simulation data");
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "  cause: %s",rapidjson::GetParseError_En(document.GetParseError()));
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "  at: %d",document.GetErrorOffset());
		return;
	}

	const auto& structure = document["szerkezet"];
	const auto& frames = document["frames"];
	
	m_data.m_structure.m_size = structure["meret"].GetDouble();

	const auto& sensors = structure["erzosejtek"];
	for (int sensorId = 0;sensorId < sensors.Size(); sensorId++)
	{
		SensoryCell sensor;
		sensor.m_phi = sensors[sensorId]["phi"].GetDouble();
		sensor.m_theta = sensors[sensorId]["theta"].GetDouble();
		m_data.m_structure.m_sensors.push_back(sensor);
	}

	m_data.m_structure.m_angle = glm::radians(structure["latoszog"].GetDouble());

	for (int frameId = 0;frameId < frames.Size(); frameId++)
	{
		Frame frame;
		const auto& rotationVector = frames[frameId]["rv"];
		const auto& translationVector = frames[frameId]["tv"];

		frame.m_rotation.x = rotationVector[0].GetDouble();
		frame.m_rotation.y = rotationVector[1].GetDouble();
		frame.m_rotation.z = rotationVector[2].GetDouble();

		frame.m_translation.x = translationVector[0].GetDouble();
		frame.m_translation.y = translationVector[1].GetDouble();
		frame.m_translation.z = translationVector[2].GetDouble();

		const auto& jsFrame = frames[frameId]["kajak"];
		for (int foodId = 0;foodId < jsFrame.Size();foodId++)
		{
			Food food;
			const auto& jsFood = jsFrame[foodId];
			food.m_position = glm::vec3(jsFood["x"].GetDouble(), jsFood["y"].GetDouble(), jsFood["z"].GetDouble());
			food.m_intensity = jsFood["i"].GetDouble();
			food.m_reset = jsFood["reset"].GetDouble();

			const auto& visibilityFactors = jsFood["i1"];
			food.m_visibilityFactors.resize(visibilityFactors.Size());
			for (int i = 0;i < visibilityFactors.Size();i++)
			{
				food.m_visibilityFactors[i] = visibilityFactors[0].GetDouble();
			}

			frame.m_foods.push_back(food);
		}
		m_data.m_frames.push_back(frame);
	}
}