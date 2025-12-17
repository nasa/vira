#include <iostream>
#include <filesystem>

#include "vira/vira.hpp"

namespace fs = std::filesystem;

int main(int argc, char* argv[])
{
    // ========================== //
    // === Validate the Input === //
    // ========================== //
    // Usage: vira_moon <path-to-qpu-dir-or-glob>
    // If a directory is supplied it will be turned into "<dir>/*".
    std::string qpuPath = "../../../data/qpu/*"; // default
    if (argc > 1 && argv[1] != nullptr) {
        try {
            fs::path p(argv[1]);
            if (fs::exists(p) && fs::is_directory(p)) {
                qpuPath = (p / "*").string();
            } else {
                // If it's not an existing directory, assume the user passed a glob or path
                qpuPath = argv[1];
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Warning: filesystem error while processing argument: " << e.what() << '\n'
                      << "Falling back to default: " << qpuPath << '\n';
        }
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
    scene[navcam].enableParallelInitialization();
    scene[navcam].setFocalLength(10. / 1000.);
    scene[navcam].setFStop(2);
    scene[navcam].setResolution(2000, 2000);
    scene[navcam].setSensorSize(4. / 1000.);
    scene[navcam].setGain(.3);
    scene[navcam].setDefaultPhotositeQuantumEfficiency(0.6);
    scene[navcam].setDefaultPhotositeLinearScaleFactor(55);

    scene[navcam].enableBlenderFrame();

    scene[navcam].setExposureTime(0.04);

    scene[navcam].setLocalPosition(0, 0, -5200000);// , 0, 0);
    scene[navcam].lookAt(vira::vec3<float>{0, 0, 0});


    // ====================== //
    // === Create the Sun === //
    // ====================== //
    auto sun = scene.newSun();
    scene[sun].setLocalPosition(0, -149597870700.f, 0);


    // ===================== //
    // === Load the Moon === //
    // ===================== //
    auto regolith = scene.newLambertianMaterial("regolith");

    auto moon = scene.newGroup();
    scene[moon].addQuipusAsInstances(qpuPath, regolith, true);


    // ============================ //
    // === Configure Pathtracer === //
    // ============================ //
    scene.pathtracer.options.samples = 100;
    scene.pathtracer.options.bounces = 0;
    scene.pathtracer.options.adaptive_sampling = true;

    scene.pathtracer.renderPasses.simulate_lighting = true;


    // ========================= //
    // === Perform Rendering === //
    // ========================= //
    fs::path outputDirectory = "vira_moon_output/";

    scene.updateLevelOfDetail(navcam);
    auto capturedImageRGB = scene.renderRGB(navcam);

    scene.imageInterface.write(outputDirectory / "render.png", vira::images::linearToSRGB(capturedImageRGB));

    scene.imageInterface.write(outputDirectory / "normals.png", vira::images::formatNormals(scene.pathtracer.renderPasses.normal_global));
    scene.imageInterface.write(outputDirectory / "meshIDs.png", vira::images::idToRGB(scene.pathtracer.renderPasses.mesh_id));

    scene.imageInterface.write(outputDirectory / "depth.png", vira::images::colorMap(scene.pathtracer.renderPasses.depth));

    return 0;
}