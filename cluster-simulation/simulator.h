#pragma once
#define OLC_PGE_APPLICATION

#include "olcPixelGameEngine.h"
#include "vector2d.h"
#include "quantizer.h"
#include "colors.h"
#include "timer.h"
#include <random>


namespace olc
{
    using pge = PixelGameEngine;
}

namespace ntf
{
    namespace cluster
    {
        using int_distribution = std::uniform_int_distribution<int32_t>;

        constexpr const char* APP_NAME = "Cluster Simulator";

        constexpr uint16_t DEFAULT_PLANE_SIZE = 10000;
        constexpr uint16_t DEFAULT_OFFSET = 100;
        constexpr uint16_t DEFAULT_INITIAL_OBSERVATIONS = 20;
        constexpr uint16_t DEFAULT_MAX_OBSERVATIONS = 40000;

        constexpr uint32_t DASHED_LINE_PATTERN = 0xF0F0F0F0;

        constexpr int32_t BASE_GAP = 5;
        constexpr int32_t STRING_HEIGHT = 12;

        class simulator : public olc::pge
        {
            using cluster_quantizers = std::vector<std::shared_ptr<quantizer>>;

        public:
            uint16_t initial_observations = DEFAULT_INITIAL_OBSERVATIONS;
            uint16_t max_observations = DEFAULT_MAX_OBSERVATIONS;
            vu16_2d offset = { DEFAULT_OFFSET, DEFAULT_OFFSET };
            vu16_2d plane_size = { DEFAULT_PLANE_SIZE, DEFAULT_PLANE_SIZE };

            std::vector<obsi> observations = {};
            std::default_random_engine random_engine;

            cluster_quantizers quantizers = {};

        private:
            size_t current_quantizer_index = 0;

            vi2d pan_start_pos;
            vi2d world_offset = { 0, 0 };
            float world_scale = 1.0f;

            microseconds quantization_elapsed_time = microseconds::zero();
            uint32_t quantization_iterations = 0;

        public:
            simulator(const cluster_quantizers& quantizers) : quantizers(quantizers)
            {
                this->init();
            }

            simulator(cluster_quantizers&& quantizers) : quantizers(std::move(quantizers))
            {
                this->init();
            }

            simulator(
                const cluster_quantizers& quantizers,
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
                quantizers(quantizers)
            {
                this->init();
            }

            simulator(
                cluster_quantizers&& quantizers,
                vu16_2d&& plane_size,
                vu16_2d&& offset,
                uint16_t initial_observations,
                uint16_t max_observations
            ) :
                initial_observations(initial_observations),
                max_observations(max_observations),
                offset(std::move(offset)),
                plane_size(std::move(plane_size)),
                observations{},
                quantizers(std::move(quantizers))
            {
                this->init();
            }

        private:
            void init()
            {
                olc::pge::sAppName = APP_NAME;
            }

            olc::Pixel get_cluster_color(size_t index)
            {
                return VISUALLY_DISTINCT_COLORS[index % VISUALLY_DISTINCT_COLORS_AMOUNT];
            }

            vi2d size_vi2d()
            {
                return { static_cast<int32_t>(this->plane_size.x), static_cast<int32_t>(this->plane_size.y) };
            }

            vi2d world_to_screen(const vi2d& position)
            {
                vi2d new_pos = position - this->world_offset;

                return {
                    static_cast<int32_t>(new_pos.x * this->world_scale),
                    static_cast<int32_t>(new_pos.y * this->world_scale),
                };
            }

            vi2d screen_to_world(const vi2d& position)
            {
                vi2d new_pos{
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
                this->seed_random_engine();
                vi2d plane_size = std::move(this->size_vi2d());

                this->observations.clear();

                for (uint16_t i = 0; i < this->initial_observations; i++) {
                    int_distribution x_distr(0, plane_size.x);
                    int_distribution y_distr(0, plane_size.y);

                    this->observations.push_back({ x_distr(this->random_engine), y_distr(this->random_engine) });
                }

                for (uint16_t i = initial_observations; i < this->max_observations; i++) {
                    int_distribution i_distr(0, static_cast<int>(this->observations.size()) - 1);

                    const obsi& random_cell = this->observations[i_distr(this->random_engine)];

                    int_distribution x_offset_distr(this->offset.x * -1, this->offset.x);
                    int_distribution y_offset_distr(this->offset.y * -1, this->offset.y);

                    int32_t x_offset = std::clamp(0, x_offset_distr(this->random_engine), plane_size.x);
                    int32_t y_offset = std::clamp(0, y_offset_distr(this->random_engine), plane_size.y);

                    vi2d offset_pos{ x_offset_distr(this->random_engine), y_offset_distr(this->random_engine) };

                    this->observations.push_back(random_cell + offset_pos);
                }
            }

            void draw_observations()
            {
                for (auto& obs : this->observations) {
                    olc::pge::FillCircle(
                        std::move(this->world_to_screen(obs)),
                        1,
                        this->get_cluster_color(obs.cluster_index)
                    );
                }
            }

            void draw_axis()
            {
                vi2d plane_size(std::move(this->size_vi2d()));

                vi2d x_axis_start(this->world_to_screen({ 0, plane_size.y / 2 }));
                vi2d x_axis_end(this->world_to_screen({ plane_size.x, plane_size.y / 2 }));

                vi2d y_axis_start(this->world_to_screen({ plane_size.x / 2, 0 }));
                vi2d y_axis_end(this->world_to_screen({ plane_size.x / 2, plane_size.y }));

                olc::pge::DrawLine(
                    { 0, x_axis_start.y },
                    { olc::pge::ScreenWidth(), x_axis_end.y },
                    olc::WHITE,
                    DASHED_LINE_PATTERN
                );

                olc::pge::DrawLine(
                    { y_axis_start.x, 0 },
                    { y_axis_end.x, olc::pge::ScreenHeight() },
                    olc::WHITE,
                    DASHED_LINE_PATTERN
                );

                olc::pge::FillCircle(this->world_to_screen(plane_size / 2), 2, olc::MAGENTA);
            }

            void zoom_and_pan()
            {
                vi2d mouse_pos{ olc::pge::GetMouseX(), olc::pge::GetMouseY() };

                if (olc::pge::GetMouse(2).bPressed)
                    this->pan_start_pos = mouse_pos;

                if (olc::pge::GetMouse(2).bHeld)
                {
                    this->world_offset.x -= static_cast<int32_t>((mouse_pos.x - this->pan_start_pos.x) / this->world_scale);
                    this->world_offset.y -= static_cast<int32_t>((mouse_pos.y - this->pan_start_pos.y) / this->world_scale);

                    this->pan_start_pos = mouse_pos;
                }

                vi2d mouse_before_zoom = this->screen_to_world(mouse_pos);

                if (olc::pge::GetMouseWheel() > 0 || olc::pge::GetKey(olc::CTRL).bHeld && olc::pge::GetKey(olc::EQUALS).bHeld)
                    this->world_scale *= 1.1f;

                if (olc::pge::GetMouseWheel() < 0 || olc::pge::GetKey(olc::CTRL).bHeld && olc::pge::GetKey(olc::MINUS).bHeld)
                    this->world_scale *= 0.9f;

                vi2d mouse_after_zoom = this->screen_to_world(mouse_pos);
                this->world_offset += (mouse_before_zoom - mouse_after_zoom);
            }

            void draw_info()
            {
                DrawString({ BASE_GAP, BASE_GAP }, "Scale: " + std::to_string(this->world_scale));
                DrawString(
                    { BASE_GAP, BASE_GAP + STRING_HEIGHT },
                    this->current_quantizer()->name + ", k=" + std::to_string(this->current_quantizer()->quantization_param)
                );

                DrawString(
                    { BASE_GAP, olc::pge::ScreenHeight() - STRING_HEIGHT * 2 - BASE_GAP },
                    "Iterations: " + std::to_string(this->quantization_iterations)
                );

                uint64_t _elapsed_time = quantization_elapsed_time.count();

                std::string elapsed_time_str = std::to_string(_elapsed_time);
                std::string time_unit = "micrs";

                if (_elapsed_time > 1000000)
                {
                    elapsed_time_str = std::to_string(_elapsed_time / 1000000.0f);
                    time_unit = "s";
                }

                else if (_elapsed_time > 10000)
                {
                    elapsed_time_str = std::to_string(_elapsed_time / 1000.0f);
                    time_unit = "ms";
                }

                DrawString(
                    { BASE_GAP, olc::pge::ScreenHeight() - STRING_HEIGHT - BASE_GAP },
                    "Elapsed time: " + elapsed_time_str + time_unit
                );
            }

        public:
            std::shared_ptr<quantizer> current_quantizer()
            {
                return this->quantizers.at(current_quantizer_index);
            }

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
                olc::pge::Clear(olc::BLACK);

                this->zoom_and_pan();
                this->draw_observations();
                this->draw_axis();
                this->draw_info();

                if (olc::pge::GetKey(olc::CTRL).bHeld && olc::pge::GetKey(olc::K).bPressed && this->current_quantizer()->quantization_param < this->current_quantizer()->quantization_param_max)
                    this->current_quantizer()->quantization_param++;

                if (olc::pge::GetKey(olc::CTRL).bHeld && olc::pge::GetKey(olc::J).bPressed && this->current_quantizer()->quantization_param > this->current_quantizer()->quantization_param_min)
                    this->current_quantizer()->quantization_param--;

                if (olc::pge::GetKey(olc::RIGHT).bPressed)
                    this->current_quantizer_index = (this->current_quantizer_index + 1) % this->quantizers.size();

                if (olc::pge::GetKey(olc::LEFT).bPressed)
                {
                    if (this->current_quantizer_index == 0)
                        this->current_quantizer_index = this->quantizers.size() - 1;
                    else
                        this->current_quantizer_index = (this->current_quantizer_index - 1) % this->quantizers.size();
                }

                if (olc::pge::GetKey(olc::S).bPressed)
                {
                    this->current_quantizer()->quantize(
                        this->observations,
                        this->quantization_elapsed_time,
                        this->quantization_iterations
                    );
                }

                if (olc::pge::GetKey(olc::R).bPressed)
                    this->generate_observations();

                return true;
            }
        };
    }
}
