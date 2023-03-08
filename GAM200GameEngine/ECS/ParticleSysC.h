/*******************************************************************************
filename    ParticleSysC.h
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu
date        February 20, 2021
brief

Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/

#pragma once

#include "IComponent.h"



namespace Engine
{
	namespace ECS
	{
		namespace Components
		{
			class ParticleSystem : public IComponent, public Serializer::ISerializer
			{
				// For sphere
				double _radius;
				Vec2D _Offset;

				// For a line, 2 points
				Vec2D _Point1OffSet;
				Vec2D _Point2OffSet;

				Vec2D _Velocity, _VelocityVariation;
				Vec4D _ColorBegin, _ColorEnd;
				double _SizeBegin, _SizeEnd, _SizeVariation;
				double _LifeTime;

				// Emiter settings
				double _Duration;
				double _Alpha;
				double _spawnRate;		// How often does it spawn
				int _spawnAmt;		// Each time it spawns, how many does it spawn

				// Texture name and pathname
				std::string TexName;
				std::string TexPath;

			public:				
				ParticleSystem() :
					isActive{ false },
					isFixedDuration{ false },
					_radius{ 1.0f },
					_Offset{0.0f, 0.0f},
					_EmitterMode{EmitterType::Normal},
					_SpawnShape{SourceType::Line},
					_Point1OffSet{ 0.0f, 0.0f }, _Point2OffSet{ 0.0f, 0.0f},
					_Velocity{ 0.0f, 0.0f }, _VelocityVariation{ 0.0f, 0.0f },
					_ColorBegin{ 1.0f, 1.0f, 1.0f, 1.0f }, _ColorEnd{ 0.0f, 0.0f, 0.0f, 1.0f },
					_SizeBegin{ 1.0f }, _SizeEnd{ 0.0f }, _SizeVariation{ 0.0f },
					_LifeTime{ 1.0f },
					_Duration{ 0.0f },
					_Alpha{ 1.0f }, 
					_spawnRate{ 0.0625f },
					_spawnAmt{2}
				{
					TexName = "No Texture";
					TexPath = "No Texture";
				}

				~ParticleSystem() {}

				bool isActive;
				bool isFixedDuration;

				enum SourceType { Line, Circle };
				SourceType _SpawnShape;

				enum EmitterType { Normal, Burst, Suction };
				EmitterType _EmitterMode;

				// Getters
				SourceType GetSourcemode() const	{ return _SpawnShape; }
				EmitterType GetEmittermode() const	{ return _EmitterMode; }

				// Line spawning
				Vec2D Point1OffSet() const			{ return _Point1OffSet; }
				Vec2D Point2OffSet() const			{ return _Point2OffSet;  }

				// Circle spawning
				double GetSphereRadius() const		{ return _radius; }
				Vec2D GetSphereOffset() const		{ return _Offset; }

				Vec2D GetVelocity() const			{ return _Velocity; }
				Vec2D GetVelocityVariation() const	{ return _VelocityVariation; }
				Vec4D GetColorBegin() const			{ return _ColorBegin; }
				Vec4D GetColorEnd() const			{ return _ColorEnd; }
				double GetSizeBegin() const			{ return _SizeBegin; }
				double GetSizeEnd() const			{ return _SizeEnd; }
				double GetSizeVariation() const		{ return _SizeVariation; }
				double GetDuration() const			{ return _Duration; }
				double GetAlpha() const				{ return _Alpha; }
				double GetLifetime() const			{ return _LifeTime; }
				double GetSpawnRate() const			{ return _spawnRate; }
				unsigned GetSpawnAmt() const		{ return _spawnAmt; }
				std::string GetTexPath() const		{ return TexPath; }
				std::string GetTexName() const		{ return TexName; }

				// Setters
				void SetEmitStatus(const bool emit = false)		{ isActive = emit; };
				void SetPoint1OffSet(const Vec2D pos)			{ _Point1OffSet = pos; };
				void SetPoint2OffSet(const Vec2D pos)			{ _Point2OffSet = pos; }
				void SetVelocity(const Vec2D vel)				{ _Velocity = vel; };
				void SetVelocityVariation(const Vec2D velVar)	{ _VelocityVariation = velVar; };
				void SetColorBegin(const Vec4D clrBegin)		{ _ColorBegin = clrBegin; };
				void SetColorEnd(const Vec4D clrEnd)			{ _ColorEnd = clrEnd; };
				void SetSizeBegin(const double sizeBegin)		{ _SizeBegin = sizeBegin; };
				void SetSizeEnd(const double sizeEnd)			{ _SizeEnd = sizeEnd; };
				void SetSizeVariation(const double sizeVar)		{ _SizeVariation = sizeVar; };
				void SetLifeTime(const double lifetime)			{ _LifeTime = lifetime; };
				void SetDuration(const double duration)			{ _Duration = duration; };
				void SetAlpha(const double alpha)				{ _Alpha = alpha; };
				void SetTexPath(const std::string path)			{ TexPath = path; }
				void SetTexName(const std::string name)			{ TexPath = name; }


				virtual std::string GetComponentName()
				{
					std::string className = __FUNCTION__;
					size_t found = className.find_last_of("::") - 1;
					className = className.substr(0, found);
					found = className.find_last_of("::") + 1;
					className = className.substr(found);
					return className;
				}

				virtual void ToEditor();

				ParticleSystem clone() const { return ParticleSystem(*this); }

				bool Deserialize(const rapidjson::Document& doc, int id)
				{
					std::string a = std::to_string(id);
					if (doc.HasMember(a.c_str()))
					{
						if (doc[a.c_str()].HasMember("ParticleSystem"))
						{
							const rapidjson::Value& obj = doc[a.c_str()]["ParticleSystem"];
							ParticleSystem sys;

							_SpawnShape = static_cast<SourceType>(AddType(obj, "Spawn Shape", sys._SpawnShape));
							_EmitterMode = static_cast<EmitterType>(AddType(obj, "Emitter Mode", sys._EmitterMode));

							// Line
							_Point1OffSet = AddType(obj, "Point1 for line", sys._Point1OffSet);
							_Point2OffSet = AddType(obj, "Point2 for line", sys._Point2OffSet);

							// Sphere
							_radius = AddType(obj, "Sphere Radius", sys._radius);
							_Offset = AddType(obj, "Offset Vec", sys._Offset);

							_Velocity = AddType(obj, "Velocity", sys._Velocity);
							_VelocityVariation = AddType(obj, "VelocityVariation", sys._VelocityVariation);
							_ColorBegin = AddType(obj, "ColorBegin", sys._ColorBegin);
							_ColorEnd = AddType(obj, "ColorEnd", sys._ColorEnd);
							_SizeBegin = AddType(obj, "SizeBegin", sys._SizeBegin);
							_SizeEnd = AddType(obj, "SizeEnd", sys._SizeEnd);
							_SizeVariation = AddType(obj, "SizeVariation", sys._SizeVariation);
							_LifeTime = AddType(obj, "LifeTime", sys._LifeTime);
							
							isFixedDuration = AddType(obj, "Fixed duration check", sys.isFixedDuration);
							_Duration = AddType(obj, "Duration", sys._Duration);
							
							_Alpha = AddType(obj, "Alpha", sys._Alpha);
							_spawnRate = AddType(obj, "SpawnRate", sys._spawnRate);
							_spawnAmt = AddType(obj, "SpawnAmount", sys._spawnAmt);

							TexPath = AddType(obj, "TexPath", sys.TexPath);
							TexName = AddType(obj, "TexPath", sys.TexName);

							return true;
						}
						else
						{
							std::cout << "EntityID" << a <<
								" : cannot find the entuty and its ParticleSystem." << std::endl;
							return false;
						}
					}
					return false;
				}

				bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const
				{
					writer->Key("ParticleSystem");
					writer->StartObject();

					writer->String("Spawn Shape");
					writer->Uint(_SpawnShape);

					writer->String("Emitter Mode");
					writer->Uint(_EmitterMode);

					writer->String("Point1 for line");
					SerializeVector(writer, _Point1OffSet);

					writer->String("Point2 for line");
					SerializeVector(writer, _Point2OffSet);

					writer->String("Sphere Radius");
					writer->Double(_radius);

					writer->String("Offset Vec");
					SerializeVector(writer, _Offset);

					writer->String("Velocity");
					SerializeVector(writer, _Velocity);

					writer->String("VelocityVariation");
					SerializeVector(writer, _VelocityVariation);

					writer->String("ColorBegin");
					Serialize4DVector(writer, _ColorBegin);

					writer->String("ColorEnd");
					Serialize4DVector(writer, _ColorEnd);

					writer->String("SizeBegin");
					writer->Double(_SizeBegin);

					writer->String("SizeEnd");
					writer->Double(_SizeEnd);

					writer->String("SizeVariation");
					writer->Double(_SizeVariation);

					writer->String("LifeTime");
					writer->Double(_LifeTime);

					writer->String("Fixed duration check");
					writer->Bool(isFixedDuration);

					writer->String("Duration");
					writer->Double(_Duration);

					writer->String("Alpha");
					writer->Double(_Alpha);

					writer->String("SpawnRate");
					writer->Double(_spawnRate);

					writer->String("SpawnAmount");
					writer->Double(_spawnAmt);

					writer->String("TexPath");
					writer->String(TexPath.c_str());

					writer->EndObject();

					return true;
				}
			};
		}
	}
}
