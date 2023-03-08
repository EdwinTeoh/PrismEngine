/*******************************************************************************
filename    RapidJsonBase.h
author(s)   Zhang Xinyu, xinyu.z@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#pragma once
#include "document.h"
#include "prettywriter.h"
#include "stringbuffer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "../System/Macro.h"
#include <istreamwrapper.h>

#define SaveDebugInfoIf(exp,file,msg) if (exp){	SaveToTextFile(file, msg);}

namespace Engine
{
	namespace Serializer
	{
		class ISerializer
		{
		public:
			Vec2D DeserializeVector(const rapidjson::Value& val);
			bool SerializeVector(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer, const Vec2D& v) const;
			Vec4D Deserialize4DVector(const rapidjson::Value& val);
			bool Serialize4DVector(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer, const Vec4D& v) const;
			double FloatToDouble(float&&);
			float DoubleToFloat(double&&);

			// AddType to add in the value from a json file
			/*
				obj -> Eg const rapidjson::Value& obj = doc[a.c_str()]["Transform"];
                name -> the name of the component member
				val -> if fail to add, this is the fallback value

				Example of how to call in component(Transform) deserialize:
				const rapidjson::Value& obj = doc[a.c_str()]["Transform"];
				Transform t;
                _edited = AddType(obj, "Edited", t._edited);
			*/
			bool AddType(const rapidjson::Value& obj, std::string name, bool val);
			unsigned AddType(const rapidjson::Value& obj, std::string name, unsigned val);
			int AddType(const rapidjson::Value& obj, std::string name, int val);
			double AddType(const rapidjson::Value& obj, std::string name, double val);
			float AddType(const rapidjson::Value& obj, std::string name, float val);
			Vec2D AddType(const rapidjson::Value& obj, std::string name, Vec2D val);
			Vec4D AddType(const rapidjson::Value& obj, std::string name, Vec4D val);
			std::string AddType(const rapidjson::Value& obj, std::string name, std::string val);
			
			virtual bool InitDocument(const std::string& s, rapidjson::Document& doc);
			virtual bool DeserializeFromFile(const std::string& file, int);
			virtual bool SerializeToFile(const std::string& file, int);
			virtual bool Serialize(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer) const { (void)(writer); return false; };
			virtual std::string Serialize() const;
			virtual bool Deserialize(const rapidjson::Document&, int) = 0;
			// virtual bool Deserialize(const rapidjson::Value& obj) = 0;
			virtual ~ISerializer() = default;
		};

		bool GetEntityFromFile(const std::string& file,rapidjson::Document&);
		void ConfigSerializer(const std::string&, int&, int&);
		void ConfigDeserializer(const std::string file, std::string& windowTitle, int& width, int& height, bool& vsync, bool& editorMode, std::string& sceneFile);
		bool SaveCurrentScene(std::string);
		bool LoadSavedScene(std::string);
		void ReloadScene(std::string fileDirectory);
	}
}
