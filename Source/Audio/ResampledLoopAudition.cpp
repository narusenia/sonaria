#include "ResampledLoopAudition.h"
#include "Synthesis/PsolaSynthesizer.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace
{
constexpr float kAuditionGain = 0.5011872336f; // -6 dB
constexpr double kFadeSeconds = 0.01;
}

ResampledLoopAudition::ResampledLoopAudition(ReadyCallback callback)
    : onReady(std::move(callback))
{
  // Start the worker only after every member it may access has been
  // constructed. Starting it from the member-initializer list allowed the
  // thread to enter workerLoop() before mutex/condition were initialized.
  worker = std::thread(&ResampledLoopAudition::workerLoop, this);
}

ResampledLoopAudition::~ResampledLoopAudition()
{
  {
    const std::lock_guard<std::mutex> lock(mutex);
    shuttingDown = true;
    hasPendingRequest = false;
    ++requestId;
  }
  condition.notify_one();
  if (worker.joinable())
    worker.join();
}

void ResampledLoopAudition::request(DspSynthesisRequest request)
{
  if (request.source.empty() || request.numOutputFrames <= 0)
    return;

  {
    const std::lock_guard<std::mutex> lock(mutex);
    pendingRequest = std::move(request);
    hasPendingRequest = true;
    ++requestId;
  }
  condition.notify_one();
}

void ResampledLoopAudition::cancel()
{
  const std::lock_guard<std::mutex> lock(mutex);
  hasPendingRequest = false;
  ++requestId;
}

void ResampledLoopAudition::workerLoop()
{
  for (;;)
  {
    DspSynthesisRequest request;
    std::uint64_t id = 0;
    {
      std::unique_lock<std::mutex> lock(mutex);
      condition.wait(lock, [this] { return shuttingDown || hasPendingRequest; });
      if (shuttingDown)
        return;

      request = std::move(pendingRequest);
      id = requestId;
      hasPendingRequest = false;
    }

    auto result = render(request);
    {
      const std::lock_guard<std::mutex> lock(mutex);
      if (shuttingDown)
        return;
    }

    auto callback = onReady;
    juce::MessageManager::callAsync(
        [callback = std::move(callback), result = std::move(result)]() mutable
        {
          if (callback)
            callback(std::move(result));
        });
  }
}

juce::AudioBuffer<float>
ResampledLoopAudition::render(const DspSynthesisRequest &request)
{
  const auto rendered = PsolaSynthesizer::render(request);
  const int numSamples = static_cast<int>(rendered.size());
  if (numSamples < 2)
    return {};

  juce::AudioBuffer<float> result(1, numSamples);
  float peak = 0.0f;
  for (int i = 0; i < numSamples; ++i)
  {
    if (!std::isfinite(rendered[static_cast<size_t>(i)]))
      return {};
    result.setSample(0, i, rendered[static_cast<size_t>(i)]);
    peak = std::max(peak, std::abs(rendered[static_cast<size_t>(i)]));
  }
  if (peak < 1.0e-7f)
    return {};

  // Audition is intentionally quieter than program playback.
  result.applyGain(kAuditionGain);
  // Fade both ends so the whole-note loop restarts without a click.
  const int fade = std::min(numSamples / 2,
      static_cast<int>(kFadeSeconds * std::max(1, request.sampleRate)));
  result.applyGainRamp(0, 0, fade, 0.0f, 1.0f);
  result.applyGainRamp(0, numSamples - fade, fade, 1.0f, 0.0f);
  return result;
}
