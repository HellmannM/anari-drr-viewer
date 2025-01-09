#pragma once

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
//#include <vector>

#include <nlohmann/json.hpp>

#include "anari/anari_cpp/ext/linalg.h"

struct prediction
{
    std::string filename;
    anari::math::float3 eye;
    anari::math::float3 center;
    anari::math::float3 up;

    prediction(std::string file,
                float eye_x,    float eye_y,    float eye_z,
                float center_x, float center_y, float center_z,
                float up_x,     float up_y,     float up_z)
        : filename(file), eye(eye_x, eye_y, eye_z), center(center_x, center_y, center_z), up(up_x, up_y, up_z) {}
};

inline std::ostream& operator<<(std::ostream& os, const anari::math::float3& v)
{
    return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

inline std::ostream& operator<<(std::ostream& os, const prediction& p)
{
    return os << p.filename << "\neye: " << p.eye << "\ncenter: " << p.center << "\nup: " << p.up << "\n";
}


struct prediction_container
{
    std::vector<prediction> predictions;
    float fovx, fovy;

    prediction_container() {}
    prediction_container(std::string json_filename)
    {
        load_json(json_filename);
    }

    prediction& operator [](size_t idx) { return predictions[idx]; }
    prediction operator [](size_t idx) const { return predictions[idx]; }
    auto begin() { return predictions.begin(); }
    const auto begin() const { return predictions.begin(); }
    auto end() { return predictions.end(); }
    const auto end() const { return predictions.end(); }

    bool load_json(std::string json_filename)
    {
        std::ifstream json_file(json_filename);
        if (json_file.fail())
        {
            std::cerr << "ERROR: Could not open json file: " << json_filename << "\n" << std::strerror(errno) << std::endl;
            return false;
        }
        predictions.clear();
        std::cout << "Loading predictions:\n";
        try
        {
            nlohmann::json data = nlohmann::json::parse(json_file);

            fovx = data["sensor"]["fov_x_rad"];
            fovy = data["sensor"]["fov_y_rad"];
            std::cout << "fovx: " << fovx << "\n";
            std::cout << "fovy: " << fovy << "\n";

            for (auto& p : data["predictions"])
            {
                predictions.push_back(prediction(
                    p["file"],
                    p["eye"]["x"], p["eye"]["y"], p["eye"]["z"],
                    p["center"]["x"], p["center"]["y"], p["center"]["z"],
                    p["up"]["x"], p["up"]["y"], p["up"]["z"]
                ));
                std::cout << predictions.back() << "\n";
            }
        } catch (...)
        {
            std::cerr << "ERROR: Could not parse json file.\n" << std::endl;
            return false;
        }
        return true;
    }

    bool load_json(std::string json_filename, float& fovx_, float& fovy_)
    {
        auto retval = load_json(json_filename);
        if (retval)
        {
            fovx_ = fovx;
            fovy_ = fovy;
        }
        return retval;
    }
};
