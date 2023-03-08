/*******************************************************************************
filename    GraphicsSys.cpp
author(s)   Teoh Yan-jie Edwin, t.yanjieedwin@digipen.edu, ---% Code Contribution
            Leow Yumi, 440002919, yumi.leow@digipen.edu, ---% Code Contribution
date        September 16, 2020
brief

Copyright (C) 2020 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*******************************************************************************/
#include "GraphicsSys.h"

#include "../framework.h"
#include "../GAM200GameEngine.h"
#include "../Animation/Animation.h"
#include "../Clock/Clock.h"
#include "../ECS/Components.h"
#include "../Editor/PE_Editor.h"
#include "../Input/Input.h"
#include "../Window/Window.h"
#include "../UI/PE_UI.h"

#include "../Resource/AssetManager.h"

#include "ParticleSystem/Particle.h"
#include "Camera.h"
#include "FontControl.h"
#include "Picker.h"
#include "Renderer.h"

#include <ImGuizmo.h>

namespace Engine
{
    namespace Graphics
    {
        GraphicsSystem* GraphicsSystem::_instance;

        //Temp-------------------------------
        ParticleSystem NewParticleSsytem;
        //------------------------------

        GraphicsSystem* GraphicsSystem::GetInstance()
        {
            if (!_instance)
            {
                _instance = new GraphicsSystem;
            }

            return _instance;
        }

        void GraphicsSystem::Init()
        {
            // In RGBA format
            glClearColor(0.47f, 0.42f, 0.54f, 1.0f);

            // 24-02-2021 This NEEDS to be called at the beginning, not in a hidden render function
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glLineWidth(0.1f);

            // Model and Shader Initialization
            GLModel::Init();
            GLShader::Init();            

            AssetManager::loadTextureFromFilePath("NOT BEING USED");

            Camera::GetInstance()->Init();
        }

        const std::multimap<float, SpriteComponent*> SortMap(std::map<EntityID, SpriteComponent>& mapToSort)
        {
            std::multimap<float, SpriteComponent*> newMultimap;

            for (auto& [id, sprite] : mapToSort)
            {
                newMultimap.insert({ ECS::compManager->GetEntityComponent<TransformComponent>(id)._z, &sprite });
            }

            return newMultimap;
        }

        const std::multimap<float, TransformComponent*> SortMap(std::map<EntityID, TransformComponent>& mapToSort)
        {
            std::multimap<float, TransformComponent*> newMultimap;

            for (auto& [id, transform] : mapToSort)
            {
                newMultimap.insert({ transform._z, &transform });
            }

            return newMultimap;
        }

        bool Optimization(const TransformComponent& trans)
        {
            AABB objAABB{ trans };
            AABB windowAABB;
            windowAABB.min.x = p_Editor->SceneMinX();
            windowAABB.min.y = p_Editor->SceneMinY();
            windowAABB.max.x = p_Editor->SceneMaxX();
            windowAABB.max.y = p_Editor->SceneMaxY();

            

            // Check for collision
            return (objAABB.min.x <= windowAABB.max.x && objAABB.max.x >= windowAABB.min.x) &&
                (objAABB.min.y <= windowAABB.max.y && objAABB.max.y >= windowAABB.min.y);
        }

        void Draw()
        {
            glClear(GL_COLOR_BUFFER_BIT);


            
#pragma region FIRST_FRAMEBUFFER
            glBindFramebuffer(GL_FRAMEBUFFER, PE_Editor::fbo_array[0].first);
            glClear(GL_COLOR_BUFFER_BIT);
            


            p_Editor->scene_camera->ZoomControls();
            glm::mat4 view = p_Editor->scene_camera->GetProjMatrix() * p_Editor->scene_camera->GetViewMatrix();

            GLShader& textureShader = GLShader::shadersCont[GLShader::SHADER_TEXTURE];
            textureShader.Use();

            // Drawing sprites
            if (ECS::compManager->CheckForList<SpriteComponent>())
            {
                std::map<SpriteComponent*, glm::mat4> mapWithTMat;

                for (auto& [id, sprite] : ECS::compManager->GetComponentList<SpriteComponent>().GetList())
                {
                    // All entities that have spritecomponent will have transform component
                    glm::mat4 TransMatrix{ 1.0f };

                    ECS::Components::Transform& Trans = ECS::compManager->GetEntityComponent<TransformComponent>(id);

                    /*if (!Optimization(Trans))
                        continue;*/

                    //std::cout << p_Editor->_gameWindowMousePosition.x << std::endl;

                    TransMatrix = glm::translate(TransMatrix, glm::vec3(Trans._position._x, Trans._position._y, 0.0f));
                    TransMatrix = glm::rotate(TransMatrix, glm::radians(Trans._rotate), glm::vec3(0.0f, 0.0f, 1.0f));
                    TransMatrix = glm::scale(TransMatrix, glm::vec3(Trans._scale._x, Trans._scale._y, 0.0f));

                    mapWithTMat.insert({ &sprite, TransMatrix });
                }

                std::multimap<float, SpriteComponent*> mm = SortMap(ECS::compManager->GetComponentList<SpriteComponent>().GetList());

                //todo: add debug only flag
                textureShader.SetUniform("useTexture", true);

                for (auto& [z, sprite] : mm)
                {
                    //textureShader.SetUniform("model", mapWithTMat.at(sprite));
                    glm::mat4 combined = view * mapWithTMat.at(sprite);
                    textureShader.SetUniform("combined", combined);

                    // Check if renderable
                    if (sprite->_renderable)
                    {
                        glm::vec4 texOffsetScale = glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f };

                        if (sprite->_isMultiple)
                        {
                            using namespace Animation;
                            AnimationClip* clip = nullptr;

                            if (sprite->_animatorIndex > -1 && sprite->_animatorIndex < Animator::_animatorCont.size())
                                clip = Animator::_animatorCont[sprite->_animatorIndex].GetCurrentClip();

                            texOffsetScale = glm::vec4(sprite->_u, sprite->_v, 1.0f, 1.0f);

                            if (clip)
                            {
                                sprite->_u = clip->GetCurrentFrameX();
                                sprite->_v = clip->GetCurrentFrameY();
                                texOffsetScale.z = (float)clip->_numOfSpritesRow;
                                texOffsetScale.w = (float)clip->_numOfSpritesCol;
                            }
                        }

                        Texture* tex = AssetManager::getTexture(sprite->_texName);
                        glBindTextureUnit(1, tex ? tex->_id : 0);

                        if (sprite->_isRepeating)
                        {
                            glBindTexture(GL_TEXTURE_2D, tex ? tex->_id : 0);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                            //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                            glBindTexture(GL_TEXTURE_2D, 0);

                            if (sprite->_u == 0.0f)
                                sprite->_u = 1.0f;
                            if (sprite->_v == 0.0f)
                                sprite->_v = 1.0f;

                            texOffsetScale.z = sprite->_u;
                            texOffsetScale.w = sprite->_v;
                        }

                        textureShader.SetUniform("tex2d", 1);
                        textureShader.SetUniform("texOffsetScale", texOffsetScale);

                        glm::vec4 col{ sprite->_color._x, sprite->_color._y, sprite->_color._z, sprite->_color._w };
                        textureShader.SetUniform("colorModifier", col);

                        GLModel::modelsCont[GLModel::QUAD_FILL].Render();
                    }
                }
            }

            // Drawing colliders
            if (Window::_showDebugDrawing && ECS::compManager->CheckForList<RectColliderComponent>())
            {
                textureShader.SetUniform("useTexture", false);

                for (auto& [id, box] : ECS::compManager->GetComponentList<RectColliderComponent>().GetList())
                {
                    glm::mat4 ColliderMatrix{ 1.0f };
                    ECS::Components::Transform& Trans = ECS::compManager->GetEntityComponent<TransformComponent>(id);
                    ECS::Components::RectCollider& RectInfo = ECS::compManager->GetEntityComponent<RectColliderComponent>(id);

                    glm::vec3 Translation;
                    Translation.x = Trans._position._x + RectInfo.GetOffSet()._x;
                    Translation.y = Trans._position._y + RectInfo.GetOffSet()._y;
                    Translation.z = 0.0f;

                    ColliderMatrix = glm::translate(ColliderMatrix, Translation);
                    ColliderMatrix = glm::rotate(ColliderMatrix, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                    ColliderMatrix = glm::scale(ColliderMatrix, glm::vec3(RectInfo.GetWidth(), RectInfo.GetHeight(), 0.0f));

                    glm::mat4 combined = view * ColliderMatrix;
                    textureShader.SetUniform("combined", combined);

                    GLModel::modelsCont[GLModel::QUAD_LINE].Render();
                }

                // Velocity line for player only
                auto& list = compManager->GetComponentList<RigidBodyComponent>().GetList();
                if (list.find(0) != list.end())
                {
                    glm::mat4 LineMatrix{ 1.0f };

                    Vec2D v = ECS::compManager->GetEntityComponent<RigidBodyComponent>(0).GetVelocity();
                    Vec2D p = ECS::compManager->GetEntityComponent<TransformComponent>(0)._position;

                    LineMatrix = glm::translate(LineMatrix, glm::vec3(p._x, p._y, 0.0f));

                    if (v._x == 0 && v._y == 0) // Not moving
                    {
                        LineMatrix = glm::rotate(LineMatrix, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                        LineMatrix = glm::scale(LineMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
                    }
                    else if (v._y == 0)  // left / right
                    {
                        LineMatrix = glm::rotate(LineMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
                        LineMatrix = glm::scale(LineMatrix, glm::vec3(0.0f, v._x, 0.0f));
                    }
                    else if (v._x == 0) // up and down
                    {
                        LineMatrix = glm::rotate(LineMatrix, 0.0f, glm::vec3(0.0f, 0.0f, 1.0f));
                        LineMatrix = glm::scale(LineMatrix, glm::vec3(0.0f, v._y / 4.0f, 0.0f));
                    }
                    else
                    {
                        // Return value in radians
                        float rotValue = std::atan(v._y / v._x); // Negate due to rotation CCW
                        rotValue = (glm::degrees(rotValue));

                        if (v._x < 0 && v._y < 0)
                            LineMatrix = glm::rotate(LineMatrix, glm::radians(rotValue + 90), glm::vec3(0.0f, 0.0f, 1.0f));
                        else if (v._x > 0 && v._y < 0)
                            LineMatrix = glm::rotate(LineMatrix, glm::radians(rotValue - 90), glm::vec3(0.0f, 0.0f, 1.0f));
                        else if (v._x < 0 && v._y > 0)
                            LineMatrix = glm::rotate(LineMatrix, glm::radians(90 + rotValue), glm::vec3(0.0f, 0.0f, 1.0f));
                        else if (v._x > 0 && v._y > 0)
                            LineMatrix = glm::rotate(LineMatrix, glm::radians(-(90 - rotValue)), glm::vec3(0.0f, 0.0f, 1.0f));

                        if (v._y < 0)
                            LineMatrix = glm::scale(LineMatrix, glm::vec3(v._x, -v._y, 0.0f));
                        else
                            LineMatrix = glm::scale(LineMatrix, glm::vec3(v._x, v._y, 0.0f));
                    }

                    glm::mat4 combined = view * LineMatrix;
                    GLShader::shadersCont[GLShader::SHADER_TEXTURE].SetUniform("combined", combined);

                    GLModel::modelsCont[GLModel::LINE].Render();
                }
            }

            if (ECS::compManager->CheckForList<VacuumComponent>())
            {
                if (!p_Editor->_isPaused)
                {
                    ParticleProps newparticle;
                    NewParticleSsytem.EmitSpawnRate(newparticle, 0.0625f, 2);
                }                           
            }


            // Render particle onto the scene space for the scene frame buffer
            NewParticleSsytem.Render(p_Editor->scene_camera);
            NewParticleSsytem.RenderECS(p_Editor->scene_camera);
            textureShader.Use();

            



            glBindFramebuffer(GL_FRAMEBUFFER, 0);


#pragma endregion FIRST_FRAMEBUFFER

            if(p_Editor->_useFBO)
            {
                glBindFramebuffer(GL_FRAMEBUFFER, PE_Editor::fbo_array[1].first);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            // Camera view and projection matrix
            //Camera::GetInstance()->ZoomControls();
            glm::mat4 ViewProj = Camera::GetInstance()->GetProjMatrix() * Camera::GetInstance()->GetViewMatrix();

            //GLShader& textureShader = GLShader::shadersCont[GLShader::SHADER_TEXTURE];
            //// Use the "Texture" shader
            //textureShader.Use();

            // Drawing sprites
            if (ECS::compManager->CheckForList<SpriteComponent>())
            {
                //if (!p_Editor->_isPaused)
                //{
                //    if(ECS::compManager->CheckForEntity<TransformComponent>(0))
                //    Camera::GetInstance()->Follow(ECS::compManager->GetEntityComponent<TransformComponent>(0));
                //}

                Camera::GetInstance()->InternalUpdate();

                std::map<SpriteComponent*, glm::mat4> mapWithTMat;

                for (auto& [id, sprite] : ECS::compManager->GetComponentList<SpriteComponent>().GetList())
                {
                    // All entities that have spritecomponent will have transform component
                    glm::mat4 TransMatrix{ 1.0f };

                    ECS::Components::Transform& Trans = ECS::compManager->GetEntityComponent<TransformComponent>(id);

                    TransMatrix = glm::translate(TransMatrix, glm::vec3(Trans._position._x, Trans._position._y, 0.0f));
                    TransMatrix = glm::rotate(TransMatrix, glm::radians(Trans._rotate), glm::vec3(0.0f, 0.0f, 1.0f));
                    TransMatrix = glm::scale(TransMatrix, glm::vec3(Trans._scale._x, Trans._scale._y, 0.0f));

                    mapWithTMat.insert({ &sprite, TransMatrix });
                }

                std::multimap<float, SpriteComponent*> mm = SortMap(ECS::compManager->GetComponentList<SpriteComponent>().GetList());

                //todo: add debug only flag
                textureShader.SetUniform("useTexture", true);

                for (auto& [z, sprite] : mm)
                {
                    glm::mat4 combined = ViewProj * mapWithTMat.at(sprite);
                    textureShader.SetUniform("combined", combined);

                    // Check if renderable
                    if (sprite->_renderable)
                    {
                        glm::vec4 texOffsetScale = glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f };

                        if (sprite->_isMultiple)
                        {
                            using namespace Animation;
                            AnimationClip* clip = nullptr;

                            if (sprite->_animatorIndex > -1 && sprite->_animatorIndex < Animator::_animatorCont.size())
                                clip = Animator::_animatorCont[sprite->_animatorIndex].GetCurrentClip();

                            texOffsetScale = glm::vec4(sprite->_u, sprite->_v, 1.0f, 1.0f);

                            if (clip)
                            {
                                sprite->_u = clip->GetCurrentFrameX();
                                sprite->_v = clip->GetCurrentFrameY();
                                texOffsetScale.z = (float)clip->_numOfSpritesRow;
                                texOffsetScale.w = (float)clip->_numOfSpritesCol;
                            }
                        }

                        Texture* tex = AssetManager::getTexture(sprite->_texName);
                        glBindTextureUnit(1, tex ? tex->_id : 0);

                        if (sprite->_isRepeating)
                        {
                            if (sprite->_u == 0.0f)
                                sprite->_u = 1.0f;
                            if (sprite->_v == 0.0f)
                                sprite->_v = 1.0f;

                            texOffsetScale.z = sprite->_u;
                            texOffsetScale.w = sprite->_v;
                        }

                        textureShader.SetUniform("tex2d", 1);
                        textureShader.SetUniform("texOffsetScale", texOffsetScale);

                        glm::vec4 col{ sprite->_color._x, sprite->_color._y, sprite->_color._z, sprite->_color._w };
                        textureShader.SetUniform("colorModifier", col);

                        GLModel::modelsCont[GLModel::QUAD_FILL].Render();
                    }
                }
            }





            if (ECS::compManager->CheckForList<ParticleComponent>())
            {
                for (auto& [id, particle] : ECS::compManager->GetComponentList<ParticleComponent>().GetList())
                {
                    particle.GetDuration();

                    TransformComponent& trans = ECS::compManager->GetEntityComponent<TransformComponent>(id);

                    // Trigger condition will change
                    // Health zone
                    if (particle.isActive)
                    {
                        if (particle.isFixedDuration)
                        {
                            particle.SetDuration(particle.GetDuration() - p_Clock.GetDT(Utility::SystemCode::GRAPHICS));
                            if (particle.GetDuration() > 0.0f)
                                NewParticleSsytem.ECSSpawn(particle, trans);
                        }
                        else
                            NewParticleSsytem.ECSSpawn(particle, trans);
                    }
                    NewParticleSsytem.UpdateECS(p_Clock.GetDT(Utility::SystemCode::GRAPHICS));
                    NewParticleSsytem.RenderECS(Camera::GetInstance());

                    // Update duration
                    /*if (particle.isFixedDuration && particle.isActive)
                    {
                        particle.SetDuration(particle.GetDuration() - p_Clock.GetDT(Utility::SystemCode::GRAPHICS));
                        if (particle.GetDuration() < 0.0f)
                            particle.isActive = false;

                        std::cout << "Run FD" << particle.GetDuration() << std::endl;
                    }*/                
                }

                // Revert back to previous shader
                textureShader.Use();
            }

            




            if (ECS::compManager->CheckForList<VacuumComponent>())
            {
                NewParticleSsytem.Update(p_Clock.GetDT(Utility::SystemCode::GRAPHICS));
                NewParticleSsytem.Render(Camera::GetInstance());
                textureShader.Use();
            }













            /****************************************/
            // UI IMAGE RENDERING
            /****************************************/
            std::vector<UIElement*> uiVector;
            if (Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Menu")
            {
                uiVector = p_UISystem->_canvas->_MainMenuUI;
            }
            else if (Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Intro" || Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Outro")
            {
                auto& img = Scene::SceneManager::sceneManager->GetCurrentSceneName() == "Intro" ? p_UISystem->_introImage : p_UISystem->_outroImage;

                glm::mat4 t_mat{ 1.0f };
                t_mat = glm::translate(t_mat, img->GetRectPosGLM());
                t_mat = glm::scale(t_mat, img->GetRectSizeGLM());
                textureShader.SetUniform("combined", t_mat);

                glm::vec4 col = img->GetColorGLM();
                textureShader.SetUniform("colorModifier", col);

                textureShader.SetUniform("useTexture", true);

                GLuint texID = dynamic_cast<Image*>(img)->GetTextureID() == 0 ?
                    AssetManager::getTexture("default-sprite.png")->_id : dynamic_cast<Image*>(img)->GetTextureID();

                texID = AssetManager::getTexture(img->GetName())->_id;

                glBindTextureUnit(1, texID);
                textureShader.SetUniform("tex2d", 1);

                glm::vec4 texOffsetScale = glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f };
                textureShader.SetUniform("texOffsetScale", texOffsetScale);

                GLModel::modelsCont[GLModel::QUAD_FILL].Render();

                Text* t = dynamic_cast<Text*>(p_UISystem->_promptText);

                glm::vec2 pos{
                    (t->GetRectPosV()._x + 0.5f * p_Camera->GetViewportWidth()),
                    (t->GetRectPosV()._y + 0.5f * p_Camera->GetViewportHeight())
                };

                p_UISystem->_textRenderer.RenderText(t->_text, { pos.x, pos.y }, t->_fontScale);
            }
            else
            {
                if (GameLogic::_gameIsPaused)
                    uiVector = p_UISystem->_canvas->_PauseMenuUI;
                else
                    uiVector = p_UISystem->_canvas->_GameLevelUI;
            }

            for (auto& ui : uiVector)
            {
                if (!ui->_enabled)
                {
                    continue;
                }

                if (ui->GetType() == "Text")
                {
                    Text* t = dynamic_cast<Text*>(ui);

                    glm::vec2 pos{
                        (t->GetRectPosV()._x + 0.5f * p_Camera->GetViewportWidth()),
                        (t->GetRectPosV()._y + 0.5f * p_Camera->GetViewportHeight())
                    };

                    p_UISystem->_textRenderer.RenderText(t->_text, { pos.x, pos.y }, t->_fontScale);

                    continue;
                }

                textureShader.SetUniform("useTexture", false);

                glm::mat4 t_mat{ 1.0f };
                t_mat = glm::translate(t_mat, ui->GetRectPosGLM());
                t_mat = glm::scale(t_mat, ui->GetRectSizeGLM());
                textureShader.SetUniform("combined", t_mat);

                glm::vec4 col = ui->GetColorGLM();

                if (ui->GetType() == "Button")
                {
                    Button* b = dynamic_cast<Button*>(ui);
                    col = b->GetColorGLM(b->_currentMode);
                }

                textureShader.SetUniform("colorModifier", col);

                if (ui->GetType() == "Image")
                {
                    textureShader.SetUniform("useTexture", true);

                    GLuint texID = dynamic_cast<Image*>(ui)->GetTextureID() == 0 ?
                        AssetManager::getTexture("default-sprite.png")->_id : dynamic_cast<Image*>(ui)->GetTextureID();

                    texID = AssetManager::getTexture(ui->GetName())->_id;

                    glBindTextureUnit(1, texID);
                    textureShader.SetUniform("tex2d", 1);

                    glm::vec4 texOffsetScale = glm::vec4{ 0.0f, 0.0f, 1.0f, 1.0f };
                    textureShader.SetUniform("texOffsetScale", texOffsetScale);
                }

                //glm::vec4 lightPosition = glm::inverse(view) * glm::vec4(p_Editor->_gameWindowMousePosition.x, p_Editor->_gameWindowMousePosition.y, 0.0f, 1.0f);
                //textureShader.SetUniform("lightPosition", lightPosition);
                //static float lightSize = 1.0f;
                //ImGui::Begin("Light Source");
                //ImGui::DragFloat("Light Size", &lightSize);
                //ImGui::End();
                //textureShader.SetUniform("lightSize", lightSize);

                GLModel::modelsCont[GLModel::QUAD_FILL].Render();
            }

            textureShader.UnUse();

            p_UISystem->Draw();

            if (p_Editor->_useFBO)
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        void GraphicsSystem::Update()
        {
            Utility::Clock::GetInstance().Start(Utility::SystemCode::GRAPHICS);

            glfwPollEvents();
            
            Draw();
            p_Editor->Draw();

            glfwSwapBuffers(Window::_pWindow);

            Utility::Clock::GetInstance().Update(Utility::SystemCode::GRAPHICS);
        }

        void GraphicsSystem::Exit()
        {
            //fontcontroller.Exit();
            Camera::GetInstance()->Exit();

            delete p_Graphics;
        }
    }
}