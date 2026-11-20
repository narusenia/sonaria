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

enum class Level { InTune, Caution, Fix };

struct Thresholds
{
    float caution = 10.0f;
    float fix = 30.0f;
    bool jumpIncludesCaution = false;
};

// shortcut: one app-wide setting shared by every editor in the process; move it
// into the views if thresholds ever need to differ per window.
inline Thresholds& thresholds()
{
    static Thresholds value;
    return value;
}

inline Level level(float cents, Thresholds limits = thresholds())
{
    const float distance = std::abs(cents);
    return distance >= limits.fix ? Level::Fix
         : distance >= limits.caution ? Level::Caution
         : Level::InTune;
}

// Cents from the nearest tone of the project's scale; positive when sharp.
inline float cents(const Project& project, const Note& note)
{
    const float midi = meanMidi(project, note);
    const float target = ScaleUtils::snapMidiToScale(midi, ScaleUtils::keyScaleMode(project),
                                                     project.getScaleRootNote(),
                                                     project.getPitchReferenceHz());
    return (midi - target) * 100.0f;
}
// The next (or previous) note needing a fix after fromFrame, wrapping around; -1 if there is none.
// Caution notes count too when the setting asks for them.
inline int findFixNote(const Project& project, int fromFrame, bool forward,
                       bool includeCaution = thresholds().jumpIncludesCaution)
{
    const auto& notes = project.getNotes();
    int next = -1, wrapped = -1;
    const auto earlier = [&](int a, int b) { return notes[(size_t) a].getStartFrame() < notes[(size_t) b].getStartFrame(); };
    for (int i = 0; i < static_cast<int>(notes.size()); ++i) {
        const auto& note = notes[(size_t) i];
        const auto noteLevel = note.isRest() ? Level::InTune : level(cents(project, note));
        if (noteLevel != Level::Fix && !(includeCaution && noteLevel == Level::Caution))
            continue;
        const int start = note.getStartFrame();
        if (forward) {
            if (start > fromFrame && (next < 0 || earlier(i, next))) next = i;
            if (wrapped < 0 || earlier(i, wrapped)) wrapped = i;
        } else {
            if (start < fromFrame && (next < 0 || earlier(next, i))) next = i;
            if (wrapped < 0 || earlier(wrapped, i)) wrapped = i;
        }
    }
    return next >= 0 ? next : wrapped;
}
} // namespace NoteDeviation
