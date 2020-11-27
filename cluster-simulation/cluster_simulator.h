#pragma once
#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"
#include <random>


namespace olc { using pge = PixelGameEngine; }

using vu16_2d = olc::v2d_generic<uint16_t>;
using int_distribution = std::uniform_int_distribution<int32_t>;

constexpr const char* APP_NAME = "Cluster Simulator";

constexpr uint16_t DEFAULT_PLANE_SIZE = 10000;
constexpr uint16_t DEFAULT_OFFSET = 100;
constexpr uint16_t DEFAULT_INITIAL_OBSERVATIONS = 20;
constexpr uint16_t DEFAULT_MAX_OBSERVATIONS = 40000;

constexpr uint32_t DASHED_LINE_PATTERN = 0xF0F0F0F0;

class cluster_simulator : public olc::PixelGameEngine
{
public:
    uint16_t initial_observations;
    uint16_t max_observations;
    vu16_2d offset;
    vu16_2d plane_size;

    std::vector<olc::vi2d> observations;
    std::default_random_engine random_engine;

private:
    olc::vi2d world_offset;
    olc::vi2d start_pan_pos;
    float world_scale;

public:
    cluster_simulator() :
        initial_observations(DEFAULT_INITIAL_OBSERVATIONS),
        max_observations(DEFAULT_MAX_OBSERVATIONS),
        offset{ DEFAULT_OFFSET, DEFAULT_OFFSET },
        plane_size{ DEFAULT_PLANE_SIZE, DEFAULT_PLANE_SIZE },
        observations{},
        world_offset{ 0, 0 },
        world_scale(1.0f)
    {
        olc::pge::sAppName = APP_NAME;
        this->seed_random_engine();
    }

    cluster_simulator(
        const vu16_2d& plane_size,
        const vu16_2d& offset,
        uint16_t initial_observations,
        uint16_t max_observations
    ) :
        initial_observations(initial_observations),
        max_observations(max_observations),
        offset(offset),
        plane_size(plane_size),
        observations{},
        world_offset{ 0, 0 },
        world_scale(1.0f)
    {
        olc::pge::sAppName = APP_NAME;
        this->seed_random_engine();
    }
    
    cluster_simulator(
        vu16_2d&& plane_size,
        vu16_2d&& offset,
        uint16_t initial_observations,
        uint16_t max_observations
    ) :
        initial_observations(initial_observations),
        max_observations(max_observations),
        offset(std::move(offset)),
        plane_size(std::move(plane_size)),
        observations{}
    {
        olc::pge::sAppName = APP_NAME;
        this->seed_random_engine();
    }

private:
    olc::vi2d size_vi2d()
    {
        return { static_cast<int32_t>(this->plane_size.x), static_cast<int32_t>(this->plane_size.y) };
    }

    olc::vi2d world_to_screen(const olc::vi2d& position)
    {
        olc::vi2d new_pos = position - this->world_offset;
        
        return {
            static_cast<int32_t>(new_pos.x * this->world_scale),
            static_cast<int32_t>(new_pos.y * this->world_scale),
        };
    }

    olc::vi2d screen_to_world(const olc::vi2d& position)
    {
        olc::vi2d new_pos{
            static_cast<int32_t>(position.x / this->world_scale),
            static_cast<int32_t>(position.y / this->world_scale),
        };

        return new_pos + this->world_offset;
    }
    
    void seed_random_engine()
    {
        this->random_engine.seed(
            static_cast<uint32_t>(std::chrono::high_resolution_clock::now().time_since_epoch().count())
        );
    }

    void generate_observations()
    {
        olc::vi2d plane_size = std::move(this->size_vi2d());

        for (uint16_t i = 0; i < this->initial_observations; i++) {
            int_distribution x_distr(0, plane_size.x);
            int_distribution y_distr(0, plane_size.y);

            this->observations.push_back({ x_distr(this->random_engine), y_distr(this->random_engine) });
        }
        
        for (uint16_t i = initial_observations; i < this->max_observations; i++) {
            int_distribution i_distr(0, static_cast<int>(this->observations.size()) - 1);

            const olc::vi2d& random_cell = this->observations[i_distr(this->random_engine)];

            int_distribution x_offset_distr(this->offset.x * -1, this->offset.x);
            int_distribution y_offset_distr(this->offset.y * -1, this->offset.y);

            int32_t x_offset = std::clamp(0, x_offset_distr(this->random_engine), plane_size.x);
            int32_t y_offset = std::clamp(0, y_offset_distr(this->random_engine), plane_size.y);

            olc::vi2d offset_pos{ x_offset_distr(this->random_engine), y_offset_distr(this->random_engine) };

            this->observations.push_back(random_cell + offset_pos);
        }
    }

    void draw_cells()
    {
        for (auto& cell : this->observations) {
            olc::vi2d position(std::move(this->world_to_screen(cell)));
            olc::pge::FillCircle(position, 1, olc::RED);
        }
    }

    void draw_axis()
    {
        olc::vi2d plane_size(std::move(this->size_vi2d()));

        olc::vi2d x_axis_start(this->world_to_screen({ 0, plane_size.y / 2 }));
        olc::vi2d x_axis_end(this->world_to_screen({ plane_size.x, plane_size.y / 2 }));

        olc::vi2d y_axis_start(this->world_to_screen({ plane_size.x / 2, 0 }));
        olc::vi2d y_axis_end(this->world_to_screen({ plane_size.x / 2, plane_size.y }));

        olc::pge::DrawLine({ 0, x_axis_start.y }, { olc::pge::ScreenWidth(), x_axis_end.y }, olc::WHITE, DASHED_LINE_PATTERN);
        olc::pge::DrawLine({ y_axis_start.x, 0 }, { y_axis_end.x, olc::pge::ScreenHeight() }, olc::WHITE, DASHED_LINE_PATTERN);
        olc::pge::FillCircle(this->world_to_screen(plane_size / 2), 2, olc::MAGENTA);
    }

    void zoom_and_pan()
    {
        olc::vi2d mouse_pos{ olc::pge::GetMouseX(), olc::pge::GetMouseY() };

        if (olc::pge::GetMouse(2).bPressed)
            this->start_pan_pos = mouse_pos;

        if (olc::pge::GetMouse(2).bHeld) {
            this->world_offset.x -= static_cast<int32_t>((mouse_pos.x - this->start_pan_pos.x) / this->world_scale);
            this->world_offset.y -= static_cast<int32_t>((mouse_pos.y - this->start_pan_pos.y) / this->world_scale);

            this->start_pan_pos = mouse_pos;
        }

        olc::vi2d mouse_before_zoom = this->screen_to_world(mouse_pos);

        if (olc::pge::GetMouseWheel() > 0 || olc::pge::GetKey(olc::R).bHeld)
            this->world_scale *= 1.1f;

        if (olc::pge::GetMouseWheel() < 0 || olc::pge::GetKey(olc::Q).bHeld)
            this->world_scale *= 0.9f;

        olc::vi2d mouse_after_zoom = this->screen_to_world(mouse_pos);
        this->world_offset += (mouse_before_zoom - mouse_after_zoom);
    }

public:
    bool OnUserCreate()
    {

        this->world_scale = std::min(
            olc::pge::ScreenWidth() / static_cast<float>(this->plane_size.x),
            olc::pge::ScreenHeight() / static_cast<float>(this->plane_size.y)
        );

        this->world_offset = this->screen_to_world({
            -(olc::pge::ScreenWidth() - static_cast<int32_t>(this->plane_size.x * this->world_scale)) / 2,
            -(olc::pge::ScreenHeight() - static_cast<int32_t>(this->plane_size.y * this->world_scale)) / 2
        });
        
        this->generate_observations();
        return true;
    }

    bool OnUserUpdate(float elapsedTime)
    {
        olc::pge::Clear(olc::DARK_BLUE);

        this->zoom_and_pan();
        this->draw_cells();
        this->draw_axis();

        DrawString({ 0, 0 }, "Scale: " + std::to_string(this->world_scale));
        return true;
    }
};

