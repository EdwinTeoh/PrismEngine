#pragma once
#include "IComponent.h"
#include <map>
#include "../Sound/SoundEnum.h"
#include <iterator>
#include <algorithm>

namespace Engine
{
	namespace ECS
	{
		namespace Components
		{
			class AudioComp : public IComponent, public Serializer::ISerializer
			{

				std::map<SoundAction, std::string> _audioNameCont;

			public:
				virtual std::string GetComponentName()
				{
					std::string className = __FUNCTION__;
					size_t found = className.find_last_of("::") - 1;
					className = className.substr(0, found);
					found = className.find_last_of("::") + 1;
					className = className.substr(found);
					return className;
				}

				void TagAudio(SoundAction action, std::string audioname)
				{
					_audioNameCont.emplace(action, audioname);
				}

				//This function returns the audio filename tagged to the action
				std::string AudioCheck(SoundAction action)
				{
					auto searchAudio = _audioNameCont.find(action);

					if (searchAudio != _audioNameCont.end())
					{
						return searchAudio->second;
					}

					return "Not Found.";
				}

				AudioComp()
				{
				}

				~AudioComp()
				{
					_audioNameCont.clear();
				}


				bool Deserialize(const rapidjson::Document& doc, int id)
				{
					SoundAction _act;
					std::string _name;
					std::string a = std::to_string(id);
					
					if (doc.HasMember(a.c_str()))
					{
						if (doc[a.c_str()].HasMember("AudioComponent"))
						{
							if (doc[a.c_str()]["AudioComponent"].HasMember("AudioContainer"))
							{
								const rapidjson::Value& obj = doc[a.c_str()]["AudioComponent"]["AudioContainer"];
								if (obj.IsArray())
								{
									for (rapidjson::SizeType i = 0; i < obj.Size(); i++)
									{
										_act = static_cast<SoundAction>(obj[i][0].GetInt());
										_name = (obj[i][1].GetString());
										TagAudio(_act, _name);
									}
								}
							}
							return true;
						}

					}

					else
					{
						std::cout << "EntityID " << a <<
							" : cannot find the entity and its AudioComponent." << std::endl;
						return false;
					}

					return false;
				}

				bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
				{
					writer->Key("AudioComponent");

					writer->StartObject();
					writer->String("AudioContainer");
					writer->StartArray();
					for (auto& it : _audioNameCont)
					{
						writer->StartArray();
						writer->SetFormatOptions(rapidjson::kFormatSingleLineArray);
						writer->Int(it.first);
						writer->String(it.second.c_str());
						writer->EndArray();
						writer->SetFormatOptions(rapidjson::kFormatDefault);
					}
					writer->EndArray();

					writer->EndObject();

					return true;
				}
			};

		}
	}

}