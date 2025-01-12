#pragma once

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

    bool operator==(const Mode & mode) const = default;

    [[nodiscard]] std::string modeToString() const;
    [[nodiscard]] std::string subModeToString() const;
    [[nodiscard]] std::string modeTypeToString() const;
};

// Function to convert ModeEnum to string
inline std::string Mode::modeToString() const {
    switch (this->mode) {
        case NONE:    return "NONE";
        case OBJECT:  return "OBJECT";
        case EDIT:    return "EDIT";
        case GRAB:    return "GRAB";
        case SCALE:   return "SCALE";
        case ROTATE:  return "ROTATE";
        case EXTRUDE: return "EXTRUDE";
        case FILL:    return "FILL";
        case MERGE:   return "MERGE";
        default:      return "UNKNOWN";
    }
}

// Function to convert SubMode to string
inline std::string Mode::subModeToString() const {
    switch (this->subMode) {
        case SubMode::NONE: return "NONE";
        case SubMode::X:    return "X";
        case SubMode::Y:    return "Y";
        case SubMode::Z:    return "Z";
        default:            return "UNKNOWN";
    }
}

inline std::string Mode::modeTypeToString() const {
    switch (this->type) {
        case ModeType::NONE:      return "NONE";
        case ModeType::VIEW:      return "VIEW";
        case ModeType::TRANSFORM: return "TRANSFORM";
        case ModeType::MESHDATA:  return "MESHDATA";
        default:                  return "UNKNOWN";
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
