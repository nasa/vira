#include "vira/materials/mcewen.hpp"

#include "vira/math.hpp"
#include "vira/spectral_data.hpp"

namespace vira::materials {
    template <IsSpectral TSpectral>
    TSpectral McEwen<TSpectral>::evaluateBSDF(const UV& uv, const Normal& N, const vec3<float>& L, const vec3<float>& V, TSpectral albedo)
    {
        (void)uv;

        // Determine if surface is illuminated:
        float cosI = std::max(dot(N, L), 0.f);
        if (cosI == 0) {
            return TSpectral{ 0.f };
        }

        // Pre-compute emission cosine:
        float cosE = std::max(dot(N, V), 0.f);

        // Lambertian component:
        float lambert = cosI;

        // Lommel-Seeliger (Lunar Lambert) component:
        // TODO Validate energy conservation here
        float lommel_seeliger = 2.f * cosI / (cosI + cosE);

        // Compute mixing fraction:
        const float alpha0 = 60; // Degrees
        float alpha = RAD2DEG<float>() * std::acos(dot(L, V)); // Phase angle in degrees
        float beta = std::exp(-alpha / alpha0);

        // Evaluate reflectance:
        return albedo / PI<float>() * (((1 - beta) * lambert) + (beta * lommel_seeliger));
    };
};