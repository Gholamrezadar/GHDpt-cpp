#include "utils/rtweekend.h"

#include "utils/color.h"
#include "utils/hittable_list.h"
#include "primitives/sphere.h"
#include "primitives/camera.h"
#include "utils/material.h"
#include "scenes.h"
#include "image.h"

// time
#include <chrono>
#include <time.h>
#include <ctime>
#include <numeric>
#include <algorithm>

#include <iostream>
#include <vector>
#include <string>
#include <execution>
#include "raw_image.h"

using std::cout;
using std::endl;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::chrono::seconds;
using std::chrono::system_clock;

enum class SceneName {
	FLOOR_SPHERE,
	THREE_SPHERES,
	THREE_SPHERES2,
	THREE_SPHERES3,
	FOV,
	RANDOM,
	GHD
};

class Renderer {
public:
	Renderer(int width, int height, int samples_per_pixel, int max_depth) : m_iteration_count(0), m_samples_per_pixel(samples_per_pixel), m_max_depth(max_depth), m_image_width(width), m_image_height(height), m_scene_name(SceneName::FLOOR_SPHERE) {
		srand(time(NULL));

		// // Camera
		// point3 lookfrom(13, 2, 3);
		// point3 lookat(0, 0, 0);
		// vec3 vup(0, 1, 0);
		// double dist_to_focus = 12.0;
		// double aperture = 0.1;
		// double aspect_ratio = static_cast<double>(m_image_width) / static_cast<double>(m_image_height);
		// m_camera = camera(lookfrom, lookat, vup, 19, aspect_ratio, aperture, dist_to_focus);		

		reset();
	}

	int get_width() const { return m_image_width; }
	int get_height() const { return m_image_height; }
	GHDImage& get_image() { return m_image; }
	void set_scene_name(SceneName scene_name) { m_scene_name = scene_name; }
	float get_render_time() const { return m_render_time; }
	void set_iteration_count(int iteration_count) { m_iteration_count = iteration_count; }
	void set_samples_per_pixel(int samples_per_pixel) { m_samples_per_pixel = samples_per_pixel; }
	void set_max_depth(int max_depth) { m_max_depth = max_depth; }
	void set_image_width(int image_width) { m_image_width = image_width; }
	void set_image_height(int image_height) { m_image_height = image_height; }
	void set_camera_aperture(double aperture) { this->aperture = aperture; }
	void set_camera_lookfrom(point3 lookfrom) { this->lookfrom = lookfrom; }
	void set_camera_lookat(point3 lookat) { this->lookat = lookat; }
	void set_camera_vup(vec3 vup) { this->vup = vup; }
	void set_camera_vfov(double vfov) { this->vfov = vfov; }
	void set_camera_dist_to_focus(double dist_to_focus) { this->dist_to_focus = dist_to_focus; }
	int get_current_iteration() { return m_current_iteration; }
	void set_current_iteration(int current_iteration) { m_current_iteration = current_iteration; }
	int get_samples_per_pixel() { return m_samples_per_pixel; }

	void reset() {
		// Set random seed
		srand(time(NULL));

		// Image
		const int image_width = m_image_width;
		const int image_height = m_image_height; 
		const auto aspect_ratio = static_cast<float>(image_width) / static_cast<float>(m_image_height);
		const int samples_per_pixel = m_samples_per_pixel;
		const int max_depth = m_max_depth;
		
		// Camera
		m_camera = camera (lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus);

		// World
		switch (m_scene_name) {
		case SceneName::FLOOR_SPHERE:
			m_world = floor_sphere_scene();
			break;
		case SceneName::THREE_SPHERES:
			m_world = three_spheres_scene();
			break;
		case SceneName::THREE_SPHERES2:
			m_world = three_spheres_scene2();
			break;
		case SceneName::THREE_SPHERES3:
			m_world = three_spheres_scene3();
			break;
		case SceneName::FOV:
			m_world = fov_scene();
			break;
		case SceneName::RANDOM:
			m_world = random_scene();
			break;
		case SceneName::GHD:
			m_world = GHD_scene();
			break;
		default:
			m_world = floor_sphere_scene();
			break;
		}

		// Create an empty image
		m_image = GHDImage(m_image_width, m_image_height);
		m_image.bind_texture();
		m_image_raw = RawImage(m_image_width, m_image_height);

		// reset the timer
		// m_start_time = system_clock::now();
		m_render_time = 0.0f;
		m_start_time = std::chrono::high_resolution_clock::now();

		// Reset the current iteration count
		m_current_iteration = 0;
	}

	void render_row(int row) {
		// Loop over pixels
		for (int i = 0; i < m_image_width; ++i)
		{

			// Screen UV coordinates
			auto u = (i + random_double()) / (m_image_width - 1);
			auto v = (row + random_double()) / (m_image_height - 1);
			ray r = m_camera.get_ray(u, v);

			// Add the color of every sample to current pixels color
			color pixel_color = ray_color(r, m_world, m_max_depth);

			// this function averages the pixel_color based on the number of samples per pixel
			// write_color(m_image, row, i, pixel_color, m_samples_per_pixel);
			color current_color = m_image_raw.get_pixel(row, i);
			m_image_raw.set_pixel(row, i, 
				current_color.x() + pixel_color.x(), 
				current_color.y() + pixel_color.y(), 
				current_color.z() + pixel_color.z()
			);
		}
	}

	void render() {

		// Render
		std::cout << "Rendering iteration " << m_current_iteration << "/" << m_samples_per_pixel << std::endl;

		// auto start_time = std::chrono::high_resolution_clock::now();
		
		// Create a vector of rows in reverse order using iota
		std::vector<int> rows(m_image_height);
		std::iota(rows.rbegin(), rows.rend(), 0);

		// for(int iteration = 0; iteration < m_samples_per_pixel; ++iteration) {
			std::for_each(
				std::execution::par,
				rows.begin(),
				rows.end(), 
				[this](int row) { 
					this->render_row(row); 
				}
			);
		// }

		// convert the raw double image to a uint8 image
		for (int i = 0; i < m_image_width; ++i) {
			for (int j = 0; j < m_image_height; ++j) {
				write_color(m_image, j, i, m_image_raw.get_pixel(j, i), m_current_iteration);
			}
		}

		std::chrono::time_point<std::chrono::high_resolution_clock> end_time = std::chrono::high_resolution_clock::now();
        m_render_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - m_start_time).count();

		// bind the texture to the image
		m_image.bind_texture();
		// std::cout << "Done in " << m_render_time << " seconds" << std::endl;
	}

private:
	int m_iteration_count;
	int m_samples_per_pixel;
	int m_max_depth;
	int m_image_width;
	int m_image_height;
	std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
	float m_render_time;
	int m_current_iteration=0;
	SceneName m_scene_name;
	camera m_camera;
	hittable_list m_world;
	GHDImage m_image;
	RawImage m_image_raw;

	//camera properties
	point3 lookfrom{13, 2, 3};
	point3 lookat{0, 0, 0};
	vec3 vup{0, 1, 0};
	double vfov = 19;
	double dist_to_focus = 12.0;
	double aperture = 0.7;

	// Returns a color for a given ray r
	color ray_color(const ray &r, const hittable &world, int depth)
	{
		hit_record rec;

		// If we've exceeded the ray bounce limit, no more light is gathered.
		if (depth <= 0)
			return color(0, 0, 0);

		if (world.hit(r, 0.001, infinity, rec))
		{
			ray scattered;
			color attenuation;
			if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
				return attenuation * ray_color(scattered, world, depth - 1);
			return color(0, 0, 0);
		}
		vec3 unit_direction = unit_vector(r.direction());
		auto t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
	}

};