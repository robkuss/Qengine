#ifndef MODE_H
#define MODE_H

#include <iostream>

enum class ModeType {
    NONE,
    VIEW,       // Select (Object & Edit Mode)
    TRANSFORM,  // Grab, Scale, Rotate
    MESHDATA    // Extrude, Fill
};

enum class SubMode {
    NONE,
    X,
    Y,
    Z
};

class Mode {
public:
    enum ModeEnum {
        NONE,
        OBJECT,
        EDIT,
        GRAB,
        SCALE,
        ROTATE,
        EXTRUDE,
        FILL,
        MERGE
    };

    ModeEnum mode;
    ModeType type;
    SubMode subMode;

    // Constructor
    Mode(const ModeEnum modeEnum, const ModeType modeType, const SubMode subMode = SubMode::NONE)
        : mode(modeEnum), type(modeType), subMode(subMode) {}
};

// Function to convert ModeEnum to string
inline std::string modeToString(const Mode::ModeEnum modeEnum) {
    switch (modeEnum) {
        case Mode::NONE:    return "NONE";
        case Mode::OBJECT:  return "OBJECT";
        case Mode::EDIT:    return "EDIT";
        case Mode::GRAB:    return "GRAB";
        case Mode::SCALE:   return "SCALE";
        case Mode::ROTATE:  return "ROTATE";
        case Mode::EXTRUDE: return "EXTRUDE";
        case Mode::FILL:    return "FILL";
        case Mode::MERGE:   return "MERGE";
        default:            return "UNKNOWN";
    }
}

// Function to convert SubMode to string
inline std::string subModeToString(const SubMode subMode) {
    switch (subMode) {
        case SubMode::NONE: return "NONE";
        case SubMode::X: return "X";
        case SubMode::Y: return "Y";
        case SubMode::Z: return "Z";
        default: return "UNKNOWN";
    }
}

// Predefined instances for each mode
inline Mode NONE	(Mode::NONE,	 ModeType::NONE);
inline Mode OBJECT	(Mode::OBJECT,	 ModeType::VIEW);
inline Mode EDIT	(Mode::EDIT,	 ModeType::VIEW);
inline Mode GRAB	(Mode::GRAB,	 ModeType::TRANSFORM);
inline Mode SCALE	(Mode::SCALE,	 ModeType::TRANSFORM);
inline Mode ROTATE	(Mode::ROTATE,  ModeType::TRANSFORM);
inline Mode EXTRUDE	(Mode::EXTRUDE, ModeType::MESHDATA);
inline Mode FILL	(Mode::FILL,	 ModeType::MESHDATA);
inline Mode MERGE	(Mode::MERGE,	 ModeType::MESHDATA);

#endif // MODE_H
