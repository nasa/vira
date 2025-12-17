#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>

#include "vira/vira.hpp"

namespace fs = std::filesystem;

struct PoseSample
{
    std::string timestamp;
    vira::vec3<float> r_MC_C;
    vira::mat3<float> R_MC;
    vira::vec3<float> svec_M;
};

std::vector<PoseSample> loadPoseSamples(const fs::path& csvPath)
{
    std::vector<PoseSample> samples;

    std::ifstream file(csvPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open pose CSV at " << csvPath << '\n';
        return samples;
    }

    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "Pose CSV is empty: " << csvPath << '\n';
        return samples;
    }

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string timestampField;
        if (!std::getline(ss, timestampField, ',')) {
            continue;
        }

        std::vector<double> values;
        std::string valueStr;
        bool parseError = false;
        while (std::getline(ss, valueStr, ',')) {
            try {
                values.push_back(std::stod(valueStr));
            } catch (const std::exception& e) {
                std::cerr << "Skipping pose entry due to parse error: " << e.what() << '\n';
                parseError = true;
                break;
            }
        }

        if (parseError || values.size() != 15) {
            std::cerr << "Skipping pose entry due to unexpected field count: " << line << '\n';
            continue;
        }

        PoseSample sample;
        sample.timestamp = timestampField;
        sample.r_MC_C = vira::vec3<float>(
            static_cast<float>(values[0] * 1000.0),
            static_cast<float>(values[1] * 1000.0),
            static_cast<float>(values[2] * 1000.0));

        vira::mat3<float> R_MC(
            static_cast<float>(values[3]),
            static_cast<float>(values[4]),
            static_cast<float>(values[5]),
            static_cast<float>(values[6]),
            static_cast<float>(values[7]),
            static_cast<float>(values[8]),
            static_cast<float>(values[9]),
            static_cast<float>(values[10]),
            static_cast<float>(values[11]));
        // sample.R_MC = glm::transpose(R_CM);
        sample.R_MC = R_MC;

        sample.svec_M = vira::vec3<float>(
            static_cast<float>(values[12]),
            static_cast<float>(values[13]),
            static_cast<float>(values[14]));

        samples.emplace_back(std::move(sample));
    }

    if (samples.empty()) {
        std::cerr << "No pose rows were parsed from " << csvPath << '\n';
    }

    return samples;
}

int main(int argc, char* argv[])
{
    // ========================== //
    // === Validate the Input === //
    // ========================== //
    // Usage: vira_moon [path-to-qpu-dir-or-glob] [pose-csv-path]
    // If a directory is supplied it will be turned into "<dir>/*".
    std::string qpuPathPattern = "../../../data/qpu/*"; // default
    fs::path poseCsvPath = "../data/random_poses.csv";
    if (argc > 1 && argv[1] != nullptr) {
        try {
            fs::path p(argv[1]);
            if (fs::exists(p) && fs::is_directory(p)) {
                qpuPathPattern = (p / "*").string();
            } else {
                // If it's not an existing directory, assume the user passed a glob or path
                qpuPathPattern = argv[1];
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Warning: filesystem error while processing argument: " << e.what() << '\n'
                      << "Falling back to default: " << qpuPathPattern << '\n';
        }
    }

    // Path to CSV file with pose information.
    if (argc > 2 && argv[2] != nullptr) {
        poseCsvPath = argv[2];
    }


    vira::enablePrintStatus();

    // ======================== //
    // === Create the Scene === //
    // ======================== //
    vira::Scene<vira::Visible_8bin, float, float> scene;


    // ============================ //
    // === Configure the Camera === //
    // ============================ //
    auto navcam = scene.newCamera();

    // Set camera intrinsics
    scene[navcam].enableParallelInitialization();
    scene[navcam].setFocalLength(10. / 1000.);  // Blue Origin camera
    scene[navcam].setFStop(2);
    scene[navcam].setResolution(4096 / 2, 3072 / 2);
    auto pixelPitch = 2.8f / 1000000.f * 2;  // Blue Origin camera
    scene[navcam].setSensorSize(pixelPitch * 4096 / 2, pixelPitch * 3072 / 2);
    scene[navcam].setGain(.3);
    scene[navcam].setDefaultPhotositeQuantumEfficiency(0.6);
    scene[navcam].setDefaultPhotositeLinearScaleFactor(55);
    scene[navcam].setExposureTime(1.f / 1000.f / 10.f);

    // ====================== //
    // === Create the Sun === //
    // ====================== //
    auto sun = scene.newSun();
    float distanceEarthToSun = 149597870700.f; // meters

    // ===================== //
    // === Load the Moon === //
    // ===================== //
    // auto regolith = scene.newLambertianMaterial("regolith");
    auto regolith = scene.newMcEwenMaterial("regolith");

    auto moon = scene.newGroup();
    scene[moon].addQuipusAsInstances(qpuPathPattern, regolith, true);


    // ============================ //
    // === Configure Pathtracer === //
    // ============================ //
    scene.pathtracer.options.samples = 200;
    scene.pathtracer.options.bounces = 0;
    scene.pathtracer.options.adaptive_sampling = true;

    scene.pathtracer.renderPasses.simulate_lighting = true;


    // ========================= //
    // === Perform Rendering === //
    // ========================= //
    auto poseSamples = loadPoseSamples(poseCsvPath);
    if (poseSamples.empty()) {
        return 1;
    }

    fs::path outputDirectory = "vira_moon_output/";

    for (std::size_t i = 0; i < poseSamples.size(); ++i) {
        const auto& pose = poseSamples[i];

        // Set camera orientation.
        scene[navcam].setLocalRotation(pose.R_MC);
        auto globalRotation = scene[navcam].getGlobalRotation().getMatrix();
        std::cout << globalRotation[0][0] << ", " << globalRotation[1][0] << ", " << globalRotation[2][0] << "\n";
        std::cout << globalRotation[0][1] << ", " << globalRotation[1][1] << ", " << globalRotation[2][1] << "\n";
        std::cout << globalRotation[0][2] << ", " << globalRotation[1][2] << ", " << globalRotation[2][2] << "\n";

        // Set camera position.
        vira::vec3<float> r_CM_M = -1.f * globalRotation * pose.r_MC_C;
        std::cout << r_CM_M[0] << ", " << r_CM_M[1] << ", " << r_CM_M[2] << "\n";
        scene[navcam].setLocalPosition(r_CM_M);
        
        // Set sun position.
        scene[sun].setLocalPosition(pose.svec_M * distanceEarthToSun);

        // Render!
        scene.updateLevelOfDetail(navcam);
        auto capturedImageRGB = scene.render(navcam);
        auto outputName = pose.timestamp + "_nav_cam.png";
        // scene.imageInterface.write(outputDirectory / outputName, vira::images::linearToSRGB(capturedImageRGB));
        scene.imageInterface.write(outputDirectory / outputName, capturedImageRGB);
    }

    return 0;
}
