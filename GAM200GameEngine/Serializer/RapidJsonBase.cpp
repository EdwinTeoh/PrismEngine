/*******************************************************************************
filename    RapidJsonBase.cpp
author(s)   Zhang Xinyu, xinyu.z@digipen.edu
            Leow Yumi, yumi.leow@digipen.edu
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "RapidJsonBase.h"
#include <string>
#include <fstream>
#include "../ECS/Entity.h"
#include "../ECS/Factory.h"
#include "../ECS/ComponentManager.h"
#include "../ECS/Components.h"


namespace Engine
{
    using namespace rapidjson;

    namespace Serializer
    {
        std::string ISerializer::Serialize() const
        {
            //StringBuffer is a simple output stream.
            StringBuffer s;
            PrettyWriter <StringBuffer> writer(s);
            writer.StartObject();
            if (Serialize(&writer))
            {
                writer.EndObject();
                //Use GetString() to obtain the buffer.
                return s.GetString();
            }
            return "";
        }

        //Parse the string into Document
        bool ISerializer::InitDocument(const std::string& s, Document& doc)
        {
            std::string str(s);
            //check the error
            bool IsError = doc.Parse(str.c_str()).HasParseError() ? true : false;
#ifdef _DEBUG
            if (IsError)
            {
                std::string errorStr = "Parse Error occured for string: " + s;
                std::cout << "Parse Error occured for string: " << s << std::endl;
            }
#endif
            return !IsError;
        }



        bool ISerializer::DeserializeFromFile(const std::string& file, int id)
        {
            std::ifstream ifs{ file };
            if (!ifs.is_open())
            {
                std::cout << "Could not open file "
                    << file << "for reading!" << std::endl;
            }

            //Save the original data to DOM
            IStreamWrapper isw(ifs);
            Document old_doc;

            bool IsError = old_doc.ParseStream(isw).HasParseError() ? true : false;
            if (IsError)
            {
                std::cout << "Serialiser: Parse Error!" << std::endl;
                return false;
            }

            return Deserialize(old_doc, id);
        }

        bool ParseFile(std::ifstream& ifs, Document& old_doc, const std::string& file)
        {
            if (!ifs)
            {
                std::cout << "File " << file << "is created for serialiser" << std::endl;
            }
            else if (ifs.peek() == std::ifstream::traits_type::eof())
            {
                std::cout << "Empty file: " << file << std::endl;
            }
            else if (!ifs.is_open())
            {
                std::cout << "Could not open file " << file << " for reading!" << std::endl;
                return false;
            }
            else
            {
                //Save the original data to DOM
                IStreamWrapper isw(ifs);
                bool IsError = old_doc.ParseStream(isw).HasParseError() ? true : false;
                (void)IsError;
#ifdef _DEBUG           
                if (IsError)
                {
                    std::cout << "Parse Error occured for file: " << file << std::endl;
                }
#endif        
            }
            return true;
        }

        std::string GetMemberName( Document& new_doc)
        {
            Value::MemberIterator m;
            std::string  key = "";
            for (m = new_doc.MemberBegin(); m != new_doc.MemberEnd(); ++m)
            {
                key = m->name.GetString();
            }
            return key;

        }

        bool ISerializer::SerializeToFile(const std::string& file, int id)
        {
            //Open the file for reading
            std::ifstream ifs{ file };

            Document old_doc;

            if (!ParseFile(ifs, old_doc, file))
            {
                return false;
            }

            Document::AllocatorType& allocator = old_doc.GetAllocator();
           
            std::ofstream f(file, std::ios_base::trunc);//overwrite the file

            //Serializer the updated entity manager 
            std::string newEM = Serialize();
            const char* s = newEM.c_str();

            //Parserd to new DOM 
            Document new_doc;
            new_doc.Parse(s);

            std::string entityId = std::to_string(id);
            char const* pid = entityId.c_str();
            Value id_(entityId.c_str(), allocator);
         
            std::string memberName = GetMemberName(new_doc);
            Value value_(memberName.c_str(), allocator);

            if (!old_doc.IsObject())
            {
                old_doc.SetObject();
                if (new_doc[value_].IsArray())
                {
                    old_doc.AddMember(value_, new_doc[value_], allocator);
                }
                else
                {
                    old_doc.AddMember(id_, new_doc, allocator);
                }
            }
            // if the member is entity manage which is an array
            else if (new_doc.HasMember(value_) && new_doc[value_].IsArray())
            {
                /*Document::AllocatorType& aa = */new_doc.GetAllocator();
                if (old_doc.HasMember(value_))
                {
                     //Remove original one
                   old_doc.RemoveMember(value_);

                   ////Add in new one
                   old_doc.AddMember(value_, new_doc[value_], allocator);
                }
                else
                {
                    old_doc.AddMember(value_, new_doc[value_], allocator);
                }
               
            }
            else if (old_doc.HasMember(pid))
            {
                /*Document::AllocatorType& aa = */new_doc.GetAllocator();

                //Check if existing transofrm for serialising transoform
                if (new_doc.HasMember(value_) && old_doc[pid].HasMember(value_))
                {
                    old_doc[pid][value_].CopyFrom(new_doc[value_], allocator);
                    std::cout << "EntityID " << entityId << ": " 
                         << ": Replaced the original " << memberName
                        << " in file " << file <<std::endl;
                }
                else
                {
                    if (new_doc.HasMember(value_))
                        old_doc[pid].AddMember(value_, new_doc[value_], allocator);
                    std::cout << "EntityID " << entityId << ": " <<
                        memberName << " successfully serialized to file: " << file << std::endl;
                }

            }
            else
            {
                old_doc.AddMember(id_, new_doc, allocator);
                std::cout << "EntityID " << entityId << ": " << memberName <<
                    ": successfully serialized to file: " << file << std::endl;
            }

            rapidjson::StringBuffer strbuf;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
            writer.SetMaxDecimalPlaces(3);
            old_doc.Accept(writer);

            std::string st = strbuf.GetString();

            f << st;
            f.flush();
            f.close();
            return true;
        }

        /*Matx4 ISerializer::DeserializeMatrix(const rapidjson::Value& val)
        {
            Matx4 mtx;

            if (val.IsArray())
            {
                mtx._m[0][0] = DoubleToFloat(val[0].GetDouble());
                mtx._m[0][1] = DoubleToFloat(val[1].GetDouble());
                mtx._m[0][2] = DoubleToFloat(val[2].GetDouble());
                mtx._m[0][3] = DoubleToFloat(val[3].GetDouble());
                mtx._m[1][0] = DoubleToFloat(val[4].GetDouble());
                mtx._m[1][1] = DoubleToFloat(val[5].GetDouble());
                mtx._m[1][2] = DoubleToFloat(val[6].GetDouble());
                mtx._m[1][3] = DoubleToFloat(val[7].GetDouble());
                mtx._m[2][0] = DoubleToFloat(val[8].GetDouble());
                mtx._m[2][1] = DoubleToFloat(val[9].GetDouble());
                mtx._m[2][2] = DoubleToFloat(val[10].GetDouble());
                mtx._m[2][3] = DoubleToFloat(val[11].GetDouble());
                mtx._m[3][0] = DoubleToFloat(val[12].GetDouble());
                mtx._m[3][1] = DoubleToFloat(val[13].GetDouble());
                mtx._m[3][2] = DoubleToFloat(val[14].GetDouble());
                mtx._m[3][3] = DoubleToFloat(val[15].GetDouble());
            }
            return mtx;
        }

        bool ISerializer::SerializeMatrix(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer, const Matx4& mtx) const
        {

            double val;
            writer->StartArray();

            for (int x = 0; x < 4; ++x)
            {
                for (int y = 0; y < 4; ++y)
                {
                    val = static_cast<double>(mtx._m[x][y]);
                    writer->Double(val);
                }
            }

            writer->EndArray();
            return true;

        }*/

        Vec2D ISerializer::DeserializeVector(const rapidjson::Value& val)
        {
            Vec2D v;

            if (val.IsArray())
            {
                v._x = DoubleToFloat(val[0].GetDouble());
                v._y = DoubleToFloat(val[1].GetDouble());

            }
            else
                std::cout << "Vector: Wrong type, using default" << std::endl;

            return v;
        }

        bool ISerializer::Serialize4DVector(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer, const Vec4D& v) const
        {

            double x, y, z, w;
            writer->StartArray();

            x = static_cast<double>(v._x);
            y = static_cast<double>(v._y);
            z = static_cast<double>(v._z);
            w = static_cast<double>(v._w);

            writer->Double(x);
            writer->Double(y);
            writer->Double(z);
            writer->Double(w);

            writer->EndArray();
            return true;

        }
        Vec4D ISerializer::Deserialize4DVector(const rapidjson::Value& val)
        {
            Vec4D v;

            if (val.IsArray())
            {
                v._x = DoubleToFloat(val[0].GetDouble());
                v._y = DoubleToFloat(val[1].GetDouble());
                v._z = DoubleToFloat(val[2].GetDouble());
                v._w = DoubleToFloat(val[3].GetDouble());
            }
            else
                std::cout << "Vector: Wrong type, using default" << std::endl;

            return v;
        }

        bool ISerializer::SerializeVector(rapidjson::PrettyWriter<rapidjson::StringBuffer>* writer, const Vec2D& v) const
        {

            double x, y;
            writer->SetFormatOptions(kFormatSingleLineArray);
            writer->StartArray();

            x = static_cast<double>(v._x);
            y = static_cast<double>(v._y);

            writer->Double(x);
            writer->Double(y);

            writer->EndArray();
            writer->SetFormatOptions(kFormatDefault);
            return true;

        }

        double ISerializer::FloatToDouble(float&& f)
        {
            double d;
            d = static_cast<double> (f);
            return d;
        }

        float ISerializer::DoubleToFloat(double&& d)
        {
            float f;
            f = static_cast<float>(d);
            return f;
        }

        bool ISerializer::AddType(const rapidjson::Value& obj, std::string name, bool val)
        {
            if (obj.HasMember(name.c_str()))
            {
                if (obj[name.c_str()].IsBool())
                {
                    return obj[name.c_str()].GetBool();
                }
            }
            std::cout << name << ": Wrong Type, using default." << std::endl;
            return val;
        }

        unsigned ISerializer::AddType(const rapidjson::Value& obj, std::string name, unsigned val)
        {
            if (obj.HasMember(name.c_str()))
            {
                if (obj[name.c_str()].IsUint())
                {
                    return obj[name.c_str()].GetUint();
                }
            }
            std::cout << name << ": Wrong Type, using default." << std::endl;
            return val;
        }

        int ISerializer::AddType(const rapidjson::Value& obj, std::string name, int val)
        {
            if (obj.HasMember(name.c_str()))
            {
                if (obj[name.c_str()].IsInt())
                {
                    return obj[name.c_str()].GetInt();
                }
            }
            std::cout << name << ": Wrong Type, using default." << std::endl;
            return val;
        }

        double ISerializer::AddType(const rapidjson::Value& obj, std::string name, double val)
        {
            if (obj.HasMember(name.c_str()))
            {
                if (obj[name.c_str()].IsFloat())
                {
                    return FloatToDouble(obj[name.c_str()].GetFloat());
                }
            }
            std::cout << name << ": Wrong Type, using default." << std::endl;
            return val;
        }

        float ISerializer::AddType(const rapidjson::Value& obj, std::string name, float  val)
        {
            if (obj.HasMember(name.c_str()))
            {
                if (obj[name.c_str()].IsDouble())
                {
                    return DoubleToFloat(obj[name.c_str()].GetDouble());
                }
            }
            std::cout << name << ": Wrong Type, using default." << std::endl;
            return val;
        }

        Vec2D ISerializer::AddType(const rapidjson::Value& obj, std::string name, Vec2D val)
        {
            (void) val;
            return DeserializeVector(obj[name.c_str()]);
        }

        Vec4D ISerializer::AddType(const rapidjson::Value& obj, std::string name, Vec4D val)
        {
            (void)val;
            return Deserialize4DVector(obj[name.c_str()]);
        }

        std::string ISerializer::AddType(const rapidjson::Value& obj, std::string name, std::string val)
        {
            if (obj.HasMember(name.c_str()))
            {
                if (obj[name.c_str()].IsString())
                {
                    return obj[name.c_str()].GetString();
                }
            }
            std::cout << name << ": Wrong Type, using default." << std::endl;
            return val;
        }

        void ConfigSerializer(const std::string& file, int& width, int& height)
        {
            rapidjson::Document doc;
            doc.SetObject();
            rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

            doc.AddMember("Width", width, allocator);
            doc.AddMember("Height", height, allocator);

            rapidjson::StringBuffer strbuf;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(strbuf);
            doc.Accept(writer);

            std::string st = strbuf.GetString();

            std::ofstream f(file, std::ios_base::trunc);

            f << st;
            f.close();
        }

        void ConfigDeserializer(const std::string file, std::string& windowTitle, int& width, int& height, bool& vsync, bool& editorMode, std::string& sceneFile)
        {
            std::ifstream ifs{ file.c_str() };
            if (!ifs.is_open())
            {
                std::cout << "Could not open file for reading!" << std::endl;
            }

            IStreamWrapper isw(ifs);
            Document d;
            d.ParseStream(isw);

            windowTitle = d["Title"].GetString();
            width = d["Width"].GetInt();
            height = d["Height"].GetInt();
            vsync = d["VSync"].GetBool();
            editorMode = d["Editor"].GetBool();
            sceneFile = d["Scene"].GetString();

            ifs.close();
        }

        bool GetEntityFromFile(const std::string& file,rapidjson::Document& d)
        {
            std::ifstream ifs{ file };
            if (!ifs.is_open())
            {
                std::cout << "Could not open file "
                    << file << "for reading!" << std::endl;
            }

            //Save the original data to DOM
            IStreamWrapper isw(ifs);
            bool IsError = d.ParseStream(isw).HasParseError() ? true : false;
            if (IsError)
            {
                std::cout << "Deserializer: Parse error from save file!" << std::endl;
                return false;
            }
            return true;
        }

        bool SaveCurrentScene(std::string SaveSceneIn)
        {
            auto managerMap = ECS::compManager->GetManagerList();
            if (ECS::compManager->CheckForList<SpriteComponent>())
            {
                for (auto& [id, obj] : ECS::compManager->GetComponentList<TransformComponent>().GetList())
                {
                    obj.SerializeToFile(SaveSceneIn, id);
                }
                for (auto& [id, sprite] : ECS::compManager->GetComponentList<SpriteComponent>().GetList())
                {
                    if (ECS::compManager->GetEntityComponent<SpriteComponent>(id)._renderable)
                    {
                        sprite.SerializeToFile(SaveSceneIn, id);
                    }
                }
                for (auto& [id, obj] : ECS::compManager->GetComponentList<RectColliderComponent>().GetList())
                {
                    obj.SerializeToFile(SaveSceneIn, id);
                }

                for (auto& [id, obj] : ECS::compManager->GetComponentList<RigidBodyComponent>().GetList())
                {
                    obj.SerializeToFile(SaveSceneIn, id);
                }

                std::cout << "Scene had been saved" << std::endl;
                return true;
            }
            else
                return false;
           
          
        }
   
        bool LoadSavedScene(std::string LoadSceneFrom)
        {
            rapidjson::Document d;
            if (Serializer::GetEntityFromFile(LoadSceneFrom, d))
            {
                for (rapidjson::Value::ConstMemberIterator itr = d.MemberBegin();
                    itr != d.MemberEnd(); ++itr)
                {
                    std::string id_name = itr->name.GetString();
                    //check if the member is number
                    if (std::all_of(id_name.begin(),
                        id_name.end(), ::isdigit))
                    {
                        Engine::EntityID id = static_cast<Engine::EntityID>(std::stoi(id_name));

                        if (id != 2)
                        {
                            auto rigidBody2d = ECS::Factory::make<RigidBodyComponent>();
                            if (rigidBody2d.DeserializeFromFile(LoadSceneFrom, id))
                            {
                                ECS::compManager->AddComponentToEntity(id, std::move(rigidBody2d));
                            }

                            auto rectCollider = ECS::Factory::make<RectColliderComponent>();
                            if (rectCollider.DeserializeFromFile(LoadSceneFrom, id))
                                ECS::compManager->AddComponentToEntity(id, std::move(rectCollider));
                        }

                        auto sprite = ECS::Factory::make<SpriteComponent>();
                        if (sprite.DeserializeFromFile(LoadSceneFrom, id))
                            ECS::compManager->AddComponentToEntity(id, std::move(sprite));

                        auto transform = ECS::Factory::make<TransformComponent>();
                        if (transform.DeserializeFromFile(LoadSceneFrom, id))
                            ECS::compManager->AddComponentToEntity(id, std::move(transform));
                    }
                    else
                    {
                        ECS::entityManager->DeserializeFromFile(LoadSceneFrom, 0);
                    }
                    
                }
                return true;
            }
            else
                return false;
        }

        void ReloadScene(std::string fileDirectory)
        {
            ECS::compManager->GetComponentList<TransformComponent>().GetList().clear();
            ECS::compManager->GetComponentList<RectColliderComponent>().GetList().clear();
            ECS::compManager->GetComponentList<RigidBodyComponent>().GetList().clear();
            ECS::compManager->GetComponentList<SpriteComponent>().GetList().clear();
            ECS::entityManager->GetEntityList().clear();
            ECS::entityManager->ClearOpenIDs();

            LoadSavedScene(fileDirectory);
        }

        template <typename value>
        void SerializeValue(const std::string& file, value val)
        {
            std::ofstream f(file, std::ios_base::app);
            std::string s = ISerializer::Serialize();
            f << s;
            f.close();
        }
    }
}