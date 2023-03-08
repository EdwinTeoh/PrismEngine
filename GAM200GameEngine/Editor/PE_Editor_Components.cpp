/*******************************************************************************
filename    PE_Editor_Components.h
author(s)   Leow Yumi, yumi.leow@digipen.edu
date        November, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "PE_Editor.h"
#include "../Animation/Animation.h"
#include "../ECS/Components.h"
#include "../Resource/AssetManager.h"

constexpr auto ALIGN_OFFSET = 84.0f;

namespace Engine
{
    namespace ECS
    {
        namespace Components
        {
            void ToEditor_ComponentName(IComponent* c)
            {
                ImVec4 newCol(0.3f, 0.4f, 0.6f, 1.0f);
                ImGui::PushStyleColor(ImGuiCol_Button, newCol);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, newCol);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, newCol);
                ImGui::Button(c->GetComponentName().c_str());
                ImGui::PopStyleColor(3);
            }

            void ToEditor_Text(std::string content, bool sameLine = false, float offset = 0.0f)
            {
                ImGui::Text(content.c_str());
                if (sameLine)
                {
                    ImGui::SameLine(offset);
                }
            }

            void ToEditor_DisabledText(std::string content, bool sameLine = false, float offset = 0.0f)
            {
                ImGui::TextDisabled(content.c_str());
                if (sameLine)
                {
                    ImGui::SameLine(offset);
                }
            }

            void ToEditor_ButtonAsText(std::string label, bool sameLine = false, float offset = 0.0f)
            {
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4());
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4());
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4());
                ImGui::Button(label.c_str());
                if (sameLine)
                {
                    ImGui::SameLine(offset);
                }
                ImGui::PopStyleColor(3);
            }

            void ToEditor_DisabledButton(std::string label, float width = 0.0f)
            {
                ImVec4 newCol = ImGui::GetStyleColorVec4(ImGuiCol_Button);
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, newCol);
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, newCol);
                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
                ImGui::Button(label.c_str(), ImVec2(width, 0.0f));
                ImGui::PopStyleVar();
                ImGui::PopStyleColor(2);
            }

            void ToEditor_ReadOnlyVec2(std::string labelX, std::string labelY, float paramX, float paramY)
            {
                // ImGui::Text("X"); ImGui::SameLine();
                ImGui::SetNextItemWidth(60); ToEditor_DisabledButton(std::to_string(paramX).c_str());
                ImGui::SameLine();
                // ImGui::Text("Y"); ImGui::SameLine();
                ImGui::SetNextItemWidth(60); ToEditor_DisabledButton(std::to_string(paramY).c_str());
            }

            void ToEditor_ReadOnlyInputVec2(std::string labelX, std::string labelY, float* paramX, float* paramY)
            {
                ImGuiInputTextFlags inputFieldFlags = ImGuiInputTextFlags_ReadOnly;
                
                ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4());

                // ImGui::Text("X"); ImGui::SameLine();
                ImGui::SetNextItemWidth(60); ImGui::InputFloat(labelX.c_str(), paramX, 0.0f, 0.0f, "%.3f", inputFieldFlags);
                ImGui::SameLine();
                // ImGui::Text("Y"); ImGui::SameLine();
                ImGui::SetNextItemWidth(60); ImGui::InputFloat(labelY.c_str(), paramY, 0.0f, 0.0f, "%.3f", inputFieldFlags);

                ImGui::PopStyleColor();
            }

            void ToEditor_DragFloat(std::string label, float* param)
            {
                //ToEditor_ButtonAsText(" ");
                ImGui::SetNextItemWidth(60);
                //ImGui::SameLine();
                ImGui::DragFloat(label.c_str(), param);
            }

            void ToEditor_DragVec2(std::string labelX, std::string labelY, float* paramX, float* paramY)
            {
                float step = 0.25f;

                // ImGui::Text("X"); ImGui::SameLine();
                ImGui::SetNextItemWidth(60); ImGui::DragFloat(labelX.c_str(), paramX, step);
                ImGui::SameLine();
                // ImGui::Text("Y"); ImGui::SameLine();
                ImGui::SetNextItemWidth(60); ImGui::DragFloat(labelY.c_str(), paramY, step);
            }

            void ToEditor_DragVec4(std::string labelX, std::string labelY, std::string labelZ, std::string labelW,
                float* paramX, float* paramY, float* paramZ, float* paramW)
            {
                float step = 1.0f;

                ImGui::Text("R"); ImGui::SameLine(); ImGui::SetNextItemWidth(60); ImGui::DragFloat(labelX.c_str(), paramX, step);
                ImGui::SameLine();
                ImGui::Text("G"); ImGui::SameLine(); ImGui::SetNextItemWidth(60); ImGui::DragFloat(labelX.c_str(), paramY, step);
                ImGui::SameLine();
                ImGui::Text("B"); ImGui::SameLine(); ImGui::SetNextItemWidth(60); ImGui::DragFloat(labelX.c_str(), paramZ, step);
                ImGui::SameLine();
                ImGui::Text("A"); ImGui::SameLine(); ImGui::SetNextItemWidth(60); ImGui::DragFloat(labelX.c_str(), paramW, step);
            }

            void ToEditor_ColorSlider(const char* name, Vec4D* location)
            {
                float col[4] = { location->_x, location->_y, location->_z, location->_w };

                ImGui::ColorEdit4(name, col);

                *location = Vec4D{ col[0], col[1], col[2], col[3] };
            }

            void ToEditor_SliderInt(const char* label, int* loc, int min, int max)
            {
                ImGui::SliderInt(label, loc, min, max);
            }

            void ToEditor_SliderFloat(const char* label, double* loc, float min, float max)
            {
                float var = static_cast<float>(*loc);
                ImGui::SliderFloat(label, &var, min, max);
                *loc = var;
            }

            void ToEditor_CheckBox(const char* label, bool* var)
            {
                ImGui::Checkbox(label, var);
            }

/******************************************************************************/

            void Attack::ToEditor()
            {
                ToEditor_ComponentName(this);

                ToEditor_ButtonAsText("Damage", true, ALIGN_OFFSET);
                ImGui::DragInt("##attackdmg", &(this->_damage));
            }

            void CircleCollider::ToEditor()
            {
                ToEditor_ComponentName(this);
            }

            void LineCollider::ToEditor()
            {
                ToEditor_ComponentName(this);
            }

            void Patrol::ToEditor()
            {
                ToEditor_ComponentName(this);

                ToEditor_ButtonAsText("Duration", true, ALIGN_OFFSET);
                int newDuration = static_cast<int>(this->_patrolTime);
                ImGui::SetNextItemWidth(80.0f);
                ImGui::InputInt("##pduration", &newDuration);
                newDuration = std::clamp(newDuration, 0, 10);
                this->_patrolTime = static_cast<float>(newDuration);

                ToEditor_ButtonAsText("Speed", true, ALIGN_OFFSET);
                int newSpeed = static_cast<int>(this->_patrolSpeed);
                ImGui::SetNextItemWidth(80.0f);
                ImGui::InputInt("##pspeed", &newSpeed);
                newSpeed = std::clamp(newSpeed, 0, 10);
                this->_patrolSpeed = static_cast<float>(newSpeed);
            }

            void VacuumC::ToEditor()
            {
                ToEditor_ComponentName(this);
            }

            void RectCollider::ToEditor()
            {
                ToEditor_ComponentName(this);
                ImGui::SameLine();
                ImGui::SmallButton("X");
                
                ToEditor_ButtonAsText("Scale", true, ALIGN_OFFSET);
                ToEditor_DragVec2("##rscaleX", "##rscaleY", &(this->_width), &(this->_height));

                ToEditor_ButtonAsText("Offset", true, ALIGN_OFFSET);
                ToEditor_DragVec2("##roffsetX", "##roffsetY", &(this->_offset._x), &(this->_offset._y));
            }

            void RigidBody2D::ToEditor()
            {
                ToEditor_ComponentName(this);

                float vX = this->GetVelocity()._x;
                float vY = this->GetVelocity()._y;

                ToEditor_ButtonAsText("Velocity", true, ALIGN_OFFSET);
                ToEditor_ReadOnlyVec2("##rbvelX", "##rbvelY", vX, vY);

                ToEditor_ButtonAsText("Mass", true, ALIGN_OFFSET);
                ImGui::SetNextItemWidth(60.0f);
                ImGui::DragFloat("##rbmass", &this->_mass);

                ToEditor_ButtonAsText("Gravity", true, ALIGN_OFFSET);
                ImGui::SetNextItemWidth(60.0f);
                ImGui::DragFloat("##rbgravity", &this->_gravityScale);
            }

            void Sprite::ToEditor()
            {
                ToEditor_ComponentName(this);

                ImGui::Checkbox("Renderable", &this->_renderable);

                //ImGui::PushStyleColor(ImGuiCol_Text, ImVec4());
                ToEditor_ButtonAsText("Path", true, ALIGN_OFFSET);
                ToEditor_DisabledButton(this->_texPath, 140.0f);
                if (ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip(this->_texPath.c_str());
                }
                ToEditor_ButtonAsText("Image", true, ALIGN_OFFSET);
                //ToEditor_DisabledButton(this->_texName, 140.0f);
                //ToEditor_ButtonAsText("Sprite", true, ALIGN_OFFSET);
                ImGui::SetNextItemWidth(140.0f);

                if (ImGui::BeginCombo("##sprite", this->_texName.c_str()))
                {
                    for (auto& [str, tex] : AssetManager::TexturesCont)
                    {
                        if (ImGui::Selectable(str.c_str()))
                        {
                            this->_texPath = "Assets/Textures/" + str;
                            this->_texName = str;
                        }
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Waazaa"))
                    {
                        this->_texName = *(const std::string*)payload->Data;
                        this->_texPath = "Assets/Textures/" + this->_texName;
                    }
                    ImGui::EndDragDropTarget();
                }

                //ImVec2 uv{ _u, _v };
                //ToEditor_DragVec2("##U", "##V", &uv.x, &uv.y);
                //_u = uv.x;
                //_v = uv.y;

                ToEditor_ButtonAsText("U", true, ALIGN_OFFSET);
                ImGui::SetNextItemWidth(100);
                ImGui::DragFloat("##U", &_u, 0.01f, 0.0f, 0.0f, "%.8f");
                ToEditor_ButtonAsText("V", true, ALIGN_OFFSET);
                ImGui::SetNextItemWidth(100);
                ImGui::DragFloat("##V", &_v, 0.01f, 0.0f, 0.0f, "%.8f");

                ImGui::Checkbox("Is Repeating", &this->_isRepeating);
                ImGui::Checkbox("Is Multiple", &this->_isMultiple);

                if (this->_isMultiple)
                {
                    ToEditor_ButtonAsText("Animator Index", true);
                    ImGui::InputInt("##ssanimidx", &_animatorIndex);
                }
            }

            void Transform::ToEditor()
            {
                ToEditor_ComponentName(this);

                // Wrap a vector2 to editor
                ToEditor_ButtonAsText("Position", true, ALIGN_OFFSET);;
                ToEditor_DragVec2("##tposX", "##tposY", &(this->_position._x), &(this->_position._y));
                ToEditor_ButtonAsText("Z-Value", true, ALIGN_OFFSET);
                ToEditor_DragFloat("##tposz", &(this->_z));

                ToEditor_ButtonAsText("Scale", true, ALIGN_OFFSET);;
                ToEditor_DragVec2("##tscaleX", "##tscaleY", &(this->_scale._x), &(this->_scale._y));

                ToEditor_ButtonAsText("Rotation", true, ALIGN_OFFSET);;
                ToEditor_DragFloat("##rot", &(this->_rotate));
            }

            void Platform::ToEditor()
            {
                ToEditor_ComponentName(this);

                ToEditor_ButtonAsText("Distance", true, ALIGN_OFFSET);;
                ToEditor_DragVec2("##posX", "##posY", &(this->_direction._x), &(this->_direction._y));

                ToEditor_ButtonAsText("Speed", true, ALIGN_OFFSET);
                ImGui::DragFloat("##speed", &(this->_speed));

                ToEditor_ButtonAsText("Timer", true, ALIGN_OFFSET);
                ImGui::DragFloat("##timer", &(this->_timer));

                ImGui::Checkbox("moveBack", &this->_moveBack);

                ToEditor_ButtonAsText("ButtonID", true, ALIGN_OFFSET);
                int id = static_cast<int>(this->_buttonID);
                ImGui::SetNextItemWidth(80.0f);
                ImGui::InputInt("##id", &id);
                this->_buttonID = static_cast<EntityID>(id);

                ToEditor_ButtonAsText("TrapBar", true, ALIGN_OFFSET);
                int id2 = static_cast<int>(this->_trapBar._x);
                int id3 = static_cast<int>(this->_trapBar._y);
                ImGui::SetNextItemWidth(80.0f);
                ImGui::InputInt("##id2", &id2);
                ImGui::InputInt("##id3", &id3);
                this->_trapBar._x = static_cast<EntityID>(id2);
                this->_trapBar._y = static_cast<EntityID>(id3);
            }
            
            void ParticleSystem::ToEditor()
            {
                ToEditor_ComponentName(this);

                // Things that need to be editable
                /*
                Point1 and Point2 of a line
                Velocity
                Velocity variation
                Colorbegin and colorend  //
                Sizebegin and sizeEnd
                sizevariation
                LifeTime
                Duration
                SpawnRate
                SpawnAmount
                TexturePath
                */

                static int e = this->_EmitterMode;
                static int selected_mode = this->_SpawnShape;
                const char* items[] = { "POINT/LINE", "SPHERE" };
                const char* combo_label = items[selected_mode];
                
                if (ImGui::BeginCombo("Spawn Mode", combo_label))   // Flags disabled
                {
                    for (int n = 0; n < IM_ARRAYSIZE(items); ++n)
                    {
                        const bool is_selected = (selected_mode == n);
                        if (ImGui::Selectable(items[n], is_selected))
                            selected_mode = n;

                        // Set intial focus
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::Button("Enable"))
                    this->isActive = true;
                if (ImGui::Button("Disable"))
                    this->isActive = false;


                // Set line
                if (selected_mode == 0)
                {
                    // Save mode for shape 
                    this->_SpawnShape = this->Line;

                    ToEditor_ButtonAsText("Line P1", true);
                    ToEditor_DragVec2("##1X", "##1Y", &this->_Point1OffSet._x, &this->_Point1OffSet._y);
                    ToEditor_ButtonAsText("Line P2", true);
                    ToEditor_DragVec2("##2X", "##2Y", &this->_Point2OffSet._x, &this->_Point2OffSet._y);
                }
                else if (selected_mode == 1)
                {
                    // Save mode for shape
                    this->_SpawnShape = this->Circle;
                    
                    // Set radius and offset
                    ToEditor_ButtonAsText("Radius", true);
                    ToEditor_SliderFloat("##Rad", &this->_radius, 0.0f, 5.0f);
                    ToEditor_ButtonAsText("Offset", true);
                    ToEditor_DragVec2("##Offset X", "##Offset Y", &this->_Offset._x, &this->_Offset._y);

                    // Set distribution modes
                    ImGui::RadioButton("Normal", &e, 0); ImGui::SameLine();
                    ImGui::RadioButton("Burst", &e, 1); ImGui::SameLine();
                    ImGui::RadioButton("Suction", &e, 2);

                    this->_EmitterMode = static_cast<EmitterType>(e);
                }
                
                
                

                // Set velocity
                ToEditor_ButtonAsText("Velocity", true);
                ToEditor_DragVec2("##tVelX", "##tVelY", &(this->_Velocity._x), &(this->_Velocity._y));

                ToEditor_ButtonAsText("Velocity Variation", true);
                ToEditor_DragVec2("##tX param", "##tY param", &(this->_VelocityVariation._x), &(this->_VelocityVariation._y));

                // Set color
                ToEditor_ColorSlider("Start Color", &this->_ColorBegin);
                ToEditor_ColorSlider("End Color", &this->_ColorEnd);

                // Set sizes
                ToEditor_SliderFloat("Start size", &this->_SizeBegin, 0.0f, 5.0f);
                ToEditor_SliderFloat("End size", &this->_SizeEnd, 0.0f, 5.0f);
                //ToEditor_SliderFloat("Size variation", &this->_SizeVariation,)

                // Set spawn rate
                ToEditor_SliderFloat("Spawn Rate", &this->_spawnRate, 0.0f, 1.0f);

                // Set spawn amount
                ToEditor_SliderInt("Spawn Amt", &this->_spawnAmt, 0, 20);

                // Handling duration
                ToEditor_CheckBox("Using fixed duration", &this->isFixedDuration);
                if (this->isFixedDuration)
                {
                    ToEditor_SliderFloat("Duration", &this->_Duration, 0.0f, 5.0f);
                }

                // Set texture path
                ToEditor_ButtonAsText("Path", true);
                ToEditor_DisabledButton(this->TexPath, 140.0f);
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(this->TexPath.c_str());

                ToEditor_ButtonAsText("Image", true);
                ImGui::SetNextItemWidth(140.0f);

                if (ImGui::BeginCombo("##Pic", this->TexName.c_str()))
                {
                    for (auto& [str, tex] : AssetManager::TexturesCont)
                    {
                        if (ImGui::Selectable(str.c_str()))
                        {
                            this->TexPath = "Assets/Textures/" + str;
                            this->TexName = str;
                        }
                    }
                    ImGui::EndCombo();
                }

                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("buuzaa"))
                    {
                        this->TexName = *(const std::string*)payload->Data;
                        this->TexPath = "Assets/Textures/" + this->TexName;
                    }
                    ImGui::EndDragDropTarget();
                }
            }
        }
    }
}