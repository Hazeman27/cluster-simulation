#pragma once
#include "cluster.h"
#include "window.h"

namespace ntf::cluster
{
    using int_distribution = std::uniform_int_distribution<int32_t>;

    constexpr const char* APP_NAME = "Cluster Simulator";

    constexpr uint16_t DEFAULT_PLANE_SIZE = 10000;
    constexpr uint16_t DEFAULT_OFFSET = 100;
    constexpr uint16_t DEFAULT_ROOT_OBSERVATIONS_AMOUNT = 20;
    constexpr uint16_t DEFAULT_OBSERVATIONS_AMOUNT = 40000;
    constexpr uint16_t OBSERVATIONS_INC = 1000;
    constexpr int32_t PANNING_SPEED = 1800;

    class simulator : public screen
    {
    private:
        using partitioner_shared_ptr = std::shared_ptr<partitioner<int32_t>>;

        std::default_random_engine random_engine;

        constrained<uint16_t, 1, UINT16_MAX> root_observations_amount = DEFAULT_ROOT_OBSERVATIONS_AMOUNT;
        constrained_uint16_t observations_amount = DEFAULT_OBSERVATIONS_AMOUNT;
        v2d_u16 offset = { DEFAULT_OFFSET, DEFAULT_OFFSET };
        v2d_u16 plane_size = { DEFAULT_PLANE_SIZE, DEFAULT_PLANE_SIZE };

        std::vector<v2d_i32> observations = {};
        std::vector<cluster<int32_t>> clusters = {};
        std::vector<partitioner_shared_ptr> partitioners = {};
            
        partitioning_profile partitioning_profile = {};
        size_t current_partitioner_index = 0;

        v2d_i32 pan_start_pos;
        v2d_i32 world_offset = { 0, 0 };
        float world_scale = 1.0f;
        
    public:
        simulator(const std::vector<partitioner_shared_ptr>& partitioners)
            : screen("Simulation", olc::P, "P"), partitioners(partitioners)
        {}

        simulator(std::vector<partitioner_shared_ptr>&& partitioners)
            : screen("Simulation", olc::P, "P"), partitioners(std::move(partitioners))
        {}

        simulator(
            const std::vector<partitioner_shared_ptr>& partitioners,
            const v2d_u16& plane_size,
            const v2d_u16& offset,
            uint16_t root_observations_amount,
            uint16_t observations_amount
        ) :
            screen("Simulation", olc::P, "P"),
            root_observations_amount(root_observations_amount),
            observations_amount(observations_amount),
            offset(offset),
            plane_size(plane_size),
            observations{},
            partitioners(partitioners)
        {}

        simulator(
            std::vector<partitioner_shared_ptr>&& partitioners,
            v2d_u16&& plane_size,
            v2d_u16&& offset,
            uint16_t root_observations_amount,
            uint16_t observations_amount
        ) :
            screen("Simulation", olc::P, "P"),
            root_observations_amount(root_observations_amount),
            observations_amount(observations_amount),
            offset(std::move(offset)),
            plane_size(std::move(plane_size)),
            observations{},
            partitioners(std::move(partitioners))
        {}

    private:
        olc::Pixel get_cluster_color(size_t index)
        {
            return VISUALLY_DISTINCT_COLORS[index % VISUALLY_DISTINCT_COLORS.size()];
        }

        v2d_i32 size_v2d_i32()
        {
            return { static_cast<int32_t>(this->plane_size.x), static_cast<int32_t>(this->plane_size.y) };
        }

        v2d_i32 world_to_screen(const v2d_i32& position)
        {
            v2d_i32 new_pos = position - this->world_offset;

            return {
                static_cast<int32_t>(new_pos.x * this->world_scale),
                static_cast<int32_t>(new_pos.y * this->world_scale),
            };
        }

        v2d_i32 screen_to_world(const v2d_i32& position)
        {
            v2d_i32 new_pos {
                static_cast<int32_t>(position.x / this->world_scale),
                static_cast<int32_t>(position.y / this->world_scale),
            };

            return new_pos + this->world_offset;
        }

        void generate_observations()
        {
            v2d_i32 plane_size = std::move(this->size_v2d_i32());

            this->clusters.clear();
            this->observations.clear();

            this->clusters.push_back({ {}, plane_size / 2, VISUALLY_DISTINCT_COLORS[0] });

            for (uint16_t i = 0; i < this->root_observations_amount; i++)
            {
                int_distribution x_distr(0, plane_size.x - 1);
                int_distribution y_distr(0, plane_size.y - 1);

                v2d_i32 observation{ x_distr(this->random_engine), y_distr(this->random_engine) };

                this->observations.push_back(observation);
                this->clusters[0].observations.push_back(std::make_shared<v2d_i32>(observation));
            }

            for (uint16_t i = root_observations_amount; i < this->observations_amount; i++)
            {
                int_distribution i_distr(0, static_cast<int>(this->observations.size()) - 1);

                auto& random_cell = this->observations[i_distr(this->random_engine)];

                int_distribution x_offset_distr(this->offset.x * -1, this->offset.x);
                int_distribution y_offset_distr(this->offset.y * -1, this->offset.y);

                v2d_i32 offset_pos{ x_offset_distr(this->random_engine), y_offset_distr(this->random_engine) };
                v2d_i32 observation{ random_cell + offset_pos };

                this->observations.push_back(observation);
                this->clusters[0].observations.push_back(std::make_shared<v2d_i32>(observation));
            }
        }

        void draw_observations()
        {
            for (auto& cluster : this->clusters)
            {
                for (auto& observation : cluster.observations)
                {
                    auto position = std::move(this->world_to_screen(*observation));
                    this->window->FillCircle(position, 1, cluster.color);
                }

                auto mean_pos = std::move(this->world_to_screen(cluster.mean));
                    
                this->window->FillCircle(mean_pos, 3, olc::BLACK);
                this->window->DrawCircle(mean_pos, 2, olc::YELLOW);
            }
        }

        void draw_axis()
        {
            v2d_i32 plane_size(std::move(this->size_v2d_i32()));

            v2d_i32 x_axis_start(std::move(this->world_to_screen({ 0, plane_size.y / 2 })));
            v2d_i32 x_axis_end(std::move(this->world_to_screen({ plane_size.x, plane_size.y / 2 })));

            v2d_i32 y_axis_start(std::move(this->world_to_screen({ plane_size.x / 2, 0 })));
            v2d_i32 y_axis_end(std::move(this->world_to_screen({ plane_size.x / 2, plane_size.y })));

            this->window->DrawLine(
                { 0, x_axis_start.y },
                { this->window->ScreenWidth(), x_axis_end.y },
                olc::WHITE,
                DASHED_LINE_PATTERN
            );

            this->window->DrawLine(
                { y_axis_start.x, 0 },
                { y_axis_end.x, this->window->ScreenHeight() },
                olc::WHITE,
                DASHED_LINE_PATTERN
            );

            this->window->FillCircle(std::move(this->world_to_screen(plane_size / 2)), 2, olc::MAGENTA);
        }

        void draw_info()
        {
            this->window->draw_text_box(
                SCREEN_POSITION,
                {
                    "Scale: " + std::to_string(this->world_scale),
                    "Root observations: " + std::to_string(this->root_observations_amount),
                    "Observations: " + std::to_string(this->observations_amount),
                    this->current_partitioner()->name,
                    this->current_partitioner()->param_name + ": " + std::to_string(this->current_partitioner()->param),
                }
            );

            this->window->DrawString(
                { BASE_GAP, this->window->ScreenHeight() - STRING_HEIGHT * 3 - BASE_GAP },
                "Clusters: " + std::to_string(this->clusters.size())
            );

            this->window->DrawString(
                { BASE_GAP, this->window->ScreenHeight() - STRING_HEIGHT * 2 - BASE_GAP },
                "Iterations: " + std::to_string(this->partitioning_profile.iterations)
            );

            uint64_t elapsed_time = this->partitioning_profile.elapsed_time.count();

            std::string elapsed_time_str = std::to_string(elapsed_time);
            std::string time_unit = "micrs";

            if (elapsed_time > 1000000)
            {
                elapsed_time_str = std::to_string(elapsed_time / 1000000.0f);
                time_unit = "s";
            }

            else if (elapsed_time > 10000)
            {
                elapsed_time_str = std::to_string(elapsed_time / 1000.0f);
                time_unit = "ms";
            }

            this->window->DrawString(
                { BASE_GAP, this->window->ScreenHeight() - STRING_HEIGHT - BASE_GAP },
                "Elapsed time: " + elapsed_time_str + time_unit
            );
        }

        void zoom_and_pan(float elapsed_time)
        {
            v2d_i32 mouse_pos{ this->window->GetMouseX(), this->window->GetMouseY() };
            int32_t panning_offset = static_cast<int32_t>(PANNING_SPEED * elapsed_time);

            if (this->window->GetMouse(2).bPressed || this->window->GetMouse(0).bPressed)
                this->pan_start_pos = mouse_pos;

            if (this->window->GetMouse(2).bHeld || this->window->GetMouse(0).bHeld)
            {
                this->world_offset.x -= static_cast<int32_t>((mouse_pos.x - this->pan_start_pos.x) / this->world_scale);
                this->world_offset.y -= static_cast<int32_t>((mouse_pos.y - this->pan_start_pos.y) / this->world_scale);

                this->pan_start_pos = mouse_pos;
            }

            if (this->window->GetKey(olc::LEFT).bHeld)
                this->world_offset.x -= panning_offset;

            if (this->window->GetKey(olc::UP).bHeld)
                this->world_offset.y -= panning_offset;

            if (this->window->GetKey(olc::RIGHT).bHeld)
                this->world_offset.x += panning_offset;

            if (this->window->GetKey(olc::DOWN).bHeld)
                this->world_offset.y += panning_offset;

            v2d_i32 mouse_before_zoom = this->screen_to_world(mouse_pos);

            if (this->window->GetMouseWheel() > 0 || this->window->GetKey(olc::CTRL).bHeld && this->window->GetKey(olc::E).bHeld)
                this->world_scale *= 1.1f;

            if (this->window->GetMouseWheel() < 0 || this->window->GetKey(olc::CTRL).bHeld && this->window->GetKey(olc::Q).bHeld)
                this->world_scale *= 0.9f;

            v2d_i32 mouse_after_zoom = this->screen_to_world(mouse_pos);
            this->world_offset += (mouse_before_zoom - mouse_after_zoom);
        }

    public:
        partitioner_shared_ptr current_partitioner()
        {
            return this->partitioners.at(current_partitioner_index);
        }

        bool on_create(std::shared_ptr<ntf::window> window) override
        {
            this->window = window;
            this->window->seed_default_random_engine(this->random_engine);

            this->world_scale = std::min(
                this->window->ScreenWidth() / static_cast<float>(this->plane_size.x),
                this->window->ScreenHeight() / static_cast<float>(this->plane_size.y)
            );

            this->world_offset = this->screen_to_world({
                -(this->window->ScreenWidth() - static_cast<int32_t>(this->plane_size.x * this->world_scale)) / 2,
                -(this->window->ScreenHeight() - static_cast<int32_t>(this->plane_size.y * this->world_scale)) / 2
            });

            this->generate_observations();
            return true;
        }

        bool draw_self(float elapsed_time)
        {
            if (this->window->GetKey(olc::CTRL).bHeld && this->window->GetKey(olc::SHIFT).bHeld && this->window->GetKey(olc::EQUALS).bPressed)
            {
                this->root_observations_amount++;
                this->generate_observations();
            }

            else if (this->window->GetKey(olc::CTRL).bHeld && this->window->GetKey(olc::SHIFT).bHeld && this->window->GetKey(olc::MINUS).bPressed)
            {
                this->root_observations_amount--;
                this->generate_observations();
            }

            else if (this->window->GetKey(olc::CTRL).bHeld && this->window->GetKey(olc::EQUALS).bPressed)
            {
                this->observations_amount += OBSERVATIONS_INC;
                this->generate_observations();
            }

            else if (this->window->GetKey(olc::CTRL).bHeld && this->window->GetKey(olc::MINUS).bPressed)
            {
                this->observations_amount -= OBSERVATIONS_INC;
                this->generate_observations();
            }

            else if (this->window->GetKey(olc::CTRL).bHeld && this->window->GetKey(olc::K).bPressed)
                this->current_partitioner()->param++;

            else if (this->window->GetKey(olc::CTRL).bHeld && this->window->GetKey(olc::J).bPressed)
                this->current_partitioner()->param--;

            else if (this->window->GetKey(olc::SHIFT).bHeld && this->window->GetKey(olc::TAB).bPressed)
            {
                if (this->current_partitioner_index == 0)
                    this->current_partitioner_index = this->partitioners.size() - 1;
                else
                    this->current_partitioner_index = (this->current_partitioner_index - 1) % this->partitioners.size();
            }

            else if (this->window->GetKey(olc::TAB).bPressed)
                this->current_partitioner_index = (this->current_partitioner_index + 1) % this->partitioners.size();

            else if (this->window->GetKey(olc::S).bPressed)
            {
                this->clusters = std::move(this->current_partitioner()->partition(this->observations, this->partitioning_profile));
            }

            else if (this->window->GetKey(olc::R).bPressed)
                this->generate_observations();

            this->zoom_and_pan(elapsed_time);
            this->draw_observations();
            this->draw_axis();
            this->draw_info();

            return true;
        }
    };
}
