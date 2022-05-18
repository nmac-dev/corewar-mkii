/// 'Dear ImGui' requirements and corewar global GUI members
#pragma once

/** IMGUI:REQUIRED: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <iostream>

/// Vendors
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <imgui.h>

/// GUI Backend (imgui)
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#define GLSL_VERSION "#version 330"

/** COREWAR:GUI:REQUIRED: ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include "corewar.hpp"
#include <unordered_map>
#include <vector>

namespace Corewar { namespace GUI
{
// Absolute window
ImGuiWindowFlags constexpr global_abs_flags ( 
    ImGuiWindowFlags_None
    // absolute
    | ImGuiWindowFlags_NoMove
    | ImGuiWindowFlags_NoResize
    | ImGuiWindowFlags_NoScrollbar
    | ImGuiWindowFlags_NoScrollWithMouse
    | ImGuiWindowFlags_NoCollapse
);
using PlayerColors = std::unordered_map<Player, ImVec4>;   

PlayerColors const plr_colors
{
    { Player::NONE, (ImVec4)ImColor::HSV( 0.0f, 0.0f, 0.3f ) },     // Gray
    { Player::P1,   (ImVec4)ImColor::HSV( 1.0f, 0.7f, 0.8f ) },     // Red      (255, 178.5, 204)
    { Player::P4,   (ImVec4)ImColor::HSV( 0.9f, 0.7f, 0.8f ) },     // Pink
    { Player::P9,   (ImVec4)ImColor::HSV( 0.8f, 0.7f, 0.8f ) },     // Purple
    { Player::P2,   (ImVec4)ImColor::HSV( 0.6f, 0.7f, 0.8f ) },     // L.Blue
    { Player::P5,   (ImVec4)ImColor::HSV( 0.5f, 0.7f, 0.8f ) },     // Cyan
    { Player::P9,   (ImVec4)ImColor::HSV( 0.4f, 0.7f, 0.8f ) },     // Teal
    { Player::P3,   (ImVec4)ImColor::HSV( 0.3f, 0.7f, 0.8f ) },     // Green
    { Player::P9,   (ImVec4)ImColor::HSV( 0.2f, 0.7f, 0.8f ) },     // Yellow
    { Player::P6,   (ImVec4)ImColor::HSV( 0.1f, 0.7f, 0.8f ) },     // Orange   (25.5, 178.5, 204)
};

/// Increases vectors color
void adjust_color(ImVec4 &_imvec4, float _pcent = 0.2f, float _alpha = 1.f)
{
    _imvec4.x += _pcent;
    _imvec4.y += _pcent;
    _imvec4.z += _pcent;
    _imvec4.w  = _alpha;
}

}}/* ::Corewar::GUI */
