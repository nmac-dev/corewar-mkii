/// 'Dear ImGui' requirements and corewar global GUI members
#pragma once

/** IMGUI:REQUIRED: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <iostream>

/// Vendors
#include <glad/glad.h> // before GLFW
#include <GLFW/glfw3.h>
#include <imgui_internal.h>
#include <imgui.h>

/// ImGui Backend
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define GLSL_VERSION "#version 330"

/** COREWAR:GUI:REQUIRED: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <unordered_map>
#include <vector>
#include "corewar.hpp"

namespace Corewar { namespace GUI
{
ImGuiWindowFlags constexpr GLOBAL_WINDOW_FLAGS
( 
    ImGuiWindowFlags_None
    | ImGuiWindowFlags_NoResize
    | ImGuiWindowFlags_NoScrollbar
    | ImGuiWindowFlags_NoScrollWithMouse
    | ImGuiWindowFlags_NoCollapse
    // | ImGuiWindowFlags_NoSavedSettings
);

ImGuiDockNodeFlags constexpr GLOBAL_DOCKSPACE_FLAGS
(
    ImGuiDockNodeFlags_None
    // | ImGuiDockNodeFlags_KeepAliveOnly
    // | ImGuiDockNodeFlags_NoDockingInCentralNode
    | ImGuiDockNodeFlags_PassthruCentralNode
    // | ImGuiDockNodeFlags_NoSplit
    // | ImGuiDockNodeFlags_NoResize
    // | ImGuiDockNodeFlags_AutoHideTabBar
);

int constexpr GLOBAL_DOCKSPACE_FLAGS_PRIVATE
(
    ImGuiDockNodeFlags_NoWindowMenuButton
    | ImGuiDockNodeFlags_NoCloseButton
);

ImGuiConfigFlags constexpr GLOBAL_CONFIG_FLAGS
(
    ImGuiConfigFlags_None
    // | ImGuiConfigFlags_NavEnableKeyboard
    | ImGuiConfigFlags_DockingEnable
    | ImGuiConfigFlags_ViewportsEnable
);

using PlayerColors = std::unordered_map<Player, ImVec4>;

PlayerColors const PLR_COLORS
{
    { Player::NONE, (ImVec4)ImColor::HSV( 0.0f, 0.0f, 0.8f, 0.15f ) },  // Grey
    { Player::P1,   (ImVec4)ImColor::HSV( 1.0f, 0.8f, 0.8f ) },         // Red      (255, 178.5, 204)  [highest]
    { Player::P2,   (ImVec4)ImColor::HSV( 0.6f, 0.8f, 0.8f ) },         // Blue
    { Player::P3,   (ImVec4)ImColor::HSV( 0.3f, 0.8f, 0.8f ) },         // Green
    { Player::P4,   (ImVec4)ImColor::HSV( 0.9f, 0.8f, 0.8f ) },         // Pink
    { Player::P5,   (ImVec4)ImColor::HSV( 0.5f, 0.8f, 0.8f ) },         // Cyan
    { Player::P6,   (ImVec4)ImColor::HSV( 0.1f, 0.8f, 0.8f ) },         // Orange   (25.5, 178.5, 204) [lowest]
    { Player::P7,   (ImVec4)ImColor::HSV( 0.4f, 0.8f, 0.8f ) },         // Teal
    { Player::P8,   (ImVec4)ImColor::HSV( 0.2f, 0.8f, 0.8f ) },         // Yellow
    { Player::P9,   (ImVec4)ImColor::HSV( 0.8f, 0.8f, 0.8f ) },         // Purple
};

/// Adjusts the color using the addition assignment operator
/// @param _color  reference to color to adjust
/// @param _adjust amount to add to the colour values [default]
/// @param _alpha  value to sets _color's alpha [default]
void inline adjust_color(ImVec4 &_color, float _adjust = 0.2f, float _alpha = 1.f)
{
    _color.x += _adjust;
    _color.y += _adjust;
    _color.z += _adjust;
    _color.w  = _alpha;
}

/// Push the style color for ImGUI buttons, dont forget ImGui::PopStyleColor(int)
/// @param _color chosen color to push
/// @param _adjust _color adjustment made before each _style push [default]
int inline push_btn_color(ImVec4 &_color, float _adjust = 0.2f)
{
    ImGui::PushStyleColor(ImGuiCol_Button, _color);

    adjust_color(_color, _adjust);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _color);

    adjust_color(_color, _adjust);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  _color);

    return 3;
}

}}/* ::Corewar::GUI */
