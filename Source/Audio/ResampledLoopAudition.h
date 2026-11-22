#pragma once

#include "../JuceHeader.h"
#include "Synthesis/DspSynthesisRequest.h"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

/**
 * Builds loopable note previews: the note's original audio, pitch-shifted
 * with PSOLA so formants stay put, with short fades so the loop seam is quiet.
 *
 * Requests are coalesced so dragging can update the requested pitch at UI
 * rate without ever rendering on the message or audio thread.
 */
class ResampledLoopAudition
{
public:
  using ReadyCallback = std::function<void(juce::AudioBuffer<float>)>;

  explicit ResampledLoopAudition(ReadyCallback onReady);
  ~ResampledLoopAudition();

  void request(DspSynthesisRequest request);
  void cancel();

private:
  void workerLoop();
  static juce::AudioBuffer<float> render(const DspSynthesisRequest &request);

  ReadyCallback onReady;
  std::mutex mutex;
  std::condition_variable condition;
  DspSynthesisRequest pendingRequest;
  std::uint64_t requestId = 0;
  bool hasPendingRequest = false;
  bool shuttingDown = false;
  // Keep the thread last: it is started after full object initialization and
  // is joined explicitly before any shared state is destroyed.
  std::thread worker;
};
