#pragma once

#include "ScaleUtils.h"

namespace NoteDeviation
{
// Mean pitch of the note's voiced frames in MIDI, or the note's own pitch when none is voiced.
inline float meanMidi(const Project& project, const Note& note)
{
    const auto& audio = project.getAudioData();
    const int end = std::min({note.getEndFrame(), static_cast<int>(audio.f0.size()),
                              static_cast<int>(audio.voicedMask.size())});
    double sum = 0.0;
    int count = 0;
    for (int i = std::max(0, note.getStartFrame()); i < end; ++i) {
        const auto index = static_cast<size_t>(i);
        if (audio.voicedMask[index] && audio.f0[index] > 0.0f) {
            sum += freqToMidi(audio.f0[index]);
            ++count;
        }
    }
    return count > 0 ? static_cast<float>(sum / count) : note.getAdjustedMidiNote();
}

// Cents from the nearest tone of the project's scale; positive when sharp.
inline float cents(const Project& project, const Note& note)
{
    const float midi = meanMidi(project, note);
    const float target = ScaleUtils::snapMidiToScale(midi, project.getScaleMode(),
                                                     project.getScaleRootNote(),
                                                     project.getPitchReferenceHz());
    return (midi - target) * 100.0f;
}
} // namespace NoteDeviation
