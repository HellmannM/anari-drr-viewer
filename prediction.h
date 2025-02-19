#pragma once

#include <exception>
#include <fstream>
#include <iostream>
#include <string>
//#include <vector>

#include <nlohmann/json.hpp>

#include "anari/anari_cpp/ext/linalg.h"

struct cam
{
    anari::math::float3 eye;
    anari::math::float3 center;
    anari::math::float3 up;
    bool initialized{false};
};

struct prediction
{
    std::string filename;
    cam initial_camera;
    cam refined_camera;

    prediction(std::string file,
                float eye_x,    float eye_y,    float eye_z,
                float center_x, float center_y, float center_z,
                float up_x,     float up_y,     float up_z)
        : filename(file)
    {
        initial_camera.eye = {eye_x, eye_y, eye_z};
        initial_camera.center = {center_x, center_y, center_z};
        initial_camera.up = {up_x, up_y, up_z};
        initial_camera.initialized = true;
    }
};

inline std::ostream& operator<<(std::ostream& os, const anari::math::float3& v)
{
    return os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
}

inline std::ostream& operator<<(std::ostream& os, const prediction& p)
{
    const auto& c1 = p.initial_camera;
    const auto& c2 = p.refined_camera;
    os << p.filename << "\n";
    if (c1.initialized)
        os << "initial camera: eye: " << c1.eye << "\ncenter: " << c1.center << "\nup: " << c1.up << "\n";
    if (c2.initialized)
        os << "refined camera: eye: " << c2.eye << "\ncenter: " << c2.center << "\nup: " << c2.up << "\n";
    return os;
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
        //std::cout << "Loading predictions:\n";
        try
        {
            nlohmann::json data = nlohmann::json::parse(json_file);

            fovx = data["sensor"]["fov_x_rad"];
            fovy = data["sensor"]["fov_y_rad"];
            std::cout << "fovx: " << fovx << "\n";
            std::cout << "fovy: " << fovy << "\n";

            for (auto& p : data["predictions"])
            {
                predictions.emplace_back(
                    p["file"],
                    p["camera"]["eye"]["x"],    p["camera"]["eye"]["y"],    p["camera"]["eye"]["z"],
                    p["camera"]["center"]["x"], p["camera"]["center"]["y"], p["camera"]["center"]["z"],
                    p["camera"]["up"]["x"],     p["camera"]["up"]["y"],     p["camera"]["up"]["z"]
                );
                if (p.contains("refined_camera"))
                {
                    predictions.back().refined_camera.eye = anari::math::float3{
                            p["refined_camera"]["eye"]["x"],
                            p["refined_camera"]["eye"]["y"],
                            p["refined_camera"]["eye"]["z"]};
                    predictions.back().refined_camera.center = anari::math::float3{
                            p["refined_camera"]["center"]["x"],
                            p["refined_camera"]["center"]["y"],
                            p["refined_camera"]["center"]["z"]};
                    predictions.back().refined_camera.up = anari::math::float3{
                            p["refined_camera"]["up"]["x"],
                            p["refined_camera"]["up"]["y"],
                            p["refined_camera"]["up"]["z"]};
                    predictions.back().refined_camera.initialized = true;
                }
                //std::cout << predictions.back() << "\n";
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

    bool export_json(std::string export_path)
    {
        std::ofstream json_file(export_path);
        if (json_file.fail())
        {
            std::cerr << "ERROR: can't write to file: " << export_path << "\n" << std::strerror(errno) << std::endl;
            return false;
        }

        //nlohmann::json predictions_array = nlohmann::json::array();
        nlohmann::json j;
        for (auto& p : predictions)
        {
            nlohmann::json entry;
            entry["file"]        = p.filename;
            if (p.initial_camera.initialized)
            {
                entry["camera"]["eye"]["x"]    = p.initial_camera.eye.x;
                entry["camera"]["eye"]["y"]    = p.initial_camera.eye.y;
                entry["camera"]["eye"]["z"]    = p.initial_camera.eye.z;
                entry["camera"]["center"]["x"] = p.initial_camera.center.x;
                entry["camera"]["center"]["y"] = p.initial_camera.center.y;
                entry["camera"]["center"]["z"] = p.initial_camera.center.z;
                entry["camera"]["up"]["x"]     = p.initial_camera.up.x;
                entry["camera"]["up"]["y"]     = p.initial_camera.up.y;
                entry["camera"]["up"]["z"]     = p.initial_camera.up.z;
            }
            if (p.refined_camera.initialized)
            {
                entry["refined_camera"]["eye"]["x"]    = p.refined_camera.eye.x;
                entry["refined_camera"]["eye"]["y"]    = p.refined_camera.eye.y;
                entry["refined_camera"]["eye"]["z"]    = p.refined_camera.eye.z;
                entry["refined_camera"]["center"]["x"] = p.refined_camera.center.x;
                entry["refined_camera"]["center"]["y"] = p.refined_camera.center.y;
                entry["refined_camera"]["center"]["z"] = p.refined_camera.center.z;
                entry["refined_camera"]["up"]["x"]     = p.refined_camera.up.x;
                entry["refined_camera"]["up"]["y"]     = p.refined_camera.up.y;
                entry["refined_camera"]["up"]["z"]     = p.refined_camera.up.z;
            }
            j["predictions"].push_back(entry);
        }
        j["sensor"]["fov_x_rad"] = fovx;
        j["sensor"]["fov_y_rad"] = fovy;
        json_file << j.dump(4);
        return true;
    }
};
